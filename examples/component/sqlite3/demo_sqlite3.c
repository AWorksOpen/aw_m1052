/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief sqlite3���ݿ���ʾ����
 *
 * - �������裺
 *   1. �򿪴洢����غ�
 *      ��nandflash������aw_prj_params.hͷ�ļ���ʹ�ܣ�
 *      - NANDFLASH�豸��
 *      - AW_COM_FS_LFFS
 *      - AW_COM_CONSOLE
 *      ��SD��������aw_prj_params.hͷ�ļ���ʹ�ܣ�
 *      - AW_COM_SDCARD
 *      - ��Ӧƽ̨��SD��
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. ���ʹ��nandflash�洢������ر�SD_TEST�����ʹ��SD���洢���Կ���SD_TEST
 *   3. ���ӵ��Դ��ڵ�PC
 *
 * - ʵ������
 *   1. �� �洢�豸 �������ļ�ϵͳ��
 *   2. �� �洢�豸 �д���db�ļ������߲鿴���ļ��Ѵ���
 *   3. ���ڻ��ӡselect����õ���Ŀ�����Ϣ
 *   4. �ڶ������У������̲߳�ͣ��ӡĿ�����Ϣ
 *   5. ��д�����У������̻߳���Ի����100����Ϣ�����ؽ��101Ϊ����ִ��
 *
 * \par Դ����
 * \snippet demo_sqlite3.c src_sqlite3
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-07  cyl, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aw_vdebug.h"
#include "string.h"
#include "sqlite3.h"
#include "aw_mem.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_led.h"
#include "aw_shell.h"
#include "fs/aw_blk_dev.h"
#include "fs/aw_mount.h"
#include "fs/aw_fs_type.h"
#include "io/aw_io_shell.h"
#include "aw_demo_config.h"

/**************************** config begin ************************************/
#define SD_TEST

#ifndef SD_TEST
#define __DISK_NAME      DE_NAND_NAME    /* nandflash�豸�� */
#define __FS_NAME        "lffs"
#else
#define __DISK_NAME      DE_SD_DEV_NAME  /* SD���豸�� */
#define __FS_NAME        "vfat"
#endif

#define __MOUNT_NAME     "/mnt"

char *db_name = "/mnt/test.db";


char *sql_create_tbl = "create table person("
                       "id   integer primary key autoincrement unique,"
                       "name text,"
                       "age  integer)";

char *sql_insert = "insert into "
                   "person (name, age) "
                   "values (?, ?)";

char *sql_select = "select * from person";

char *sql_delete = "delete from person where id = 2";

/**************************** config end **************************************/



/**************************** threadsafe test *********************************/
#define AW_SQLITE_TASK_PRIO        3
#define AW_SQLITE_TASK_STACK_SIZE  8192

AW_TASK_DECL_STATIC(sqlite3_task1, AW_SQLITE_TASK_STACK_SIZE);
AW_TASK_DECL_STATIC(sqlite3_task2, AW_SQLITE_TASK_STACK_SIZE);


AW_MUTEX_DECL_STATIC(__sqlite_print_mutex);
AW_MUTEX_DECL_STATIC(__sqlite_write_mutex);

/**
 * \brief callback function
 */
static int callback(void *p_arg, int argc, char **argv, char **colname)
{
    int i;

    for (i = 0; i < argc; i++) {
        AW_MUTEX_LOCK(__sqlite_print_mutex, AW_SEM_WAIT_FOREVER);
        AW_INFOF(("%s: %s = %s\n",
                  (char *)p_arg,
                  colname[i],
                  argv[i] ? argv[i] : "NULL"));
        AW_MUTEX_UNLOCK(__sqlite_print_mutex);
    }

    AW_INFOF(("\n"));

    return 0;
}

/**
 * \brief sqlite3 read thread1, SQL(select)
 */
