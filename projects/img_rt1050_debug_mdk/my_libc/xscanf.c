/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief Contains routines related to base format of numbers.
 *
 * \internal
 * \par modification history:
 * - 1.00 15-01-08 zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include <stdarg.h>

extern void xsfef(char *pvar, char *str, int isdouble);

#define EMPTY       -2
#define SETSIZE     32
#define MAXWIDTH    32767
#define NUL     '\0'

#define isset(a, i)     (a[(i) >> 3] & (1 << ((i) & 7)))
#define setbit(a, i)    a[(i) >> 3] |= 1 << ((i) & 7)
#define clrbit(a, i)    a[(i) >> 3] &= ~(1 << ((i) & 7))

#define FMT_NONE    0
#define FMT_EOF     1
#define FMT_LITNOMATCH  2
#define FMT_WS      3
#define FMT_LITERAL 4
#define FMT_CHAR    5
#define FMT_STRING  6
#define FMT_INT     7
#define FMT_FLOAT   8
#define FMT_CHARSET 9

#define FIF_CONTIN  010
#define FIF_SKIP_WS 004
#define FIF_NEED_CHAR   002
#define FIF_ASSIGNS 001


char FORMAT_FLAGS[] =    "\000\000\000\014\012\013\017\017\017\013";

/******************************************************************************/
static int i_isodigit(int c)
{
    return (c >= '0' && c <= '7');
}

/******************************************************************************/
static int i_isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

/******************************************************************************/
static int i_isxdigit(int c)
{
    return ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F'));
}

/******************************************************************************/
static int toint(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return (ch - '0');
    }
    if (ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + 10);
    }
    if (ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + 10);
    }

    return (-1);
}


/******************************************************************************/
static int ss_getc(char **ptr)
{
    int ch;

    ch = **ptr;

    if(ch == NUL) {
        return (EOF);
    }

    ++*ptr;

    return (ch);
}

/******************************************************************************/
int xsfff(int *inp, int (*getcfn)(char *), char *param, char *pc, int width)
{
    int in = *inp;
    int numberp = 0;

    /* 
     * This routine parse floating point numbers in the following format
     * -3.9E49 
     */

    /* First of all parse + or - */
    if (in == '+' || in == '-') {
        *pc++ = (char )in;
        in = --width > 0 ? (*getcfn)(param) : EMPTY;
    }

    /* Then parse digits after + or - sign */
    if (width > 0) {
        for (; (in >= '0' && in <= '9'); in = (*getcfn)(param)) {
            *pc++ = (char )in;
            numberp = 1;
            if (--width <= 0) {
                in = EMPTY;
                break;
            }
        }
    }

    /* Then parse the decimal sign */
    if (width > 0 && in == '.') {
        *pc++ =(char ) in;
        numberp = 1;
        in = --width > 0 ? (*getcfn)(param) : EMPTY;
    }

    /* Parse digits after the decimal point */
    if (width > 0) {
        for (; (in >= '0' && in <= '9') ; in = (*getcfn)(param)) {
            *pc++ = (char )in;
            numberp = 1;
            if (--width <= 0) {
                in = EMPTY;
                break;
            }
        }
    }

    /* Parse exponent */
    if (width > 0 && numberp && (in == 'e' || in == 'E')) {
        *pc++ = (char )in;
        in = --width > 0 ? (*getcfn)(param) : EMPTY;
        if (width > 0 && (in == '+' || in == '-')) {
            *pc++ = (char )in;
            in = --width > 0 ? (*getcfn)(param) : EMPTY;
        }

        if (width > 0) {
            for (; (in >= '0' && in <= '9') ; in = (*getcfn)(param)) {
                *pc++ = (char )in;
                if (--width <= 0) {
                    in = EMPTY;
                    break;
                }
            }
        }
    }

    *pc = NUL;
    *inp = in;

    return (numberp);
}


/**
 * \brief Low level scanf function.
 *
 * \param getcfn   Pointer to a function that gets the next character.
 *                    If NULL, input is from the NULL-terminated string "param".
 *
 * \param ungetcfn Pointer to a function that ungets a character.
 *                 If NULL, this function does not unget a character.
 *
 * \param param    An arbitrary pointer parameter that is passed to the above.
 *                   If getcfn is NULL, this is a NULL-terminated string with input.
 * \param fmt      The format string.
 * \param parglist Pointer to the argument list.
 *
 * \return         Number of matched and assigned input fields.
 */
