/*  Filename:       interrupts.h
    Author:         Corey Davyduke
    Created:        2012-06-18
    Modified:       2012-06-18
    Compiler:       GNU GCC
    Description:    This one of the include files for the Shutter Jig project.
*/

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <param.h>

/*! Interrupt handler prototype.  */
typedef void (* interrupt_t) (void);

/*! Interrupt vectors as a struct.  */
struct interrupt_vectors
{
  interrupt_t res0_handler;
  interrupt_t res1_handler;
  interrupt_t res2_handler;
  interrupt_t res3_handler;
  interrupt_t res4_handler;
  interrupt_t res5_handler;
  interrupt_t res6_handler;
  interrupt_t res7_handler;
  interrupt_t res8_handler;
  interrupt_t res9_handler;
  interrupt_t res10_handler;

  /** SCI interrupt handler.  */
  interrupt_t sci_handler;

  /** SPI interrupt handler.  */
  interrupt_t spi_handler;

  /** Accumulator input handler.  */
  interrupt_t acc_input_handler;

  /** Accumulator overflow interrupt handler.  */
  interrupt_t acc_overflow_handler;

  /** Timer overflow interrupt handler.  */
  interrupt_t timer_overflow_handler;

  /** Output compare 5 interrupt handler.  */
  interrupt_t output5_handler;

  /** Output compare 4 interrupt handler.  */
  interrupt_t output4_handler;

  /** Output compare 3 interrupt handler.  */
  interrupt_t output3_handler;

  /** Output compare 2 interrupt handler.  */
  interrupt_t output2_handler;

  /** Output compare 1 interrupt handler.  */
  interrupt_t output1_handler;

  /** Input capture 3 interrupt handler.  */
  interrupt_t capture3_handler;

  /** Input capture 2 interrupt handler.  */
  interrupt_t capture2_handler;

  /** Input capture 1 interrupt handler.  */
  interrupt_t capture1_handler;

  /** Realtime timer interrupt handler.  */
  interrupt_t rtii_handler;

  /** External interrupt handler.  */
  interrupt_t irq_handler;

  /** Non-maskable interrupt handler.  */
  interrupt_t xirq_handler;

  /** Software interrupt handler.  */
  interrupt_t swi_handler;

  /** Illegal instruction interrupt handler.  */
  interrupt_t illegal_handler;

  /** COP fail interrupt handler.  */
  interrupt_t cop_fail_handler;

  /** COP clock failure interrupt handler.  */
  interrupt_t cop_clock_handler;

  /** Reset handler.  */
  interrupt_t reset_handler;
};
typedef struct interrupt_vectors interrupt_vectors_t;

/*! Interrupt vector id. */
enum interrupt_vector_id
{
  RES0_VECTOR = 0,
  RES1_VECTOR,
  RES2_VECTOR,
  RES3_VECTOR,
  RES4_VECTOR,
  RES5_VECTOR,
  RES6_VECTOR,
  RES7_VECTOR,
  RES8_VECTOR,
  RES9_VECTOR,
  RES10_VECTOR,
  SCI_VECTOR,
  SPI_VECTOR,
  ACC_INPUT_VECTOR,
  ACC_OVERFLOW_VECTOR,
  TIMER_OVERFLOW_VECTOR,
  TIMER_OUTPUT5_VECTOR,
  TIMER_OUTPUT4_VECTOR,
  TIMER_OUTPUT3_VECTOR,
  TIMER_OUTPUT2_VECTOR,
  TIMER_OUTPUT1_VECTOR,
  TIMER_INPUT3_VECTOR,
  TIMER_INPUT2_VECTOR,
  TIMER_INPUT1_VECTOR,
  RTI_VECTOR,
  IRQ_VECTOR,
  XIRQ_VECTOR,
  SWI_VECTOR,
  ILLEGAL_OPCODE_VECTOR,
  COP_FAIL_VECTOR,
  COP_CLOCK_VECTOR,
  RESET_VECTOR,
  MAX_VECTORS
};
typedef enum interrupt_vector_id interrupt_vector_id;

/*! Interrupt vector table.

    The interrupt vector table is in general located at `0xffc0'
    in memory.  It is at the same address as the interrupt
    vectors structure (alias).  */
extern interrupt_t _vectors_addr[MAX_VECTORS];

extern interrupt_vectors_t _vectors __asm__("_vectors_addr");

/*! Install an interrupt handler.

    Install the interrupt handler for an exception.  The handler
    is installed for \b bootstrap mode and also for \b normal operating
    mode.
    
    @param id the interrupt number to be installed
    @param handler the interrupt handler entry point
*/
extern void
set_interrupt_handler (interrupt_vector_id id, interrupt_t handler);

/*! Default and fatal interrupt handler.

    This function is an interrupt handler intended to be used to
    handle all interrupt not used by a program.  Since it is an
    error to have an interrupt when it is not handled, the default
    behavior is to print a message and stop.  */
extern void __attribute__((interrupt, noreturn))
fatal_interrupt (void);

#define M68HC11_JMP 0x7e

extern inline void
set_interrupt_handler (interrupt_vector_id id, interrupt_t handler)
{
  unsigned char *p;

  /* Set interrupt handler for bootstrap mode.
     Compute address of that vector (3-bytes for each).
     Cast to unsigned short to avoid a Gcc warnings if sizeof int == 32.  */
  p = (unsigned char*) ((unsigned short) (0x100 - 3 * (RESET_VECTOR - id)));
  *p++ = M68HC11_JMP;
  ((interrupt_t*)p)[0] = handler;
}

/*! Entry point of any program.

    This function should never be called by itself.  It represents the
    entry point of any program.  It is intended to be used in an
    interrupt table to specify the function to jump to after reset.  */
extern void _start (void);

#endif