static void __sqlite3_thread_read1 (void *p_arg)
{
    int      rc     = SQLITE_OK;
    sqlite3 *db     = (sqlite3 *)p_arg;
    char    *errmsg = 0;

    AW_FOREVER {

        rc = sqlite3_exec(db,
                          sql_select,
                          callback,
                          (void *)"thread_read1",
                          &errmsg);
        if (rc != SQLITE_OK) {
            AW_INFOF(("__sqlite3_thread_read1: sqlite3_exec error, %s\n",
                      sqlite3_errmsg(db)));
            sqlite3_free(errmsg);
        }

        sqlite3_sleep(1000);

    }
}

/**
 * \brief sqlite3 read thread2, SQL(select)
 */
static void __sqlite3_thread_read2 (void *p_arg)
{
    int      rc     = SQLITE_OK;
    sqlite3 *db     = (sqlite3 *)p_arg;
    char    *errmsg = 0;

    AW_FOREVER {

        rc = sqlite3_exec(db,
                          sql_select,
                          callback,
                          (void *)"thread_read2",
                          &errmsg);
        if (rc != SQLITE_OK) {
            AW_INFOF(("__sqlite3_thread_read2: sqlite3_exec error, %s\n",
                      sqlite3_errmsg(db)));
            sqlite3_free(errmsg);
        }

        sqlite3_sleep(1000);

    }
}


/**
 * \brief sqlite3 write thread1 SQL(insert into)
 */
static void __sqlite3_thread_write1 (void *p_arg)
{
    int           i    = 0, j;
    int           rc   = SQLITE_OK;
    sqlite3      *db   = (sqlite3 *)p_arg;
    sqlite3_stmt *stat = 0;

    for (j = 0; j < 100; j++) {

        //todo: insert some data into table
        {
            AW_MUTEX_LOCK(__sqlite_write_mutex, AW_SEM_WAIT_FOREVER);

            i++;
            sqlite3_prepare(db,
                            sql_insert,
                            -1,
                            &stat,
                            NULL);
            sqlite3_bind_text(stat,
                              1,
                              "biili",
                              strlen("biili"),
                              NULL);
            sqlite3_bind_int(stat, 2, 24);

            rc = sqlite3_step(stat);
            AW_INFOF(("%d: __sqlite3_thread_write1: "
                      "sqlite3_step ec: %d\n",
                      i,
                      rc));

            sqlite3_finalize(stat);

            AW_MUTEX_UNLOCK(__sqlite_write_mutex);
        }

        sqlite3_sleep(1000);
    }

    sqlite3_close(db);
}


/**
 * \brief sqlite3 write thread2 SQL(insert into)
 */
static void __sqlite3_thread_write2 (void *p_arg)
{
    int           i    = 0, j;
    int           rc   = SQLITE_OK;
    sqlite3      *db   = (sqlite3 *)p_arg;
    sqlite3_stmt *stat = 0;

    for (j = 0; j < 100; j++) {

        //todo: insert some data into table
        {
            AW_MUTEX_LOCK(__sqlite_write_mutex, AW_SEM_WAIT_FOREVER);

            i++;
            sqlite3_prepare(db,
                            sql_insert,
                            -1,
                            &stat,
                            NULL);
            sqlite3_bind_text(stat,
                              1,
                              "werti",
                              strlen("werti"),
                              NULL);
            sqlite3_bind_int(stat, 2, 60);

            rc = sqlite3_step(stat);
            AW_INFOF(("%d: __sqlite3_thread_write2: "
                      "sqlite3_step ec: %d\n",
                      i,
                      rc));

            sqlite3_finalize(stat);

            AW_MUTEX_UNLOCK(__sqlite_write_mutex);
        }

        sqlite3_sleep(1000);
    }

    sqlite3_close(db);
}