int xscanf(int (*getcfn)(void *),int (*ungetcfn)(int, void *),
           void *param, const char *fmt, va_list parglist)
{
    char *format_flags = FORMAT_FLAGS;      /* Replaces table */
    int in = EMPTY;                         /* Current character from
                                               input stream */
    int fch;                                /* Current character from
                                               format string */
    int format = FMT_WS;                    /* Field attribute:
                                                    format code */
    int width;                              /* Field attribute:
                                                    total width */
    int isassigned;                         /* Field attribute:
                                                    assign value? */
    int size;                               /* Field attribute: h=short,
                                                    l=long, other=int */
    int (*isdigitfn)(int);                  /* Field attribute:
                                                    is-digit? function */
    int base;                               /* Field attribute:
                                                    numeric base */
    char set[SETSIZE];                      /* Set of characters matched
                                               by %[, also %f buffer */
    int nmatch = 0;                         /* Number of matched and
                                               assigned fields */
    int ismatch;                            /* Did this field match? */
    char *pvar;                             /* Pointer to variable we are
                                               assigning to */
    char *inputstr;                         /* Input string */

    /* Handles warnings */
    ismatch = 0;
    size = 0;
    isdigitfn = i_isdigit;
    base = 0;
    if(getcfn == NUL) {
        /* If the function for getting the next character from the stream
         * is not specified then use our function   
         */
        getcfn      = (int (*)(void *))ss_getc;
        inputstr    = (char *)param;
        param       = (char *) &inputstr;
    }

    while(in != EOF && format_flags[format] & FIF_CONTIN) {

        /* 
         * While input characters does not reach End Of File And format
         * character exists 
         */
        format  = FMT_NONE;
        width   = 0;
        isassigned = 1;
        ismatch = 0;

    /* Take actions according to the format */
    switch(fch = *fmt++) {
        /* Format is NULL */
        case NUL:
            break;

        case ' ':
        case '\t':
        case '\n':
        /* Format is white Space characters */
            format = FMT_WS;
            break;

        case '%':
        /* Format is a conversion specifier */
            fch = *fmt++;

            if(fch == '*') {
                /* Suppresses assignment of the next input field */
                isassigned = 0;
                fch = *fmt++;
            }

            for (; (fch >= '0' && fch <= '9'); fch = *fmt++) {
                width = 10 * width + fch - '0';
            }
            size = fch;

            /* 
             * Specifies that conversion specifier applies to an argument
             * with pointer to short or long 
             */
            if(size == 'h' || size == 'l') {
                fch = *fmt++;
            }
            /* Fill format according to the conversion specifier */
            switch (fch) {
                case NUL:
                /* End Of File reached */
                    format = FMT_EOF;
                    break;

                case '%':
                /* 
                 * Matches a single '%' character. No Conversion or
                 * assignment occurs 
                 */
                    format = FMT_LITERAL;
                    break;

                case 'c':
                /* 
                 * Matches a sequence of bytes of the number specified by
                 * the filed width  
                 */
                    format = FMT_CHAR;
                    break;

                case 's':
                /* 
                 * Matches a sequence of bytes that are not white space
                 * characters
                 */
                    format = FMT_STRING;
                    break;

                case 'o':
                /* Matches an optionally signed integer */
                    format      = FMT_INT;
                    isdigitfn   = i_isodigit;
                    base = 8;
                    break;

                case 'u':
                case 'd':
                /* Matches an optionally signed decimal integer */
                    format      = FMT_INT;
                    isdigitfn   = i_isdigit;
                    base = 10;
                break;

                case 'x':
                /* Matches an optionally signed hexadecimal integer */
                    format      = FMT_INT;
                    isdigitfn   = i_isxdigit;
                    base = 16;
                    break;

                case 'e':
                case 'f':
                /* 
                 * Matches an optionally signed floating-point number or
                 * NaN 
                 */
                    format  = FMT_FLOAT;
                    width   = width < (SETSIZE - 1) ? width : (SETSIZE - 1);
                    break;

                case '[':
                /* 
                 * Matches a non empty sequence of bytes from a set of
                 * expected bytes (the scanset)
                 */
                    format = FMT_CHARSET;
                    {
                        int iscomplement;
                        int i;

                        fch = *fmt++;

                        iscomplement = fch;

                        if(iscomplement == '^')
                            fch = *fmt++;

                        for (i = 0; i < SETSIZE; i++)
                            set[i] = 0;

                        for(; fch != NUL && fch != ']'; fch = *fmt++ & 0xFF)
                            setbit(set, fch);

                        if (fch == NUL)
                            format = FMT_EOF;

                        if (iscomplement)
                            for (i = 0; i < SETSIZE; i++)
                                set[i] ^= 0xFF;

                        clrbit(set, NUL);   /* NULL is always terminator */
                    }
                    break;
                }
                break;
            default:
                /* No Conversion or assignment occurs */
                format = FMT_LITERAL;
                break;
    }   /*  End of Switch Statement.    */

    /* If width is still zero assign width */
    if (width == 0) {
        switch (format) {
            case FMT_CHAR:
            /* For character set a width of one byte */
                width = 1;
                break;

            case FMT_STRING:
            case FMT_INT:
            case FMT_CHARSET:
            /* For string , integer and character set the maximum
             width */
                width = MAXWIDTH;
                break;

            case FMT_FLOAT:
            /* For float set the width of 31 bytes (Single Precession) */
                width = SETSIZE - 1;
                break;
        }   /*  End of Switch Statement */
    }

    /* In case of the whitespace characters */
    if (format_flags[format] & FIF_SKIP_WS) {
        /* If next character is empty call a function to fill input
         stream */
        if(in == EMPTY)
            in = (*getcfn)(param);


        /* Fill the input stream until End Of File is reached or whitespace
        character is encountered */
        for (; in != EOF && (in == ' ' || (in >= '\t' && in <= '\r')); in = (*getcfn)(param)) {
        }
    } /*    End of If Statement */

    if(format_flags[format] & FIF_NEED_CHAR && in == EMPTY) {
        in = (*getcfn)(param);
    }
    pvar = format_flags[format] & FIF_ASSIGNS && isassigned  ?
            va_arg(parglist, char *) :NUL;

    /* 
     * Additional checking whenever pvar is null,
     * we will need to set isassigned to zero as defined in the
     * case statement below 
     */
    if (pvar == NUL) {
        isassigned = 0;
    }

    /* Take actions according to the conversion specifier */
    switch (format) {
        /* No Conversion or assignment occurs */
        case FMT_LITERAL:
            if(in == fch) {
                in = EMPTY;
            } else {
                format = FMT_LITNOMATCH;
            }
            break;

        /* Matches a sequence of bytes of the number specified by
         the filed width  */
        case FMT_CHAR:
            for (; in != EOF; in = (*getcfn)(param)) {
                ismatch = 1;

                if (isassigned) {
                    *pvar++ = (char)in;
                }
                if (--width <= 0) {
                    in = EMPTY;
                    break;
                }
            }
            break;

        /* 
         * Matches a sequence of bytes that are not white space
         * characters 
         */
        case FMT_STRING:
            for(; in != EOF && !(in == ' ' || (in >= '\t' && in <= '\r'));
                in = (*getcfn)(param)) {
                ismatch = 1;

                if (isassigned) {
                    *pvar++ = (char)in;
                }
                if (--width <= 0) {
                    in = EMPTY;
                    break;
                }
            }

            if (ismatch && isassigned) {
                *pvar = NUL;
            }
            break;

        /* 
         * Matches a non empty sequence of bytes from a set of
         * expected bytes (the scanset) 
         */
        case FMT_CHARSET:
            for(; in != EOF && isset(set, in); in = (*getcfn)(param)) {
                ismatch = 1;

                if(isassigned) {
                    *pvar++ = (char)in;
                }

                if(--width <= 0) {
                {
                    in = EMPTY;
                }
                    break;
                }
            }

            if(ismatch && isassigned) {
                *pvar = NUL;
            }
            break;

        /* Matches an optionally signed decimal integer */
        case FMT_INT:
        {
            int isminus = 0;

            if(in == '+' || in == '-') {
                isminus = in == '-';

                in = --width > 0 ? (*getcfn)(param) : EMPTY;
            }

            if(width > 0 && (*isdigitfn)(in)) {
                long num = 0L;

                for (; (*isdigitfn)(in); in = (*getcfn)(param))    {
                    num = base * num + toint(in);

                    if (--width <= 0) {
                        in = EMPTY;
                        break;
                    }
                }

                ismatch = 1;

                if(isassigned) {
                    if (isminus) {
                        num = -num;
                    }
                    if (size == 'h') {
                        * (short *) pvar = (short) num;
                    } else if (size == 'l') {
                        * (long *) pvar = num;
                    } else {
                        * (int *) pvar = (int) num;
                    }
                }
            }
        }
            break;

        /* 
         * Matches an optionally signed floating-point number or
         * NaN 
         */
        case FMT_FLOAT:
            if(xsfff(&in, (int (*)(char *))getcfn, param, set, width)) {
                ismatch = 1;

                if (isassigned) {
                    xsfef(pvar, set, size == 'l');
                }
            }

            break;

    } /*    End of Switch Statement */

    if(ismatch && isassigned) {
        nmatch++;
    }

    }   /*  End of While Loop   */

    if(in != EOF && in != EMPTY && ungetcfn != NUL) {
        (*ungetcfn)(in, param);
    }
    if (format == FMT_EOF) {
        return EOF;
    }
    if (in == EOF) {
        if (format == FMT_LITNOMATCH) {
            return EOF;
        } else if (!ismatch && format_flags[format] & FIF_ASSIGNS) {
            return EOF;
        }
     }

    return(nmatch);
}

/* end of file */


