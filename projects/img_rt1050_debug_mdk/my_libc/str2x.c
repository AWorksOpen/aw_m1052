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
 * \brief This file contains the routine to convert strings to various
 *        integer and floating-point data types.
 *
 * \internal
 * \par modification history:
 * - 1.00 15-01-08 zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include <math.h>
#include <limits.h>
#include <errno.h>

/* #define IEEE */

#ifndef ULLONG_MAX
#define ULLONG_MAX    ((uint64_t)(~(uint64_t)0))
#endif

#ifndef LONG_MAX
#define LONG_MAX    ((int32_t)((~(uint32_t)0) >> 1))
#endif

#ifndef LONG_MIN
#define LONG_MIN    (-LONG_MAX - 1)
#endif

#ifndef LLONG_MAX
#define LLONG_MAX    ((int64_t)((~(uint64_t)0) >> 1))
#endif

#ifndef LLONG_MIN
#define LLONG_MIN    (-LLONG_MAX - 1)
#endif

/* Positions Table (long double) of base-10 exponents. */

static long double STRTOX__posTbl[] = {1e1,1e2,1e4,1e8,1e16,1e32

#ifdef IEEE

    ,1e64,1e128,1e256

#endif /* IEEE */

}; /* STRTOX__pos */

/* Positions Table size. */

#ifdef IEEE

#define STRTOX_POS_TBL_SIZE        8

#else

#define STRTOX_POS_TBL_SIZE        5

#endif /* IEEE */

/* Maximum Exponent value */

#ifdef IEEE

#define STRTOX_EXP_MAX             306

#else

#define STRTOX_EXP_MAX             76

#endif /* IEEE */

/* Exponent Too Big value */

#ifdef IEEE

#define STRTOX_EXP_TOO_BIG         512

#else

#define STRTOX_EXP_TOO_BIG         64

#endif /* IEEE */

/* Implicit Exponent Maximum value */

#define STRTOX_IMPL_EXP_MAX        214748364L

/* Explicit Exponent Maximum value */

#define STRTOX_EXPL_EXP_MAX        3276

/* Local function prototypes */

static long double STRTOX__Exp10(long double      num,
                                 int              exponent,
                                 aw_bool_t *      pIsInvalid);

static aw_err_t STRTOX__Str_Is_Neg(const char      str[],
                                   uint32_t *      pChCnt,
                                   aw_bool_t *     pIsNeg);

static aw_err_t STRTOX__Process_Prefix(const char           str[],
                                       int                  base,
                                       uint32_t *           pChCnt,
                                       int *                pBase,
                                       aw_bool_t *          pIsNeg,
                                       aw_bool_t *          pIsInvalid);

static aw_err_t STRTOX__Process_Ch(char                 ch,
                                   int                  base,
                                   int *                pChVal,
                                   aw_bool_t *          pIsInvalid);

static aw_err_t STRTOX__Process_To_End(const char    str[],
                                       int           base,
                                       uint32_t *    pChCnt);

static int STRTOX__Is_Space(int c);

static aw_err_t STRTOX__Process_Str(const char             str[],
                                    int                    base,
                                    long long              min,
                                    long long              max,
                                    unsigned long long *   pResult,
                                    uint32_t *             pChCnt,
                                    aw_bool_t *            pIsNonNum,
                                    aw_bool_t *            pIsOverflow);

static long long STRTOX__Str_To_Int(const char      str[],
                                    int             base,
                                    long long       min,
                                    long long       max,
                                    uint32_t *      pChCnt);

static unsigned long long STRTOX__Str_To_Uint(const char            str[],
                                              int                   base,
                                              unsigned long long    min,
                                              unsigned long long    max,
                                              uint32_t *            pChCnt);

static long double STRTOX__Str_To_Flt(const char    str[],
                                      uint32_t *        pChCnt);

/* Local functions */