/** \brief read all data */
static int __sqlite3_readall (int argc, char *argv[])
{
    sqlite3      *db   = NULL;
    sqlite3_stmt *stat = NULL;

    int   rc  = SQLITE_OK;
    char *sql = "select * from person";

    int id, age;
    const unsigned char *name;

    rc = sqlite3_open(argv[0], &db);
    if (SQLITE_OK != rc) {
        AW_INFOF(("Open dbfile(%s) error\n",
                  argv[0]));
        return AW_ERROR;
    }

    sqlite3_prepare(db,
                    sql,
                    -1,
                    &stat,
                    NULL);

    while (SQLITE_ROW == sqlite3_step(stat)) {
        id   = sqlite3_column_int(stat, 0);
        name = sqlite3_column_text(stat, 1);
        age  = sqlite3_column_int(stat, 2);

        AW_INFOF(("SELECT: id = %d, name = %s, age = %d.\n",
                  id,
                  name,
                  age));
    }

    sqlite3_finalize(stat);

    sqlite3_close(db);

    return AW_OK;
}


/** \brief sqlite3 cmd */
static const struct aw_shell_cmd __g_sqlite3_test_cmds[] = {
    {__sqlite3_readall, "sqlite3_readall", "<dbname>  - "
                        "read all the data which in the table person of dbname"}
};


static void __sqlite3_thread_safe_test (char *dbname)
{
    sqlite3      *db   = 0;

    int rc = sqlite3_open((char *)dbname, &db);
    if (rc != SQLITE_OK) {
        AW_INFOF(("thread test:open database error, %s\n",
                  sqlite3_errmsg(db)));
        sqlite3_close(db);
        return;
    }

/**
 * \brief ���ԣ� �����߳�ͬʱ�ڶ�
 *        ����Ҫ�Ӷ���Ķ���
 */
#if 1

    AW_MUTEX_INIT(__sqlite_print_mutex, AW_SEM_Q_FIFO);

    /* ��ʼ������sqlite3_task1 */
    AW_TASK_INIT(sqlite3_task1,              /* ����ʵ�� */
                "sqlite3_task1",             /* �������� */
                AW_SQLITE_TASK_PRIO,         /* �������ȼ� */
                AW_SQLITE_TASK_STACK_SIZE,   /* �����ջ��С */
                __sqlite3_thread_read1,      /* ������ں��� */
                 (void *)db);                /* ������ڲ��� */

    /* ��ʼ������sqlite3_task2 */
    AW_TASK_INIT(sqlite3_task2,              /* ����ʵ�� */
                "sqlite3_task2",             /* �������� */
                AW_SQLITE_TASK_PRIO,         /* �������ȼ� */
                AW_SQLITE_TASK_STACK_SIZE,   /* �����ջ��С */
                __sqlite3_thread_read2,      /* ������ں��� */
                 (void *)db);                /* ������ڲ��� */

    /* ��������sqlite3_task1 */
    AW_TASK_STARTUP(sqlite3_task1);

    /* ��������sqlite3_task2 */
    AW_TASK_STARTUP(sqlite3_task2);

#endif


/**
 * \brief ���ԣ� �����߳�ͬʱ��д
 *        ��Ҫ�Ӷ����д�������򽫻���ִ���
 *        �ڶ��̻߳����£�ֻҪ��д�������̣߳����ж�д�������̶߳�Ӧ�ü��϶����
 *        �������������ٽ���(���ݿ�)��
 */
#if 0

    static struct aw_shell_cmd_list cl;

    AW_MUTEX_INIT(__sqlite_print_mutex, AW_SEM_Q_FIFO);
    AW_MUTEX_INIT(__sqlite_write_mutex, AW_SEM_Q_FIFO);

    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_sqlite3_test_cmds);

    /* ��ʼ������sqlite3_task1 */
    AW_TASK_INIT(sqlite3_task1,              /* ����ʵ�� */
                "sqlite3_task1",             /* �������� */
                AW_SQLITE_TASK_PRIO,         /* �������ȼ� */
                AW_SQLITE_TASK_STACK_SIZE,   /* �����ջ��С */
                __sqlite3_thread_write1,     /* ������ں��� */
                 (void *)db);                /* ������ڲ��� */

    /* ��ʼ������sqlite3_task2 */
    AW_TASK_INIT(sqlite3_task2,              /* ����ʵ�� */
                "sqlite3_task2",             /* �������� */
                AW_SQLITE_TASK_PRIO,         /* �������ȼ� */
                AW_SQLITE_TASK_STACK_SIZE,   /* �����ջ��С */
                __sqlite3_thread_write2,     /* ������ں��� */
                 (void *)db);                /* ������ڲ��� */

    /* ��������sqlite3_task1 */
    AW_TASK_STARTUP(sqlite3_task1);

    /* ��������sqlite3_task2 */
    AW_TASK_STARTUP(sqlite3_task2);

