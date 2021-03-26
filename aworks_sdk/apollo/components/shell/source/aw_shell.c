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
 * \brief A simple shell.
 *
 * \internal
 * \par History
 * - 1.07 17-06-19, vih, �������������tab��ȫ
 * - 1.06 16-01-07, deo, remove aw_fs_state_t
 * - 1.05 15-12-03, dcf, add aw_shell_printf
 * - 1.04 15-04-28, rnk, Rename files and support multiuser
 * - 1.03 12-07-15, orz, Modify for iron os.
 * - 1.02 11-10-10, orz, Add support for no OS application.
 * - 1.01 11-07-17, orz, Fix _run_as_daemon().
 * - 1.00 11-03-22, orz, First implementation.
 * @endinternal
 */

#include "apollo.h"

#include "aw_errno.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_mem.h"

#include "aw_shell_core.h"

#include "io/aw_unistd.h"
#include "io/aw_dirent.h"

#include "io/sys/aw_stat.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/******************************************************************************/
#define _SHELL_DEBUG
/******************************************************************************/
#if  defined(_SHELL_DEBUG)
#define _kprintf                          aw_kprintf

#define _ERR_DMSG(msg)  \
        aw_shell_color_set(AW_DBG_SHELL_IO, AW_SHELL_FCOLOR_RED);\
        _kprintf( "\nSSH MSG(%d): ", __LINE__);\
        aw_shell_color_set(AW_DBG_SHELL_IO, AW_SHELL_FCOLOR_DEFAULT);\
        _kprintf msg;


#else
#define _kprintf
#define _ERR_DMSG(msg)
#endif
/******************************************************************************/
#ifndef count_of
#define count_of(a)         (sizeof(a) / sizeof((a)[0]))
#endif
#define skip_spaces(ptr)    while ((int)*(ptr) == ' ' || (int)*(ptr) == '\t') { (ptr)++; }

#define __PATH_MAX_LEN   AW_IO_PATH_LEN_MAX
//#define _cursor_mover(p_user, ofs)  aw_shell_printf(p_user, "\033[%dC", ofs)
//#define _cursor_movel(p_user, ofs)  aw_shell_printf(p_user, "\033[%dD", ofs)
/******************************************************************************/
/* ǰ������ */
static struct aw_shell_cmdset  _g_cmdset_list_local;
static struct aw_shell_cmd_list gcmd_list_local;

struct _shell_comp {
    struct _shell_comp *next;
    const char        **start;
    const char        **end;
};
static struct _shell_comp _g_local_comp;

struct _key_info {
    const char     *p_key;
    uint8_t         key_len;

    int            (*pfn_do) (struct aw_shell_context *sc,
                              struct aw_shell_user    *user);
};
static struct _key_info _g_key_tab[];
static uint8_t _g_key_tab_num;

typedef enum {
    STAT_INIT,
    STAT_IDLE,
    STAT_RECEIVING,
    STAT_RUNING
} server_stat_t;
/******************************************************************************/
struct __alias_info {
    struct aw_list_head    node;
    char                  *p_str;
    char                  *p_name;

};

/******************************************************************************/
struct _str_complete_info ;

static int help (int argc, char *argv[], struct aw_shell_user *p_user);

aw_local void _help_scan (struct aw_shell_context *sc,
                          struct aw_shell_user    *user);

#ifdef AW_SHELL_USE_HISTORY
aw_local int _history_get (struct aw_shell_history *p_this,
                           uint8_t                  *p_to,
                           uint32_t                   index);
aw_local int _history_add (struct aw_shell_history *p_this, uint8_t *p_from);
#endif

static void _cmd_help_print (const char                *p_name,
                             const char                *p_desc,
                             struct aw_shell_user      *p_user);

aw_local int _shell_complete (struct aw_shell_context *sc,
                              struct aw_shell_user    *user);
static void _escape_recovery (char *src, char ch, int max_size);
aw_local int _cmdset_complete (struct aw_shell_cmdset    *cmdset,
                               struct _str_complete_info *p_info);

aw_local int _cmdset_complete (struct aw_shell_cmdset    *cmdset,
                               struct _str_complete_info *p_info);

static aw_bool_t _is_cmdset_exit (
        struct aw_shell_cmdset   *cmdset,
        const char                *exit);

static const struct aw_shell_cmd *_cmdset_cmd_find (
        struct aw_shell_cmdset   *cmdset,
        const char                *name,
        const char               **cmd_arg);

static struct aw_shell_cmdset * _cmdset_find (const char *name);

aw_err_t aw_shell_unregister_cmdset(struct aw_shell_cmdset *cmdset);

aw_err_t aw_shell_register_cmdset( struct aw_shell_cmdset      *cmdset,
                                    const struct aw_shell_cmd   *start,
                                    const struct aw_shell_cmd   *end);

aw_local int _shell_cmd_gets (struct aw_shell_context *sc,
                              struct aw_shell_user *p_user);

static int __shell_alias_find (struct aw_shell_user *sh,
                               const char *p_name,
                               char       *p_to,
                               uint32_t    to_size);
static int __shell_alias_del (struct aw_shell_user *sh, const char *p_name);

#ifdef  AW_SHELL_USE_HISTORY
aw_local aw_err_t _shell_history_init (struct aw_shell_user *sh,
                                       char                 *p_mem,
                                       uint32_t              mem_size);
static int _puts( const char *str, void *f );
static int _putc( char c, void *f );
#endif /*  AW_SHELL_USE_HISTORY */
/******************************************************************************/
static void __shell_puts (aw_shell_user_t *p_user, const char *fmt, ... )
{
    va_list ap;
    int     len;
    (void)len;

    AW_MUTEX_LOCK(p_user->mutex, AW_SEM_WAIT_FOREVER);
    va_start( ap, fmt );
    len = aw_vfprintf_do(p_user, _putc, _puts, fmt, ap );
    va_end( ap );
    AW_MUTEX_UNLOCK(p_user->mutex);
}

/* ����ӹ�괦����ĩ������ */
static void _cursor_pos_clr (struct aw_shell_user *p_user)
{
    p_user->io_puts(p_user, "\x1B[K");
}

static void _cursor_up (struct aw_shell_user *p_user, int len)
{
    if (len) {
        __shell_puts(p_user, "\x1B[%dA", len);
    }
}

static void _cursor_right (struct aw_shell_user *p_user, int len)
{
    if (len) {
        __shell_puts(p_user, "\x1B[%dC", len);
    }
}

static void _cursor_left (struct aw_shell_user *p_user, int len)
{
    if (len) {
        if (p_user->sc->p_opt->use_escape_seq && p_user->sc->p_opt->use_cursor) {
            __shell_puts(p_user, "\x1B[%dD", len);
        }
    }
}

static void _cursor_down (struct aw_shell_user *p_user, int len)
{
    if (len) {
        __shell_puts(p_user, "\x1B[%dB", len);
    }
}

static void _cursor_back_to_head (struct aw_shell_context *sc,
                                  struct aw_shell_user    *p_user)
{
    int len = sc->cursor_pos;

    if (p_user->sc->p_opt->use_escape_seq && p_user->sc->p_opt->use_cursor) {
        _cursor_left(p_user, len);
        _cursor_pos_clr(p_user);
    } else {
        while (len > 0) {
            __shell_puts(p_user, "\b \b");
            len--;
        }
    }
}

static void _line_echo_clear (struct aw_shell_context *sc,
                              struct aw_shell_user    *p_user)
{
    if (p_user->sc->p_opt->use_escape_seq) {
        p_user->io_putc(p_user, '\r');

        _cursor_pos_clr(p_user);
        sc->des_exist = AW_FALSE;
    } else {
        _cursor_back_to_head(sc, p_user);
    }

    if (!sc->des_exist ) {
        aw_shell_color_set(p_user, sc->des_color);
        p_user->io_puts(p_user, sc->des);
        aw_shell_color_set(p_user, AW_SHELL_FCOLOR_DEFAULT);
        sc->des_exist = AW_TRUE;
    }
}

/******************************************************************************/
/* memory copy, can do memory left shift */
static void _mem_cpyl (char *dst, const char *src, int len)
{
    while (len-- > 0) {
        *dst++ = *src++;
    }
}

/* memory copy, can do memory right shift */
static void _mem_cpyr (char *dst, const char *src, int len)
{
    dst += len - 1;
    src += len - 1;
    while (len-- > 0) {
        *dst-- = *src--;
    }
}

/* memory insert data */
static void _mem_cpyi (char       *dst,
                       int         dst_len,
                       const char *insert,
                       int         insert_len,
                       int         offs)
{
    int shift_size = dst_len - offs;

    if (insert_len == 0) {
        return;
    }
    _mem_cpyr(dst + offs + insert_len, dst + offs, shift_size);
    memcpy(dst + offs, insert, insert_len);
}

/******************************************************************************/

/* �ж��ַ�����ʲô���� */
aw_local aw_bool_t _str_key_is (const char *p_key, uint8_t key_len, const char *p_src)
{
    if (key_len == 1) {
        uint8_t  i  = 0;
        uint32_t t  = (uint32_t)p_key;
        char     ch = (char)t;

        while (p_src[i] != 0) {
            if (ch == p_src[i++]) { //(uint8_t)p_key
                return AW_TRUE;
            }
        }
    } else {
        if (strstr(p_src, p_key)) {
            return AW_TRUE;
        }
    }
    return AW_FALSE;
}

/******************************************************************************/
static const struct aw_shell_cmd * _cmd_find (const char  *name,
                                              const char **cmd_arg)
{
    struct aw_shell_cmd_list  *list     = &gcmd_list_local;
    const struct aw_shell_cmd *cmd;
    int                        len;
    const char                *args;

    while (list != NULL) {
        for (cmd = list->start; cmd < list->end; cmd++) {
            len = strlen(cmd->name);
            if (strncmp(name, cmd->name, len) != 0) {  /* CMD name match ?*/
                continue;
            }
            args = name + len; /* Get args */
            if (' ' != *args && '\0' != *args) { /* valid command ? */
                continue;
            }
            skip_spaces(args); /* Skip leading spaces */
            *cmd_arg = args;
            return cmd;
        }
        list = list->next;
    }

    return NULL;
}

/******************************************************************************/
static aw_err_t __cmd_alias_find (struct aw_shell_user *p_user,  char *name)
{
    struct aw_list_head       *list     = &p_user->sc->alias_list;
    struct __alias_info       *p_pos;
    const char                *args;
    char                       buf[AW_SHELL_LINE_SIZE + 1];
    int                        buf_len;
    int                        len;

    aw_list_for_each_entry(p_pos, list, struct __alias_info, node) {
        len = strlen(p_pos->p_name);
        if (strncmp(name, p_pos->p_name, len) != 0) {  /* CMD name match ?*/
            continue;
        }
        args = name + len; /* Get args */
        if (' ' != *args && '\0' != *args) { /* valid command ? */
            continue;
        }

        __shell_alias_find(p_user, p_pos->p_name, buf, AW_SHELL_LINE_SIZE + 1);
        buf_len = strlen(buf);

        if (buf_len + strlen(args) > AW_SHELL_LINE_SIZE) {
            _ERR_DMSG(("string too long. (strlen:%d)", buf_len + strlen(args)));
            return -AW_ENOMEM;
        }

        memcpy(buf + buf_len, args, strlen(args) + 1);
        memcpy(name, buf, strlen(buf) + 1);

        return AW_OK;
    }

    return -AW_ENODATA;
}
/******************************************************************************/
/*
 * ����ת���ַ����ҵ��ַ����е�����"\\x"��
 * ��� "\\x" �ж���ת�壬��"\\"ɾ����ͬʱ��'x'�ı䶨����ַ� ��
 */
static void _escape_process (char *p_src)
{
    int     i       = 0;
    int     len     = strlen((const char *)p_src);
    aw_bool_t  match  = AW_FALSE;

    i = 1;
    while (i < len) {
//        if (p_src[i] != '\\' && p_src[i - 1] == '\\') {
        if (p_src[i - 1] == '\\') {
            switch (p_src[i]) {

            case 'e' :
                p_src[i] = '\x1B'; match = AW_TRUE; break;  /* ���Ʒ� */

            case 'a' :
                p_src[i] = '\a'; match = AW_TRUE; break;    /* ����� */

            case 'b' :
                p_src[i] = '\b'; match = AW_TRUE; break;    /* �˸�� */

            case 'f' :
                p_src[i] = '\f'; match = AW_TRUE; break;    /* ��ҳ�� */

            case 'n' :
                p_src[i] = '\n'; match = AW_TRUE; break;    /* ���з� */

            case 'r' :
                p_src[i] = '\r'; match = AW_TRUE; break;    /* �س��� */

            case 't' :
                p_src[i] = '\t'; match = AW_TRUE; break;    /* �����Ʊ�� */

            case 'v' :
                p_src[i] = '\v'; match = AW_TRUE; break;    /* �����Ʊ�� */

            case '0' :
                p_src[i] = '\0'; match = AW_TRUE; break;

            case '\\' :
                p_src[i] = '\\'; match = AW_TRUE; break;

            case ' ' :
                p_src[i] = ' '; match = AW_TRUE; break;

            case '\"' :
                p_src[i] = '\"'; match = AW_TRUE; break;
            }
            if (match ) {
                _mem_cpyl(p_src + i - 1, p_src + i, len - i + 1);
                match = AW_FALSE;
//                return;
            }
        }
        i++;
    }
}

/*******************************************************************************
  ������
*******************************************************************************/
#ifdef AW_SHELL_USE_CHART

static int _chart_str_echo (struct aw_shell_chart *p_this,
                            uint8_t id,
                            const char *str,
                            int len)
{
    struct aw_shell_user *p_user = p_this->user;
    int i;
    int offs = 0;

    for (i = 0; i < p_this->nitems; i++) {
        if (p_this->v_frame) {
            p_user->io_putc(p_user, p_this->v_frame);
        }
        _cursor_right(p_user, p_this->item_max_len[i]);
    }

    if (p_this->v_frame) {
        p_user->io_putc(p_user, p_this->v_frame);
    }
    p_user->io_putc(p_user, '\r');
    for (i = 0; i < id; i++) {
        if (p_this->v_frame) {
            p_user->io_putc(p_user, p_this->v_frame);
        }
        _cursor_right(p_user, p_this->item_max_len[i]);
    }

    if (p_this->v_frame) {
        p_user->io_putc(p_user, p_this->v_frame);
    }

    /* Ĭ�ϻ���ʶ���ֵ��ʹ������� */
    if (p_this->align_mode == AW_SHELL_CHART_ALIGN_MID) {
        offs    = (p_this->item_max_len[id] - len) >> 1;
    } else if (p_this->align_mode == AW_SHELL_CHART_ALIGN_RIGHT) {
        offs    = p_this->item_max_len[id] - len;
    } else {
        offs    = 0;
    }

    if (offs == 0) {
        offs++;
    }

    _cursor_right(p_user, offs);

    i = 0;
    while (i < len) {
        if (isprint(str[i])) {
            p_this->user->io_putc(p_this->user, str[i]);
        } else {
            p_this->user->io_putc(p_this->user, ' ');
        }
        i++;
    }

    if (p_this->v_frame) {
        i = p_this->item_max_len[id] - offs - len;
        _cursor_right(p_user, i);

        if (p_this->v_frame) {
             p_this->user->io_putc(p_this->user, p_this->v_frame);
        }
    }
    return AW_OK;
}

/* �ڵ�ǰ��괦�����ַ��� */
static int _chart_align_echo (struct aw_shell_chart *p_this,
                              const char            *str,
                              uint8_t                id)
{
    struct aw_shell_user    *p_user         = p_this->user;
    int                      ninlines   = 0;
    int                      maxlen     = 0;
    int                      i          = 0;
    const char              *tmp        = NULL;
    int                      last_space = 0;

    i = 0;
    tmp = str;
    maxlen = p_this->item_max_len[id] - 2;
    while (tmp[i] != 0) {
        tmp         = tmp + i;
        last_space  = 0;
        for (i = 0; i < maxlen; i++) {
            if (tmp[i] == ' ') {
                last_space = i;
            }

            if (tmp[i] == '\n') {
                _chart_str_echo(p_this, id, tmp, i);
                i++;
                break;
            } else if (tmp[i] == '\0') {
                _chart_str_echo(p_this, id, tmp, i);
                break;
            } else if (i == maxlen - 1) {
                if (last_space != 0) {
                    i = last_space;
                }

                _chart_str_echo(p_this, id, tmp, i + 1);

                if (i != maxlen - 1) {
                    i++;
                    break;
                }
            }
        }
        ninlines++;
        p_user->io_puts(p_user, "\r\n");
    }

    if (ninlines > p_this->cur_inline) {
        p_this->cur_inline = ninlines;
    } else {
        while (ninlines < p_this->cur_inline) {
            _chart_str_echo(p_this, id, NULL, 0);
            ninlines++;
            p_user->io_puts(p_user, "\r\n");
        }
    }

    /* �������˵���line����λ */
    _cursor_up(p_user, ninlines);
    return AW_OK;
}