/******************************************************************************/
static long double STRTOX__Exp10(long double      num,
                                 int              exponent,
                                 aw_bool_t *      pIsInvalid)
{
    long double         prodExp;
    long double         result;
    aw_bool_t             isInvalid;
    aw_bool_t             isNegExp;
    aw_bool_t             isNonZero;
    uint32_t              i;

    /* Set initial function state values. */

    isInvalid = AW_FALSE;
    isNegExp = AW_FALSE;
    isNonZero = AW_FALSE;

    /* Check for number or exponent of zero. */

    if ((exponent == 0) || (num == 0.0))
    {
        /* In either case the result is the number value. */

        result = num;

    }
    else
    {
        /* Check for a valid exponent value. */

        if (exponent < -STRTOX_EXP_MAX)
        {
            /* ERROR: Exponent is out of range (negative). */

            isInvalid = AW_TRUE;

            /* Update result. */

            result = 0.0;

        }
        else
        {
            if (exponent > STRTOX_EXP_MAX)
            {
                /* ERROR: Exponent is out of range (positive). */

                isInvalid = AW_TRUE;

                result = HUGE_VAL;

            } /* if */

        } /* if */

        if (!isInvalid )
        {
            /* Setup the exponent value for calculations. */

            if (exponent < 0)
            {
                /* Convert the exponent to positive. */

                exponent = -exponent;

                /* Indicate that the exponent is actually negative. */

                isNegExp = AW_TRUE;
            }
            else
            {
                /* Indicate exponent is actually positive. */

                isNegExp = AW_FALSE;

            } /* if */

            /* Adjust the number based on the exponent (?) */

            if (exponent >= STRTOX_EXP_TOO_BIG)
            {
                exponent -= (STRTOX_EXP_TOO_BIG / 2);

                if (isNegExp )
                {
                    num /= STRTOX__posTbl[STRTOX_POS_TBL_SIZE];
                }
                else
                {
                    num *= STRTOX__posTbl[STRTOX_POS_TBL_SIZE];

                } /* if */

            } /* if */

            /* Calculate the product of the base-10 exponents. */

            prodExp = 1.0;
            i = 0;
            while ((exponent > 0) &&
                   (i < STRTOX_POS_TBL_SIZE))
            {
                /* Determine if the current exponent value has a non-zero
                   digit in the unit's column. */

                isNonZero = ((exponent & 0x1) > 0);

                if (isNonZero )
                {
                    /* 
                     * Add the non-zero value currently at the unit's
                     * into the current product value using the matching
                     * exponent table entry. 
                     */

                    prodExp = (prodExp * STRTOX__posTbl[i]);

                } /* if */

                /* Move to the next column in the exponent. */

                exponent = (exponent >> 0x1);

                /* Move to the next table entry. */

                i++;

            } /* while */

            /* Apply the product exponent to the number in the appropriate
               manner based on the sign of the exponent.  Update
               result. */

            if (isNegExp )
            {
                result = (num / prodExp);

            }
            else
            {
                result = (num * prodExp);

            } /* if */

        } /* if */

    } /* if */

    /* Update the return parameters. */

    *pIsInvalid = isInvalid;

    return (result);
}

/*******************************************************************************

   FUNCTION

       STRTOX__Str_Is_Neg

   DESCRIPTION

       The function determines if a string indicates a negative value
       or not.  If the string contains a sign character ("-" or "+")
       then it will be used to determine the sign value.  The default
       value is positive (no "-" or "+" indicates positive).

       NOTE: It is safe to pass a string pointer to the first parameter
             and a reference to that same string as thesecond parameter;
             the update is only performed at the end of the function.

   INPUTS

       str - Pointer to the start of the string to be examined.

       pChCnt - Return parameter that will be updated with the number
                   of characters processed if the operation is
                   successful.  If the operation fails the value is
                   undefined.

       pIsNeg - Return parameter that will be updated to indicate if the
                string value is considered negative (AW_TRUE) or positive
                (AW_FALSE) if the operation is successful.  If the operation
                fails the value is undefined.

   OUTPUTS

        AW_OK - Indicates successful completion.

*******************************************************************************/
static aw_err_t STRTOX__Str_Is_Neg(const char      str[],
                                   uint32_t *      pChCnt,
                                   aw_bool_t *     pIsNeg)
{
    aw_err_t      status;
    aw_bool_t     isNeg;
    uint32_t      strIdx;

    /* Set initial function status. */

    status = AW_OK;

    /* Default to a positive string value. */

    isNeg = AW_FALSE;

    /* Set initial string parsing counter. */

    strIdx= 0;

    /* Examine the character to determine the sign value of the string. */

    if (str[strIdx] == '-')
    {
        ++strIdx;

        /* Indicate that the mantissa sign is negative. */

        isNeg = AW_TRUE;
    }
    else
    {
        if (str[strIdx] == '+')
        {
            ++strIdx;

        } /* if */

    } /* if */

    /* Update return parameters. */

    *pChCnt = strIdx;
    *pIsNeg = isNeg;

    return (status);
}

