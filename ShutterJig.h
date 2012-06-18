/*  Filename:       ShutterBits.h
    Author:         Corey Davyduke
    Created:        2012-06-14
    Modified:       2012-06-18
    Compiler:       GNU GCC
    Description:    This is the header file for the Shutter Jig project.
*/

#ifndef _SHUTTERJIG_H
#define _SHUTTERJIG_H

#include <param.h>
#include <interrupts.h>
#include <sio.h>
#include <locks.h>
#include <stdarg.h>

extern void timer_interrupt (void) __attribute__((interrupt));

#ifdef USE_INTERRUPT_TABLE

/* Interrupt table used to connect our timer_interrupt handler.

   Note: the `XXX_handler: foo' notation is a GNU extension which is
   used here to ensure correct association of the handler in the struct.
   This is why the order of handlers declared below does not follow
   the HC11 order.  */
struct interrupt_vectors __attribute__((section(".vectors"))) vectors =
{
  res0_handler:           fatal_interrupt, /* res0 */
  res1_handler:           fatal_interrupt,
  res2_handler:           fatal_interrupt,
  res3_handler:           fatal_interrupt,
  res4_handler:           fatal_interrupt,
  res5_handler:           fatal_interrupt,
  res6_handler:           fatal_interrupt,
  res7_handler:           fatal_interrupt,
  res8_handler:           fatal_interrupt,
  res9_handler:           fatal_interrupt,
  res10_handler:          fatal_interrupt, /* res 10 */
  sci_handler:            fatal_interrupt, /* sci */
  spi_handler:            fatal_interrupt, /* spi */
  acc_overflow_handler:   fatal_interrupt, /* acc overflow */
  acc_input_handler:      fatal_interrupt,
  timer_overflow_handler: fatal_interrupt,
  output5_handler:        fatal_interrupt, /* out compare 5 */
  output4_handler:        fatal_interrupt, /* out compare 4 */
  output3_handler:        fatal_interrupt, /* out compare 3 */
  output2_handler:        fatal_interrupt, /* out compare 2 */
  output1_handler:        fatal_interrupt, /* out compare 1 */
  capture3_handler:       fatal_interrupt, /* in capt 3 */
  capture2_handler:       fatal_interrupt, /* in capt 2 */
  capture1_handler:       fatal_interrupt, /* in capt 1 */
  irq_handler:            fatal_interrupt, /* IRQ */
  xirq_handler:           fatal_interrupt, /* XIRQ */
  swi_handler:            fatal_interrupt, /* swi */
  illegal_handler:        fatal_interrupt, /* illegal */
  cop_fail_handler:       fatal_interrupt,
  cop_clock_handler:      fatal_interrupt,

  /* What we really need.  */
  rtii_handler:           timer_interrupt,
  reset_handler:          _start
};

#endif

inline static void print (const char* msg)
{
  serial_print (msg);
}

#endif