static void _chart_separator_echo (struct aw_shell_chart *p_this,
                                   uint32_t               sep,
                                   const char            *p_context)
{
    uint32_t maxlen;
    uint32_t i;
    uint32_t offs;

    maxlen = p_this->line_max_len;

    /* �ָ��������ݾ��Ǿ��ж��� */
    if (p_context) {
        offs = ( maxlen - strlen(p_context) ) >> 1;

        i = 0;
        while (i++ < offs) {
            p_this->user->io_putc(p_this->user, sep);
        }

        p_this->user->io_puts(p_this->user, p_context);

        i = 0;
        offs = maxlen - strlen(p_context) - offs;
        while (i++ < offs) {
            p_this->user->io_putc(p_this->user, sep);
        }
    } else if (isprint(sep)) {
        i = 0;
        while (i++ < maxlen) {
            p_this->user->io_putc(p_this->user, sep);
        }
    } else {
        return;
    }

    __shell_puts(p_this->user, "\n");
}

/******************************************************************************/

int aw_shell_chart_init (   struct aw_shell_chart           *p_this,
                            struct aw_shell_user            *user,
                            char                             mode,
                            uint32_t                         h_frame,
                            uint32_t                         v_frame)
{
    if (!p_this || !user) {
        return -AW_EINVAL;
    }

    if (!user->sc->p_opt->use_chart ) {
        return -AW_ENOTSUP;
    }

    memset(p_this, 0, sizeof(struct aw_shell_chart));

    p_this->user     = user;
    p_this->h_frame  = h_frame;
    p_this->v_frame  = v_frame;
    p_this->item_sep = h_frame;
    if (mode == 0) {
        p_this->align_mode = AW_SHELL_CHART_ALIGN_LEFT;
    } else {
        p_this->align_mode = mode;
    }

    p_this->cur_line     = -1;

    if (v_frame) {
        p_this->line_max_len = 1;
    }

    p_this->is_default_hframe   = AW_FALSE;
    p_this->wait_use_hframe = AW_FALSE;

    return AW_OK;
}

int aw_shell_chart_item (struct aw_shell_chart            *p_this,
                         const char                       *item,
                         uint8_t                           maxlen)
{
    struct aw_shell_user *p_user = p_this->user;
    int id;
    uint32_t sep = 0;

    if (maxlen < 1 || p_this->nitems > __CHART_ITEM_MAX) {
        return -AW_EINVAL;
    }

    if (!p_user->sc->p_opt->use_chart ) {
        return -AW_ENOTSUP;
    }

    id = p_this->nitems;
    p_this->item_max_len[id] = maxlen + 2;
    p_this->nitems++;
    p_this->line_max_len += maxlen + 2;
    if (p_this->v_frame){
        p_this->line_max_len += 1;
    }

    if (item == NULL) {
        return id;
    }

    sep = p_this->item_sep;
    if (sep) {
        /* ��ӡ��ˮƽ�߿�����ƶ�����һ�� */
        _chart_separator_echo(p_this, sep, p_this->item_sep_context);

        p_this->is_default_hframe = AW_TRUE;
    }

    _chart_align_echo(p_this, item, id);

    if (sep) {
        /* ������ƶ���ˮƽ�߿� */
        _cursor_up(p_user, 1);
    }

    return id;
}

int aw_shell_chart_line (struct aw_shell_chart           *p_this,
                         const  char                     *p_str,
                         uint8_t                          item_id,
                         int                              line)
{
    struct aw_shell_user *p_user = p_this->user;
    int len = 0;

    if (line < p_this->cur_line) {
        return -AW_EINVAL;
    }

    if (!p_user->sc->p_opt->use_chart) {
        return -AW_ENOTSUP;
    }

    if (line != p_this->cur_line) {
        p_this->cur_line = line;

        if (p_this->item_sep) {
            p_this->item_sep = 0;
            _cursor_down(p_user, 1);
        }

        /* ��ʱ���������һ�е���λ�� */
        _cursor_down(p_user, p_this->cur_inline);
        p_this->cur_inline = 0;

        if (p_this->line_sep) {
            p_this->line_sep = 0;
        } else if (p_this->h_frame) {
            len = 0;
            while (len++ < p_this->line_max_len) {
                p_user->io_putc(p_user, p_this->h_frame);
            }
            p_user->io_putc(p_user, '\n');
        }

        if (p_this->wait_use_hframe) {
            p_this->is_default_hframe   = AW_TRUE;
            p_this->wait_use_hframe = AW_FALSE;
        } else if (!p_this->is_default_hframe) {
            p_this->wait_use_hframe = AW_TRUE;
        }
    }

    _chart_align_echo(p_this, p_str, item_id);

    if (p_this->h_frame && p_this->v_frame && p_this->is_default_hframe) {
        int i = 0;

        _cursor_up(p_user, 1);
        for (i = 0; i < item_id; i++) {
            p_user->io_putc(p_user, p_this->v_frame);
            _cursor_right(p_user, p_this->item_max_len[i]);
        }
        p_user->io_putc(p_user, p_this->v_frame);
        _cursor_right(p_user, p_this->item_max_len[i]);
        p_user->io_putc(p_user, p_this->v_frame);
        p_user->io_puts(p_user, "\r\n");
    }

    return AW_OK;
}



void aw_shell_chart_item_separator ( struct aw_shell_chart    *p_this,
                                     uint32_t                   sep,
                                     const char                *p_context)
{
    if (p_this->user->sc->p_opt->use_chart != AW_TRUE) {
        return ;
    }

    p_this->item_sep = sep;
    p_this->item_sep_context = p_context;
}

void aw_shell_chart_line_separator (struct aw_shell_chart         *p_this,
                                    uint32_t                       sep,
                                    const char                    *p_context)
{
    if (!p_this->user->sc->p_opt->use_chart) {
        return ;
    }

    if (p_this->item_sep) {
        p_this->item_sep = 0;
        _cursor_down(p_this->user, 1);
    }

    p_this->line_sep = sep;

    /* ��ʱ���������һ�е���λ�� */
    _cursor_down(p_this->user, p_this->cur_inline);
    p_this->cur_inline = 0;
    _chart_separator_echo(p_this, sep, p_context);

    p_this->is_default_hframe   = AW_FALSE;
    p_this->wait_use_hframe     = AW_FALSE;
}

void aw_shell_chart_finish (struct aw_shell_chart      *p_this,
                            uint32_t                    sep,
                            const char                 *p_context)
{
    if (!p_this->user->sc->p_opt->use_chart) {
        return ;
    }

    /* ��ʱ���������һ�е���λ�� */
    _cursor_down(p_this->user, p_this->cur_inline);
    p_this->cur_inline = 0;

    if (sep) {
        _chart_separator_echo(p_this, sep, p_context);
    } else {
        _chart_separator_echo(p_this, p_this->h_frame, p_context);
    }
}

#else

int aw_shell_chart_init (struct aw_shell_chart *p_this,
                         struct aw_shell_user  *user,
                         char                   mode,
                         uint32_t               h_frame,
                         uint32_t               v_frame)
{
    return -AW_ENOTSUP;
}


int  aw_shell_chart_item (struct aw_shell_chart  *p_this,
                          const char             *item,
                          uint8_t                 maxlen)
{
    return -AW_ENOTSUP;
}

int  aw_shell_chart_line (struct aw_shell_chart  *p_this,
                          const  char            *p_str,
                          uint8_t                 item_id,
                          int                     line)
{
    return -AW_ENOTSUP;
}

void aw_shell_chart_item_separator ( struct aw_shell_chart    *p_this,
                                     uint32_t                  sep,
                                     const char               *p_context)
{}

void aw_shell_chart_line_separator ( struct aw_shell_chart  *p_this,
                                     uint32_t                sep,
                                     const char             *p_context)
{}

void aw_shell_chart_finish (struct aw_shell_chart      *p_this,
                            uint32_t                    sep,
                            const char                 *p_context)
{}

#endif /* AW_SHELL_USE_CHART */
/*******************************************************************************
  ���ȫ����ʵ��
*******************************************************************************/

/*******************************************************************************
    shell֧�ֵĹ��ܼ�
*******************************************************************************/


/* ctrl+v �� ȡ����ǰ���벢���´�ӡshell�������� */
aw_local int _key_ctrl_v (struct aw_shell_context *sc,
                                  struct aw_shell_user    *user)
{
    if (!sc->des_exist ) {
        user->io_puts(user, "\r\n");
    }
    sc->line_len    = 0;
    sc->cursor_pos  = 0;
    sc->line_buf[0] = 0;
    return AW_OK;
}

/* ctrl+c ���˳���� */
aw_local int _key_component_exit (struct aw_shell_context *sc,
                                  struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_CMDSET
    if (sc->is_cmdset) {
        if (sc->cur_cmdset->p_exit_name) {
            strcpy((char *)sc->line_buf, sc->cur_cmdset->p_exit_name);
            sc->line_len = strlen((char *)sc->line_buf);
            user->io_putc(user, '\n');
            return AW_OK;
        }
    }
#endif /* AW_SHELL_USE_CMDSET */
    return -AW_EINVAL;
}

/* tab�� ����  ����������ӡ�� 1��������ȫ */
aw_local int _key_tab_function (struct aw_shell_context *sc,
                                struct aw_shell_user    *user)
{
    if (!sc->p_opt->use_complete) {
        return -AW_ENOTSUP;
    }

    /* �����������ȫ */
    if (sc->line_len > 0) {
        return _shell_complete(sc, user);
    } else {
        /* û���������ӡ������Ϣ */
        user->io_putc(user, '\n');
        _help_scan(sc, user);
        sc->rec[0] = '\r';
        user->io_putc(user, '\n');

        return AW_OK;
    }

    return -AW_EINPROGRESS;
}

/* �˸�    */
aw_local int _echo_backspace (struct aw_shell_context *sc,
                              struct aw_shell_user    *user)
{
    if (sc->cursor_pos > 0) {
        if (sc->cursor_pos < sc->line_len) {
            int len_right = sc->line_len - sc->cursor_pos;
            _mem_cpyl((char *)&sc->line_buf[sc->cursor_pos - 1],
                      (char *)&sc->line_buf[sc->cursor_pos],
                      len_right + 1);
            sc->cursor_pos--;
            sc->line_len--;
            __shell_puts(user, "\b \b%s ", &sc->line_buf[sc->cursor_pos]);
            _cursor_left(user, len_right + 1);
        } else {
            sc->cursor_pos--;
            sc->line_len--;
            sc->line_buf[sc->line_len] = 0;
            user->io_puts(user, "\b \b");

        }
    }

    return -AW_EINPROGRESS;
}

aw_local int _echo_delete (struct aw_shell_context *sc,
                            struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_CURSOR
    if (sc->cursor_pos < sc->line_len) {
        int len_right = sc->line_len - sc->cursor_pos;
        _mem_cpyl((char *)&sc->line_buf[sc->cursor_pos],
                  (char *)&sc->line_buf[sc->cursor_pos + 1],
                  len_right + 1);
        sc->line_len--;
        user->io_puts(user, (char *)&sc->line_buf[sc->cursor_pos]);
        user->io_putc(user, ' ');
        _cursor_left(user, len_right);

        if (sc->line_len == 0) {
            sc->line_buf[0] = 0;
            user->io_putc(user, '\r');
            return AW_OK;
        }
    }
#endif

    return -AW_EINPROGRESS;
}

/* ���ϵİ��� */
aw_local int _echo_prev_get (struct aw_shell_context *sc,
                             struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_HISTORY
    if ( !sc->p_opt->use_history ) {
        return -AW_EINPROGRESS;
    }

    if (sc->his.buf == NULL) {
        _ERR_DMSG(("history buf is NULL"));
        return -AW_EINVAL;
    }

    /* ���Ǹոճ�ʼ�����������ʷ�Ժ����� */
    if (sc->his.count  == 0 && sc->his.newest == 0 && \
        sc->his.oldest == 0 && sc->his_index  == 0) {
        goto line_display;
    } else if (sc->his_index < sc->his.buf_num) {

                if (sc->his_index == 0) {

                    /* ��his_index = 0ʱ�����浱ǰ�������ַ���  */
                    if (_history_add(&sc->his, sc->line_buf) == AW_OK) {
                        sc->his_index++;
                    }
                }

                if (sc->his_index <= sc->his.count) {
                    sc->his_index++;
                }

    } else {

    sc->his_index = sc->his.buf_num;
    }

    _cursor_back_to_head(sc, user);

    if (sc->his.count < sc->his.buf_num) {

        if (sc->his_index <= sc->his.count) {
            if (_history_get(&sc->his,
                             sc->line_buf,
                             sc->his_index) != AW_OK &&
                sc->his_index > 0 ) {

                sc->his_index--;
            }
        } else {
            /* ��his_index = 12ʱ���������������ʾָʾ��  */
            memset(sc->line_buf, 0, sc->line_len);
            sc->line_len   = 0;
            sc->cursor_pos = 0;
        }
    } else {
        if (_history_get(&sc->his,
                         sc->line_buf,
                         sc->his_index) != AW_OK &&
            sc->his_index > 0 ) {

            sc->his_index--;
        }
    }

line_display:
    sc->line_len   = strlen(sc->line_buf);
    sc->cursor_pos = sc->line_len;

    user->io_puts(user, sc->line_buf);
#endif

    return -AW_EINPROGRESS;
}


aw_local int _echo_next_get(struct aw_shell_context *sc,
                            struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_HISTORY
    if ( !sc->p_opt->use_history ) {
        return -AW_EINPROGRESS;
    }

    if (sc->his.buf == NULL) {
        return -AW_EINVAL;
    }

    _line_echo_clear(sc, user);

    if (sc->his_index > 0) {
        sc->his_index--;
        if (sc->his_index > 0) {
            _history_get(&sc->his, sc->line_buf, sc->his_index);

            sc->line_len   = strlen((char *)sc->line_buf);
            sc->cursor_pos = sc->line_len;

            user->io_puts(user, (char *)sc->line_buf);
        } else {
            /* ��his_index = 0ʱ���������������ʾָʾ��  */
            memset(sc->line_buf, 0, sc->line_len);
            sc->line_len   = 0;
            sc->cursor_pos = 0;
        }
    } else {
        /* ��his_index = 0ʱ�����浱ǰ�������ַ���  */
        _history_add(&sc->his, sc->line_buf);

        /* ��his_index = 0ʱ���������������ʾָʾ��  */
        memset(sc->line_buf, 0, sc->line_len);
        sc->line_len   = 0;
        sc->cursor_pos = 0;
    }
#endif
    return -AW_EINPROGRESS;
}
aw_local int _echo_cursor_right_shift (struct aw_shell_context *sc,
                                        struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_CURSOR
    if (sc->p_opt->use_cursor) {
        if (sc->cursor_pos < sc->line_len) {
            sc->cursor_pos++;
            _cursor_right(user, 1);
        }
    }

#endif

    return -AW_EINPROGRESS;
}
aw_local int _echo_cursor_left_shift (struct aw_shell_context *sc,
                                       struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_CURSOR
    if (sc->p_opt->use_cursor) {
        if (sc->cursor_pos > 0) {
            sc->cursor_pos--;
            _cursor_left(user, 1);
        }
    }
#endif

    return -AW_EINPROGRESS;
}
aw_local int _echo_cursor_home_shift (struct aw_shell_context *sc,
                                       struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_CURSOR
    if (sc->p_opt->use_cursor) {
        if (sc->cursor_pos > 0) {
            _cursor_left(user, sc->cursor_pos);

            sc->cursor_pos = 0;
        }
    }
#endif

    return -AW_EINPROGRESS;
}
aw_local int _echo_cursor_end_shift (struct aw_shell_context *sc,
                                      struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_CURSOR
    if (sc->p_opt->use_cursor) {
        if (sc->cursor_pos < sc->line_len) {
            _cursor_right(user, sc->line_len - sc->cursor_pos);

            sc->cursor_pos = sc->line_len;
        }
    }

#endif
    return -AW_EINPROGRESS;
}


#ifdef AW_SHELL_USE_HISTORY

/**
 * \brief ��ȡָ��id����ʷ�������ִ�У��ɶԸ�������б༭��ȷ��ִ�С�
 *        �������������������˲��ܷ���AW_OK
 *        ���뷽ʽ��![history id]
 * \note: '!' Ҫ�����ף�
 */
aw_local int _key_history_get (struct aw_shell_context *sc,
                               struct aw_shell_user    *user)
{
    char ch;
    int  ret;
    int  i = 0;
    char buf[8];
    int  index;
    int  id;

    int tab_id;

    if (!sc->p_opt->use_history) {
        return -AW_EINPROGRESS;
    }