/*******************************************************************************

   FUNCTION

        STRTOX__Process_Prefix

   DESCRIPTION

        This function attempts to process the prefix of a number string.

   INPUTS

       str - The string to be parsed.

       base - The initial base value indicated.  This value is used to
              determine how the string is parsed to determine the base
              if the base value is not already set to a valid value.

       pChCnt - Return parameter that will be updated with the number
                   of characters processed if the operation is
                   successful.  If the operation fails the value is
                   undefined.

       pBase - Return parameter that will be updated based on the parsing
               process (e.g. a base of 0 indicates that the string will
               dictate the base).  If the operation fails the value is
               undefined.

       pIsNeg - Return parameter that will be updated to indicate if the
                number is negative or not.  If the operation fails the
                value is undefined.

       pIsInvalid - Return parameter that will be updated to indicate if
                    an invalid character or base was encountered.  If the
                    operation fails the value is undefined.

   OUTPUTS

       AW_OK - The operation succeeded.

       -AW_EPERM - Indicates that the conversion data is
                              invalid.

*******************************************************************************/
static aw_err_t STRTOX__Process_Prefix(const char           str[],
                                       int                  base,
                                       uint32_t *           pChCnt,
                                       int *                pBase,
                                       aw_bool_t *          pIsNeg,
                                       aw_bool_t *          pIsInvalid)
{
    aw_err_t      status;
    aw_bool_t     isNeg;
    aw_bool_t     isInvalid;
    uint32_t      strIdx;
    uint32_t      chCnt;

    /* Set initial function status. */

    status = AW_OK;

    /* Set initial state values. */

    isInvalid = AW_FALSE;
    isNeg = AW_FALSE;

    /* Set initial character parsing counter. */

    strIdx = 0;

    /* Ensure that a valid base parameter has been supplied. */

    if ((base < 0) ||
        (base == 1) ||
        (base > 36))
    {
        /* ERROR: Invalid base. */

        isInvalid = AW_TRUE;

    } /* if */

    if (!isInvalid )
    {
        /* 
         * Skip any proceeding white space (as defined by the isspace()
         * function). 
         */

        while (STRTOX__Is_Space(str[strIdx]) != 0)
        {
            ++strIdx;

        } /* while */

        /* 
         * Determine the sign of the number string, moving past any
         * encountered sign character.  Defaults to positive. 
         */

        status = STRTOX__Str_Is_Neg(&str[strIdx],
                                    &chCnt,
                                    &isNeg);

   } /* if */

   if ((status == AW_OK) &&
       (!isInvalid ))
   {
       /* 
        *  Update the current string counter with the number of characters
        *  processed by the last function call. 
        */

        strIdx = (strIdx + chCnt);

        /* Determine the base of the number string if none is given
         * (base parameter is zero) or handle the optional hexadecimal
         * prefix if the base is 16 or octal prefix if the base is 8. 
         */

        if ((base == 0) ||
            (base == 16))
        {
            if (str[strIdx] == '0')
            {
                ++strIdx;

                if ((str[strIdx] == 'x') || (str[strIdx] == 'X'))
                {
                    *pBase = 16;

                    ++strIdx;
                }
                else
                {
                    if (base != 16)
                    {
                        *pBase = 8;

                    } /* if */

                } /* if */
            }
            else
            {
                if (base != 16)
                {
                    *pBase = 10;

                } /* if */

            } /* if */

        }
        else
        {
            /* The base of the string is what was passed in as the base
             * parameter. 
             */

            *pBase = base;

        } /* if */

    } /* if */

    if (status == AW_OK)
    {
        /* Update return parameters. */

        *pChCnt = strIdx;
        *pIsNeg = isNeg;
        *pIsInvalid = isInvalid;

    } /* if */

    return(status);
}


static aw_err_t STRTOX__Process_Ch(char                 ch,
                                   int                  base,
                                   int *                pChVal,
                                   aw_bool_t *          pIsInvalid)
{
    aw_err_t      status;
    int         chVal;
    aw_bool_t     isInvalid;

    /* Set initial function status. */

    status = AW_OK;

    /* Get character value. */

    chVal = ch;

    /* Set initial state values. */

    isInvalid = AW_FALSE;

    /* If the character is a digit, ensure that it is appropriate for
       the base. */

    if (chVal >= '0' && chVal <= '9')
    {
        if ((chVal -= '0') >= base)
        {
            /* ERROR: Numeric character is invalid. */

            isInvalid = AW_TRUE;

        } /* if */

    }
    else
    {
        /* If the character is a lower-case letter, ensure that it is
           appropriate for the base. */

        if (chVal >= 'a' && chVal <= 'z')
        {
            if ((chVal -= ('a' - 10)) >= base)
            {
                /* ERROR: Non-numeric, lower-case letter is invalid. */

                isInvalid = AW_TRUE;

            } /* if */

        }
        else
        {
            /* If the character is an upper-case letter, ensure that
               it is appropriate for the base. */

            if (chVal >= 'A' && chVal <= 'Z')
            {
                if ((chVal -= ('A' - 10)) >= base)
                {
                    /* ERROR: Non-numeric, upper-case letter invalid. */

                    isInvalid = AW_TRUE;

                } /* if */

            }
            else
            {
                /* 
				 * Non-number string character encountered.  May be
                 * NULL or other invalid character, either way it is
                 * invalid. 
				 */

                isInvalid = AW_TRUE;

            } /* if */

        } /* if */

    } /* if */

    /* Update return parameters. */

    *pChVal = chVal;
    *pIsInvalid = isInvalid;

    return(status);
}

