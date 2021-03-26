/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ini config file
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-31  imp, add some interface.
 * - 1.00 16-10-20  imp, refactoring code, use htab interface.
 *                       add aw_ini_size() implementation.
 * - 1.00 15-11-27  deo, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_errno.h"
#include "aw_assert.h"
#include "aw_list.h"
#include "string.h"
#include "aw_ini_lib.h"

#if      __INI_DEBUG  ==  1
#define  __DEBUG(x)          AW_DBGF(__FILE__" "x)
#define __INFO(format, ...)  AW_DBGF("\n\t*****%s*****\t"format"\n\n", \
                                     __FUNCTION__, ##__VA_ARGS__)
#define __TEST(format, ...)  AW_DBGF("TEST: %u\t"format"\n", \
                                    __LINE__, ##__VA_ARGS__)

#define __STATUS(format, ...)  AW_DBGF("TEST: %u\t"format"\n", \
                             __LINE__, ##__VA_ARGS__)
#else
#define  __DEBUG(x)            (void)0
#define  __INFO(format, ...)   (void)0
#define  __TEST(format, ...)   (void)0
#define __STATUS(format, ...)  (void)0
#endif

/*******************************************************************************
 ** ENUM TYPE
 ******************************************************************************/
/**
 *  \brief The type of the ini file key-string matching state machine.
 *
 *  The status indicates what the matching state machine are being on.
 */
enum __status_t {
    __MATCH_FAIL,      /**< \brief The start status  */
    __MATCH_SECT_START,/**< \brief The section start '[' matching status*/
    __MATCH_SECT_END,  /**< \brief The section end ']' matching status*/
    __MATCH_KEY_SKIP,  /**< \brief Skip the surplus symbol */
    __MATCH_KEY_START, /**< \brief The key start '\n'  matching status */
    __MATCH_KEY_END,   /**< \brief The key end '=' matching status */
    __MATCH_END,       /**< \brief The end symbol '\n''\r\n' matching status */
};

/**
 * \brief The type of symbols appeared in the ini-file.
 */
enum __symbol_t {
    __DIVI_SYM,   /**< \brief '\r','\n', ';'  */
    __START_SYM,  /**< \brief '\nKEY'.  */
    __NORM_SYM,   /**< \brief 'A','a'.  */
};




/******************************************************************************/
static void __brk_map (char *p_file, uint32_t len)
{

    char *p_char = p_file;
    char *p_end  = p_char + len;

    aw_assert(p_char < p_end);

    for(; p_char < p_end; p_char++) {

        if(('\r' == *p_char) ||
           ('\n' == *p_char) ||
           (' '  == *p_char)) {
            *p_char = '\0';
        }
    }
}


/******************************************************************************/
/*  return the number which has matched. */
int __strnmatch(const char *src, const char *dest, unsigned int len)
{

    aw_assert(src), aw_assert(dest);

    int dismatch_len = len;

    for(; dismatch_len>0; dismatch_len--) {
        if(*src == *dest) {
            src++,dest++;
            continue;

        } else {
            break;
        }
    }
    return len - dismatch_len;
}

/******************************************************************************/
int __end_symbol (char  *p_char, char  *p_end)
{
    aw_assert(p_char), aw_assert(p_end);
    aw_assert(p_end>p_char);

    if('\n' == *p_char ||
       '\0' == *p_char) {
        /* Skip the comment. */
        if (';'  == *(p_char + 1)) {
            return __DIVI_SYM;

        /* Skip the continuous '\r' or '\n'. */
        } else if ('\n' == *(p_char + 1) ||
                   '\r' == *(p_char + 1) ||
                   '\0' == *(p_char + 1)) {

            return __DIVI_SYM;

        } else if('=' == *(p_char + 1)) {
            return __DIVI_SYM;
        }
        /* Skip the eof. */
        if((p_end - p_char) <= 1) {
            return __DIVI_SYM;
        }
        return __START_SYM;
    }
    return __NORM_SYM;
}









/******************************************************************************/
char *aw_ini_read_file_key (char            *p_file,
                            unsigned int     len,
                            const char      *section,
                            const char      *key)
{
    char              *p_char     = NULL;
    char              *p_end      = NULL;
    char              *p_tmp      = NULL;
    enum  __status_t   status     = __MATCH_FAIL;
    unsigned int       match_len  = 0;
    unsigned int       sect_len   = 0;
    unsigned int       key_len    = 0;

    if ((p_file == NULL) ||
        (len == 0) ||
        (section == NULL) ||
        (key == NULL)) {
        return NULL;
    }

    sect_len = strlen(section);
    key_len  = strlen(key);

    if((!sect_len) || (!key_len)) {
        return NULL;
    }

    p_char = p_file;
    p_end  = p_char + len;

    __brk_map(p_file, len);

    for(; p_char < p_end; p_char++) {

        /* when you matching a '[', it means entering a new section */
        if('[' == *p_char) {
            /* It should not step into another section. */
            if(__MATCH_KEY_START == status) {
                return NULL;
            }
            status = __MATCH_SECT_START;
            continue;
        }

        switch(status) {
            /* It has not met any '[' yet. */
            case __MATCH_FAIL : {
                __STATUS("__MATCH_FAIL");
                break;
            }

            /* Find out the start of the section. */
            case __MATCH_SECT_START : {
                __STATUS("__MATCH_SECT_START");
                /* It has optimized here.Use __strnmatch instead of strncmp.*/
                match_len = __strnmatch(p_char, section, sect_len);

                if(match_len == sect_len) {
                    aw_assert(match_len);

                    p_char += match_len - 1;
                    status  = __MATCH_SECT_END;

                } else {
                    p_char += match_len ;
                    status  = __MATCH_FAIL;
                }
                break;
            }

            /* Make sure you match the whole word, not a prefix. */
            case __MATCH_SECT_END : {
                __STATUS("__MATCH_SECT_END");
                if(']' == *p_char) {
                    status = __MATCH_KEY_SKIP;
                } else {
                    status = __MATCH_FAIL;
                }
                break;
            }

            case __MATCH_KEY_SKIP : {
                __STATUS("__MATCH_KEY_SKIP");
 
                if(__end_symbol(p_char, p_end) == __START_SYM) {
                    status = __MATCH_KEY_START;
                }
                break;
            }
            
            case __MATCH_KEY_START : {
                __STATUS("__MATCH_KEY_START");

                    match_len =  __strnmatch(p_char, key, key_len);

                    if(match_len == key_len) { 
                        aw_assert(match_len);

                        p_char += match_len - 1;
                        status  = __MATCH_KEY_END;
                    } else {
                        p_char += match_len;
                        status  = __MATCH_KEY_SKIP;
                    }

                break;
            }

            /*  Make sure it is not a prefix also. */
            case __MATCH_KEY_END : {
                __STATUS("__MATCH_KEY_END");
                if('=' == *p_char ) {

                    if('\0' == (*p_char+1)
                    || '\r' == (*p_char+1)
                    || '\n' == (*p_char+1)) {
                        return NULL;
                    }

                    p_tmp = ++p_char;

                    status  = __MATCH_END;
                } else {
                    status  = __MATCH_KEY_SKIP;
                }
                break;
            }

            case __MATCH_END : {
                __STATUS("__MATCH_END");

                if('\0' == *p_char
                || '\r' == *p_char
                || '\n' == *p_char) {
                    return p_tmp;
                }                 
                break;
            }
            default : {
                aw_assert(0);
                return NULL;
            }
        }
    }
    __STATUS("__MATCH_FAIL");
    return NULL;
}

