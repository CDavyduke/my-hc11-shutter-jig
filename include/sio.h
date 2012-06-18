/*  Filename:       sio.h
    Author:         Corey Davyduke
    Created:        2012-06-18
    Modified:       2012-06-18
    Compiler:       GNU GCC
    Description:    This one of the include files for the Shutter Jig project.
*/

#ifndef SIO_H
#define SIO_H

#include <param.h>
#include <ports.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void serial_init (void);
extern unsigned char serial_recv (void);
extern void serial_send (char);
extern void serial_flush (void);
extern unsigned char serial_receive_pending (void);
  
/*! Initialize the SCI.

    This function configures the SCI to send at \c M6811_DEF_BAUD baud
    rate.  It must be called before any other serial operation unless
    the program is started from another one (such as a monitor or
    in bootstrap mode).

    @see serial_print, M6811_DEF_BAUD
*/
extern inline void
serial_init (void)
{
  _io_ports[M6811_BAUD] = M6811_DEF_BAUD;

  /* Setup character format 1 start, 8-bits, 1 stop.  */
  _io_ports[M6811_SCCR1] = 0;

  /* Enable reciever and transmitter.  */
  _io_ports[M6811_SCCR2] = 0xc;
}

/*! Test for pending character on the reception.
    
    Return != 0 if there is something to read on the serial line.

    @return 1 if there is something to read on the serial line.
*/
extern inline unsigned char
serial_receive_pending (void)
{
  return _io_ports[M6811_SCSR] & M6811_RDRF;
}

/*! Wait until the SIO has finished to send the character.

    This function waits for the transmission of the current character.
    While waiting, the COP is reset using \c cop_optional_reset.

    @see cop_optional_reset, serial_init, serial_send
*/
extern inline void
serial_flush (void)
{
  while (!(_io_ports[M6811_SCSR] & M6811_TDRE))
    cop_optional_reset ();
}

/*! Send the character on the serial line.

    This function sends the character \a c on the serial line.
    Before sending, it calls \c serial_flush to make sure the
    transmitter is ready.  Once the function returns, the
    character is in the SCI queue and it may not be sent completely
    over the serial line.

    @param c character to send on the serial line.

    @see serial_init, serial_flush
*/
extern inline void
serial_send (char c)
{
  serial_flush ();
  _io_ports[M6811_SCDR] = c;
  _io_ports[M6811_SCCR2] |= M6811_TE;
}

/*! Wait for a character on the serial line and return it.

    This function waits for a character to be received by the SCI.
    While waiting, it calls \c cop_optional_reset to probe the COP
    regularly.

    @return the character received from the SCI.

    @see serial_init, cop_optional_reset
*/
extern inline unsigned char
serial_recv (void)
{
  while (!(_io_ports[M6811_SCSR] & M6811_RDRF))
    cop_optional_reset ();

  return _io_ports[M6811_SCDR];
}

extern void serial_init (void);

/* Return != 0 if there is something to read on the serial line.  */
extern unsigned char serial_receive_pending (void);

/* Wait until the SIO has finished to send the character.  */
extern void serial_flush (void);

/* Send the character on the serial line.  */
extern void serial_send (char c);

/* Wait for a character on the serial line and return it.  */
extern unsigned char serial_recv (void);

/** Write the string on the serial line.

    @param msg null terminated string to write.

    @see serial_init, serial_send
*/
extern void serial_print (const char *msg);

/** Wait for a string from serial line.

    @param msg buffer that will hold the string.

    @see serial_init, serial_recv
*/
extern void serial_getline (char *buf);

#ifdef __cplusplus
};
#endif
#endif /* _SYS_SIO_H */