static aw_err_t STRTOX__Process_To_End(const char    str[],
                                       int           base,
                                       uint32_t *    pChCnt)
{
    aw_err_t      status;
    int         ch;
    uint32_t      strIdx;

    /* Set initial function status. */

    status = AW_OK;

    /* Initialize string counter. */

    strIdx = 0;

    /* Skip to the end of the number string. */

    do
    {
        /* Get current character from string. */

        ch = str[strIdx];

        /* Move to the next character. */

        ++strIdx;

    } while ((ch >= '0' && ch <= '9' && ch <= ('0' + base)) ||
             (ch >= 'a' && ch <= ('a' - 10 + base)) ||
             (ch >= 'A' && ch <= ('A' - 10 + base)));

    /* Back up one to last valid character (may be end of the number
       string). */

    --strIdx;

    /* Update the return parameters. */

    *pChCnt = strIdx;

    return(status);
}

/******************************************************************************/
static int STRTOX__Is_Space(int c)
{
    char    ch;
    int     isSpace;

    /* Translate the character value into a character type. */

    ch = (char)c;

    /* Determine if the character is a space and update the return value
       accordingly. */

    if ((ch == ' ' ) || ((ch >= '\t') && (ch <= '\r')))
    {
        isSpace = 1;

    }
    else
    {
        isSpace = 0;

    } /* if */

    return (isSpace);
}


/******************************************************************************/
static aw_err_t STRTOX__Process_Str(const char             str[],
                                  int                    base,
                                  long long              min,
                                  long long              max,
                                  unsigned long long *   pResult,
                                  uint32_t *               pChCnt,
                                  aw_bool_t *              pIsNonNum,
                                  aw_bool_t *              pIsOverflow)
{
    aw_err_t              status;
    unsigned long long  result;
    unsigned long long  tempResult;
    int                 chVal;
    aw_bool_t             isNonNum;
    aw_bool_t             isOverflow;
    uint32_t              strIdx;
    uint32_t              chCnt;

    /* Set initial function status. */

    status = AW_OK;

    /* Initialize cumulative result value. */

    result = 0;

    /* Initialize the string index. */

    strIdx = 0;

    /* Initialize internal state variables. */

    isNonNum = AW_FALSE;
    isOverflow = AW_FALSE;

    /* Convert all digits in the number string to appropriate values as
       long as there are no errors detected. */

    while ((status == AW_OK) &&
           (!isOverflow ) &&
           (!isNonNum ))
    {
        /* Process the character and retrieve its value. */

        status = STRTOX__Process_Ch(str[strIdx],
                                    base,
                                    &chVal,
                                    &isNonNum);

        /* Continue processing only if there is no error detected. */

        if ((status == AW_OK) &&
            (!isNonNum ))
        {
            /* Save a temporary copy of the of the current result. */

            tempResult = result;

            /* Update cumulative result value with current character's
               value by shifting the previous result one column to the
               left (multiply by base) and adding the current character's
               value in the 1's column. */

            result = ((base * result) + chVal);

            if ((tempResult & 0xfe000000) &&
                (((result / base) != tempResult) ||
                 (result < min) ||
                 (result > max)))
            {
                /* ERROR: Range overflow occurred. */

                isOverflow = AW_TRUE;

                status = STRTOX__Process_To_End(&str[strIdx],
                                                base,
                                                &chCnt);

                if (status == AW_OK)
                {
                    /* Update string index to account for processed
                       characters. */

                    strIdx = (strIdx + chCnt);

                } /* if */

            }
            else
            {
                /* Update the string index to account for processed
                   characters. */

                strIdx++;

            } /* if */

        } /* if */

    } /* while */

    if (status == AW_OK)
    {
        /* Update return parameters. */

        *pResult = result;
        *pChCnt = strIdx;
        *pIsNonNum = isNonNum;
        *pIsOverflow = isOverflow;

    } /* if */

    return (status);
}