    if (sc->line_len == 0) {
        aw_shell_putc(user, '!');
        while (1) {
            ret = aw_shell_getc(user);
            if (ret < 0) {
                continue;
            }
            ch = ret;

            if (ch == '\r' || ch == '\n') {
                /* ֻҪ���յ���һ�������ַ������ַ����˳�  */
                buf[i] = 0;
                break;
            }

            /* ֻ����0~9���ַ� */
            if (isdigit(ch)) {
                buf[i] = ch;
                aw_shell_putc(user, ch);
                i++;
                continue;
            }

            /* �����ɾ������ */
            for (tab_id = 0; tab_id < _g_key_tab_num; tab_id++) {
                if (_g_key_tab[tab_id].pfn_do == _echo_backspace) {
                    if (_g_key_tab[tab_id].key_len == 1) {
                        ret = (int)_g_key_tab[tab_id].p_key;
                        if (ch == (char)ret) {

                            if (i > 0) {
                                aw_shell_puts(user, "\b \b");
                                i--;
                                buf[i] = 0;
                            }
                        }
                    }
                }
            }
        }

        if (i > 0) {
            id = strtol(buf, 0, 10);

            if (id >= 0 && id < user->sc->his.count) {
                index = user->sc->his.count - id;

                sc->cursor_pos = 1 + strlen(buf);
                _line_echo_clear(sc, user);
                _history_get(&sc->his, sc->line_buf, index);
                sc->line_len   = strlen((char *)sc->line_buf);
                sc->cursor_pos = sc->line_len;
                aw_shell_puts(user, (char *)sc->line_buf);
            } else {
                __shell_puts(user, "\nnot found history command %d.\n", id);
                _line_echo_clear(sc, user);
            }
        } else {
            aw_shell_puts(user, "\b \b");
        }
    } else {
        if (sc->cursor_pos < sc->line_len) {
            int len_right = sc->line_len - sc->cursor_pos;

            _mem_cpyr((char *)&sc->line_buf[sc->cursor_pos + 1],
                      (char *)&sc->line_buf[sc->cursor_pos],
                      len_right);

            sc->line_buf[sc->cursor_pos] = '!';
            sc->line_buf[sc->line_len + 1] = 0;

            aw_shell_puts(user, (char *)&sc->line_buf[sc->cursor_pos]);
            _cursor_left(user, len_right);
        } else {
            sc->line_buf[sc->cursor_pos] = '!';
            aw_shell_putc(user, sc->line_buf[sc->cursor_pos]);
        }

        if (sc->line_len < AW_SHELL_LINE_SIZE) {
            sc->line_len++;
        }
        if (sc->cursor_pos < AW_SHELL_LINE_SIZE) {
            sc->cursor_pos++;
        }
    }

    return -AW_EINPROGRESS;
}
#endif /* AW_SHELL_USE_HISTORY */


// todo

static struct _key_info _g_key_tab[] = {
    {(char *)'\t',      1, _key_tab_function},
    {(char *)8,         1, _echo_backspace},
    {(char *)127,       1, _echo_backspace},
    {(char *)'\003',    1, _key_component_exit},
    {(char *)22,        1, _key_ctrl_v},

#ifdef AW_SHELL_USE_HISTORY
    {(char *)'!',       1, _key_history_get},
#endif /* AW_SHELL_USE_HISTORY */

    {"\x1B[3~", 4, _echo_delete},
    {"\x1B[A",  3, _echo_prev_get},
    {"\x1B[B",  3, _echo_next_get},
    {"\x1B[C",  3, _echo_cursor_right_shift},
    {"\x1B[D",  3, _echo_cursor_left_shift},
    {"\x1B[1~", 4, _echo_cursor_home_shift},
    {"\x1B[4~", 4, _echo_cursor_end_shift},
    {"\x1B[5~", 4, NULL},
    {"\x1B[6~", 4, NULL},
    {"\x1B[2~", 4, NULL},
};

static uint8_t _g_key_tab_num = AW_NELEMENTS(_g_key_tab);
/******************************************************************************/
aw_local aw_inline aw_bool_t _line_buf_is_empty (struct aw_shell_context *sc)
{
    aw_bool_t ret;

    AW_MUTEX_LOCK(sc->lock, AW_SEM_WAIT_FOREVER);
    ret = (sc->line_buf[0] != 0 || sc->line_len != 0) ? AW_FALSE : AW_TRUE;
    AW_MUTEX_UNLOCK(sc->lock);

    return ret;
}

aw_local void _line_buf_clr (struct aw_shell_context *shell)
{
    shell->line_len   = 0;
    memset(shell->line_buf, '\0', AW_SHELL_LINE_SIZE + 1);

    shell->cursor_pos = 0;

#ifdef AW_SHELL_USE_HISTORY
    shell->his_index = 0;
#endif

    shell->opt_start    = 0;
    shell->opt_end      = 0;
    shell->opt_is_valid = AW_FALSE;
    shell->argc         = 0;
    shell->argv         = NULL;
}
/*******************************************************************************
    history
*******************************************************************************/
#ifdef AW_SHELL_USE_HISTORY
aw_inline aw_local aw_bool_t _history_is_full (struct aw_shell_history *p_this)
{
    return (p_this->count == p_this->buf_num)  ? AW_TRUE : AW_FALSE;
}

/* ��ȡ���ݵ����ݣ�index(1~AW_SHELL_HISTORY_COUNT)ԽС������Խ�� */
aw_local int _history_get (struct aw_shell_history *p_this,
                           uint8_t                 *p_to,
                           uint32_t                 index)
{
    uint32_t newest = p_this->newest;
    uint32_t i = 0;
    uint16_t buf_size = AW_SHELL_LINE_SIZE + 1;
    int     ret = AW_OK;

    if (p_this->buf == NULL) {
        return -AW_EINVAL;
    }

    if (index > p_this->buf_num || !p_to || !p_this) {
        return -AW_EINVAL;
    }

    if ((index - 1) <= newest) {
        i = newest - (index - 1);
    } else if (_history_is_full(p_this)) {
        i = newest + p_this->buf_num - (index - 1);
    } else {
        i = 0;
        ret = -AW_ENODATA;
    }

    if (i <= p_this->buf_num - 1) {
        memset(p_to, 0, strlen((char *)p_to));
        strcpy((char *)p_to, (char *)(p_this->buf + i * buf_size));
    } else {
        memset(p_to, 0, strlen((char *)p_to));
        strcpy((char *)p_to, (char *)(p_this->buf + (i - 10) * buf_size));
    }

    return ret;
}

aw_local int _history_add (struct aw_shell_history *p_this, uint8_t *p_from)
{
    uint8_t len = strlen((char *)p_from);
    uint16_t buf_size = AW_SHELL_LINE_SIZE + 1;

    if (p_this->buf == NULL) {
        return -AW_EINVAL;
    }

    if (len > AW_SHELL_LINE_SIZE) {
        _ERR_DMSG(("cmd is too long"));
        return -AW_EINVAL;
    }

    if (len == 0) {
        return -AW_ENODATA;
    }

    /* ������һ��������ַ����Ƿ��Ǹ��ַ��������������� */
    if (p_this->count) {
        if (strcmp((char *)(p_this->buf + p_this->newest * buf_size),
                   (char *)p_from) == 0) {
            return -AW_EINVAL;
        }
    }

    if (p_this->newest >= p_this->buf_num - 1) {
        p_this->newest = 0;
    } else {
        p_this->newest++;
    }

    if (_history_is_full(p_this)) {

        if (p_this->oldest >= p_this->buf_num - 1) {
            p_this->oldest = 0;
        } else {
            p_this->oldest++;
        }
    } else {
        p_this->count++;
    }
    memset(p_this->buf + p_this->newest * buf_size, 0, AW_SHELL_LINE_SIZE + 1);
    strcpy((char *)(p_this->buf + p_this->newest * buf_size), (char *)p_from);

    return AW_OK;
}

#endif /* AW_SHELL_USE_HISTORY */

/******************************************************************************/
/**
 * @brief   Get arguments from string
 * @param   arg     The argument string
 * @param   argvc   The length of argv array
 * @param   argv    char pointor array to hold the args
 * @retval  The count of args get from "arg"
 */
static int get_args(char *arg, char *argv[], int argvc)
{
    int  argc = 0;
    char *tmp = NULL;
    char c;
    aw_bool_t is_spec = AW_FALSE;

    if ((NULL == arg) || (NULL == argv)) {
        return 0;
    } else if ((argvc < 1) || ('\0' == *arg)) {
        return 0;
    }

    if (*arg == '\"') {
        tmp = arg;
        while ((tmp = strchr(tmp + 1, '\"')) != NULL) {
            if (*(tmp - 1) != '\\') {
                is_spec = AW_TRUE;
                *tmp = '\0';
                arg++;
                break;
            }
        }
    }

    argv[0] = arg;  /* The first arg */
    argc    = 1;

    if (is_spec) {
        is_spec = AW_FALSE;
        arg = tmp + 1;
    }

    while ((argc < argvc) && ((c = *arg) != '\0')) {
        if (is_spec) {
            tmp = arg - 1;
            while ((tmp = strchr(tmp + 1, '\"')) != NULL) {
                /*
                 * ���ҵ����� " ���һ�������� " �ַ���'"' ǰû�� '\'�����Ը� '"' Ϊ������
                 * �������'"'���������ַ�����Щ�ַ��ᶪ��
                 */
                if (*(tmp - 1) != '\\') {
                    is_spec = AW_FALSE;
                    *tmp = '\0';
                    arg = tmp + 1;
                    break;
                }
            }

            if (!is_spec ) {
                continue;
            }
        }

        if ('\\' == c) {
            arg += 2;
            continue;
        }
        if (' ' == c) {     /* Find an arg */
            *arg++ = '\0';  /* Terminator of prev arg */
            _escape_process(argv[argc-1]);
            skip_spaces(arg);

            if ('\0' != *arg) {
                argv[argc++] = arg;

                /* ֻ�о��׵� " ����Ч�����Ǿ���������ͨ�ַ�  */
                if (*arg == '\"') {
                    argv[argc - 1]++;
                    is_spec = AW_TRUE;
                }
            } else {
                break;
            }
        }
        arg++;
    }

    if (*arg == '\0') {
        _escape_process(argv[argc-1]);
    }
    return argc;
}

/******************************************************************************/
static const struct aw_shell_cmd *_symbol_find(const char *cmd_str,
                                               const char **cmd_arg,
                                               struct aw_shell_user *p_user)
{
    struct aw_shell_cmd_list  *list     = &gcmd_list_local;
    const struct aw_shell_cmd *cmd_find = NULL;
    const struct aw_shell_cmd *cmd;
    const char                *args;
    int                        len;

    while (NULL != list) { /* Scan symbol list */
        /* Scan symbol array */
        for (cmd = list->start; cmd < list->end; cmd++) {
            len = strlen(cmd->name);
            if (strncmp(cmd_str, cmd->name, len) != 0) { /* CMD name match ?*/
                continue;
            }
            args = cmd_str + len; /* Get args */
            if (' ' != *args && '\0' != *args) { /* valid command ? */
                continue;
            }
            skip_spaces(args); /* Skip leading spaces */
            *cmd_arg = args;
            cmd_find = cmd;
            break;
        }
        if (NULL != cmd_find) {
            break;
        }
        list = list->next;
    }

    return cmd_find;
}

static int _run_cmd (const struct aw_shell_cmd *cmd,
                     char                      *args,
                     struct aw_shell_user      *p_user)
{
    char *argv[20];
    int   ret;
    int   i = 0;
    struct aw_shell_context *sc = p_user->sc;

    if (cmd->entry) {
        sc->argc      = get_args(args, argv, count_of(argv));
        sc->argv      = argv;
        ret = cmd->entry(sc->argc, sc->argv, p_user);

        if (sc->opt_is_valid) {
            __shell_puts(p_user, "command invalid : %s", cmd->name);

            while (sc->opt_start + i < sc->opt_end) {
                __shell_puts(p_user, " %s", argv[sc->opt_start + i]);
                i++;
            }
            aw_shell_putc(p_user, '\n');
            ret = -1;
        }else if(ret < 0 && !sc->opt_is_valid){
            __shell_puts(p_user,"%s:invalid arguments\r\n",cmd->name);
            ret = -1;
        }

        if (ret < 0) {
            __shell_puts(p_user,"Try \"%s --help\" for more information.\r\n\n",cmd->name);
        }
    } else {
        return AW_OK;
    }

    return ret;
}


/*******************************************************************************/
aw_err_t aw_shell_init(struct aw_shell_context *sc,
                       struct aw_shell_user *p_user,
                       struct aw_shell_opt *p_opt)
{
    if ((sc == NULL) || (p_user == NULL)) {
        return -AW_EINVAL;
    }

    if (p_user->io_getc == NULL ||
        p_user->io_putc == NULL ||
        p_user->io_puts == NULL ) {
        return -AW_EINVAL;
    }

    if (p_user->io_printf == NULL) {
        p_user->io_printf = aw_shell_printf;
    }

    p_user->sc = sc;

    if (!(sc->inited)) {

        memset(sc, 0, sizeof(struct aw_shell_context));

        AW_MUTEX_INIT(sc->lock, AW_SEM_Q_PRIORITY);
        AW_MUTEX_INIT(p_user->mutex, AW_SEM_Q_FIFO);

        sc->p_opt = p_opt;

#ifndef AW_SHELL_USE_CHART
        sc->p_opt->use_chart = AW_FALSE;
#endif

#ifdef  AW_SHELL_USE_HISTORY
        _shell_history_init(p_user, p_opt->p_his_buf, p_opt->his_buf_size);
#else
        p_opt->use_history = AW_FALSE;
#endif

#ifndef AW_SHELL_USE_COMPLETE
        p_opt->use_complete = AW_FALSE;
#endif

#ifndef AW_SHELL_USE_COLOR
        p_opt->use_color = AW_FALSE;
#endif

#ifndef AW_SHELL_USE_CURSOR
        p_opt->use_cursor = AW_FALSE;
#endif

#ifndef AW_SHELL_USE_CMDSET
        p_opt->use_cmdset = AW_FALSE;
#endif

        if (!p_opt->use_escape_seq ) {
            p_opt->use_color    = AW_FALSE;
            p_opt->use_cursor   = AW_FALSE;
            p_opt->use_chart    = AW_FALSE;
        }

        if (p_opt->p_endchar != NULL) {
            p_opt->endchar_len = strlen(p_opt->p_endchar);
        } else {
            /* Ĭ��Ϊ\r��\n */
            p_opt->endchar_len = 1;
        }

        aw_list_head_init(&sc->alias_list);

        if (p_opt->p_desc != NULL) {
            memcpy(sc->des_store, p_opt->p_desc, strlen(p_opt->p_desc) + 1);
        } else {
            memcpy(sc->des_store, "->> ", 5);  /* default */
        }

        if (p_opt->p_desc_color != NULL) {
            memcpy(sc->des_color,
                   p_opt->p_desc_color,
                   strlen(p_opt->p_desc_color) + 1);
        } else {
            /* default */
            memcpy(sc->des_color,
                   AW_SHELL_FCOLOR_DEFAULT,
                   strlen(AW_SHELL_FCOLOR_DEFAULT) + 1);
        }

        p_user->state = AW_SHELL_SUS_RUNING;

        sc->inited = AW_TRUE;
    }

    if (aw_default_shell_get() == NULL) {
        aw_default_shell_set(p_user);
    }

    return AW_OK;
}

/*******************************************************************************
  �����ַ��������״̬��
*******************************************************************************/
struct _cmd_stat_info {
    struct _key_info        *p_tab;
    uint8_t                  nkeys;
    struct aw_shell_context *sc;
    struct aw_shell_user    *p_user;
    int                      ret;
    uint8_t                  rec_cnt;
};

struct _cmd_gets_stat {
    struct _cmd_gets_stat *(*pfn_process) (struct _cmd_stat_info *p_info, uint8_t ch);
};

/* ǰ������ */
static struct _cmd_gets_stat _g_stat_idle;
static struct _cmd_gets_stat _g_stat_ctrl;
static struct _cmd_gets_stat _g_stat_nor;
static struct _cmd_gets_stat _g_stat_exit;

static struct _cmd_gets_stat * _idle_process (struct _cmd_stat_info *p_info,
                                              uint8_t                ch)
{
    if (p_info->ret < 0) {
        return NULL;
    }

    if (isprint(ch) || ch > 127) {
        return _g_stat_nor.pfn_process(p_info, ch);
    }

    p_info->rec_cnt = 0;
    return _g_stat_ctrl.pfn_process(p_info, ch);
}

