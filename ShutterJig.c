/*  Filename:       ShutterJig.c
    Author:         Corey Davyduke
    Created:        2012-06-14
    Modified:       2012-06-18
    Compiler:       GNU GCC
    Description:    This project is based upon the "timer" project found
    under the Gel examples.  The HC11 uses the serial port to prompt the
    user for the current time and uses the timer to keep time thereafter.
    This project is an excellent use of interrupts.  I have modified the
    original project to open or close a shutter motor by asserting one
    half of an H-bridge driver for 100ms in response to either one of two
    keys being pressed.
*/

#include "ShutterJig.h"

#define TIMER_DIV  (8192L)
#define TIMER_TICK (M6811_CPU_E_CLOCK / TIMER_DIV)

// Define the bits for Port A.
#define PA0 (1<<0)
#define PA1 (1<<1)
#define PA2 (1<<2)
#define PA3 (1<<3)
#define PA4 (1<<4)
#define PA5 (1<<5)
#define PA6 (1<<6)
#define PA7 (1<<7)

#define LINE_1      0x80                // beginning position of LCD line 1
#define LINE_2      0xC0                // beginning position of LCD line 2
#define LINE_3      0x94                // beginning position of LCD line 3
#define LINE_4      0xD4                // beginning position of LCD line 4

// I/O Port Addresses
#define LCD_CMD  *(unsigned char *)(0xB5F0)
#define LCD_DAT  *(unsigned char *)(0xB5F1)

// On and off times (in tenths of a second)
#define ON_TIME   1
#define OFF_TIME  3

unsigned long timer_count;
unsigned long boot_time;

// Some shutter-related flags and counters.
unsigned short shutter_opened;
unsigned short shutter_closed;
unsigned short open_shutter;
unsigned short close_shutter;
unsigned short on_count;
unsigned short off_count;

int __attribute__((noreturn)) main (void);
void _start (void);

// LCD function prototypes.
void LCD_Command(unsigned char cval);
void LCD_busy(void);
void cprint(char dval);
void LCDprint(char *sptr);
void LCD_Initialize(void);

// Function prototype for the button press routine.
unsigned short ButtonPressed(void);

// To be called before main();
void _start()
{
  asm ("lds #_stack");
  set_bus_expanded ();
  main ();
}

// Timer interrupt handler.
void __attribute__((interrupt)) timer_interrupt(void)
{
  timer_count++;
  timer_acknowledge();
}

// Returns the current number of ticks that ellapsed since we started.
static inline unsigned long timer_get_ticks()
{
  unsigned long t;

  lock();
  t = timer_count;
  unlock();
  return t;
}

// Translate the number of ticks into some seconds.
static unsigned long timer_seconds(unsigned long ntime)
{
  unsigned long n;

  /* To compute SECS = NTIME * TIMER_DIV / M6811_CPU_E_CLOCK accurately,
     use Bezous relation (A = BQ + R).  */
  n = ntime * (TIMER_DIV / M6811_CPU_E_CLOCK);
  n += (ntime * (TIMER_DIV % M6811_CPU_E_CLOCK)) / M6811_CPU_E_CLOCK;
  n += boot_time;
  return n;
}

// Translate the number of ticks into some microseconds.
static unsigned long timer_microseconds(unsigned long ntime)
{
  unsigned long n;

  /* To compute SECS = NTIME * TIMER_DIV / M6811_CPU_E_CLOCK accurately,
     use Bezous relation (A = BQ + R).  */
  n = ntime * (TIMER_DIV / 2);
  n += (ntime * (TIMER_DIV % 2)) / 2;
  n = n % 1000000L;
  return n;
}

/* Translate the string pointed to by *p into a number.
   Update *p to point to the end of that number.  */
static unsigned short get_value(char **p)
{
  char *q;
  unsigned short val;

  q = *p;
  while(*q == ' ')
    q++;

  val = 0;
  while(1)
  {
    char c = *q++;
    if(c < '0' || c > '9')
      break;
    val = (val * 10) + (c - '0');
  }
  q--;
  *p = q;
  return val;
}

// Ask for the boot time.
static void get_time()
{
  char buf[32];
  int pos;
  char c;
  unsigned short hours, mins, secs;
  char *p;
  int error = 0;

  print("\r\nBoot time ? ");
  pos = 0;
  while(1)
  {
    c = serial_recv();
    if(c == '\r' || c == '\n')
      break;

    if(c == '\b')
    {
      print("\b \b");
      pos--;
      if(pos < 0)
        pos = 0;
    }
    else if(pos < sizeof (buf) - 1)
    {
      buf[pos] = c;
      buf[pos+1] = 0;
      print(&buf[pos]);
      pos++;
    }
  }

  print("\n");
  buf[pos] = 0;
  p = buf;
  hours = get_value(&p);
  if(*p++ != ':')
    error = 1;
  mins = get_value(&p);
  if(*p++ != ':' || mins >= 60)
    error = 1;
  secs = get_value(&p);
  if(*p++ != 0 || secs >= 60)
    error = 1;

  if(error == 0)
  {
    boot_time = (hours * 3600) + (mins * 60) + (secs);
    print("Boot time is set.\r\n");
  }
  else
  {
    print("Invalid boot time.\r\n");
    print("Format is: HH:MM:SS\r\n");
  }
}