/******************************************************************************/
static long long STRTOX__Str_To_Int(const char      str[],
                                    int             base,
                                    long long       min,
                                    long long       max,
                                    uint32_t *      pChCnt)
{
    aw_err_t              status;
    long long           result;
    unsigned long long  ullResult;
    aw_bool_t             isNeg;
    aw_bool_t             isInvalid;
    aw_bool_t             isNonNum;
    aw_bool_t             isOverflow;
    uint32_t              strIdx;
    uint32_t              chCnt;

    /* Initialize cumulative result value. */

    result = 0;

    /* Initialize the string counter. */

    strIdx = 0;

    /* Initialize internal state variables. */

    isNeg = AW_FALSE;
    isInvalid = AW_FALSE;
    isNonNum = AW_FALSE;
    isOverflow = AW_FALSE;

    /* Parse the prefix of the string to determine the base and
       (optionally) the sign of the number string.  The function will also
       indicate if the number is negative and/or invalid. */

    status = STRTOX__Process_Prefix(&str[strIdx],
                                    base,
                                    &chCnt,
                                    &base,
                                    &isNeg,
                                    &isInvalid);

    if (status == AW_OK)
    {
       /* Update the current string counter with the number of characters
          processed by the last function call. */

        strIdx = (strIdx + chCnt);

        /* Process the string. */

        status = STRTOX__Process_Str(&str[strIdx],
                                     base,
                                     0,
                                     max,
                                     &ullResult,
                                     &chCnt,
                                     &isNonNum,
                                     &isOverflow);

    } /* if */

    if (status == AW_OK)
    {
        /* Handle situation where no conversion could be performed due
           to an invalid parameter (str or base) or invalid subject
           sequence (no characters processed). */

        if ((isInvalid ) ||
            (chCnt == 0))
        {
            /* Update the string index to indicate no characters have been
               processed (index is reset to zero).  This will cause the
               end-pointer to be updated to the start of the string. */

            strIdx = 0;

            /* Update the (returned) result. */

            result = 0;

            /* Update errno value. */

            errno = -AW_EINVAL;

        }
        else
        {
            /* Update the current string counter with the number of characters
               processed by the last function call. */

            strIdx = (strIdx + chCnt);

            /* Determine if an overflow occurred and update the result value
               accordingly. */

            if (isOverflow )
            {
                /* Update the ERRNO value. */

                errno = ERANGE;

                /* Update the return value. */

                if (isNeg )
                {
                    result = min;

                }
                else
                {
                    result = max;

                } /* if */

            }
            else
            {
                /* Convert the result to a long long value (should be in the
                   proper range!). */

                result = ullResult;

            } /* if */

            /* Negate the resulting value if the processing ended normally (no
               errors detected). */

            if ((isNonNum ) &&
                (isNeg ))
            {
                result = (-1 * result);

            } /* if */

        } /* if */

        /* Update return parameters */

        *pChCnt = strIdx;

    } /* if */

    return (result);
}

/******************************************************************************/
static unsigned long long STRTOX__Str_To_Uint(const char            str[],
                                              int                   base,
                                              unsigned long long    min,
                                              unsigned long long    max,
                                              uint32_t *            pChCnt)
{
    aw_err_t              status;
    unsigned long long  result;
    unsigned long long  ullResult;
    aw_bool_t             isNeg;
    aw_bool_t             isInvalid;
    aw_bool_t             isNonNum;
    aw_bool_t             isOverflow;
    uint32_t              strIdx;
    uint32_t              chCnt;

    /* Check current stack validity. */



    /* Initialize cumulative result value. */

    result = 0;

    /* Initialize the string index. */

    strIdx = 0;

    /* Initialize internal state variables. */

    isNeg = AW_FALSE;
    isInvalid = AW_FALSE;
    isNonNum = AW_FALSE;
    isOverflow = AW_FALSE;

    /* Parse the prefix of the string to determine the base and
       (optionally) the sign of the number string.  The function will also
       indicate if the number is negative and/or invalid. */

    status = STRTOX__Process_Prefix(&str[strIdx],
                                    base,
                                    &chCnt,
                                    &base,
                                    &isNeg,
                                    &isInvalid);

    if ((status == AW_OK) &&
        (isNeg ))
    {
        /* ERROR: String prefix indicates a negative number. */

        isInvalid = AW_TRUE;

    } /* if */

    if (status == AW_OK)
    {
       /* Update the current string counter with the number of characters
          processed by the last function call. */

        strIdx = (strIdx + chCnt);

        /* Process the string. */

        status = STRTOX__Process_Str(&str[strIdx],
                                     base,
                                     0,
                                     max,
                                     &ullResult,
                                     &chCnt,
                                     &isNonNum,
                                     &isOverflow);

    } /* if */

    if (status == AW_OK)
    {
       /* Update the current string counter with the number of characters
          processed by the last function call. */

        strIdx = (strIdx + chCnt);

        /* Determine if an overflow occurred and update the result value
           accordingly. */

        if (isOverflow )
        {
            /* Update the ERRNO value. */

            errno = ERANGE;

            /* Update the return value. */

            result = max;

        }
        else
        {
            /* Convert the result to a long long value (should be in the
               proper range!). */

            result = ullResult;

        } /* if */

        /* Handle situation where no conversion could be performed due to
           an invalid parameter (str or base) or invalid subject
           sequence (no characters processed). */

        if ((isInvalid ) ||
            (strIdx == 0))
        {
            /* Update the string index to indicate no characters have been
               processed (index is reset to zero).  This will cause the
               end-pointer to be updated to the start of the string. */

            strIdx = 0;

            /* Update the (returned) result. */

            result = 0;

            /* Update errno value. */

            errno = -AW_EINVAL;

        } /* if */

        /* Update return parameters */

        *pChCnt = strIdx;

    } /* if */

    return (result);
}