static struct _cmd_gets_stat * _ctrl_process (struct _cmd_stat_info *p_info,
                                              uint8_t                ch)
{
    static uint8_t          __cnt = 0;
    int                      i          = 0;
    struct _key_info        *p_tab;
    struct aw_shell_context *sc         = p_info->sc;
    aw_bool_t                   is_max_len = AW_TRUE;

    if (p_info->ret < 0) {

        /*
         * ���������ַ�һ����������,��������˸ý��տ��������״̬,
         * ���жϽ��յ� 10 ��
         */
        /*  ��Ч�ַ����˳�*/
        if (__cnt > 10) {
            memset(sc->rec, 0, p_info->rec_cnt);
            p_info->rec_cnt = 0;

            __cnt = 0;
            return &_g_stat_nor;
        }

        __cnt++;
        return &_g_stat_ctrl;
    } else if (ch == '\r' || ch == '\n') {
        p_info->rec_cnt = 0;
        return _g_stat_exit.pfn_process(p_info, ch);
    }

    sc->rec[p_info->rec_cnt] = ch;
    p_info->rec_cnt++;

    /**
     * ����key���ж�����������Ƿ��������Ĺ��ܣ���tab up down�ȣ�
     * ���p_tab->pfn_do����AW_OK������Ҫ����������ֱ���˳��ô������tab���
     * ����������������������˸�����
     */
    for (i = 0, p_tab = p_info->p_tab; i < p_info->nkeys; i++, p_tab++) {

        if (p_info->rec_cnt < p_tab->key_len) {
            is_max_len = AW_FALSE;
        }

        if (p_info->rec_cnt != p_tab->key_len) {
            continue;
        }

        if (_str_key_is(p_tab->p_key, p_tab->key_len, (char *)sc->rec)) {

            memset(sc->rec, 0, p_info->rec_cnt);
            p_info->rec_cnt = 0;

            if (p_tab->pfn_do) {
                p_info->ret = p_tab->pfn_do(sc, p_info->p_user);

                /* �������OK��ֱ���˳� */
                if (p_info->ret == AW_OK) {
                    return NULL;
                }
            }
            return &_g_stat_nor;
        }
    }

    /**
     * �ж��Ƿ���յ���������tab����󳤶ȵ��ַ�������
     * ������յ�������ַ���������û��ƥ�䵽���ʾ�öο����ַ�������Ч��
     */
    if (is_max_len) {
        memset(sc->rec, 0, p_info->rec_cnt);
        p_info->rec_cnt = 0;

        __cnt = 0;
        return &_g_stat_nor;
    }

    return &_g_stat_ctrl;
}

static aw_bool_t _is_ctrl_first_char (struct _cmd_stat_info *p_info, uint8_t ch)
{
    int i = 0;
    int ret;

    for (i = 0; i < p_info->nkeys; i++) {
        if (p_info->p_tab[i].key_len == 1) {
            ret = (int)p_info->p_tab[i].p_key;
            if (ch == (char)ret) {
                return AW_TRUE;
            }
        } else {
            if (ch == (char)p_info->p_tab[i].p_key[0]) {
                return AW_TRUE;
            }
        }
    }

    return AW_FALSE;
}

static struct _cmd_gets_stat * _nor_process  (struct _cmd_stat_info *p_info,
                                              uint8_t                ch)
{
    struct aw_shell_context *sc = p_info->sc;

    if (p_info->ret < 0) {
        /* ���յ����ݣ���������Ч�ַ����������½��� */
        return &_g_stat_nor;

    } else if (ch == '\r' || ch == '\n') {
        p_info->rec_cnt = 0;
        return _g_stat_exit.pfn_process(p_info, ch);

    } else if (ch <= 127 && (!isprint(ch) || _is_ctrl_first_char(p_info, ch))) {
        p_info->rec_cnt = 0;
        return _g_stat_ctrl.pfn_process(p_info, ch);
    }

    if (!sc->des_exist ) {
        sc->des_exist = AW_TRUE;

        aw_shell_color_set(p_info->p_user, sc->des_color);
        __shell_puts(p_info->p_user, "\r\n%s", sc->des);
        aw_shell_color_set(p_info->p_user, AW_SHELL_FCOLOR_DEFAULT);

        if (p_info->p_user->sc->line_len > 0) {
            p_info->p_user->io_puts(p_info->p_user, (char *)sc->line_buf);
        }
        sc->line_len = sc->cursor_pos;
    }

    if (sc->cursor_pos < sc->line_len) {
        int len_right = sc->line_len - sc->cursor_pos;

        _mem_cpyr((char *)&sc->line_buf[sc->cursor_pos + 1],
                  (char *)&sc->line_buf[sc->cursor_pos],
                  len_right);

        sc->line_buf[sc->cursor_pos] = ch;
        sc->line_buf[sc->line_len + 1] = 0;

        p_info->p_user->io_puts(p_info->p_user,
                                (char *)&sc->line_buf[sc->cursor_pos]);
        _cursor_left(p_info->p_user, len_right);
    } else {
        sc->line_buf[sc->cursor_pos] = ch;
        p_info->p_user->io_putc(p_info->p_user, sc->line_buf[sc->cursor_pos]);
    }

    if (sc->line_len < AW_SHELL_LINE_SIZE) {
        sc->line_len++;
    }
    if (sc->cursor_pos < AW_SHELL_LINE_SIZE) {
        sc->cursor_pos++;
    }

    return &_g_stat_nor;
}

static struct _cmd_gets_stat * _exit_process  (struct _cmd_stat_info *p_info,
                                               uint8_t                ch)
{
    uint8_t  enter_len = 0;
    struct aw_shell_context *sc     = p_info->sc;
    struct aw_shell_user    *p_user = p_info->p_user;

    if (p_info->ret < 0) {
        return &_g_stat_exit;
    }

    sc->rec[p_info->rec_cnt] = ch;
    p_info->rec_cnt++;

    if (sc->p_opt->p_endchar) {
        if (p_info->rec_cnt == sc->p_opt->endchar_len &&
            memcmp(sc->rec, sc->p_opt->p_endchar, p_info->rec_cnt) == 0) {

            for (enter_len = 0; enter_len < sc->p_opt->endchar_len; enter_len++) {
                sc->line_buf[sc->line_len + enter_len] = 0;
            }
            p_info->ret = AW_OK;
        }

    } else {
        if (sc->rec[p_info->rec_cnt - 1] == '\r' || sc->rec[p_info->rec_cnt - 1] == '\n') {
            p_info->ret = AW_OK;
        }
    }

    if (p_info->ret == AW_OK) {
        if (sc->line_len > 0) {
#ifdef AW_SHELL_USE_HISTORY
            if (sc->p_opt->use_history ) {
                _history_add(&sc->his, sc->line_buf);
            }
#endif
        }

        p_user->io_puts(p_user, "\r\n");
    } else if (p_info->rec_cnt > sc->p_opt->endchar_len) {
        p_info->ret = -AW_EBADMSG;
    } else {
        return &_g_stat_exit;
    }

    return NULL;
}

static struct _cmd_gets_stat _g_stat_idle = {_idle_process};

/* ���տ����ַ���״̬ */
static struct _cmd_gets_stat _g_stat_ctrl = {_ctrl_process};

/* ������������ʾ�ַ���״̬ */
static struct _cmd_gets_stat _g_stat_nor  = {_nor_process};

/* �����˳��ַ���״̬ */
static struct _cmd_gets_stat _g_stat_exit = {_exit_process};

/******************************************************************************/

/**
 * \brief   Get a line from the input
 * \param   shell:  Shell struct pointor
 * \retval  Pointor of the Line buf head if no error.
 * \NOTE    The callers should make sure the paramters are crect.
 * \ref     ת���ַ� ���� ASCII��ֵ(ʮ����)��
 * \a ����(BEL) 007
 * \b �˸�(BS) 008
 * \f ��ҳ(FF) 012
 * \n ����(LF) 010
 * \r �س�(CR) 013
 * \t ˮƽ�Ʊ�(HT) 009
 * \v ��ֱ�Ʊ�(VT) 011
 * \\ ��б�� 092
 * \? �ʺ��ַ� 063
 * \' �������ַ� 039
 * \" ˫�����ַ� 034
 * \0 ���ַ�(NULL) 000
 * \ddd �����ַ� ��λ�˽���
 * \xhh �����ַ� ��λʮ������
 */
aw_local int _shell_cmd_gets (struct aw_shell_context *sc,
                              struct aw_shell_user *p_user)
{
    uint8_t                ch;
    struct _cmd_gets_stat *p_stat = &_g_stat_idle;
    struct _cmd_stat_info  info;

    info.nkeys      = AW_NELEMENTS(_g_key_tab);
    info.p_tab      = _g_key_tab;
    info.rec_cnt    = 0;
    info.p_user     = p_user;
    info.sc         = sc;

    if (sc->rec[0] != 0) {
        memset(sc->rec, 0, 10);
    }

    while (p_stat != NULL) {
        info.ret = p_user->io_getc(p_user);
        ch = info.ret;
        p_stat = p_stat->pfn_process(&info, ch);
    }

    return info.ret;
}

// todo
aw_err_t aw_shell_server(struct aw_shell_context *sc, struct aw_shell_user *p_user)
{
    char                           *args;
    const struct aw_shell_cmd      *cmd_find;
    struct aw_shell_cmdset         *cmdset                  = NULL;
    aw_bool_t                       cmdset_need_recovery    = AW_FALSE;
    aw_bool_t                       cmdset_need_clear       = AW_FALSE;
    int                             ret                     = 0;

    server_stat_t stat = STAT_INIT;

    while (1) {
        sc->stat = stat;
        switch (stat) {

        case STAT_INIT:

            /* line_buf������������� */
            if (!_line_buf_is_empty(sc)) {
                _line_buf_clr(sc);
                p_user->io_puts(p_user, "\r\n");
            }

#ifdef AW_SHELL_USE_CMDSET
            /* ��ӡ������ */
            if (!sc->is_cmdset) {
                memcpy(sc->des, sc->des_store, strlen(sc->des_store) + 1);
            } else {
                if (cmdset->p_enter_name[0] == '.') {
                    aw_snprintf(sc->des,
                                AW_SHELL_LINE_SIZE >> 1,
                                "[%s]%s",
                                cmdset->p_enter_name + 1,
                                sc->des_store);
                } else {
                    aw_snprintf(sc->des,
                                AW_SHELL_LINE_SIZE >> 1,
                                "[%s]%s",
                                cmdset->p_enter_name,
                                sc->des_store);
                }
            }

#endif /* AW_SHELL_USE_CMDSET */
            sc->des_exist = AW_FALSE;

            AW_MUTEX_LOCK(p_user->mutex, AW_SEM_WAIT_FOREVER);
            _line_echo_clear(sc, p_user);
            AW_MUTEX_UNLOCK(p_user->mutex);

            stat = STAT_IDLE;
            break;

        case STAT_IDLE:
            if (p_user->state == AW_SHELL_SUS_EXITING) {
                p_user->state = AW_SHELL_SUS_DEAD;
                while (p_user->state != AW_SHELL_SUS_RUNING) {
                    aw_mdelay(500);
                }
            }

#ifdef AW_SHELL_USE_CMDSET
            cmdset = sc->cur_cmdset;
            if (sc->is_cmdset && cmdset && cmdset->pfn_always_run) {
                cmdset->pfn_always_run(p_user);
            }
#endif /* AW_SHELL_USE_CMDSET */
            stat = STAT_RECEIVING;
            break;

        case STAT_RECEIVING:
            ret = _shell_cmd_gets(sc, p_user);
            if (ret == AW_OK) {
                /*
                 * ���յ��ַ���������linebuf��û�����ݣ�
                 * ������������Ѿ�ִ��������⹦�������"tab"����
                 */
                if (sc->line_len != 0) {
                    args = (char *)sc->line_buf;
                    stat = STAT_RUNING;

                    skip_spaces(args);
                } else {
                    stat = STAT_INIT;
                }
            } else {
                args = NULL;
                stat = STAT_IDLE;
            }
            break;

        case STAT_RUNING:
            stat = STAT_INIT;

            __cmd_alias_find(p_user, args);

#ifdef AW_SHELL_USE_CMDSET

            if (!sc->is_cmdset ) {
                /* Find commands */
                cmd_find = _cmd_find(args, (const char **) &args);
                if (NULL == cmd_find) {
                    cmdset   = _cmdset_find(args);

                    /*
                     * ���argsֻ����������������;
                     * ����Ϊ "���+������" ����ʽ�����������������ִ�и������
                     */
                    if (NULL != cmdset) {
                        args    += strlen(cmdset->p_enter_name);
                        skip_spaces(args);

                        if ((AW_SEMB_TAKE(cmdset->init_sem, AW_SEM_NO_WAIT) == AW_OK) &&
                            cmdset->pfn_entry_init) {
                            cmdset->pfn_entry_init(p_user);
                        }
                        sc->is_cmdset  = AW_TRUE;
                        sc->cur_cmdset = cmdset;

                        if (*args == '\0') {
                            break;
                        } else {
                            cmdset_need_clear = AW_TRUE;

                            // ����������Ƿ�Ϊalias
                            __cmd_alias_find(p_user, args);

                            cmd_find = _cmdset_cmd_find(cmdset, args, (const char **) &args);
                            if (NULL == cmd_find && strncmp(args, "help", 4) == 0) {
                                cmd_find = _cmd_find(args, (const char **) &args);
                            }
                        }
                    }
                }
            } else {
                cmd_find = _cmdset_cmd_find(cmdset, args, (const char **) &args);
                if (NULL == cmd_find) {
                    if (_is_cmdset_exit(cmdset, args)) {
                        sc->is_cmdset  = AW_FALSE;
                        sc->cur_cmdset = NULL;
                        cmdset         = NULL;
                        break;
                    /* �ж�һ���Ƿ���һ������� */
                    } else if (strncmp(args, "../", 3) == 0) {
                        _mem_cpyl(args, args + 3, strlen(args));
                        cmd_find = _cmd_find(args, (const char **) &args);
                        cmdset_need_recovery = AW_TRUE;
                        sc->is_cmdset        = AW_FALSE;

                    } else if (strncmp(args, "help", 4) == 0) {
                        cmd_find = _cmd_find(args, (const char **) &args);
                    }
                }
            }

            if (cmd_find) {
                _run_cmd(cmd_find, args, p_user);
            } else {
                __shell_puts(p_user, "\"%s\" unkown. Try \"help\"\n",
                                    sc->line_buf);
            }

            if (cmdset_need_recovery) {
                cmdset_need_recovery = AW_FALSE;
                sc->is_cmdset        = AW_TRUE;
            }

            if (cmdset_need_clear) {
                cmdset_need_clear = AW_FALSE;
                sc->is_cmdset     = AW_FALSE;
            }

#else
            cmd_find = _cmd_find(args, (const char **) &args);
            if (cmd_find) {
                _run_cmd(cmd_find, args, p_user);
            } else {
                __shell_puts(p_user, "\"%s\" unkown. Try \"help\"\n", sc->line_buf);
            }
#endif /* AW_SHELL_USE_CMDSET */
            break;

        default: break;
        }
    }

    return AW_OK;
}
/******************************************************************************/
aw_err_t aw_shell_args_get (struct aw_shell_user *user,
                            const char           *opt,
                            int                   args)
{
#ifdef AW_SHELL_USE_CMDSET

    int i = 0;
    struct aw_shell_context *sc = user->sc;

    if (user == NULL || sc->argc == 0) {
        return -AW_EINVAL;
    }

    /*
     * opt �� args Ϊ0����ֻ����ѡ����ж�ƥ�䣺
     * �� sc->opt_is_valid �� ʱ����һ��ѡ���������ɹ����� 0��
     * ��� sc->opt_is_valid Ϊ��ʱ�����߽���ʧ�ܷ��� < 0
     */
    if (opt == NULL && args == 0) {
        if (sc->opt_is_valid ) {
            return -AW_EINVAL;
        }
    }

    if (!sc->opt_is_valid ) {
//        if (sc->opt_end + 1 + args > sc->argc) {
//            return -AW_ENODATA;
//        }
        if (sc->opt_end >= sc->argc) {
            return -AW_ENODATA;   // û��������
        }

        if (sc->opt_end > sc->opt_start) {
            sc->opt_start = sc->opt_end;
        }

        while (1) {
            /*
             * ��һ�� argv ֱ����Ϊѡ�����Ҫ�ж��ַ������ݣ�
             * �ڶ��� argv ��ʼ����Ϊ�����������жϵ�һ���ַ��Ƿ�Ϊ'-'��
             * ������Ϊ�� argv ��ѡ���������ĵ�һ���ַ�ҪΪ'-'����Ҫ��'\'��
             * ��һ������У�����û��ѡ�֮��Ĳ����б���Ҫ��ѡ������������룺
             * cmd 123 456 -w file.c abcd -r file.c 4
             */
            i++;
            if (sc->opt_start + i >= sc->argc ||
                sc->argv[sc->opt_start + i][0] == '-') {
                break;
            } else if (strncmp(sc->argv[sc->opt_start + i], "\\-", 2) == 0) {
                _mem_cpyl(  sc->argv[sc->opt_start + i],
                            sc->argv[sc->opt_start + i] + 1,
                            strlen(sc->argv[sc->opt_start + i]));
            }
        }

        sc->opt_is_valid = AW_TRUE;
        sc->opt_end = sc->opt_start + i;
    }

    if (opt == NULL && args == 0) {
        return AW_OK;
    }

    if (opt != NULL) {
        if (strcmp(sc->argv[sc->opt_start], opt) == 0 &&
            sc->opt_end - sc->opt_start == args + 1) {
            sc->opt_is_valid = AW_FALSE;
            return AW_OK;
        }
    } else {
        if (sc->opt_end - sc->opt_start == args &&
            sc->argv[sc->opt_start][0] != '-') {
            if (strncmp(sc->argv[sc->opt_start], "\\-", 2) == 0) {
                _mem_cpyl(  sc->argv[sc->opt_start],
                            sc->argv[sc->opt_start] + 1,
                            strlen(sc->argv[sc->opt_start]));
            }
            sc->opt_is_valid = AW_FALSE;
            return AW_OK;
        }
    }

    return -AW_EINVAL;

#else
    return -AW_ENOTSUP;
#endif /* AW_SHELL_USE_CMDSET */
}

