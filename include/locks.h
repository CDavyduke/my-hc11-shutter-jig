/*  Filename:       locks.h
    Author:         Corey Davyduke
    Created:        2012-06-18
    Modified:       2012-06-18
    Compiler:       GNU GCC
    Description:    This one of the include files for the Shutter Jig project.
*/

#ifndef LOCKS_H
#define LOCKS_H

/*! @defgroup locks Locking

   Locking operations are defined to allow the use of two strategies to
   protect critical sections from interrupts:

   <dl>
   <dt>Lock/Unlock
   <dd>
      This simple strategy consists in disabling interrupts (\c sei ) and
      enabling them after the critical section.  In general,
      this method assumes the processor accepts interrupts when \c lock()
      is called.  This strategy is not suitable to implement functions
      that will be called with interrupts masked because this will create
      a side-effect: enable interrupts.

      <p>
      Example:
      <pre>
        lock ();
        ...
        unlock ();
      </pre>

   <dt>Lock/Restore
   <dd>
      This strategy is suitable to protect critical sections in
      functions that will be called with interrupts masked.
      The processor mask is saved before locking interrupts.
      It is restored at the end of the critical section.  When entering
      the critical section, if interrupts are masked, they will
      remain masked at end of critical section.  There is no side effect.

      <p>
      Example:
      <pre>
        unsigned short mask;

        mask = lock ();
        ...
        restore (mask);
      </pre>
    </dl>

    Sometimes it is necessary to allow interrupt processing again to make
    sure interrupts are not masked for a too long period.  In this case,
    unlocking and relocking interrupts afterwards allows this.  The function
    \c interruption_point is defined for this.

*/
/*@{*/
  
/*! Lock the processor to prevent interrupts to occur.
    When the processor is locked, interrupts are not accepted.
    They are deferred until the processor is unlocked.
    The previous locking mask is returned in the high part
    (bits 15..8) to avoid a \b tab instruction.

    @return the previous locking mask.
    @see unlock, restore   */
static __inline__ unsigned short
lock (void)
{
  unsigned short mask;

  __asm__ __volatile__ ("tpa\n\tsei" : "=d"(mask));
  return mask;
}

/*! Unlock the processor to accept interrupts.
    When the processor is unlocked, interrupts are accepted.
    Use this operation if you want, authoritatively, accept
    the interrupts.

    @see lock, restore  */
static __inline__ void
unlock (void)
{
  __asm__ __volatile__ ("cli");
}

/*! Restore the interrupt mask of the processor.
    The mask is assumed to be in the high part (bits 15..8)
    to avoid a \b tba instruction. 

    @param mask the previous interrupt mask to restore
    @see lock, unlock  */
static __inline__ void
restore (unsigned short mask)
{
  __asm__ __volatile__ ("tap" : : "d"(mask));
}

/*! Define an interruption point.

  In a big masked section, it is sometimes useful to release the interrupt
  mask to accept pending interrupts.

  This operation is intended to be used in some very specific situations
  where interrupts are masked and the code is ready to release the lock
  for a while.  It must not be used within a critical section: it would
  break it.

  @see lock, unlock  */
static __inline__ void
interruption_point (void)
{
  __asm__ __volatile__ ("cli\n\t"
                        "nop\n\t"
                        "sei");  
}

/*@}*/

#endif