// Display the current time on the serial line.
static void display_time(unsigned long ntime)
{
  unsigned long seconds;
  unsigned short hours, mins;
  unsigned long nus;
  char time_display[20];
  char tick_display[20];

  static unsigned long last_sec = 0xffffffff;
  static unsigned long last_us = 0;

  // Translate the number of ticks in seconds and milliseconds.
  seconds = timer_seconds(ntime);
  nus = timer_microseconds(ntime);

  // Get the raw number of "micro-seconds" before processing.
  sprintf(tick_display, "t=%ld, %ld, %ld", timer_count, nus, seconds);

  // Write the timer count, microseconds, and seconds
  // out to the LCD display for diagnostic purposes.
  LCD_Command(LINE_3);               // goto lcd line 3
  LCDprint(tick_display);

  nus = nus / 100000L;

  // If the seconds changed, re-display everything.
  if(seconds != last_sec)
  {
    last_sec = seconds;
    last_us = nus;
    hours = (unsigned short) (seconds / 3600L);
    mins = (unsigned short) (seconds % 3600L);
    seconds = (unsigned long) (mins % 60);
    mins = mins / 60;
    time_display[0] = '0' + (hours / 10);
    time_display[1] = '0' + (hours % 10);
    time_display[2] = ':';
    time_display[3] = '0' + (mins / 10);
    time_display[4] = '0' + (mins % 10);
    time_display[5] = ':';
    time_display[6] = '0' + (seconds / 10);
    time_display[7] = '0' + (seconds % 10);
    time_display[8] = 0;
    serial_print("\r");
    serial_print(time_display);

    // Write the clock time out to the LCD display.
    LCD_Command(LINE_2);               // goto lcd line 2
    LCDprint(time_display);
  }

  // Do this stuff every tenth of a second.
  else if(last_us != nus)
  {
    last_us = nus;

	  // If the open shutter flag has been set and the shutter is not already in the opened position,
	  // turn on the appropriate half of the H-bridge driver for the desired number of milliseconds.
	  if(open_shutter)
	  {
      // If the user is requesting that the shutter be opened, then we must be moving away from the
      // shutter closed position.
      close_shutter = 0;
      shutter_closed = 0;

      // If the shutter is not already opened, then enable the open pulse for the desired pulse duration.
      if(!shutter_opened)
      {
        // If the duration is still not yet acheived, keep the pulse on.
        if(on_count < ON_TIME)
        {
          // Open the shutter.
          _io_ports[M6811_PORTA] &= ~PA4;
          _io_ports[M6811_PORTA] |= PA5;

          // Increase the counter to indicate the number of ms the pulse has been asserted.
          on_count++;
        }

        // If the pulse has been on for the required number of ms, put the H-bridge back into idle
        // for a minimum amount of time before accepting a new command to open the shutter.
        else if(off_count < OFF_TIME)
        {
          // Bring the H-bridge driver back into the idle state.
          _io_ports[M6811_PORTA] &= ~PA4;
          _io_ports[M6811_PORTA] &= ~PA5;
          
          // Increase the counter to indicate the number of ms the pulse has been de-asserted.
          off_count++;
        }
        
        else
        {
          // After the pulse has been on for the desired amount of time and then off for the minimum
          // amount of time, reset the open shutter flag, the pulse delay counters, and signal that 
          // the shutter is now opened.
          on_count = 0;
          off_count = 0;
          open_shutter = 0;
          shutter_opened = 1;
        }
      }

      // If the shutter is already opened, reset the open shutter flag.
      else
      {
        open_shutter = 0;
      }
    }

	  // If the close shutter flag has been set and the shutter is not already in the closed position,
	  // turn on the appropriate half of the H-bridge driver for the desired number of milliseconds.
    if(close_shutter)
    {
      // If the user is requesting that the shutter be closed, then we must be transitioning from the
      // shutter opened position.
      open_shutter = 0;
      shutter_opened = 0;

      // If the shutter is not already closed, then enable the close pulse for the desired pulse duration.
      if(!shutter_closed)
      {
        // If the duration is still not yet acheived, keep the pulse on.
        if(on_count < ON_TIME)
        {
          // Close the shutter.
          _io_ports[M6811_PORTA] |= PA4;
          _io_ports[M6811_PORTA] &= ~PA5;

          // Increase the counter to indicate the number of ms the pulse has been asserted.
          on_count++;
        }

        // If the pulse has been on for the required number of ms, put the H-bridge back into idle
        // for a minimum amount of time before accepting another command to close the shutter.
        else if(off_count < OFF_TIME)
        {
          // Bring the H-bridge driver back into the idle state.
          _io_ports[M6811_PORTA] &= ~PA4;
          _io_ports[M6811_PORTA] &= ~PA5;

          // Increase the counter to indicate the number of ms the pulse has been de-asserted.
          off_count++;
        }

        else
        {
          // After the pulse has been on for the desired amount of time and then off for the minimum 
          // amount of time, reset the close shutter flag, the pulse delay counters, and signal that 
          // the shutter is now closed.
          on_count = 0;
          off_count = 0;
          close_shutter = 0;
          shutter_closed = 1;
        }
      }

      // If the shutter is already closed, reset the close shutter flag.
      else
      {
        close_shutter = 0;
      }
    }
  }

  serial_flush();
}