long int aw_shell_long_arg (struct aw_shell_user *user, int index)
{
#ifdef AW_SHELL_USE_CMDSET
    if (user->sc->opt_start + index < user->sc->opt_end) {
        return strtol(user->sc->argv[user->sc->opt_start + index], NULL, 0);
    }
#endif /* AW_SHELL_USE_CMDSET */
    return 0;
}

unsigned long int aw_shell_ulong_arg (struct aw_shell_user *user, int index)
{
#ifdef AW_SHELL_USE_CMDSET
    if (user->sc->opt_start + index < user->sc->opt_end) {
        return strtoul(user->sc->argv[user->sc->opt_start + index], NULL, 0);
    }
#endif /* AW_SHELL_USE_CMDSET */
    return 0;
}

char *aw_shell_str_arg (struct aw_shell_user *user, int index)
{
#ifdef AW_SHELL_USE_CMDSET
    if (user->sc->opt_start + index < user->sc->opt_end) {
        return user->sc->argv[user->sc->opt_start + index];
    }
#endif /* AW_SHELL_USE_CMDSET */
    return NULL;
}
/******************************************************************************/
int aw_shell_gets (struct aw_shell_user *p_user, char *p_line, int count)
{
    int rec_cnt = 0;

    if (p_user == NULL) {
        return 0;
    }
    while (rec_cnt < count) {
        p_line[rec_cnt] = p_user->io_getc(p_user);

        if (p_line[rec_cnt] > 0) {
            p_user->io_putc(p_user, p_line[rec_cnt]);
            rec_cnt++;
        } else if (rec_cnt == 0){
            return 0;
        } else {
            continue;
        }

        if (p_user->sc->p_opt->p_endchar) {
            if (strncmp(p_line + rec_cnt - p_user->sc->p_opt->endchar_len,
                        p_user->sc->p_opt->p_endchar,
                        p_user->sc->p_opt->endchar_len) == 0 ) {
                break;
            }
        } else {
            if (p_line[rec_cnt - p_user->sc->p_opt->endchar_len] == '\r' ||
                p_line[rec_cnt - p_user->sc->p_opt->endchar_len] == '\n') {
                break;
            }
        }
    }
    p_line[rec_cnt - 1] = 0;

    return rec_cnt;
}


aw_err_t aw_shell_register_cmds (struct aw_shell_cmd_list  *list,
                                 const struct aw_shell_cmd *start,
                                 const struct aw_shell_cmd *end)
{
    struct aw_shell_cmd_list *local_list = &gcmd_list_local;
    const struct aw_shell_cmd *cmd;
    const char *args;

    for (cmd = start; cmd < end; cmd++) {
        if (_cmd_find(cmd->name, (const char **)&args)) {
            return -AW_EEXIST;
        }
    }

    while (NULL != local_list->next && list != local_list) {
        local_list = local_list->next;
    }
    if (list != local_list) {
        list->next       = NULL;
        list->start      = start;
        list->end        = end;
        local_list->next = list;
        return AW_OK;
    }
    return -AW_EEXIST;
}

aw_err_t aw_shell_unregister_cmds (struct aw_shell_cmd_list *list)
{
    struct aw_shell_cmd_list *local_list = &gcmd_list_local;

    while (NULL != local_list->next && list != local_list->next) {
        local_list = local_list->next;
    }
    if (list == local_list->next) {
        local_list->next = list->next;
        return AW_OK;
    }
    return -AW_ENOENT;
}

