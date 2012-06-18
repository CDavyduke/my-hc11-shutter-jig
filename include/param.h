/*  Filename:       param.h
    Author:         Corey Davyduke
    Created:        2012-06-18
    Modified:       2012-06-18
    Compiler:       GNU GCC
    Description:    This one of the include files for the Shutter Jig project.
*/

#ifndef PARAM_H
#define PARAM_H

/*! Attribute unused.
    Use this attribute to indicate that a parameter, a variable or a
    static function is not used.  The compiler will not warn about the
    unused variable.  */
#define ATTRIBUTE_UNUSED __attribute__((unused))

/*! Attribute page0.
    Use this attribute to put a global or static variable in page0. */
#define PAGE0_ATTRIBUTE __attribute__((section(".page0")))

/*! Data section size.

    Define the size of the data section.  This is used by some examples
    to tune and setup some of their internal tables.  */
#ifndef DATA_SIZE
# define DATA_SIZE      (1024)
#endif

/*! Text section size.

    Define the size of the text section.  This is used by some examples
    to tune and setup some of their data types and features.  */
#ifndef TEXT_SIZE
# define TEXT_SIZE      (1024)
#endif

/*! CPU Clock frequency.
 
    Define the frequency of the oscillator plugged on the processor.
    The value is in hertz.  By default, use 8Mhz.  */
#ifndef M6811_CPU_CLOCK
# define M6811_CPU_CLOCK (8000000L)
#endif

/*! CPU E clock.
    
    The E clock frequency.  This frequency is used as the
    basis for timer computation.  The value is in hertz.  */
#ifndef M6811_CPU_E_CLOCK
# define M6811_CPU_E_CLOCK (M6811_CPU_CLOCK / 4)
#endif


/*! SIO default baud rate.
    
    Defines the default baud rate of the SIO.  This baud rate
    is used to configure the M6811_BAUD register.
    <ul>
      <li>0x33 -> 1200 baud (8Mhz cpu)
      <li>0x30 -> 9600 baud (8Mhz cpu)
    </ul>  */
#ifndef M6811_DEF_BAUD
# define M6811_DEF_BAUD   0x33
#endif

/*! Use the 68HC11 COP.
    
    Define this if you are using the COP timer.
    This activate the COP reset while polling and writing on
    the serial line.  */
#ifndef M6811_USE_COP
# define M6811_USE_COP 0
#endif

/** Timer prescaler value.  */
#ifndef M6811_DEF_TPR
# define M6811_DEF_TPR 0
#endif

#ifndef M6811_DEF_RTR
# define M6811_DEF_RTR 0
#endif

/* Axiom Manufacturing CME11E9-EVBU board definitions.  */
#define AX_CME11E9

#undef M6811_DEF_BAUD
#define M6811_DEF_BAUD 0x30            /* 9600 baud.  */

#undef RAM_SIZE
#define RAM_SIZE       32768           /* 32Kb of memory (43256).  */

#undef ROM_SIZE
#define ROM_SIZE       8192            /* 8Kb eeprom (2864).  */

#undef DATA_SIZE
# define DATA_SIZE     (0x1000-0x040)  /* Data section size.  */

#undef TEXT_SIZE
#define TEXT_SIZE      8192            /* Text section size.  */

#define GNU_LINKER_WARNING(SYMBOL, MSG) \
  asm (".section .gnu.warning." SYMBOL "\n\t.string \"" MSG "\"\n\t.previous");

#endif