#endif

    AW_FOREVER{
        sqlite3_sleep(3000);
    }
}

aw_local void __disk_evt_cb (const char *name, int event, void *p_arg)
{
    /* ͨ�� SDcard �Ŀ��豸�����ж��Ƿ�Ϊ SDcard ��� */
    if (strcmp(name, __DISK_NAME)) {
        return;
    }

    if (event == AW_BD_EVT_INJECT) {
        AW_INFOF(("sdcard insert...\r\n"));
    }

    if (event == AW_BD_EVT_EJECT) {
        AW_INFOF(("sdcard remove...\r\n"));
    }
}

aw_local aw_err_t fs_mount(void)
{
    aw_err_t ret;

#ifdef SD_TEST
    int event_index = 0;
    aw_bool_t regist_ok = AW_TRUE;
    /*
     * ��ӿ��豸����¼����, ����SDcard���ʱ, ��ص�__disk_evt_cb
     */
    ret = aw_blk_dev_event_register(__disk_evt_cb, NULL, &event_index);
    if (ret != AW_OK) {
        AW_ERRF(("block event register error: %d!\n", ret));
        regist_ok = AW_FALSE;
    }

    /* ���SD�����豸�Ƿ���� */
    ret = aw_blk_dev_wait_install(__DISK_NAME, 10000);
    if (ret != AW_OK) {
        AW_ERRF(("wait sdcard insert failed, err = %d\r\n", ret));
        goto __task_sd_fs_end;
    }
#endif

#if 0  /* ��ʽ��һ�μ���  */
#ifdef SD_TEST
    /* ����Ϊ"awdisk"�� ���СΪ4k  */
    struct aw_fs_format_arg fmt = {"awdisk", 1024 * 4, 0};
#else
    struct aw_fs_format_arg fmt = {"awdisk", 512, 0};
#endif
    /* �����ļ�ϵͳ   */
    ret = aw_make_fs(__DISK_NAME, __FS_NAME, &fmt);
    if (ret != AW_OK) {
        AW_ERRF(("failed: %d\n", ret));
#ifdef SD_TEST
        goto __task_sd_fs_end;
#endif
    }
    AW_INFOF(("make fs OK\n"));

#endif

    /* �ļ�ϵͳ���ص� MOUNT_NAME ���  */
    ret = aw_mount(__MOUNT_NAME, __DISK_NAME, __FS_NAME, 0);
    if (ret != AW_OK) {
        AW_ERRF(("mount error: %d!\n", ret));
    } else {
        AW_INFOF(("mount OK\n"));
    }

#ifdef SD_TEST
    __task_sd_fs_end:
    /* ע���¼� */
    if (regist_ok) {
        ret = aw_blk_dev_event_unregister(event_index);
        if (ret != AW_OK) {
            AW_ERRF(("block event unregister error: %d!\n", ret));
        }
    }
#endif

    return ret;
}