aw_err_t aw_shell_system (const char *cmd_name_arg)
{
    struct aw_shell_user       *p_user = aw_dbg_shell_user();
    struct aw_shell_context    *sc;
    char                       *args;
    const struct aw_shell_cmd  *cmd_find;
    char                        buf[AW_SHELL_LINE_SIZE + 1];
    int                         ret = -AW_ENOENT;
    int                              opt_start = 0;
    int                              opt_end = 0;
    aw_bool_t                           opt_valid = AW_FALSE;
    static struct aw_shell_cmdset   *cmdset;

    if (p_user == NULL) {
        return -AW_ENOENT;
    }
    sc = p_user->sc;
    if (strlen(cmd_name_arg) > AW_SHELL_LINE_SIZE) {
        AW_INFOF(("[err] [aw_shell_system] string param too long."));
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(sc->lock, AW_SEM_WAIT_FOREVER);
    /* copy command string to line buffer */
    args = strncpy(buf, cmd_name_arg, AW_SHELL_LINE_SIZE);
    buf[AW_SHELL_LINE_SIZE] = 0;
    __cmd_alias_find(p_user, buf);

    /* Find the command and execute it if fond */
    if (NULL != (cmd_find = _symbol_find(args, (const char **)&args, p_user))) {
        opt_start    = sc->opt_start;
        opt_end      = sc->opt_end;
        opt_valid    = sc->opt_is_valid;

        sc->opt_start    = 0;
        sc->opt_end      = 0;
        sc->opt_is_valid = AW_FALSE;

        ret = _run_cmd(cmd_find, args, p_user);

        sc->opt_start    = opt_start;
        sc->opt_end      = opt_end;
        sc->opt_is_valid = opt_valid;

    /* ʹ�� aw_shell_system(".xxxx") ����������ܹ�ֱ�Ӵ�һ�����ֱ�ӽ�����һ�����
     * �������˳���1�����
     */
    } else if ((cmdset = _cmdset_find(args)) != NULL) {
        /* ���������� */
        if (cmdset->p_enter_name[0] == '.') {
            aw_snprintf(sc->des,
                        AW_SHELL_LINE_SIZE >> 1,
                        "[%s]%s",
                        cmdset->p_enter_name + 1,
                        sc->des_store);
        } else {
            aw_snprintf(sc->des,
                        AW_SHELL_LINE_SIZE >> 1,
                        "[%s]%s",
                        cmdset->p_enter_name,
                        sc->des_store);
        }

        sc->is_cmdset  = AW_TRUE;
        sc->cur_cmdset = cmdset;
        if ((AW_SEMB_TAKE(cmdset->init_sem, AW_SEM_NO_WAIT) == AW_OK) &&
            cmdset->pfn_entry_init) {
            cmdset->pfn_entry_init(p_user);
        }
        __shell_puts(p_user, "\n");
    }

    AW_MUTEX_UNLOCK(sc->lock);

    return ret;
}

aw_err_t aw_shell_system_ex (struct aw_shell_user *p_user,
                             const char           *cmdset_name,
                             const char           *cmd_name_arg)
{
//    struct aw_shell_user            *p_user = aw_dbg_shell_user();
    struct aw_shell_context         *sc = p_user->sc;
    char                            *args;
    const struct aw_shell_cmd       *cmd_find = NULL;
    struct aw_shell_cmdset          *cmdset   = NULL;
    char                             buf[AW_SHELL_LINE_SIZE + 1];
    int                              ret = -AW_ENOENT;
    int                              opt_start = 0;
    int                              opt_end = 0;
    aw_bool_t                           opt_valid = AW_FALSE;

    if (p_user == NULL) {
        return -AW_ENOENT;
    }

    if (strlen(cmd_name_arg) > AW_SHELL_LINE_SIZE) {
        AW_INFOF(("[err] [aw_shell_system_ex] string param too long."));
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(sc->lock, AW_SEM_WAIT_FOREVER);

    /* copy command string to line buffer */
    args = strncpy(buf, cmd_name_arg, AW_SHELL_LINE_SIZE);
    buf[AW_SHELL_LINE_SIZE] = 0;
    __cmd_alias_find(p_user, buf);

    if (cmdset_name != NULL) {
        cmdset   = _cmdset_find(cmdset_name);
        if (NULL == cmdset) {
            ret = -AW_EINVAL;
            goto exit;
        }

        cmd_find = _cmdset_cmd_find(cmdset, args, (const char **) &args);
        if (NULL == cmd_find) {
            ret = -AW_EINVAL;
            goto exit;
        }

        if ((AW_SEMB_TAKE(cmdset->init_sem, AW_SEM_NO_WAIT) == AW_OK) &&
            cmdset->pfn_entry_init) {
            cmdset->pfn_entry_init(p_user);
        }
    } else {
        cmd_find = _cmd_find(args, (const char **)&args);
    }

    if (cmd_find) {
        opt_start    = sc->opt_start;
        opt_end      = sc->opt_end;
        opt_valid    = sc->opt_is_valid;

        sc->opt_start    = 0;
        sc->opt_end      = 0;
        sc->opt_is_valid = AW_FALSE;

        ret = _run_cmd(cmd_find, args, p_user);

        sc->opt_start    = opt_start;
        sc->opt_end      = opt_end;
        sc->opt_is_valid = opt_valid;
    }

exit:
    AW_MUTEX_UNLOCK(sc->lock);

    return ret;
}
/******************************************************************************/
static int _putc( char c, void *f )
{
    aw_shell_user_t *p_user = (aw_shell_user_t *)f;
    if ( p_user ) {
        return p_user->io_putc( p_user, c );
    } else {
        return -1;
    }
}

/******************************************************************************/
static int _puts( const char *str, void *f )
{
    aw_shell_user_t *p_user = (aw_shell_user_t *)f;
    if ( p_user ) {
        return p_user->io_puts( p_user, str );
    } else {
        return -1;
    }
}


/******************************************************************************/
void aw_shell_color_enable (aw_shell_user_t *p_user)
{
    if (p_user) {
        p_user->sc->p_opt->use_color = AW_TRUE;
    }
}

void aw_shell_color_disable (aw_shell_user_t *p_user)
{
    if (p_user) {
        p_user->sc->p_opt->use_color = AW_FALSE;
    }
}

aw_err_t aw_shell_color_set (aw_shell_user_t *p_user, const char *p_color)
{
#ifdef AW_SHELL_USE_COLOR
    if (p_user && p_color && p_user->sc->p_opt->use_color) {
        p_user->io_puts(p_user, p_color);
        return AW_OK;
    } else {
        return -AW_EINVAL;
    }
#endif

    return -AW_ENOTSUP;
}

// todo

int aw_shell_printf(aw_shell_user_t *p_user, const char *fmt, ... )
{
    va_list ap;
    int len;
    int line_feed = 0;

    if (p_user == NULL) {
        uint8_t data[256];
        va_start( ap, fmt );
        aw_vsnprintf((char *)data, 256, fmt, ap);
        va_end( ap );
        aw_kprintf((char *)data);
        return AW_OK;
    }

    AW_MUTEX_LOCK(p_user->mutex, AW_SEM_WAIT_FOREVER);
    {
        if (p_user->sc->des_exist) {
            p_user->io_puts(p_user, "\r");
            _cursor_pos_clr(p_user);
        }

        va_start( ap, fmt );
        len = aw_vfprintf_do( p_user, _putc, _puts, fmt, ap );
        if (fmt[strlen(fmt) - 1] == '\n') {
            line_feed = 1;
        }
        va_end( ap );

        // �л��з����ҵ�ǰ shell δִ������
        if (line_feed && p_user->sc->stat != STAT_RUNING) {
            _line_echo_clear(p_user->sc, p_user);

            if (p_user->sc->line_len > 0) {
                p_user->io_puts(p_user, (char *)p_user->sc->line_buf);
            }
        } else {
            p_user->sc->des_exist = AW_FALSE;
        }
    }
    AW_MUTEX_UNLOCK(p_user->mutex);

    return len;
}

static const char * __color_get_by_name (const char *func_name)
{
    static char color[] =  "\x1B[38;5;231m";
    int     i   = 0;
    int     sum = 0, tmp;
    uint8_t len;

    if (func_name == NULL) {
        return NULL;
    }

    len = strlen(func_name);

    for (; i < len; i++) {
        sum += func_name[i];
    }

    tmp = sum % 231;

    if (tmp % 2 == 1) {
        tmp = 230 - tmp;
    }

    while (1) {
        if (tmp == 0 || tmp == 1 || tmp == 8 || tmp == 16 || tmp == 17 || tmp == 18  ||
            tmp == 19 || tmp == 20 || tmp == 21 || tmp == 52 || tmp == 53 || tmp == 88 || 
            tmp == 89 || tmp == 124 || tmp == 125 || tmp == 126 || tmp == 160 || tmp == 161 || 
            tmp == 162 || tmp == 196 || tmp == 197 || tmp == 198) {

            tmp += 1;
        } else {
            break;
        }
    }

    aw_snprintf(color, sizeof(color), "\x1B[38;5;%dm", tmp);

    return color;
}

const char *aw_shell_color_get_by_name (const char *func_name)
{
    return __color_get_by_name(func_name);
}

void aw_shell_color_auto_set (aw_shell_user_t *p_user, const char *func_name)
{
    const char *color = NULL;

#ifdef AW_SHELL_USE_COLOR
    if (color == NULL) {
        color = __color_get_by_name(func_name);
    }

    if (p_user && color) {
        p_user->io_puts(p_user, color);
    }

    if (p_user && color == NULL) {
        p_user->io_puts(p_user, AW_SHELL_FCOLOR_DEFAULT);
    }
#endif
}

int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                const char *color,
                                const char *func_name,
                                const char *fmt,
                                ...)
{
    va_list ap;
    int len;
    int line_feed = 0;

    if (p_user == NULL) {
        uint8_t data[256];
        va_start( ap, fmt );
        aw_vsnprintf((char *)data, 256, fmt, ap);
        va_end( ap );
        aw_kprintf((char *)data);
        return AW_OK;
    }

    AW_MUTEX_LOCK(p_user->mutex, AW_SEM_WAIT_FOREVER);
    {

#ifdef AW_SHELL_USE_COLOR
        if (color == NULL) {
            color = __color_get_by_name(func_name);
        }

        if (p_user && color) {
            p_user->io_puts(p_user, color);
        }
#endif

        if (p_user->sc->des_exist) {
            p_user->io_puts(p_user, "\r");
            _cursor_pos_clr(p_user);
        }

        va_start( ap, fmt );
        len = aw_vfprintf_do( p_user, _putc, _puts, fmt, ap );
        if (fmt[strlen(fmt) - 1] == '\n') {
            line_feed = 1;
        }
        va_end( ap );

        /* �л��з����ҵ�ǰ shell δִ������*/
        if (line_feed && p_user->sc->stat != STAT_RUNING) {
            _line_echo_clear(p_user->sc, p_user);

            if (p_user->sc->line_len > 0) {
                p_user->io_puts(p_user, (char *)p_user->sc->line_buf);
            }
        } else {
            p_user->sc->des_exist = AW_FALSE;
        }
#ifdef AW_SHELL_USE_COLOR
        if (p_user && color) {
            p_user->io_puts(p_user, AW_SHELL_FCOLOR_DEFAULT);
        }
#endif
    }
    AW_MUTEX_UNLOCK(p_user->mutex);

    return len;
}
/******************************************************************************/
#ifdef AW_SHELL_USE_CHART

static void _cmd_help_print_by_chart (const char                *p_name,
                                      const char                *p_desc,
                                      struct aw_shell_user      *p_user)
{
    struct aw_shell_chart       chart;

    aw_shell_chart_init(&chart, p_user, AW_SHELL_CHART_ALIGN_MID, '-', '|');

    aw_shell_chart_item(&chart, NULL, 12);
    aw_shell_chart_item(&chart, NULL, 52);

    aw_shell_chart_line(&chart, p_name,   0, 0);

    chart.align_mode = AW_SHELL_CHART_ALIGN_LEFT;
    aw_shell_chart_line(&chart, p_desc,   1, 0);

    aw_shell_chart_finish(&chart, '=', NULL);
}

aw_local void _help_scan_by_chart (struct aw_shell_context *sc,
                                   struct aw_shell_user    *user)
{
    struct aw_shell_cmd_list   *list = &gcmd_list_local;

#ifdef AW_SHELL_USE_CMDSET
    struct aw_shell_cmdset     *cmdset = &_g_cmdset_list_local;
#endif
    const struct aw_shell_cmd  *cmd;
    struct aw_shell_chart       chart;
    uint8_t                     cur_line = 0;
    int                         name_id;
    int                         desc_id;
    int                         align_mode = AW_SHELL_CHART_ALIGN_MID;

    aw_shell_chart_init(&chart, user, align_mode, '-', '|');

    name_id = aw_shell_chart_item(&chart, NULL, 12);
    desc_id = aw_shell_chart_item(&chart, NULL, 52);

    aw_shell_chart_line_separator(&chart, '=', "commands");

#ifdef AW_SHELL_USE_CMDSET
    /* �������������������ӡ1���������� */
    if (!sc->is_cmdset) {
        /* ��ӡ1������ */
        while (list != NULL) {
            for (cmd = list->start; cmd < list->end; cmd++) {
                cur_line++;
                aw_shell_chart_line(&chart, cmd->name, name_id, cur_line);

                chart.align_mode = AW_SHELL_CHART_ALIGN_LEFT;
                aw_shell_chart_line(&chart, cmd->desc, desc_id, cur_line);
                chart.align_mode = align_mode;
            }
            list = list->next;
        }

        if (cmdset->next != NULL) {
            aw_shell_chart_line_separator(&chart, '=', "command set");
        }

        /* ��ӡ����Ľ������� */
        while (cmdset->next != NULL) {
            cmdset = cmdset->next;
            cur_line++;

            aw_shell_chart_line(&chart, cmdset->p_enter_name, name_id, cur_line);
            aw_shell_chart_line(&chart, cmdset->p_man, desc_id, cur_line);
        }
    } else {
        /* ��ӡ������2������ */
        if (sc->cur_cmdset) {
            list   = &sc->cur_cmdset->cmds;
            for (cmd = list->start; cmd < list->end; cmd++) {
                cur_line++;
                aw_shell_chart_line(&chart, cmd->name, name_id, cur_line);

                chart.align_mode = AW_SHELL_CHART_ALIGN_LEFT;
                aw_shell_chart_line(&chart, cmd->desc, desc_id, cur_line);
                chart.align_mode = align_mode;
            }
        }

        /* �˳����� */
        if (sc->cur_cmdset && sc->cur_cmdset->p_exit_name) {
            cur_line++;
            aw_shell_chart_line(&chart,
                                 sc->cur_cmdset->p_exit_name,
                                 name_id,
                                 cur_line);
            aw_shell_chart_line(&chart,
                                 "exit the command layer.",
                                 desc_id,
                                 cur_line);
        }
    }

#else
    /* ��ӡ1������ */
    while (list != NULL) {
        for (cmd = list->start; cmd < list->end; cmd++) {
            cur_line++;
            aw_shell_chart_line(&chart, cmd->name, name_id, cur_line);
            aw_shell_chart_line(&chart, cmd->desc, desc_id, cur_line);
        }
        list = list->next;
    }
#endif /* AW_SHELL_USE_CMDSET */

    aw_shell_chart_finish(&chart, '=', NULL);
}

#endif /* AW_SHELL_USE_CHART */

static void _cmd_help_print_by_tab (const char                *p_name,
                                    const char                *p_desc,
                                    struct aw_shell_user      *p_user)
{
    int i = 0;

    __shell_puts(p_user, "%s\t\t", p_name);
    while (p_desc[i] != 0) {
        if (p_desc[i] == '\n' || ((i % 64 == 0) && i > 0)) {
            p_user->io_puts(p_user, "\n\t\t");
            i++;
            continue;
        }
        if (isprint(p_desc[i])) {
            p_user->io_putc(p_user, p_desc[i]);
        }
        i++;
    }
    p_user->io_puts(p_user, "\r\n");
}

aw_local void _help_scan_by_tab (struct aw_shell_context *sc,
                                 struct aw_shell_user    *user)
{
    struct aw_shell_cmd_list   *list = &gcmd_list_local;
    struct aw_shell_cmdset     *cmdset = &_g_cmdset_list_local;
    const struct aw_shell_cmd  *cmd;

#ifdef AW_SHELL_USE_CMDSET
    /* �������������������ӡ1���������� */
    if (!sc->is_cmdset) {
        /* ��ӡ1������ */
        while (list != NULL) {
            for (cmd = list->start; cmd < list->end; cmd++) {
                _cmd_help_print(cmd->name, cmd->desc ,user);
            }
            list = list->next;
        }

        /* ��ӡ����Ľ������� */
        while (cmdset->next != NULL) {
            cmdset = cmdset->next;
            _cmd_help_print(cmdset->p_enter_name, cmdset->p_man, user);
        }
    } else {
        /* ��ӡ������2������ */
        if (sc->cur_cmdset) {
            list   = &sc->cur_cmdset->cmds;
            for (cmd = list->start; cmd < list->end; cmd++) {
                _cmd_help_print(cmd->name, cmd->desc ,user);
            }
        }

        /* �˳����� */
        if (sc->cur_cmdset && sc->cur_cmdset->p_exit_name) {
            _cmd_help_print(sc->cur_cmdset->p_exit_name, "exit the command layer." ,user);
        }
    }

#else
    /* ��ӡ1������ */
    while (list != NULL) {
        for (cmd = list->start; cmd < list->end; cmd++) {
            _cmd_help_print(cmd->name, cmd->desc ,user);
        }
        list = list->next;
    }
#endif /* AW_SHELL_USE_CMDSET */
}

static void _cmd_help_print (const char                *p_name,
                             const char                *p_desc,
                             struct aw_shell_user      *p_user)
{
#ifdef AW_SHELL_USE_CHART
    if (p_user->sc->p_opt->use_chart && p_user->sc->p_opt->use_escape_seq) {
        _cmd_help_print_by_chart(p_name, p_desc, p_user);
        return;
    }
#endif

    _cmd_help_print_by_tab(p_name, p_desc, p_user);
}


aw_local aw_const char hotkey_help_info[] = {
    "Shell system shortcuts:\n"
    "-[Tab-key]:list all commands\n"
    "-[Up-key]:display last history command\n"
    "-[Down-key]:display next history command\n"
    "-[Left-key/Right-key]:moving the cursor\n"
    "-[Backspace-key]:delete character\n"
    "-[Enter-key]:send command\n\n\n"
};

aw_local aw_const char tips_help_info[] = {
    "\nFor more information on a command,please try \"command --help\"\n\n"
};
aw_local void _help_scan (struct aw_shell_context *sc,
                          struct aw_shell_user    *user)
{
#ifdef AW_SHELL_USE_CHART
    if (sc->p_opt->use_chart && sc->p_opt->use_escape_seq) {
        _help_scan_by_chart(sc, user);
        __shell_puts(user,tips_help_info);
        __shell_puts(user,hotkey_help_info);
        return;
    }
#endif

    _help_scan_by_tab(sc, user);
    __shell_puts(user,tips_help_info);
    __shell_puts(user,hotkey_help_info);
    return ;
}


/* help [command] - help */
static int help (int argc, char *argv[], struct aw_shell_user *p_user)
{
    const struct aw_shell_cmd  *cmd_find;
    struct aw_shell_cmdset     *cmdset;
    const char                 *ptr; /* a dumy pointer */
    struct aw_shell_context    *sc = p_user->sc;
    char                        buf[AW_SHELL_LINE_SIZE + 1];

    if (0 == argc) { /* If no argument present, print all command names */
        _help_scan(p_user->sc, p_user);

    } else { /* Find command name match with arg */

        memcpy(buf, argv[0], strlen(argv[0]) + 1);
        if (__cmd_alias_find(p_user, buf) == AW_OK) {
            __shell_puts(p_user, "alias : \"%s\".\n", buf);
            return AW_OK;
        }

#ifdef AW_SHELL_USE_CMDSET
        if (!sc->is_cmdset) {
            /* Find commands */
            cmd_find = _cmd_find(argv[0], (const char **) &ptr);
            if (cmd_find != NULL) {
                _cmd_help_print(cmd_find->name, cmd_find->desc, p_user);
            } else {
                cmdset   = _cmdset_find(argv[0]);
                if (NULL != cmdset) {
                    _cmd_help_print(cmdset->p_enter_name,
                                    cmdset->p_man,
                                    p_user);
                }
            }
        } else {
            cmd_find = _cmdset_cmd_find(sc->cur_cmdset, argv[0], &ptr);
            if (cmd_find != NULL) {
                _cmd_help_print(cmd_find->name, cmd_find->desc, p_user);

            } else if (strncmp(argv[0], "../", 3) == 0) {

                _mem_cpyl(argv[0], argv[0] + 3, strlen(argv[0]));
                cmd_find = _cmd_find(argv[0], (const char **) &argv[0]);
                if (cmd_find != NULL) {
                    _cmd_help_print(cmd_find->name, cmd_find->desc, p_user);
                }
            }
        }
#else
        /* Find commands */
        cmd_find = _cmd_find(argv[0], (const char **) &ptr);
        if (cmd_find != NULL) {
            _cmd_help_print(cmd_find->name, cmd_find->desc, p_user);
        }
#endif /* AW_SHELL_USE_CMDSET */
    }
    return AW_OK;
}

/******************************************************************************/
/* clr - clear VT100 console screen */
static int clr (int argc, char *argv[], struct aw_shell_user *p_user)
{
    (void)argc;
    (void)argv;

    /* Clear VT100 srceen and goto [0, 0] */
    if (p_user->sc->p_opt->use_escape_seq) {
        p_user->io_puts(p_user, "\033[2J" "\033[0;0H");
    } else {
        p_user->io_puts(p_user, "\n\n");
    }
//    _line_echo_clear(p_user->sc, p_user);

    return AW_OK;
}


aw_local aw_const char history_help_info[] = {
    "Command:history [id]\n"
    "        history [-clr]\n\n"
    "Description:Get the history Shell command and\n"
    "            clear the history Shell command\n\n"
    "Examples:\n"
    "    1.history 1\n"
    "    2.history -clr\n"
};

/******************************************************************************/
#ifdef AW_SHELL_USE_HISTORY
static int history (int argc, char *argv[], struct aw_shell_user *p_user)
{
    if(argc ==1 && !strcmp(argv[0],"--help")){
        __shell_puts(p_user,history_help_info);
        return AW_OK;
    }

    char buf[AW_SHELL_LINE_SIZE + 1];
    char *tmp;
    int  id = 0;
    int  index = 0;
    struct aw_shell_context *sc = p_user->sc;

    if (!sc->p_opt->use_history ) {
        return AW_OK;
    }

    if (argc == 0) {
        /* �Ӿɵ��� */
        index = p_user->sc->his.count;
        while (id < p_user->sc->his.count) {
            _history_get(&p_user->sc->his, (uint8_t *)buf, index);
            __shell_puts(p_user, "[%d] %s\r\n", id, buf);

            id++;
            index--;
        }

        return AW_OK;
    }

    /* history [id] : ֱ�����и�id ������ɱ༭����� */
    if (argc == 1 && isdigit(*argv[0])) {
        index = strtol(argv[0], 0 ,10);
        index = p_user->sc->his.count - index;

        /**
         * index�ķ�ΧΪ 1~his.count��
         * index Ϊ1 �������µ�history�������µ�history�ǵ�ǰ������
         */
        if (index <= 1) {
            return -AW_EINVAL;
        }
        _history_get(&p_user->sc->his, (uint8_t *)buf, index);
        tmp = buf;
        skip_spaces(tmp);

        /* ������history����history [id] */
        if (strncmp(tmp, "history ", strlen("history ")) == 0) {
            tmp = tmp + strlen("history");
            skip_spaces(tmp);
            if (*tmp != 0) {
                return -AW_EINVAL;
            }
        }
        aw_shell_puts(p_user, buf);
        aw_shell_putc(p_user, '\n');

        if (!sc->is_cmdset) {
            aw_shell_system_ex(p_user, NULL, buf);
        } else {
            aw_shell_system_ex(p_user, sc->cur_cmdset->p_enter_name, buf);
        }

        return AW_OK;
    }

    if (aw_shell_args_get(p_user, "-clr", 0) == AW_OK) {
        p_user->sc->his.count  = 0;
        p_user->sc->his.newest = 0;
        p_user->sc->his.oldest = 0;
        p_user->sc->his_index  = 0;

        return AW_OK;
    }

    return -AW_EINVAL;
}
#endif /* AW_SHELL_USE_HISTORY */

aw_local aw_const char shell_help_info[] = {
        "Command:shell [-echo] [string]\n"
        "        shell [-desc] [string]\n"
        "        shell [-desccol] [color string]\n\n"
        "[color string]:\n"
        "\tblack     : \\e[30m   \n"
        "\tred       : \\e[31m   \n"
        "\tgreen     : \\e[32m   \n"
        "\tyellow    : \\e[33m   \n"
        "\tblue      : \\e[34m   \n"
        "\tmagenta   : \\e[35m   \n"
        "\tcyan      : \\e[36m   \n"
        "\twhite     : \\e[37m   \n\n"
        "Description:Echo a string,set shell description or set description color\n\n"
        "Examples:\n"
        "    1.Echo a string:\n"
        "      shell -echo string\n"
        "    2.Set shell description:\n"
        "      shell -desc Aworks_test\n"
        "    3.Set the description color to yellow:\n"
        "      shell -desccol \\e[33m\n"
};

/******************************************************************************/
static int shell (int argc, char *argv[], struct aw_shell_user *sh)
{
    char *p_str;
    int len;

    if (argc == 0) {
        return -AW_EINVAL;
    }

    if (argc == 1 && (strcmp(argv[0], "--help")) == 0){
        __shell_puts(sh, shell_help_info);
        return AW_OK;
    }

    /*
     *  ������ʽ��-desc [arg1]
     *  ʹ��aw_shell_args_getʱ�����ĵ�һ���ַ�����Ϊ'-'������������ָʾ��
     *  ���ܻ���ȵ�һ���ַ���'-'��������ﲻʹ��aw_shell_args_get
     */
    if (argc == 2 && strncmp(argv[0], "-desc", 6) == 0) {
        p_str = argv[1];
        len = min((AW_SHELL_LINE_SIZE >> 1) - 1, strlen(p_str));
        memcpy(sh->sc->des_store, p_str, len);
        sh->sc->des_store[len] = 0;

        return AW_OK;
    } else if (aw_shell_args_get(sh, "-echo", 1) == AW_OK) {
        p_str = aw_shell_str_arg(sh, 1);
        __shell_puts(sh, p_str);

    } else if (aw_shell_args_get(sh, "-desccol", 1) == AW_OK) {
        p_str = aw_shell_str_arg(sh, 1);
        len = min((AW_SHELL_LINE_SIZE >> 1) - 1, strlen(p_str));
        memcpy(sh->sc->des_color, p_str, len);
        sh->sc->des_color[len] = 0;
    } else {
        return -AW_EINVAL;
    }

    return AW_OK;
}
/******************************************************************************/
/**
 * ÿ��ctx����һ��alist_list��ÿ��ʵ�����Լ��ı�����
 * �������ʱ��ֱ�Ӹ����Ѵ��ڱ�����
 * tab���ɲ鿴��������
 * ����֧�ֲ�ȫ�������ܻ����������ظ���
 * �����������������ͬ����ִ�б��������ȼ��ߣ���
 * �����������滻����Ӧ���ַ����������ӵı������κ�����ﶼ��ʹ�ã�
 * ��������һ������������ʹ�úͲ�ȫ
 */
static int __shell_alias_add (struct aw_shell_user *sh, const char *p_name, const char *p_str)
{
    struct __alias_info *p_pos;
    int name_len;
    int str_len;

    if (__shell_alias_find(sh, p_name, NULL, 0) == AW_OK) {
        __shell_alias_del(sh, p_name);
    }

    name_len = strlen(p_name) + 1;
    str_len  = strlen(p_str) + 1;

    p_pos = (struct __alias_info * )aw_mem_alloc(sizeof(*p_pos) + name_len + str_len);
    if (p_pos == NULL) {
        return -AW_ENOMEM;
    }

    aw_list_add(&p_pos->node, &sh->sc->alias_list);
    p_pos->p_name = (void *)((char *)((void *)p_pos) + sizeof(*p_pos));
    p_pos->p_str  = (void *)((char *)((void *)p_pos->p_name) + name_len);

    memcpy((void *)p_pos->p_name, (void *)p_name, name_len);
    memcpy((void *)p_pos->p_str, (void *)p_str, str_len);

    return AW_OK;
}

static int __shell_alias_del (struct aw_shell_user *sh, const char *p_name)
{
    struct __alias_info *p_pos;
    aw_bool_t  find = AW_FALSE;

    aw_list_for_each_entry(p_pos, &sh->sc->alias_list, struct __alias_info, node) {
        if (strcmp(p_name, p_pos->p_name) == 0){
            find = AW_TRUE;
            break;
        }
    }

    if(find ){
        aw_list_del_init(&p_pos->node);
        aw_mem_free(p_pos);
    }

    return AW_OK;
}

/**
 * \param p_to  ��������ַ�����buf����Ϊnull
 */
static int __shell_alias_find (struct aw_shell_user *sh,
                               const char *p_name,
                               char       *p_to,
                               uint32_t    to_size)
{
    struct __alias_info *p_pos;
    aw_bool_t  find = AW_FALSE;

    aw_list_for_each_entry(p_pos, &sh->sc->alias_list, struct __alias_info, node) {
        if (strcmp(p_name, p_pos->p_name) == 0){
            find = AW_TRUE;
            break;
        }
    }

    if (!find ) {
        return -AW_ENODATA;
    }

    if (p_to) {
        memset(p_to, 0, to_size);
        memcpy(p_to, p_pos->p_str, min(strlen(p_pos->p_str), to_size - 1));
    }
    return AW_OK;
}

aw_local aw_const char alias_help_info[] = {
    "Command:alias [another name] [name]\n"
    "        alias [-d] [alias]\n"
    "        alias [-list]\n\n"
    "Description:Set an alias for a command,delete an alias and\n"
    "            list all aliases\n\n"
    "Examples:\n"
    "    1.alias clear clr\n"
    "    2.alias -d clear\n"
    "    3.alias -list\n"
};

static int alias (int argc, char *argv[], struct aw_shell_user *sh)
{
    if(argc == 1 && !strcmp(argv[0],"--help")){
        __shell_puts(sh,alias_help_info);
        return AW_OK;
    }

    struct __alias_info *p_pos;

    if (argc < 1) {
        return -AW_EINVAL;
    }

    if (argv[0][0] != '-' && argc == 2) {
        __shell_alias_add(sh, argv[0], argv[1]);
        return AW_OK;
    }

    if (aw_shell_args_get(sh, "-d", 1) == AW_OK) {
        __shell_alias_del(sh, argv[1]);
    } else if (aw_shell_args_get(sh, "-list", 0) == AW_OK) {
        aw_list_for_each_entry(p_pos, &sh->sc->alias_list, struct __alias_info, node) {
            __shell_puts(sh, "%s=\"%s\"\n", p_pos->p_name, p_pos->p_str);
        }
    }else{
        return -AW_EINVAL;
    }

    return AW_OK;
}
/******************************************************************************/
// todo
static const struct aw_shell_cmd gcmd_local[] = {
    {help,     "help",     "Provide help information for AWorks system commands"},
    {clr,      "clr",      "Clear screen"},
    {alias,    "alias",    "Set alias,delete alias or list all alias"},
    {shell,    "shell",    "Echo a string,set shell description or set description color"},

#ifdef AW_SHELL_USE_HISTORY
    {history,  "history",  "Get history or clear all history."}
#endif /* AW_SHELL_USE_HISTORY */
};

static struct aw_shell_cmd_list gcmd_list_local = {
    NULL,
    &gcmd_local[0],
    &gcmd_local[count_of(gcmd_local)]
};



static struct _shell_comp _g_local_comp = {
    NULL,
    NULL,
    NULL
};


/**
 * \brief shell put a char
 */
int aw_shell_putc (aw_shell_user_t *sh, int c)
{
    if (sh->io_putc) {
        return sh->io_putc(sh, c);
    } else {
        return -AW_ENOTSUP;
    }
}

/**
 * \brief shell put a string
 */
int aw_shell_puts (aw_shell_user_t *sh, const char *str)
{
    if (sh->io_puts) {
        return sh->io_puts(sh, str);
    } else {
        return -AW_ENOTSUP;
    }
}

/**
 * \brief shell user getc
 *
 */
int aw_shell_getc (aw_shell_user_t *sh)
{
    if (sh->io_getc) {
        return sh->io_getc(sh);
    } else {
        return -AW_ENOTSUP;
    }
}

int aw_shell_try_getc (aw_shell_user_t *sh)
{
    if (sh->io_try_getc) {
        return sh->io_try_getc(sh);
    } else {
        return -AW_ENOTSUP;
    }
}


#ifdef  AW_SHELL_USE_HISTORY
aw_local aw_err_t _shell_history_init (struct aw_shell_user *sh,
                                       char                 *p_mem,
                                       uint32_t              mem_size)
{
    struct aw_shell_history *p_his = &sh->sc->his;
    if (sh == NULL || p_mem == NULL || mem_size == 0) {
        return -AW_EINVAL;
    }

    sh->sc->his_index = 0;

    p_his->buf     = (uint8_t *)p_mem;
    p_his->buf_num = mem_size / (AW_SHELL_LINE_SIZE + 1);
    p_his->newest  = 0;
    p_his->oldest  = 0;
    p_his->count   = 0;

    return AW_OK;
}
#endif /*  AW_SHELL_USE_HISTORY */

/*******************************************************************************
    ��ȫ����
*******************************************************************************/

#ifdef AW_SHELL_USE_COMPLETE

/** \brief ��ȫ����Ϣ�ṹ��  */
struct _str_complete_info {

    struct aw_shell_user *user;

    /** \brief ��ŵ�����Ҫ��ȫ������  */
    char             *p_from;

#define _STR_COMPLETE_BUF_SIZE    AW_SHELL_LINE_SIZE
    /** \brief ���ڴ�Ų�ȫ�������  */
    char              to[_STR_COMPLETE_BUF_SIZE];

    const char       *color;

    /** \brief ��ŵ��ǵ�һ��ƥ����ַ���  */
    const char       *p_once;
    const char       *p_once_color;

    /** \brief �ҵ�ƥ����ַ���������  */
    uint32_t          obj_cnt;

    /** \brief ���ڼ����ӡƥ���ַ�����λ��  */
    uint8_t           pr_pos;

    /** \brief һ���ܻ��Ե����������  */
    uint8_t           echo_num;

    char              space[_STR_COMPLETE_BUF_SIZE];

    struct aw_shell_chart   *p_chart;
    uint8_t                  chart_line;
};

/* ��ͷ��ʼɨ���ַ�����������ͬ�Ĳ��֣�һ������ͬ���ַ�������ȫ���� */
aw_local void _str_scan_reserve(char *p_to, const char *p_src)
{
    uint8_t len = strlen(p_to);
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        if (p_to[i] != p_src[i]) {
            memset(p_to + i, 0, len - i);
            return;
        }
    }
}

