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
 * \brief ʱ���׼����
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_time.h
 *
 * \par ʹ��ʾ��
 * \code
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 13-09-06  zen, first implementation
 * \endinternal
 */

#ifndef __AW_TIME_H
#define __AW_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_time
 * \copydoc aw_time.h
 * @{
 */

#include "aworks.h"
#include "time.h"

/** \brief ����ʱ�䣬��1970 �� 1 �� 1 �� 0 ʱ 0 �� 0 �� ���������� */
typedef time_t aw_time_t;

/** \brief ����������ʾ������ʱ�� */
typedef struct aw_timespec {
    aw_time_t       tv_sec;     /**< \brief  �� */
    unsigned long   tv_nsec;    /**< \brief  ����  */
} aw_timespec_t;

/**
 * \brief ϸ��ʱ��, ���ṹ���� <time.h> �е� struct tm ����, ��ʹ�� <time.h> ��
 * �ĸ��������������� (ʹ��ǿ��ת�� (struct tm *)xx )
 */
typedef struct aw_tm {
    int tm_sec;     /**< \brief seconds after the minute     - [0, 59] */
    int tm_min;     /**< \brief minutes after the hour       - [0, 59] */
    int tm_hour;    /**< \brief hours after midnight         - [0, 23] */
    int tm_mday;    /**< \brief day of the month             - [1, 31] */
    int tm_mon;     /**< \brief months since January         - [0, 11] */
    int tm_year;    /**< \brief years since 1900     */
    int tm_wday;    /**< \brief days since Sunday            - [0, 6] */
    int tm_yday;    /**< \brief days since January 1         - [0, 365] */
    int tm_isdst;   /**< \brief Daylight Saving Time flag */
} aw_tm_t;

/**
 * \brief ��ȡ��ǰ����ʱ�� (�� <time.h> �� time() ��Ϊ��ͬ)
 *
 * \note ��ʹ�� aw_time_to_tm() ������ʱ��ת��Ϊϸ��ʱ��
 *
 * \param[out] p_time   ����ΪNULL, �򷵻�����ʱ��
 *
 * \return ��ǰ����ʱ��
 *
 * \par ʾ��
 * \code
 * #include "aw_time.h"
 *
 * aw_time_t time;      // ����ʱ��
 * aw_tm_t   tm;        // ϸ��ʱ��
 *
 * time = aw_time(NULL);                            // �õ�����ʱ��
 * printf("%s\n", ctime(const time_t *)&time);      // ��ӡ����ʱ��
 *
 * aw_time_to_tm(time, &tm);                        // ������ʱ��ת��Ϊϸ��ʱ��
 * printf("%s\n", asctime(const struct tm *)&tm);   // ��ӡϸ��ʱ��
 * \endcode
 */
aw_time_t aw_time(aw_time_t *p_time);

/**
 * \brief ��ȡ��ǰ����ʱ�� (�롢�����ʾ)
 *
 * ��� aw_time() aw_tm_get(), �������ܶ��ȡһ������ֵ��
 *
 * \note ��ʹ�� aw_tm_get() ֱ�ӻ�ȡ��ǰʱ���ϸ�ֱ�ʾ��ʽ
 *
 * \param[out] p_tv  ʱ��ֵ
 *
 * \retval AW_OK    �ɹ�
 * \retval -AW_EINVAL  ��������
 *
 * \par ʾ��
 * \code
 * #include "aw_time.h"
 *
 * aw_timespec_t timespec;      // ����ʱ��
 *
 * aw_timespec_get(&timespec);                            // �õ�����ʱ��
 * printf("%s", ctime((const time_t *)&timespec.tv_sec)); // ��ӡ����ʱ��
 * printf(" %d ms \n", timespec.tv_nsec / 1000000);       // ��ӡ����
 * \endcode
 * \sa aw_time() aw_timespec_set()
 */
aw_err_t aw_timespec_get(struct aw_timespec *p_tv);

/**
 * \brief ���õ�ǰ����ʱ��
 *
 * \note ��ʹ�� aw_tm_set() ��ϸ�ֱ�ʾ��ʽ�����õ�ǰʱ��
 *
 * \param[in]  p_tv  Ҫ���õ�ʱ��
 *
 * \retval AW_OK        �ɹ�
 * \retval -AW_EINVAL   ��������
 *
 * \par ʾ��
 * \code
 * #include "aw_time.h"
 *
 * aw_tm_t       tm;            // ϸ��ʱ��
 * aw_timespec_t timespec;      // ����ʱ��
 *
 * tm.tm_sec    = 6;
 * tm.tm_min    = 6;
 * tm.tm_hour   = 6;
 * tm.tm_mday   = 6;
 * tm.tm_mon    = 6 - 1;        // ʵ���·�Ҫ��1
 * tm.tm_year   = 2013 - 1900;  // ��1900������
 * tm.tm_isdst  = -1;           // ����ʱ������
 *
 * aw_tm_to_time(&tm, &timespec.tv_sec);    // ת��Ϊ����ʱ��
 * timespec.tv_nsec = 0;                    // ���벿����0
 *
 * aw_timespec_set(&timespec);              // ��������ʱ��
 *
 * \endcode

 * \sa aw_time() aw_timespec_Get()
 */
aw_err_t aw_timespec_set(struct aw_timespec *p_tv);

/**
 * \brief ��ȡ��ǰʱ�� (ϸ����ʽ��ʾ)
 *
 * \note ��ʹ�� aw_tm_get() ֱ�ӻ�ȡ��ǰʱ���ϸ�ֱ�ʾ��ʽ
 *
 * \param[out] p_tv  ʱ��ֵ
 *
 * \retval AW_OK        �ɹ�
 * \retval -AW_EINVAL   ��������
 *
 * \par ʾ��
 * \code
 * #include "aw_time.h"
 * aw_tm_t tm;          // ϸ��ʱ��
 *
 * aw_tm_get(&tm);      // �õ���ǰʱ��
 * printf("%s", asctime((const struct tm *)&tm)); // ��ӡʱ��
 *
 * printf("%d-%d-%d %d:%d:%d wday:%d yday:%d\n",
 *        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
 *        tm.tm_hour, tm.tm_min, tm.tm_sec,
 *        tm.tm_wday, tm.tm_yday);              // ��ӡʱ��
 *
 * \endcode
 * \sa aw_time() aw_tm_set()
 */
aw_err_t aw_tm_get (aw_tm_t *p_tv);

/**
 * \brief ���õ�ǰʱ��(ϸ����ʽ��ʾ)
 *
 * \param[in] p_tm      Ҫ���õ�ʱ��
 *
 * \retval AW_OK        �ɹ�
 * \retval -AW_EINVAL   ��������
 *
 * \par ʾ��
 * \code
 * #include "aw_time.h"
 *
 * aw_tm_t       tm;            // ϸ��ʱ��
 *
 * tm.tm_sec    = 6;
 * tm.tm_min    = 6;
 * tm.tm_hour   = 6;
 * tm.tm_mday   = 6;
 * tm.tm_mon    = 6 - 1;        // ʵ���·�Ҫ��1
 * tm.tm_year   = 2013 - 1900;  // ��1900������
 * tm.tm_isdst  = -1;           // ����ʱ������
 *
 * aw_tm_set(&tm);              // ��������ʱ��
 *
 * \endcode
 *
 * \sa aw_time() aw_timespec_Get()
 */
aw_err_t aw_tm_set (aw_tm_t *p_tm);

/**
 * \brief ��ϸ��ʱ����ʽת��Ϊ����ʱ����ʽ
 *
 * \param[in,out] p_tm    ϸ��ʱ��, ��Ա tm_wday, tm_yday �ڵ��ú󱻸���;
 *                        ������ʱ��Ч, tm_isdst Ϊ��, ����Ч��Ϊ0, ����Ϣ������
 *                        ��Ϊ��
 * \param[out]    p_time  ����ʱ��
 *
 * \retval AW_OK        �ɹ�
 * \retval -AW_EINVAL   ��������
 * \retval -AW_EBADF    ϸ��ʱ�����, ϸ��ʱ�����Чֵ��ο� aw_tm_t
 */
aw_err_t aw_tm_to_time (aw_tm_t *p_tm, aw_time_t *p_time);

/**
 * \brief ������ʱ����ʽת��Ϊϸ��ʱ����ʽ
 *
 * \attention ��������������
 *
 * \param[in]   p_time  ����ʱ��
 * \param[out]  p_tm    ϸ��ʱ��
 *
 * \retval AW_OK        �ɹ�
 * \retval -AW_EINVAL   ��������
 */
aw_err_t aw_time_to_tm (aw_time_t *p_time, aw_tm_t *p_tm);

/** @} grp_aw_if_time */

#ifdef __cplusplus
}
#endif

#endif /* __AW_TIME_H */

/* end of file */

