/*  Filename:       ShutterBits.h
    Author:         Corey Davyduke
    Created:        2012-06-14
    Modified:       2012-04-14
    Description:    This is the header file for the Shutter Jig project.
*/

#ifndef _SHUTTERJIG_H
#define _SHUTTERJIG_H

#include <sys/param.h>
#include <sys/interrupts.h>
#include <sys/sio.h>
#include <sys/locks.h>
#include <stdarg.h>

extern void timer_interrupt (void) __attribute__((interrupt));

inline static void print (const char* msg)
{
  serial_print (msg);
}

#endif