aw_local aw_bool_t _is_match_from_head (const char *p_src, char  *p_buf)
{
    char *p_addr = NULL;

    /* ���p_buf[0] = 0 ��p_addr��ֱ�ӵ���p_src */
    p_addr = strstr(p_src, p_buf);

    return (p_addr == p_src) ? AW_TRUE : AW_FALSE;
}

aw_local int _cmd_print (const char                *p_cmd,
                         struct _str_complete_info *p_info,
                         const char                *color)
{
    struct aw_shell_user *user = p_info->user;

    if (user->sc->p_opt->use_chart) {
        /* һ�еĳ��ȳ���80���ַ��� ���� */
        if (p_info->pr_pos >= p_info->echo_num) {
            p_info->pr_pos = 0;
            p_info->chart_line++;
        }

        if (p_info->p_once) {
            aw_shell_color_set(user, p_info->p_once_color);
            user->io_puts(user, "\r\n");


            aw_shell_chart_line(p_info->p_chart,
                                p_info->p_once,
                                p_info->pr_pos,
                                p_info->chart_line);
            p_info->pr_pos++;
        }

        aw_shell_color_set(user, color);
        aw_shell_chart_line(p_info->p_chart,
                            p_cmd,
                            p_info->pr_pos,
                            p_info->chart_line);
        p_info->pr_pos++;
        aw_shell_color_set(user, AW_SHELL_FCOLOR_DEFAULT);
    } else {

        if (p_info->p_once) {
            p_info->pr_pos += strlen(p_info->p_once) + 8;
            aw_shell_color_set(user, p_info->p_once_color);
            __shell_puts(user, "\r\n%s\t\t", p_info->p_once);
        }

        p_info->pr_pos += strlen(p_cmd) + 8;
        /* һ�еĳ��ȳ���80���ַ��� ���� */
        if (p_info->pr_pos >= AW_SHELL_LINE_SIZE) {
            p_info->pr_pos = strlen(p_cmd) + 8;
            user->io_puts(user, "\r\n");
        }

        aw_shell_color_set(user, color);
        __shell_puts(user, "%s\t\t", p_cmd);
        aw_shell_color_set(user, AW_SHELL_FCOLOR_DEFAULT);
    }

    return AW_OK;
}

/* �ַ����Ĳ�ȫ����ʾ */
aw_local int _cmd_complete (const char                 *p_src,
                            struct _str_complete_info  *p_info)
{
    if (p_src == NULL) {
        return -AW_ENODATA;
    }

    /* �ж������Ƿ���׵�ַ��ʼƥ�䣻���p_info->p_from[0] = 0 ��ֱ���ж�Ϊƥ�� */
    if (_is_match_from_head(p_src, p_info->p_from)) {

         p_info->obj_cnt++;

         if (p_info->obj_cnt == 1) {
             /* ��һ��ƥ�䵽���ַ����ᱣ�浽to�p_info->p_onceֻ��ʾһ��  */
             p_info->p_once       = p_info->to;
             p_info->p_once_color = p_info->color;
             strncpy(p_info->to, p_src, _STR_COMPLETE_BUF_SIZE);

         } else if (p_info->obj_cnt > 1) {

             /* ���ظ�ƥ�䵽���ַ��������������ж�����ԣ������p_once���ӡp_once */
             _cmd_print(p_src, p_info, p_info->color);
             if (p_info->p_once) {
                 p_info->p_once       = NULL;
                 p_info->p_once_color = NULL;
             }

             /* ��ͷ��ʼɨ���ַ�����������ͬ�Ĳ��֣�һ������ͬ���ַ�������ȫ���� */
             _str_scan_reserve(p_info->to, p_src);
         }
     }

    return AW_OK;
}

static void _comp_color_set (struct _str_complete_info  *p_info,
                             const char                 *color)
{
    if (p_info) {
        p_info->color = color;
    }
}

/* ��ȫ1�������������� */
aw_local int _cmd_list_complete (struct aw_shell_cmd_list  *list,
                                 struct _str_complete_info  *p_info)
{
    const struct aw_shell_cmd *cmd;

    while (list != NULL) {
        for (cmd = list->start; cmd < list->end; cmd++) {
            _cmd_complete(cmd->name, p_info);
        }
        list = list->next;
    }

    return AW_OK;
}

/* �Զ�����ַ�����ȫ���� */
aw_local void _custom_auto_complete (struct aw_shell_context   *sc,
                                     struct _str_complete_info *p_info)
{
    const char    **str;

#ifdef AW_SHELL_USE_CMDSET

    if (!sc->is_cmdset) {
        struct _shell_comp *p_comp = &_g_local_comp;

        while (p_comp->next != NULL) {
            p_comp = p_comp->next;

            for (str = p_comp->start; str < p_comp->end; str++) {
                if (*str == NULL) {
                    continue;
                }
                _cmd_complete(*str, p_info);
            }
        }
    } else if (sc->cur_cmdset &&
               sc->cur_cmdset->comp.start != NULL){

        for (str = sc->cur_cmdset->comp.start;
             str < sc->cur_cmdset->comp.end;
             str++) {
            _cmd_complete(*str, p_info);
        }
    }
#else
    struct _shell_comp *p_comp = &_g_local_comp;
    while (p_comp->next != NULL) {
        p_comp = p_comp->next;

        for (str = p_comp->start; str < p_comp->end; str++) {
            if (*str == NULL) {
                continue;
            }
            _cmd_complete(*str, p_info);
        }
    }
#endif /* AW_SHELL_USE_CMDSET */
}


aw_local aw_err_t __cmd_alias_complete (struct aw_shell_user       *user,
                                        struct _str_complete_info  *p_info)
{
    struct __alias_info *p_pos;

    aw_list_for_each_entry(p_pos, &user->sc->alias_list, struct __alias_info, node) {
        _cmd_complete(p_pos->p_name, p_info);
    }

    return AW_OK;
}

/* tab�� ����  ����������ӡ�� 1��������ȫ */
aw_local int _shell_cmd_auto_complete (struct aw_shell_context *sc,
                                       struct aw_shell_user    *user,
                                       uint8_t                 *p_src,
                                       struct aw_shell_chart   *p_chart)
{
    struct aw_shell_cmdset    *cmdset   = &_g_cmdset_list_local;
    struct _str_complete_info  comp_info;

    memset(&comp_info, 0, sizeof(comp_info));

    comp_info.user         = user;
    comp_info.p_from       = (char *)p_src;
    comp_info.pr_pos       = 0;
    comp_info.echo_num     = 4;  // echo maxlen 80
    comp_info.p_chart      = p_chart;
    comp_info.chart_line   = 0;

#ifdef AW_SHELL_USE_CMDSET

    /* ��ȫ������������������������ظ���������������ʾ��������������ִ�е� */
    __cmd_alias_complete(user, &comp_info);

    if (!sc->is_cmdset) {
        /* ��һ������ */
        _cmd_list_complete(&gcmd_list_local, &comp_info);

        _cmdset_complete(cmdset, &comp_info);

    /* �ڶ������� */
    } else if (sc->is_cmdset && sc->cur_cmdset) {

        _cmd_list_complete(&sc->cur_cmdset->cmds, &comp_info);

        /* �˳����� */
        _cmd_complete(sc->cur_cmdset->p_exit_name, &comp_info);
    }
#else
    _cmd_list_complete(&gcmd_list_local, &comp_info);
#endif /* AW_SHELL_USE_CMDSET */

    _comp_color_set(&comp_info, AW_SHELL_FCOLOR_ORANGE);
    _custom_auto_complete(sc, &comp_info);

    if (comp_info.obj_cnt == 0) {
        /* Ĭ��help�Ĳ�ȫ */
        _cmd_complete("help", &comp_info);
    }

    if (comp_info.obj_cnt > 0) {
        strcpy((char *)p_src, comp_info.to);
    }

    return comp_info.obj_cnt;
}

/*******************************************************************************
     �ļ���ȫ����ʵ��
*******************************************************************************/
static int __path_back_a_dir (char *path, int offset)
{
    if (offset < 2) { /* if we reach the root '/' */
        return -AW_EINVAL;
    }
    if ('/' == path[offset]) { /* ignore the last '/' in path */
        offset--;
    }
    while (path[offset] != '/') {
        offset--;
    }
    return offset;
}

static int __path_cat (char *cwd, const char *path)
{
    char ch;
    int  err = 0, len = strlen(cwd) - 1;

    if ('/' == *path) {             /* is an absolute path */
        len = 0;
    } else if ('/' != cwd[len]) {   /* add a '/' to end of path */
        cwd[++len] = '/';
    }
    while ((ch = *path++) != '\0') {
     /* cwd[len + 1] = '\0'; */ /* for debug only */
        if ('/' == ch) {
            if ('/' != cwd[len]) {
                cwd[++len] = '/';
            }
            continue;
        } else if ('.' != ch) {
            cwd[++len] = ch;
            continue;
        }
        /* '.' find */
        if ('/' == *path) { /* case "./" */
            path++;
            continue;
        }
        if ('.' != *path) { /* case ".x", x != '.' */
            cwd[++len] = '.';
            continue;
        }
        /* ".." find */
        path++;
        if (('\0' == *path) || ('/' == *path)) { /* ".." or "../" */
            if ((len = __path_back_a_dir(cwd, len)) < 0) {
                err = -AW_EINVAL;
                break;
            }
        } else { /* invalid path */
            err = -AW_EINVAL;
            break;
        }
    }
    if ((len > 0) && ('/' == cwd[len])) {
        cwd[len] = '\0'; /* remove the last '/' */
    } else {
        cwd[len + 1] = '\0';
    }
    return err;
}

static int _path_name_get (char *path, char *filename, char *src)
{
    int err;
    int len;

    /* �Ȼ�ȡ��ǰ·�� */
    if (aw_getcwd(path, __PATH_MAX_LEN) == NULL) {
        return 0;
    }

    /* ���src��û��'/'����ֱ����Ϊ�ļ��� */
    if (strchr(src, '/') == NULL) {
        strncpy(filename, src, __PATH_MAX_LEN);
        return AW_OK;
    }

    /* build full path here */
    if ((err = __path_cat(path, src)) < 0) {
        return err;
    }

    len = strlen(path) - 1;
    /* ���src�����һ���ַ���'/',���������src����·����û���ļ��� */
    if (src[strlen(src) - 1] == '/') {
        filename[0]  = 0;
        return AW_OK;
    }

    /* ���ļ�����ȡ��filename�path��ֻ����·�� */
    while (path[len] != '/') {
        len--;
    }
    strncpy(filename, path + len + 1, __PATH_MAX_LEN);
    if (len == 0) {
        path[len + 1] = 0;
    } else {
        path[len] = 0;
    }

    return AW_OK;
}

// todo

/* tab�� ����  ����������ӡ�� 1��������ȫ */
aw_local int _shell_file_auto_complete (struct aw_shell_context  *sc,
                                        struct aw_shell_user     *user,
                                        uint8_t                  *p_src,
                                        struct aw_shell_chart    *p_chart)
{
    int ret = 1;
    struct _str_complete_info  comp_info;
    int   len;
    char  path[__PATH_MAX_LEN + 1];
    char  filename[__PATH_MAX_LEN + 1];
    char  buf[__PATH_MAX_LEN + 1];
    struct aw_stat     sbuf;
    struct aw_dirent  *pdirent;
    struct aw_dir     *pdir     = NULL;

