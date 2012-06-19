/*  Filename:       memory.x
    Author:         Corey Davyduke
    Created:        2012-06-18
    Modified:       2012-06-18
    Compiler:       GNU GCC
    Description:    This is the memory definition file for
										the ShutterJig project.
*/

MEMORY
{
  page0 (rwx) : ORIGIN = 0x0, LENGTH = 0xFF
  text  (rx)  : ORIGIN = 0xE000, LENGTH = 0x1FFF
  data        : ORIGIN = 0x2000, LENGTH = 0x1FFF
  eeprom (rx) : ORIGIN = 0xB600, LENGTH = 0xFF
}

/* Setup the stack on the top of the data memory bank.  */
PROVIDE (_stack = 0x3FFF - 1);

/* Setup the LCD access variables.  */
PROVIDE (_gdm_lcd_cmd = 0xB5F0);
PROVIDE (_gdm_lcd_data = 0xB5F1);
