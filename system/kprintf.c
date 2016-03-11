/**
 * @file kprintf.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

/*TA-BOT:MAILTO Noah.Baertsch@marquette.edu Emmali.Hanson@marquette.edu */

#include <xinu.h>

#define UNGETMAX 10		/* Can un-get at most 10 characters. */

static unsigned char ungetArray[UNGETMAX] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};

/**
 * Synchronously read a character from a UART.  This blocks until a character is
 * available.  The interrupt handler is not used.
 *
 * @return
 *      The character read from the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kgetc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    unsigned char c;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x20201000;

    // TODO: First, check the unget buffer for a character.
    //       Otherwise, check UART flags register, and
    //       once the receiver is not empty, get character c.
    int i;
    for (i = 0; i < UNGETMAX; i++){
    	if (ungetArray[i] != '\0'){
    		c = ungetArray[i];
    		ungetArray[i] = '\0';
    		return (int)c;
    	}
    }
    while((*regptr).fr & PL011_FR_RXFE);	//block
    c = (*regptr).dr;
    return (int)c;
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
int kcheckc(void)
{
    volatile struct pl011_uart_csreg *regptr;
    regptr = (struct pl011_uart_csreg *)0x20201000;

    // TODO: Check the unget buffer and the UART for characters.
    int i;
    for (i = 0; i < UNGETMAX; i++){
		if (ungetArray[i] != '\0'){
			return 1;
		}
    }
    if((*regptr).fr & PL011_FR_RXFE)
    	return 0;
    return 1;
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
	// TODO: Check for room in unget buffer, put the character in or discard.
	//UNGETMAX
	int i;
	for (i = 0; i < UNGETMAX; i++){
		if (ungetArray[i] == '\0'){
			ungetArray[i] = c;
			return c;
		}
	}
	return SYSERR;
}


/**
 * Synchronously write a character to a UART.  This blocks until the character
 * has been written to the hardware.  The interrupt handler is not used.
 *
 * @param c
 *      The character to write.
 *
 * @return
 *      The character written to the UART as an <code>unsigned char</code> cast
 *      to an <code>int</code>.
 */
syscall kputc(unsigned char c)
{
    volatile struct pl011_uart_csreg *regptr;

    /* Pointer to the UART control and status registers.  */
    regptr = (struct pl011_uart_csreg *)0x20201000;

    // TODO: Check UART flags register.
    //       Once the Trasnsmitter FIFO is not full, send character c.
    while((*regptr).fr & PL011_FR_TXFF);
    (*regptr).dr = c;
    return (int)c;
}

/**
 * kernel printf: formatted, synchronous output to SERIAL0.
 *
 * @param format
 *      The format string.  Not all standard format specifiers are supported by
 *      this implementation.  See _doprnt() for a description of supported
 *      conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
syscall kprintf(const char *format, ...)
{
    int retval;
    va_list ap;

    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(int, int))kputc, 0);
    va_end(ap);
    return retval;
}