    memset(&comp_info, 0, sizeof(comp_info));
    _path_name_get(path, filename, (char *)p_src);

    comp_info.user         = user;
    comp_info.p_from       = filename;
    comp_info.pr_pos       = 0;
    comp_info.p_chart      = p_chart;
    comp_info.chart_line   = 0;
    comp_info.echo_num     = 4;

    if ((pdir = aw_opendir(path)) != NULL) {

        while ((pdirent = aw_readdir(pdir)) != NULL) {
            strcpy(buf, path);
            ret = __path_cat(buf, pdirent->d_name);
            ret = aw_stat(buf, &sbuf);
            if (ret == 0 && S_ISDIR(sbuf.st_mode)) {
//                /* ������ļ��������ֺ������'/' */
//                len = strlen(pdirent->d_name);
//                pdirent->d_name[len] = '/';
//                pdirent->d_name[len + 1] = '\0';

                _comp_color_set(&comp_info, AW_SHELL_FCOLOR_LGREEN);
            } else {
                _comp_color_set(&comp_info, AW_SHELL_FCOLOR_PINK);
            }

            _cmd_complete(pdirent->d_name, &comp_info);
        }
        aw_closedir(pdir);

        /* ��λ��p_src���'/'�ĵ�ַ��Ҫ��'/'��'/'֮ǰ���ַ������뵽to�� */
        len = strlen((char *)p_src);
        while (len > 0) {
            len--;
            if (p_src[len] == '/') {
                len++;
                break;
            }
        }
        _mem_cpyi(comp_info.to, strlen(comp_info.to), (char *)p_src, len, 0);
    }

    comp_info.p_from       = (char *)p_src;
    _comp_color_set(&comp_info, AW_SHELL_FCOLOR_ORANGE);
    _custom_auto_complete(sc, &comp_info);

    if (comp_info.obj_cnt > 0) {
        strcpy((char *)p_src, comp_info.to);
    }

    return comp_info.obj_cnt;
}


/* �Կո�Ϊ�߽磬��ȡbuf�ַ����е����һ���ַ��� */
static char * _last_str_get (char *buf, uint32_t len)
{
    if ( !len ) {
        return buf;
    }

    if (buf[len - 1] == ' ') {
        if (len > 1) {
            if ( buf[len - 2] != '\\') {
                return buf + len;  /*  ���һ���ַ�Ϊ�ո��򷵻��ַ��������ĵ�ַ */
            }
        } else {
            return buf + len;
        }
    }
    while (len > 0) {
        len--;
        if (buf[len] == ' ' && buf[len - 1] != '\\') {
            len++;
            break;
        }
    }
    return buf + len;
}

aw_local int _shell_complete (struct aw_shell_context *sc,
                              struct aw_shell_user    *user)
{
    int      ret = 0;
    char    *buf;
    char     buffer[128];
    uint32_t len;
    uint32_t offs = 0;
    char    *p_addr = NULL;
    aw_bool_t   cmdset_need_recovery = AW_FALSE;
    aw_bool_t   cmdset_need_clear    = AW_FALSE;
    struct aw_shell_chart chart;

    if (sc->p_opt->use_chart) {
        aw_shell_chart_init(&chart, user, 0, 0, 0);
        aw_shell_chart_item(&chart, NULL, 18);
        aw_shell_chart_item(&chart, NULL, 18);
        aw_shell_chart_item(&chart, NULL, 18);
        aw_shell_chart_item(&chart, NULL, 18);
    }

    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, sc->line_buf, sc->line_len + 1);
    buf = buffer;

    buf[sc->line_len] = 0;
    len = sc->line_len;

#ifdef AW_SHELL_USE_CMDSET
    /* �������"../"��ʼ������һ������ */
    if (_is_match_from_head(buf, "../") &&
        sc->is_cmdset) {
        _mem_cpyl(buf, buf + 3, sc->line_len);
        len -= 3;
        cmdset_need_recovery = AW_TRUE;
        sc->is_cmdset        = AW_FALSE;
    }
#endif /* AW_SHELL_USE_CMDSET */

    p_addr = _last_str_get(buf, len);
    offs = p_addr - buf;

#ifdef AW_SHELL_USE_CMDSET
    /*
     * �����һ���ַ����������ڶ����ַ�����Ϊ����������Ӧ��
     * �����ȫ��֮�����Ϊ������Ӧ���ļ�����ȫ���Զ����ַ�����ȫ;
     */
    if (p_addr != buf && !sc->is_cmdset ) {
        struct aw_shell_cmdset * cmdset = NULL;

        cmdset   = _cmdset_find(buf);
        if (cmdset != NULL) {
            cmdset_need_clear   = AW_TRUE;
            sc->is_cmdset       = AW_TRUE;
            sc->cur_cmdset      = cmdset;

            buf += strlen(cmdset->p_enter_name);
            skip_spaces(buf);
        }
    }

#endif /* AW_SHELL_USE_CMDSET */
    /*
     * �Կո�Ϊ�ָ�������һ���ַ���ֻ��ȫ���
     * �ڶ�����֮�������Ϊ���������Բ�ȫ�ļ����Զ�����ַ���;
     */
    if (p_addr == buf) {
        /* ��ȫ���� */
        ret = _shell_cmd_auto_complete(sc, user, (uint8_t *)p_addr, &chart);
        if (ret > 0) {
            len = strlen(p_addr);
        }
    } else {
        if (strncmp(buf, "help", 4) == 0) {
            /* ��ȫhelp�������  */
            ret = _shell_cmd_auto_complete(sc, user, (uint8_t *)p_addr, &chart);
            if (ret > 0) {
                len = strlen(p_addr);
            }
        } else {
            /* ��"\\ "ת��" " */
            _escape_process(p_addr);

            /* ��ȫ�ļ� */
            ret = _shell_file_auto_complete(sc, user, (uint8_t *)p_addr, &chart);

            if (ret > 0) {
//                    _space_trans(p_addr);
                /* ��" "ת��"\\ " */
                _escape_recovery(p_addr, ' ', 127 - offs);
                len = strlen(p_addr);
            }
        }
    }

#ifdef AW_SHELL_USE_CMDSET
    if (cmdset_need_recovery) {
        sc->is_cmdset = AW_TRUE;
        offs += 3;
    }

    if (cmdset_need_clear ) {
        sc->is_cmdset   = AW_FALSE;
        sc->cur_cmdset  = NULL;
        cmdset_need_clear = AW_FALSE;
    }
#endif /* AW_SHELL_USE_CMDSET */

    if (ret == 1) {
        _cursor_back_to_head(sc, user);

        memcpy(sc->line_buf + offs, p_addr, len + 1);

        sc->line_len   = strlen((char *)sc->line_buf);
        sc->cursor_pos = sc->line_len;

        user->io_puts(user, (char *)sc->line_buf);

    } else if (ret > 1) {
        if (sc->p_opt->use_chart) {
            aw_shell_chart_finish(&chart, 0, NULL);
        }

        memcpy(sc->line_buf + offs, p_addr, len + 1);

        sc->line_len   = strlen((char *)sc->line_buf);
        sc->cursor_pos = sc->line_len;

        user->io_puts(user, "\n");

        aw_shell_color_set(user, sc->des_color);
        user->io_puts(user, sc->des);
        aw_shell_color_set(user, AW_SHELL_FCOLOR_DEFAULT);

        user->io_puts(user, (char *)sc->line_buf);
    } else {
        return -AW_EINPROGRESS;
    }
}

static int _str_multi_clr ( const char  **start,
                            const char  **end )
{
    const char **tmpi;
    const char **tmpj;

    for (tmpi = start; tmpi < end; tmpi++) {

        tmpj = tmpi;
        for (tmpj++; tmpj < end; tmpj++) {
            if (strcmp(*tmpi, *tmpj) == 0) {
                *tmpj = NULL;
            }
        }
    }

    return AW_OK;
}

aw_err_t aw_shell_comp_add (const char  *cmdset_name,
                            const char **start,
                            uint8_t      nelements)
{
    const char **tmp;

    if (cmdset_name == NULL) {
        struct _shell_comp *p_comp = &_g_local_comp;

        while (p_comp->next != NULL) {
            p_comp = p_comp->next;
        }
        p_comp->next = (struct _shell_comp *)aw_mem_alloc(
                                sizeof(struct _shell_comp));
        if (p_comp->next == NULL) {
            return -AW_ENOMEM;
        }
        p_comp = p_comp->next;
        p_comp->start  = start;
        p_comp->end    = start + nelements;
        p_comp->next   = NULL;

        /* ��⵽���ظ����ַ������ظ����ַ������� */
        _str_multi_clr(start, start + nelements);
        for (tmp = start; tmp < p_comp->end; tmp++) {
            if (_cmdset_find(*tmp)) {
                *tmp = NULL;
            }
        }
    } else {
        struct aw_shell_cmdset *cur;

        cur = _cmdset_find(cmdset_name);
        if (cur == NULL) {
            return -AW_ENODEV;
        }
        cur->comp.start = start;
        cur->comp.end   = start + nelements;
        _str_multi_clr(start, start + nelements);
    }
    return AW_OK;
}

/* ���ַ����Ĳ���ƥ����ַ��������ַ���ת��"\\x"����ʽ */
static void _escape_recovery (char *src, char ch, int max_size)
{
    int len = 0;
    int src_len = strlen(src);
    int spare   = max_size - src_len - 1;

    while (len < max_size && src[len] != '\0' && spare > 0) {
        if (src[len] == ch) {
            switch (ch) {

            case '\x1B' :
                src[len] = 'e'; break;  /* ���Ʒ� */

            case '\a' :
                src[len] = 'a'; break;  /* ����� */

            case '\b' :
                src[len] = 'b'; break;  /* �˸�� */

            case '\f' :
                src[len] = 'f'; break;  /* ��ҳ�� */

            case '\n' :
                src[len] = 'n'; break;  /* ���з� */

            case '\r' :
                src[len] = 'r'; break;  /* �س��� */

            case '\t' :
                src[len] = 't'; break;  /* �����Ʊ�� */

            case '\v' :
                src[len] = 'v'; break;  /* �����Ʊ�� */

            case '\0' :
                src[len] = '0'; break;
            }

            _mem_cpyi(src, src_len + 1, "\\", 1, len);
            src_len++;
            spare--;
            len++;
        }
        len++;
    }
}


#else

static int _str_multi_clr ( const char  **start,
                            const char  **end ) {}
aw_err_t aw_shell_comp_add (const char  *cmdset_name,
                            const char **start,
                            uint8_t      nelements) {return -AW_ENOTSUP;}

aw_local int _shell_complete (struct aw_shell_context *sc,
                              struct aw_shell_user    *user) {return -AW_ENOTSUP;}
static void _escape_recovery (char *src, char ch, int max_size) {}
static char * _last_str_get (char *buf, uint32_t len) {return NULL;}
aw_local int _shell_file_auto_complete (struct aw_shell_context  *sc,
                                        struct aw_shell_user     *user,
                                        uint8_t                  *p_src,
                                        struct aw_shell_chart    *p_chart) {return -AW_ENOTSUP;}
static int _path_name_get (char *path, char *filename, char *src) {return -AW_ENOTSUP;}
static int __path_cat (char *cwd, const char *path) {return -AW_ENOTSUP;}
static int __path_back_a_dir (char *path, int offset) {return -AW_ENOTSUP;}
aw_local int _shell_cmd_auto_complete (struct aw_shell_context *sc,
                                       struct aw_shell_user    *user,
                                       uint8_t                 *p_src,
                                       struct aw_shell_chart   *p_chart) {return -AW_ENOTSUP;}
aw_local void _custom_auto_complete (struct aw_shell_context   *sc,
                                     struct _str_complete_info *p_info) {}

aw_local int _cmd_list_complete (struct aw_shell_cmd_list  *list,
                                 struct _str_complete_info  *p_info) {return -AW_ENOTSUP;}
static void _comp_color_set (struct _str_complete_info  *p_info,
                             const char                 *color) {}
aw_local int _cmd_complete (const char                 *p_src,
                            struct _str_complete_info  *p_info) {return -AW_ENOTSUP;}
aw_local int _cmd_print (const char                *p_cmd,
                         struct _str_complete_info *p_info,
                         const char                *color) {return -AW_ENOTSUP;}
aw_local aw_bool_t _is_match_from_head (const char *p_src, char  *p_buf) {return AW_FALSE;}
aw_local void _str_scan_reserve(uint8_t *p_to, const char *p_src) {}

#endif /* AW_SHELL_USE_COMPLETE */


/*******************************************************************************
    �������
*******************************************************************************/

#ifdef AW_SHELL_USE_CMDSET

/* ��ȫ��� */
aw_local int _cmdset_complete (struct aw_shell_cmdset    *cmdset,
                               struct _str_complete_info *p_info)
{
    while (cmdset->next != NULL){
        cmdset = cmdset->next;
        _cmd_complete(cmdset->p_enter_name, p_info);
    }

    return AW_OK;
}


static struct aw_shell_cmdset * _cmdset_find (const char *name)
{

    struct aw_shell_cmdset    *cmdset = &_g_cmdset_list_local;
    int                        len;

    while (cmdset->next != NULL) {
        cmdset   = cmdset->next;

        len = strlen(cmdset->p_enter_name);
        if (strncmp(name, cmdset->p_enter_name, len) == 0) {
            return cmdset;
        }
    }
    return NULL;
}

static const struct aw_shell_cmd *_cmdset_cmd_find (
        struct aw_shell_cmdset   *cmdset,
        const char                *name,
        const char               **cmd_arg)
{
    const struct aw_shell_cmd *cmd;
    struct aw_shell_cmd_list  *list;
    int                         len;
    const char                 *args;

    list = &cmdset->cmds;

    for (cmd = list->start; cmd < list->end; cmd++) {
        len = strlen(cmd->name);
        if (strncmp(name, cmd->name, len) != 0) {
            continue;
        }
        args = name + len; /* Get args */
        if (' ' != *args && '\0' != *args) { /* valid command ? */
            continue;
        }
        skip_spaces(args); /* Skip leading spaces */
        *cmd_arg = args;
        return cmd;
    }
    return NULL;
}

static aw_bool_t _is_cmdset_exit (
        struct aw_shell_cmdset   *cmdset,
        const char                *exit)
{
    int                         len;

    len = strlen(cmdset->p_exit_name);
    if (strncmp(exit, cmdset->p_exit_name, len) == 0) {
        return AW_TRUE;
    }
    return AW_FALSE;
}

/******************************************************************************/
static struct aw_shell_cmdset  _g_cmdset_list_local = {
    .next = NULL
};

aw_err_t aw_shell_register_cmdset (struct aw_shell_cmdset    *cmdset,
                                   const struct aw_shell_cmd *start,
                                   const struct aw_shell_cmd *end)
{
    struct aw_shell_cmdset    *local_list = &_g_cmdset_list_local;

    if (_cmdset_find(cmdset->p_enter_name)) {
        return -AW_EEXIST;
    }

    while (NULL != local_list->next && cmdset != local_list) {
        local_list = local_list->next;
    }

    if (cmdset != local_list) {
        cmdset->next       = NULL;
        local_list->next   = cmdset;
        cmdset->cmds.start = start;
        cmdset->cmds.end   = end;

        AW_SEMB_INIT(cmdset->init_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
        AW_SEMB_GIVE(cmdset->init_sem);

        if (cmdset->p_exit_name == NULL) {
            cmdset->p_exit_name = ".exit";
        }

        return AW_OK;
    }
    return -AW_EEXIST;
}

aw_err_t aw_shell_unregister_cmdset (struct aw_shell_cmdset *cmdset)
{
    struct aw_shell_cmdset *local_list = &_g_cmdset_list_local;

    while (NULL != local_list->next && cmdset != local_list->next) {
        local_list = local_list->next;
    }
    if (cmdset == local_list->next) {
        local_list->next = cmdset->next;

        AW_SEMB_TERMINATE(cmdset->init_sem);
        return AW_OK;
    }
    return -AW_ENOENT;
}

#else
aw_local int _cmdset_complete (struct aw_shell_cmdset    *cmdset,
                               struct _str_complete_info *p_info) {return -AW_ENOTSUP;}

static aw_bool_t _is_cmdset_exit (
        struct aw_shell_cmdset   *cmdset,
        const char                *exit) {return AW_FALSE;}

static const struct aw_shell_cmd *_cmdset_cmd_find (
        struct aw_shell_cmdset   *cmdset,
        const char                *name,
        const char               **cmd_arg) {return NULL;}

static struct aw_shell_cmdset * _cmdset_find (const char *name) {}

aw_err_t aw_shell_unregister_cmdset(struct aw_shell_cmdset *cmdset) {return -AW_ENOTSUP;}

aw_err_t aw_shell_register_cmdset( struct aw_shell_cmdset      *cmdset,
                                    const struct aw_shell_cmd   *start,
                                    const struct aw_shell_cmd   *end) {return -AW_ENOTSUP;}
#endif /* AW_SHELL_USE_CMDSET */