// Wait for the LCD busy pin to clear
void LCD_busy()
{
  while ((LCD_CMD & 0x80)) ;
}

void LCD_Command(unsigned char cval)
{
  LCD_busy();                         // wait for busy to clear
  LCD_CMD = cval;                     // ouptut command
}

void LCD_Initialize(void)
{
  // Initialize the LCD
  LCD_Command(0x3C);                 // initialize command
  LCD_Command(0x0C);                 // display on, cursor off
  LCD_Command(0x06);
  LCD_Command(0x01);
}

// LCD Display Character
void cprint(char dval)
{
  LCD_busy();                         // wait for busy to clear
  LCD_DAT = dval;                     // ouptut data
}

// LCD Display String
void LCDprint(char *sptr)
{
	while( *sptr )
  {
		cprint(*sptr);
		++sptr;
	}
}

// returns 1 if a key is pressed.
// the key value/index is stored in the global variable NewKey.
unsigned short ButtonPressed(void)
{
  unsigned short buttons = 0;
  buttons = _io_ports[M6811_PORTA] & 0x07;
  return buttons;
}

int main()
{
  unsigned long prev_time;
  unsigned short buttons = 0;
  unsigned short button_open = 0;
  unsigned short button_close = 0;
  unsigned short button_clear = 0;
  unsigned short button_open_count = 0;
  unsigned short button_close_count = 0;
  char button_display[20];

  serial_init();
  lock();
  boot_time = 0;
  timer_count = 0;

  // Set the shutter flags and counters to zero to start off with.
  shutter_opened = 0;
  shutter_closed = 0;
  open_shutter = 0;
  close_shutter = 0;
  on_count = 0;
  off_count = 0;

  // Set interrupt handler for bootstrap mode.
  set_interrupt_handler(RTI_VECTOR, timer_interrupt);

  // Initialize the timer.
  timer_initialize_rate(M6811_TPR_16);
  prev_time = timer_count;

  unlock();

  // Get the LCD ready for use.
  LCD_Initialize();

  // Print the "welcome" message out the serial port and on the LCD.
  print("\nHello, world!\n");
  LCD_Command(LINE_1);               // goto lcd line 1
  LCDprint("Hello, world!");

  // Loop waiting for the time to change and redisplay it.
  while(1)
  {
    unsigned long ntime;

    // Reset the COP (in case it is active).
    cop_optional_reset();

    /* If something is received on the serial line,
       ask for the boot time again.  */
    if(serial_receive_pending())
      get_time();

    buttons = ButtonPressed();

    button_open = buttons & PA0;
    button_close = buttons & PA1;
    button_clear = buttons & PA2;

    // If the shutter open button has been pressed, assert the shutter open flag.
    if(button_open)
    {
      open_shutter = 1;
      button_open_count++;
    }

    // If the shutter close button has been pressed, assert the shutter close flag.
    if(button_close)
    {
      close_shutter = 1;
      button_close_count++;
    }
    
    // If you push the "clear" button, then that means you want the counts to go back to zero.
    if(button_clear)
    {
      button_open_count = 0;
      button_close_count = 0;
    }

    // Display the buttons for diagnostic purposes.
    sprintf(button_display,"b=%d,%d,%d,%d,%d,%d", button_open, button_close, open_shutter, close_shutter, shutter_opened, shutter_closed);
    LCD_Command(LINE_4);               // goto lcd line 1
    LCDprint(button_display);

    // Get current time and see if we must re-display it.
    ntime = timer_get_ticks();
    if(ntime != prev_time)
    {
      prev_time = ntime;
      display_time(ntime);
    }
  }
}