void demo_sqlite3 (void)
{
    sqlite3      *db      = 0;
    sqlite3_stmt *stat    = 0;
    char         *err_msg = 0;
    int           rc      = SQLITE_OK;

    int   id   = 0;
    const unsigned char *name = 0;
    int   age  = 0;

    char *sql_update = 0;

    //�����ļ�ϵͳ
    if (fs_mount() != AW_OK) {
        return;
    }

//    //ָ���߳�ģʽΪ����ģʽ
//    sqlite3_config(SQLITE_CONFIG_SERIALIZED);

    //��ӡsqlite3����ʱ�������߳�ģʽ�����߳�ģʽfalse������true
    AW_INFOF(("sqlite3 threadsafe: %d\n", sqlite3_threadsafe()));

    //todo: open a db file
    {
        rc = sqlite3_open(db_name, &db);
        if (rc) {
            AW_INFOF(("Can't open database: %s\n", sqlite3_errmsg(db)));
            sqlite3_close(db);
            return;
        }
    }

    //todo: create a table in db
    {
        rc = sqlite3_exec(db,
                          sql_create_tbl,
                          callback,
                          0,
                          &err_msg);
        if (rc != SQLITE_OK) {
            AW_INFOF(("SQL error: %s\n", err_msg));
            sqlite3_free(err_msg);
        }
    }

    //todo: insert some data into table
    //note: the num in sqlite3_bind_*()'s secondary param begin from 1
    {
        sqlite3_prepare(db,
                        sql_insert,
                        -1,
                        &stat,
                        NULL);
        sqlite3_bind_text(stat,
                          1,
                          "lily",
                          strlen("lily"),
                          NULL);
        sqlite3_bind_int(stat, 2, 18);

        sqlite3_step(stat);
        sqlite3_finalize(stat);
    }

    //todo: insert some data into table
    //note: the num in sqlite3_bind_*()'s secondary param begin from 1
    {
        sqlite3_prepare(db,
                        sql_insert,
                        -1,
                        &stat,
                        NULL);
        sqlite3_bind_text(stat,
                          1,
                          "toche",
                          strlen("toche"),
                          NULL);
        sqlite3_bind_int(stat, 2, 24);

        sqlite3_step(stat);
        sqlite3_finalize(stat);
    }

    //todo: insert some data into table
    //note: the num in sqlite3_bind_*()'s secondary param begin from 1
    {
        sqlite3_prepare(db,
                        sql_insert,
                        -1,
                        &stat,
                        NULL);
        sqlite3_bind_text(stat,
                          1,
                          "vi",
                          strlen("vi"),
                          NULL);
        sqlite3_bind_int(stat, 2, 19);

        sqlite3_step(stat);
        sqlite3_finalize(stat);
    }

    //todo: update data
    {
        sql_update = sqlite3_mprintf("update person set name = ?, age = ? where "
                                     "id = 3");
        sqlite3_prepare(db,
                        sql_update,
                        -1,
                        &stat,
                        NULL);
        sqlite3_bind_text(stat,
                          1,
                          "vivi",
                          strlen("vivi"),
                          NULL);
        sqlite3_bind_int(stat, 2, 20);

        sqlite3_step(stat);
        sqlite3_finalize(stat);
    }

    //todo: delete data
    {
        sqlite3_prepare(db,
                        sql_delete,
                        -1,
                        &stat,
                        NULL);

        sqlite3_step(stat);
        sqlite3_finalize(stat);
    }

    //todo: insert some data into table
    //note: the num in sqlite3_bind_*()'s secondary param begin from 1
    {
        sqlite3_prepare(db,
                        sql_insert,
                        -1,
                        &stat,
                        NULL);
        sqlite3_bind_text(stat,
                          1,
                          "olaf",
                          strlen("olaf"),
                          NULL);
        sqlite3_bind_int(stat, 2, 18);

        sqlite3_step(stat);
        sqlite3_finalize(stat);
    }

    //todo: read some data from table
    {
        sqlite3_prepare(db,
                        sql_select,
                        -1,
                        &stat,
                        NULL);
        while (SQLITE_ROW == sqlite3_step(stat)) {
            id   = sqlite3_column_int(stat, 0);
            name = sqlite3_column_text(stat, 1);
            age  = sqlite3_column_int(stat, 2);

            AW_INFOF(("SELECT: id = %d, name = %s, age = %d.\n",
                      id,
                      name,
                      age));
        }

        sqlite3_finalize(stat);
    }

    //todo: close a db
    {
        sqlite3_close(db);
    }

    //todo: some threadsafe test
    {
        __sqlite3_thread_safe_test(db_name);
    }

    return;
}

/* end of file */