/******************************************************************************/
static long double STRTOX__Str_To_Flt(const char    str[],
                                      uint32_t *    pChCnt)
{
    aw_err_t          status;
    int             chVal;
    int             implExp;
    long long       iNum;
    long            iNumHigh;
    int             digitCnt;
    long double     fNum;
    int             mantBase;
    int             expBase;
    aw_bool_t         isMantSignNeg;
    aw_bool_t         isPastDecPnt;
    aw_bool_t         isInvalid;
    aw_bool_t         isMantEnd;
    aw_bool_t         isNonNum;
    uint32_t          strIdx;
    uint32_t          chCnt;

    /* Set initial function status. */

    status = AW_OK;

    /* Initialize the string index. */

    strIdx = 0;

    /* Set initial numeric variable values. */

    implExp = 0;
    iNum = 0L;
    iNumHigh = 0L;
    digitCnt = 0;
    mantBase = 0;
    expBase = 0;

    /* Set initial state variable values. */

    isMantSignNeg = AW_FALSE;
    isPastDecPnt = AW_FALSE;
    isInvalid = AW_FALSE;
    isMantEnd = AW_FALSE;
    isNonNum = AW_FALSE;

    /* Process the mantissa string prefix.  This includes moving past
       any preceding white spaces, determining the sign and base of
       the mantissa. */

    status = STRTOX__Process_Prefix(&str[strIdx],
                                    mantBase,
                                    &chCnt,
                                    &mantBase,
                                    &isMantSignNeg,
                                    &isInvalid);

    if ((status == AW_OK) &&
        (isInvalid ))
    {
        /* ERROR: Invalid character detected in mantissa prefix. */

        status = -AW_EPERM;

    } /* if */

    if (status == AW_OK)
    {
       /* Update the current string counter with the number of characters
          processed by the last function call. */

        strIdx = (strIdx + chCnt);

        /* Intercept the situation where the string is of the form "0.x"
           and should be considered decimal, but the prefix parsing code
           considers this octal due to the leading "0" character... */

        if (mantBase == 8)
        {
            mantBase = 10;

        } /* if */

        /* Ensure that the subject sequence mantissa base is valid and
           supported (only decimal).  Update the exponent base. */

        if (mantBase == 10)
        {
            expBase = 10;

        }
        else
        {
            /* ERROR: Invalid or unsupported mantissa base. */

            status = -AW_EPERM;

        } /* if */

    } /* if */

    /* Process characters in the mantissa string. */

    while ((status == AW_OK) &&
           (!isMantEnd ))
    {
        /* Examine the current character validity (determined by the
           base of the mantissa) and whether it is a number or not. */

        status = STRTOX__Process_Ch(str[strIdx],
                                    mantBase,
                                    &chVal,
                                    &isNonNum);

        if (status == AW_OK)
        {
            /* Check for an invalid (by base) or non-number character. */

            if (isNonNum )
            {
                /*  Check if the decimal point has already been
                    encountered or that the current character is not a
                    decimal point. */

                if ((isPastDecPnt ) || (str[strIdx] != '.'))
                {
                    /* A non-digit, non-decimal point character has been
                       encountered, possibly the NULL-terminator of the
                       string or possibly the exponent symbol... either
                       way the mantissa string is concluded. */

                    isMantEnd = AW_TRUE;

                }
                else
                {
                    isPastDecPnt = AW_TRUE;

                    /* Move to the next character in the mantissa
                       string. */

                    ++strIdx;

                } /* if */

            }
            else
            {
                /* Check if mantissa has out-grown the bounds of the
                   integer representation and the high order number is in
                   use. */

                if ((iNum >= STRTOX_IMPL_EXP_MAX) &&
                    (iNumHigh > 0))
                {
                    /* Increment the implicit exponent. */

                    ++implExp;

                }
                else
                {
                    /* Check if the current integer representation has out
                       -grown the maximum implicit exponent size. */

                    if (iNum >= STRTOX_IMPL_EXP_MAX)
                    {
                        /* The integer representation of the mantissa has
                           become too large (larger than the implicit
                           exponent can handle) so transfer the current
                           integer number, which represents the high-order
                           digits of the mantissa, into another variable
                           and reset the integer number accumulator
                           variable so that the process may continue. */

                        iNumHigh = iNum;
                        iNum = 0L;

                        /* Initialize the digit count. */

                        digitCnt = 0;

                    } /* if */

                    /* Add in this mantissa character value to the total
                       mantissa value. */

                    iNum = ((iNum * mantBase) + chVal);

                    /* Increment the digit count. */

                    ++digitCnt;

                } /* if */

                /* If the decimal point has already been encountered then
                   reduce the implicit exponent. */

                if  (isPastDecPnt )
                {
                    --implExp;

                } /* if */

                /* Move to the next character in the mantissa string. */

                ++strIdx;

            } /* if */

        } /* if */

    } /* while */

    if (status == AW_OK)
    {
        /* Check for optional exponent part. */

        if ((str[strIdx] == 'e') ||
            (str[strIdx] == 'E') ||
            (str[strIdx] == '^'))
        {
            int         explExp;
            aw_bool_t     isExpSignNeg;

            /* Initialize explicit exponent value. */

            explExp = 0;

            /* Move past the exponent indicator character. */

            ++strIdx;

            /* Determine the exponent sign. */

            status = STRTOX__Str_Is_Neg(&str[strIdx],
                                        &chCnt,
                                        &isExpSignNeg);

            if (status == AW_OK)
            {
               /* Update the current string counter with the number of characters
                  processed by the last function call. */

                strIdx = (strIdx + chCnt);

                /* Reset parsing state flags. */

                isNonNum = AW_FALSE;

                /* Parse the (explicit) exponent string. */

                while ((status == AW_OK) &&
                       (!isNonNum ))
                {
                    /* Examine the current character validity (determined
                       by the base of the exponent) and whether it is a
                       number or not. */

                    status = STRTOX__Process_Ch(str[strIdx],
                                                expBase,
                                                &chVal,
                                                &isNonNum);

                    if ((status == AW_OK) &&
                        (!isNonNum ))
                    {
                        /* Add the character value to the total value for
                           the explicit exponent as long as the value is
                           within the limits. */

                        if (explExp < STRTOX_EXPL_EXP_MAX)
                        {
                            explExp = ((explExp * expBase) + chVal);

                        } /* if */

                        /* Move to the next character. */

                        ++strIdx;

                    } /* if */

                } /* while */

            } /* if */

            if (status == AW_OK)
            {
                /* Integrate the explicit exponent value into the implicit
                   exponent value taking the sign of the explicit exponent
                   into account. */

                if (isExpSignNeg )
                {
                    implExp -= explExp;

                }
                else
                {
                    implExp += explExp;

                } /* if */

            } /* if */

        } /* if */

    } /* if */

    if (status == AW_OK)
    {
        /* Convert the number from integer to floating point. */

        fNum = iNum;

        /* Determine if any portion of the number was stored in the high
           -order digits variable. */

        if (iNumHigh > 0)
        {
            /* Calculate the value of the stored high-order digits of the
               number and add their value to the floating point
               representation of the number. */

            fNum += STRTOX__Exp10((long double)iNumHigh,
                                   digitCnt,
                                   &isInvalid);

            if (isInvalid )
            {
                status = -AW_EPERM;

            } /* if */

        } /* if */

    } /* if */

    if (status == AW_OK)
    {
        fNum = STRTOX__Exp10(fNum,
                             implExp,
                             &isInvalid);

        if (isInvalid )
        {
            status = -AW_EPERM;

        } /* if */

    } /* if */

    if (status == AW_OK)
    {
        /* Account for the sign of the mantissa. */

        if (isMantSignNeg )
        {
            fNum = -fNum;

        } /* if */

        /* Update return parameters. */

        *pChCnt = strIdx;

    } /* if */

    /* Intercept and handle any internal errors. */

    if (status != AW_OK)
    {
        /* An internal error has occurred and so no conversion can be
           performed.  In this situation errno will be updated
           appropriately and zero (0) will be returned. */

        errno = -AW_EINVAL;
        fNum = 0;

        /* Update return parameters. */

        *pChCnt = 0;

    } /* if */

    return (fNum);
}

/* Global functions */

/******************************************************************************/
long strtol(const char *str, char **endptr, int base)
{
    long        result;
    uint32_t      chCnt;



    /* Check current stack validity. */





    /* Call internal function to perform conversion. */

    result = (long)STRTOX__Str_To_Int(str,
                                      base,
                                      LONG_MIN,
                                      LONG_MAX,
                                      &chCnt);

    /* Conditionally update return parameters. */

    if (endptr != NULL)
    {
        *endptr = (char *)&str[chCnt];

    } /* if */



    return (result);
}

/******************************************************************************/
long long strtoll(const char *str, char **endptr, int base)
{
    long long       result;
    uint32_t          chCnt;



    /* Check current stack validity. */





    /* Call internal function to perform conversion. */

    result = STRTOX__Str_To_Int(str,
                                base,
                                LLONG_MIN,
                                LLONG_MAX,
                                &chCnt);

    /* Conditionally update return parameters. */

    if (endptr != NULL)
    {
        *endptr = (char *)&str[chCnt];

    } /* if */



    return (result);
}

/******************************************************************************/
unsigned long strtoul(const char *str, char **endptr, int base)
{
    unsigned long       result;
    uint32_t              chCnt;



    /* Check current stack validity. */





    /* Call internal function. */

    result = (unsigned long)STRTOX__Str_To_Uint(str,
                                                base,
                                                0,
                                                ULLONG_MAX,
                                                &chCnt);

    /* Conditionally update return parameters. */

    if (endptr != NULL)
    {
        *endptr = (char *)&str[chCnt];

    } /* if */



    return (result);
}


/******************************************************************************/
unsigned long long strtoull(const char *str, char **endptr, int base)
{
    unsigned long long  result;
    uint32_t              chCnt;



    /* Check current stack validity. */





    /* Call internal function. */

    result = STRTOX__Str_To_Uint(str,
                                 base,
                                 0,
                                 ULLONG_MAX,
                                 &chCnt);

    /* Conditionally update return parameters. */

    if (endptr != NULL)
    {
        *endptr = (char *)&str[chCnt];

    } /* if */



    return (result);
}

/******************************************************************************/
intmax_t strtoimax(const char *nptr, char **endptr, int base)
{
    intmax_t            result;



    /* Check current stack validity. */





    /* The intmax_t is a long long integer in the Nucleus POSIX
       implementation so a call to strtoll() is appropriate. */

    result = (intmax_t)strtoll(nptr, endptr, base);



    return (result);
}

/******************************************************************************/
uintmax_t strtoumax(const char *nptr, char **endptr, int base)
{
    uintmax_t       result;



    /* Check current stack validity. */





    /* The uintmax_t is a long long integer in the Nucleus POSIX
       implementation so a call to strtoull() is appropriate. */

    result = (uintmax_t)strtoull(nptr, endptr, base);



    return (result);
}

/******************************************************************************/
float strtof(const char *nptr, char **endptr)
{
    float       fNum;
    uint32_t      chCnt;



    /* Check current stack validity. */





    /* Call internal function to perform conversion. */

    fNum = (float)STRTOX__Str_To_Flt(nptr,
                                     &chCnt);

    /* Conditionally update return parameters. */

    if (endptr != NULL)
    {
        *endptr = (char *)&nptr[chCnt];

    } /* if */



    return (fNum);
}


/******************************************************************************/
double strtod(const char *nptr, char **endptr)
{
    double      fNum;
    uint32_t      chCnt;



    /* Check current stack validity. */





    /* Call internal function to perform conversion. */

    fNum = (double)STRTOX__Str_To_Flt(nptr,
                                      &chCnt);

    /* Conditionally update return parameters. */

    if (endptr != NULL)
    {
        *endptr = (char *)&nptr[chCnt];

    } /* if */



    return (fNum);
}


/******************************************************************************/
long double strtold(const char *nptr, char **endptr)
{
    long double     fNum;
    uint32_t          chCnt;



    /* Check current stack validity. */





    /* Call internal function to perform conversion. */

    fNum = STRTOX__Str_To_Flt(nptr,
                              &chCnt);

    /* Conditionally update return parameters. */

    if (endptr != NULL)
    {
        *endptr = (char *)&nptr[chCnt];

    } /* if */



    return (fNum);
}

/******************************************************************************/
int atoi(const char *str)
{
    int  result;



    /* Check current stack validity. */





    /* The atoi() function is a limited version of the strtol()
       function, as defined by the POSIX specification. */

    result = (int)strtol(str, NULL, 10);



    return(result);
}


/******************************************************************************/
long atol(const char *str)
{
    long  result;



    /* Check current stack validity. */





    /* The atol() function is a limited version of the strtol()
       function, as defined by the POSIX specification. */

    result = strtol(str, NULL, 10);



    return(result);
}


/******************************************************************************/
long long atoll(const char *nptr)
{
    long long   result;



    /* Check current stack validity. */





    /* The atoll() function is a limited version of the strtoll()
       function, as defined by the POSIX specification. */

    result = strtoll(nptr, NULL, 10);



    return(result);
}


/******************************************************************************/
double atof(const char *str)
{
    double     result;



    /* Check current stack validity. */





    /* The atof() function is a limited version of the strtod()
       function, as defined by the POSIX specification. */

    result = strtod(str, NULL);



    return (result);
}

/* end of file */
