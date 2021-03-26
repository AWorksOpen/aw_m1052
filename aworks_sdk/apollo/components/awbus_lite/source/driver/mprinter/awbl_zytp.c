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
 * \brief  zytp driver
 *
 * \internal
 * \par modification history:
 *
 * - 1.00 16-04-25  bob, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include <string.h>

#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_sem.h"

#include "awbus_lite.h"
#include "driver/mprinter/awbl_zytp.h"
#include "aw_ioctl.h"
#include "aw_serial.h"
#include "aw_mprinter.h"



#define __EP24CXX_UNIT_DECL(unit, p_dev) \
                            uint8_t unit = \
                           (uint8_t)AWBL_DEV_UNIT_GET(p_dev)

/*******************************************************************************
  defines
*******************************************************************************/

#define __ZYTP_POS_NUM        62    /**< \brief ZYTP系列打印机支持的指令条数 */
#define __ZYTP_POS_DATA_NUM   3     /**< \brief ZYTP系列打印机最大指令宽度 */

struct zytp_pos {
    int      mp_cmd;                         /**< \brief 标准打印机功能码 */
    uint8_t  cmd_dats[__ZYTP_POS_DATA_NUM];  /**< \brief ZYTP系列打印机功能码数据 */
};

/**< \brief 驱动指令列表 */
struct zytp_pos __g_zytp_pos[__ZYTP_POS_NUM] =
{
    
    /** \brief 打印并进纸 (LF)*/
    {AW_MPCPRINT,     {0x0a, 0x00, 0x00}}, 
    
    /** \brief 回车(CR)*/
    {AW_MPCENTER,     {0x0d, 0x00, 0x00}}, 
    
    /** \brief 打印并将标签纸进纸到打印起始位置(FF)*/
    {AW_MPCPAFSP,     {0x0c, 0x00, 0x00}}, 
    
    /** \brief 将标签纸进纸到打印起始位置(GS FF)*/
    {AW_MPEXPANDTYPE, {0x1d, 0x0c, 0x00}}, 
    
    /** \brief 打印并进纸n点(GS FF)*/
    {AW_MPCPAFNP,     {0x1b, 0x4a, 0x00}}, 
    
    /** \brief 打印并退纸n点(ESC K)*/
    {AW_MPCPABNP,     {0x1b, 0x4b, 0x00}}, 
    
    /** \brief 打印并进纸n行(ESC d)*/
    {AW_MPCPAFNFL,    {0x1b, 0x64, 0x00}}, 
    
    /** \brief 打印并退纸n行(ESC e)*/
    {AW_MPCPAFBFL,    {0x1b, 0x65, 0x00}}, 
    
    /** \brief 设置打印位置为打印行起点(GS T)*/
    {AW_MPCSPPTFLS,   {0x1d, 0x54, 0x00}}, 
    
    /** \brief 设置行间距为n点(ESC 3) */
    {AW_MPCSLSNP,     {0x1b, 0x33, 0x00}}, 
    
    /** \brief 设置行间距为默认值33点(ESC 2) */
    {AW_MPCSLSDF,     {0x1b, 0x32, 0x00}}, 
    
    /** \brief 设置左边距(ESC l) */
    {AW_MPEXPANDTYPE, {0x1b, 0x6c, 0x00}}, 
    
    /** \brief 设置右边距(ESC Q) */
    {AW_MPEXPANDTYPE, {0x1b, 0x51, 0x00}}, 
    
    /** \brief 设置左侧空白量(GS L) */
    {AW_MPCSLFBLK,    {0x1d, 0x4c, 0x00}}, 
    
    /** \brief 设置右侧字符间距(ESC SP) */
    {AW_MPCSRGCSP,    {0x1b, 0x20, 0x00}}, 
    
    /** \brief 设置打印区域宽度(GS W) */
    {AW_MPCSPSWD,     {0x1d, 0x57, 0x00}}, 
    
    /** \brief 设置绝对打印位置(ESC $) */
    {AW_MPCSABSPP,    {0x1b, 0x24, 0x00}}, 
    
    /** \brief 设置相对打印位置(ESC \) */
    {AW_MPCSRLTPP,    {0x1b, 0x5c, 0x00}}, 
    
    /** \brief 设置字符打印方式(ESC !) */
    {AW_MPCSCPT,      {0x1b, 0x21, 0x00}}, 
    
    /** \brief 设置/解除下划线(ESC -) */
    {AW_MPCSRUNDL,    {0x1b, 0x2d, 0x00}}, 
    
    /** \brief 设置/解除粗体打印(ESC E) */
    {AW_MPCSRBFP,     {0x1b, 0x45, 0x00}}, 
    
    /** \brief 设置打印对齐方式(ESC a) */
    {AW_MPCSPALIT,    {0x1b, 0x61, 0x00}}, 
    
    /** \brief 设置/解除颠倒打印模式(ESC {) */
    {AW_MPCSRRVSMD,   {0x1b, 0x7b, 0x00}}, 
    
    /** \brief 设置/解除反白打印模式(GS B) */
    {AW_MPCSRANTWMD,  {0x1d, 0x42, 0x00}}, 
    
    /** \brief 设置打印灰度(ESC m) */
    {AW_MPEXPANDTYPE, {0x1b, 0x6d, 0x00}}, 
    
    /** \brief 设置打印速度(FS s) */
    {AW_MPEXPANDTYPE, {0x1c, 0x73, 0x00}}, 
    
    /** \brief 设置打印字体(ESC M) */
    {AW_MPCSPFONT,    {0x1b, 0x4d, 0x00}}, 
    
    /** \brief 设置/解除顺时针90°旋转(ESC V) */
    {AW_MPCSRCLKWSRT, {0x1b, 0x56, 0x00}}, 
    
    /** \brief 设定字符大小(GS !) */
    {AW_MPCSFNTSZ,    {0x1b, 0x21, 0x00}}, 
    
    /** \brief 选择汉字模式(FS &) */
    {AW_MPCSCHNMD,    {0x1c, 0x26, 0x00}}, 
    
    /** \brief 取消汉字模式(FS .) */
    {AW_MPCRCHNMD,    {0x1c, 0x2e, 0x00}}, 
    
    /** \brief 选择国际字符集(ESC R) */
    {AW_MPCSINTLFNT,  {0x1b, 0x52, 0x00}}, 
    
    /** \brief 选择字符代码页(ESC t) */
    {AW_MPCSFNTCDP,   {0x1b, 0x74, 0x00}}, 
    
    /** \brief 选择打印纸传感器以停止打印(ESC c 4) */
    {AW_MPCSSNRTSP,   {0x1b, 0x63, 0x34}}, 
    
    /** \brief 激活/禁止面板按键(ESC c 5) */
    {AW_MPCSRBLKKY,   {0x1b, 0x63, 0x35}}, 
    
    /** \brief 图形垂直取模数据填充(ESC *) */
    {AW_MPCGDMVP,     {0x1b, 0x2a, 0x00}}, 
    
    /** \brief 图片水平取模数据打印(GS v 0) */
    {AW_MPCGLMDP,     {0x1d, 0x76, 0x30}}, 
    
    /** \brief 打印NV位图(FS p) */
    {AW_MPCPNVIMG,    {0x1c, 0x70, 0x00}}, 
    
    /** \brief 下载NV位图(FS q) */
    {AW_MPCDLNVIMG,   {0x1c, 0x71, 0x00}}, 
    
    /** \brief 水平制表(HT) */
    {AW_MPCLVLCTTAB,  {0x09, 0x00, 0x00}}, 
    
    /** \brief 设置水平制表位置(ESC D) */
    {AW_MPCLVLCTTABP, {0x1b, 0x44, 0x00}}, 
    
    /** \brief 打印90°表格(FS V) */
    {AW_MPEXPANDTYPE, {0x1c, 0x56, 0x00}}, 
    
    /** \brief 设置条码可读字符（HRI）打印位置(GS H) */
    {AW_MPCSHRIPP,    {0x1d, 0x48, 0x00}}, 
    
    /** \brief 设置一维条码高度(GS h) */
    {AW_MPCSODCH,     {0x1d, 0x68, 0x00}}, 
    
    /** \brief 设置一维条码宽度(GS w) */
    {AW_MPCSODCW,     {0x1d, 0x77, 0x00}}, 
    
    /** \brief 设置一维条码字体(GS f) */
    {AW_MPCSODCF,     {0x1d, 0x66, 0x00}}, 
    
    /** \brief 打印一维条码(GS k) */
    {AW_MPCPODC,      {0x1d, 0x6b, 0x00}}, 
    
    /** \brief 设置/打印二维码(GS ( k) */   
    {AW_MPCSPQRC,     {0x1d, 0x28, 0x5b}}, 
    
    /** \brief 标签纸/黑标纸指令(FS ( L) */  
    {AW_MPCSLBABLKP,  {0x1c, 0x28, 0x4c}}, 
    
    /** \brief 设置调整值(GS ( F) */
    {AW_MPEXPANDTYPE, {0x1d, 0x28, 0x46}}, 
    
    /** \brief 实时查询打印机状态(DLE EOT) */
    {AW_MPCRTQPS,     {0x10, 0x04, 0x00}}, 
    
    /** \brief 设置/取消打印机状态自动返回(GS a) */
    {AW_MPCSRPSAB,    {0x1d, 0x61, 0x00}}, 
    
    /** \brief 非实时传送状态(GS r) */
    {AW_MPCDLSTAT,    {0x1d, 0x72, 0x00}}, 
    
    /** \brief 初始化打印机(ESC @) */
    {AW_MPCPINIT,     {0x1b, 0x40, 0x00}}, 
    
    /** \brief 清打印机缓存 */
    {AW_MPCRLCLPBUF,  {0x10, 0x14, 0x08}}, 
    
    /** \brief 进纸并切纸(GS V) */
    {AW_MPCFDCTPAP,   {0x1d, 0x56, 0x00}}, 
    
    /** \brief 设置串口通讯参数(GS ( E) */
    {AW_MPCSCMMPAP,   {0x1d, 0x28, 0x45}}, 
    
    /** \brief 选择打印控制模式(GS ( K) */
    {AW_MPCSPCTLMD,   {0x1d, 0x28, 0x4b}}, 
    
    /** \brief 选择打印头控制模式(GS E) */
    {AW_MPEXPANDTYPE, {0x1b, 0x45, 0x00}}, 
    
    /** \brief 传送打印机ID(GS I) */
    {AW_MPCDLPRTID,   {0x1b, 0x49, 0x00}}, 
    
    /** \brief 进入低功耗 */
    {AW_MPCSRLOWPMD,  {0x10, 0x14, 0x02}}, 
    
    /** \brief 设置读取打印机配置 */
    {AW_MPEXPANDTYPE, {0x1B, 0x5A, 0x00}}, 
};

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __zytp_inst_init1(awbl_dev_t *p_dev);
aw_local void __zytp_inst_init2(awbl_dev_t *p_dev);
aw_local void __zytp_inst_connect(awbl_dev_t *p_dev);

aw_local void * __zytp_open (struct aw_iodev *p_dev,
                             const char      *name,
                             int              oflag,
                             mode_t           mode,
                             int             *err);

aw_local int    __zytp_close (void             *pfd);
aw_local int    __zytp_ioctl (void             *pfd, int cmd, void  *arg);
aw_local ssize_t __zytp_write (void *p_fd, const void *p_buf, size_t nbyte);

aw_local int  __zytp_rsp_isr(void *p_arg, char  ch);
aw_local char * __zytp_pos_find(awbl_dev_t *p_dev, int cmd);
aw_local aw_err_t __zytp_expand_pos_chect(awbl_dev_t           *p_dev,
                                          aw_mprinter_params_t *p_mp_params);


/*******************************************************************************
  locals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_zytp_drvfuncs = {
    __zytp_inst_init1,
    __zytp_inst_init2,
    __zytp_inst_connect
};


/** \brief driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_zytp_drv_registration = {
    AWBL_VER_1,                         /**< \brief awb_ver */
    AWBL_BUSID_PLB,                     /**< \brief bus_id */
    AWBL_ZYTP_XX_NAME,                  /**< \brief p_drvname */
   &__g_zytp_drvfuncs,                  /**< \brief p_busfuncs */
    NULL,                               /**< \brief p_methods */
    NULL                                /**< \brief pfunc_drv_probe */
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief 数据接收回调函数
 */
aw_local int  __zytp_rsp_isr(void *p_arg, char  ch)
{
    struct awbl_zytp_dev *p_zytp_dev = (struct awbl_zytp_dev *)p_arg;
    aw_mprinter_params_t   *p_mp_param = p_zytp_dev->p_mp_param;

    /* 
     * 如果此时自动返回状态为FALSE，说明数据不是自动状态数据，而是主机主动读取
     * 微打设备的某些数据，此时数据存放到指令包rsp数据缓冲区中  
     */
    if (!p_zytp_dev->auto_stat_flag ) {
        p_mp_param->p_rsp_buf[p_mp_param->ret_len] = ch;
        p_mp_param->ret_len++;
        if (p_mp_param->ret_len == p_mp_param->rsp_len) {
            aw_semb_give(p_mp_param->sem_id);
        }
        return AW_OK;
    }


    /* 
     * 数据是自动状态返回数据，根据当前的状态判断哪些状态发生变化并通知应用，之
     * 后更新当前打印机的状态  
     */
    if ((p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_OVERVOLATGE)  &&
        (!(ch & AW_MPRINTER_STAT_FLAG_OVERVOLATGE))) {
        if (p_zytp_dev->pfn_over_vltg_resume) {
            p_zytp_dev->pfn_over_vltg_resume(p_zytp_dev->p_over_vltg_resume_arg);
        }
    } else if ((!(p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_OVERVOLATGE)) &&
               (ch & AW_MPRINTER_STAT_FLAG_OVERVOLATGE) ){
        if (p_zytp_dev->pfn_over_vltg) {
            p_zytp_dev->pfn_over_vltg(p_zytp_dev->p_over_vltg_arg);
        }
    } else {

    }


    if ((p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_AXOOPEN)  &&
        (!(ch & AW_MPRINTER_STAT_FLAG_AXOOPEN))) {
        if (p_zytp_dev->pfn_axo_close) {
            p_zytp_dev->pfn_axo_close(p_zytp_dev->p_axo_close_arg);
        }
    } else if ((!(p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_AXOOPEN)) &&
               (ch & AW_MPRINTER_STAT_FLAG_AXOOPEN) ){
        if (p_zytp_dev->pfn_axo_open) {
            p_zytp_dev->pfn_axo_open(p_zytp_dev->p_axo_open_arg);
        }
    } else {

    }

    if ((p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_PAPER_NONE)  &&
        (!(ch & AW_MPRINTER_STAT_FLAG_PAPER_NONE))) {
        if (p_zytp_dev->pfn_of_paper_resume) {
            p_zytp_dev->pfn_of_paper_resume(p_zytp_dev->p_of_paper_resume_arg);
        }
    } else if ((!(p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_PAPER_NONE)) &&
               (ch & AW_MPRINTER_STAT_FLAG_PAPER_NONE) ){
        if (p_zytp_dev->pfn_of_paper) {
            p_zytp_dev->pfn_of_paper(p_zytp_dev->p_of_paper_arg);
        }
    } else {
    }

    if ((p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_OVERHEAT)  &&
        (!(ch & AW_MPRINTER_STAT_FLAG_OVERHEAT))) {
        if (p_zytp_dev->pfn_over_heat_resume) {
            p_zytp_dev->pfn_over_heat_resume(p_zytp_dev->p_over_heat_resume_arg);
        }
    } else if ((!(p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_OVERHEAT)) &&
               (ch & AW_MPRINTER_STAT_FLAG_OVERHEAT) ){
        if (p_zytp_dev->pfn_over_heat) {
            p_zytp_dev->pfn_over_heat(p_zytp_dev->p_over_heat_resume_arg);
        }
    } else {

    }

    if ((p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_CUTTERON)  &&
        (!(ch & AW_MPRINTER_STAT_FLAG_CUTTERON))) {
        if (p_zytp_dev->pfn_cutter_off) {
            p_zytp_dev->pfn_cutter_off(p_zytp_dev->p_cutter_off_arg);
        }
    } else if ((!(p_zytp_dev->zytp_stat & AW_MPRINTER_STAT_FLAG_CUTTERON)) &&
               (ch & AW_MPRINTER_STAT_FLAG_CUTTERON) ){
        if (p_zytp_dev->pfn_cutter_on) {
            p_zytp_dev->pfn_cutter_on(p_zytp_dev->p_cutter_on_arg);
        }
    } else {

    }

    p_zytp_dev->zytp_stat = ch;

    return AW_OK;
}

/**
 * \brief 检查该指令类型是否在指令列表中
 */
aw_local char * __zytp_pos_find(awbl_dev_t *p_dev, int cmd)
{
    uint16_t                   i;
    struct zytp_pos           *p_pos = &__g_zytp_pos[0];
    struct awbl_zytp_devinfo  *p_devinfo = NULL;

    p_devinfo = (struct awbl_zytp_devinfo *)AWBL_DEVINFO_GET(p_dev);

    /* 在指令列表中查找功能码为cmd的指令数据，如果匹配成功，返回指令数据首地址*/
    for (i = 0; i < __ZYTP_POS_NUM; i++) {
        if (p_pos[i].mp_cmd == cmd &&
           (p_devinfo->type & ((uint64_t)1 << i)) &&
            cmd != AW_MPEXPANDTYPE) {
            return (char *)(p_pos[i].cmd_dats);
        }
    }
    return NULL;
}

/**
 * \brief 检查该指令包是否为扩展指令
 */
aw_local aw_err_t __zytp_expand_pos_chect(awbl_dev_t           *p_dev,
                                          aw_mprinter_params_t *p_mp_params)
{
    uint16_t         i, j;
    struct zytp_pos *p_pos = &__g_zytp_pos[0];

    for (i = 0; i < __ZYTP_POS_NUM; i++) {
        /* 在指令列表中匹配功能码为AW_MPEXPANDTYPE的指令数据  */
        if (p_pos[i].mp_cmd == AW_MPEXPANDTYPE) {
            for (j = 0;  j < __ZYTP_POS_DATA_NUM; j++) {
                if (p_pos[i].cmd_dats[j] == 0) {
                    /* 匹配成功返回AW_OK  */
                    if (j > 0) {
                        return AW_OK;
                    }
                } else if (p_pos[i].cmd_dats[j] != p_mp_params->p_buf1[j]) {
                    break;
                } else {

                }
            }

            /* 匹配成功返回AW_OK  */
            if (j == __ZYTP_POS_DATA_NUM) {
                return AW_OK;
            }
        }
    }

    return AW_ERROR;
}

/**
 * \brief 读微打设备数据
 */
aw_local aw_err_t __zytp_rece_data(awbl_dev_t           *p_dev,
                                   aw_mprinter_params_t *p_mp_params)
{
    struct awbl_zytp_dev *p_zytp_dev = NULL;
    uint8_t               uart_com = AWBL_DEV_UNIT_GET(p_dev);
    AW_SEMB_DECL(         sem);
    aw_semb_id_t          sem_id;
    aw_err_t              ret;
    uint8_t pos[3] = {0x1D, 0x61, 0x00};

    p_zytp_dev = AW_CONTAINER_OF(p_dev,  struct awbl_zytp_dev ,dev);
    memset(&sem,0,sizeof(&sem));
    sem_id = AW_SEMB_INIT(sem, 0, AW_SEM_Q_PRIORITY);

    p_zytp_dev->p_mp_param = p_mp_params;
    p_mp_params->ret_len = 0;
    p_mp_params->sem_id  = sem_id;


    /* 读数据前关闭自动状态返回，以免影响数据接收  */
    aw_serial_write(uart_com, (char *)pos, 3);

    p_zytp_dev->auto_stat_flag = AW_FALSE;
    ret = aw_semb_take(sem_id, p_mp_params->timeout);

    /* 读完数据后开启自动状态返回  */
    pos[2] = 0x2F;
    aw_serial_write(uart_com, (char *)pos, 3);

    if (ret == -AW_ETIME) {
       goto __exit;
    }

    p_zytp_dev->auto_stat_flag = AW_TRUE;

__exit:
        AW_SEMB_TERMINATE(sem);
        return ret;
}


aw_local aw_err_t __zytp_callback_set(struct awbl_zytp_dev       *p_zytp_dev,
                                      aw_mprinter_stat_cbk_t     *p_callback)
{
    uint8_t pos[3]   = {0x1D, 0x61, 0x2F};
    uint8_t uart_com = AWBL_DEV_UNIT_GET(&p_zytp_dev->dev);

    switch (p_callback->stat_cbk_type) {
    case AW_MPRINTER_CBK_OVER_VOLTAGE:
        p_zytp_dev->pfn_over_vltg = p_callback->pfn_callback;
        p_zytp_dev->p_over_vltg_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_OVER_VOLTAGE_RESUME:
        p_zytp_dev->pfn_over_vltg_resume = p_callback->pfn_callback;
        p_zytp_dev->p_over_vltg_resume_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_AXO_OPEN:
        p_zytp_dev->pfn_axo_open = p_callback->pfn_callback;
        p_zytp_dev->p_axo_open_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_AXO_CLOSE:
        p_zytp_dev->pfn_axo_close = p_callback->pfn_callback;
        p_zytp_dev->p_axo_close_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_OVER_HEAT:
        p_zytp_dev->pfn_over_heat = p_callback->pfn_callback;
        p_zytp_dev->p_over_heat_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_OVER_HEAT_RESUME:
        p_zytp_dev->pfn_over_heat_resume = p_callback->pfn_callback;
        p_zytp_dev->p_over_heat_resume_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_OF_PAPER:
        p_zytp_dev->pfn_of_paper = p_callback->pfn_callback;
        p_zytp_dev->p_of_paper_arg = p_callback->p_callback_arg;
        break;


    case AW_MPRINTER_CBK_OF_PAPER_RESUME:
        p_zytp_dev->pfn_of_paper_resume = p_callback->pfn_callback;
        p_zytp_dev->p_of_paper_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_CUTTER_ON:
        p_zytp_dev->pfn_cutter_on = p_callback->pfn_callback;
        p_zytp_dev->p_cutter_on_arg = p_callback->p_callback_arg;
        break;

    case AW_MPRINTER_CBK_CUTTER_OFF:
        p_zytp_dev->pfn_cutter_off = p_callback->pfn_callback;
        p_zytp_dev->p_cutter_off_arg = p_callback->p_callback_arg;
        break;
    default :
        break;

    }

    aw_serial_write(uart_com, (char *)pos, 3);

    return AW_OK;
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zytp_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zytp_inst_init2(awbl_dev_t *p_dev)
{
    struct awbl_zytp_dev      *p_zytp_dev = NULL;
    struct awbl_zytp_devinfo  *p_devinfo  = NULL;

    p_zytp_dev = AW_CONTAINER_OF(p_dev,  struct awbl_zytp_dev ,dev);
    p_devinfo = (struct awbl_zytp_devinfo  *)AWBL_DEVINFO_GET(p_dev);
    aw_iodev_create(&p_zytp_dev->io_dev,
                    p_devinfo->p_name);

    p_zytp_dev->io_dev.pfn_ent_open   = __zytp_open;
    p_zytp_dev->io_dev.pfn_ent_close  = __zytp_close;
    p_zytp_dev->io_dev.pfn_ent_write  = __zytp_write;
    p_zytp_dev->io_dev.pfn_ent_ioctrl = __zytp_ioctl;


    /* 初始化微打设备  */
    p_zytp_dev->auto_stat_flag = AW_TRUE;
    p_zytp_dev->zytp_stat      = 0;
    p_zytp_dev->pfn_axo_close = NULL;
    p_zytp_dev->pfn_axo_open  = NULL;
    p_zytp_dev->pfn_cutter_off = NULL;
    p_zytp_dev->pfn_cutter_on = NULL;
    p_zytp_dev->pfn_of_paper = NULL;
    p_zytp_dev->pfn_of_paper_resume = NULL;
    p_zytp_dev->pfn_over_heat = NULL;
    p_zytp_dev->pfn_over_heat_resume = NULL;
    p_zytp_dev->pfn_over_vltg = NULL;
    p_zytp_dev->pfn_over_vltg_resume = NULL;

    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zytp_inst_connect(awbl_dev_t *p_dev)
{
    return;
}


aw_local void * __zytp_open (struct aw_iodev *p_dev,
                             const char      *name,
                             int              oflag,
                             mode_t           mode,
                             int             *err)
{
    struct awbl_zytp_dev *p_zytp_dev  = NULL;
    uint8_t               uart_com;

    p_zytp_dev  = AW_CONTAINER_OF(p_dev, struct awbl_zytp_dev, io_dev);

    uart_com = AWBL_DEV_UNIT_GET(&p_zytp_dev->dev);

    /* 初始化UART  */
    aw_serial_ioctl(uart_com, SIO_BAUD_SET, (void *)9600);
    aw_serial_ioctl(uart_com, SIO_HW_OPTS_SET, (void *)( CLOCAL | CREAD | CS8 ));

    /* 注册TY钩子函数，数据实时处理  */
    aw_serial_ioctl(uart_com, AW_TIOPROTOHOOK, (void *)__zytp_rsp_isr);
    aw_serial_ioctl(uart_com, AW_TIOPROTOARG, (void *)p_zytp_dev);

    /* 其他代码 */
    return p_zytp_dev;
}

aw_local int __zytp_close (void *p_fd)
{
    uint8_t               uart_com;
    struct awbl_zytp_dev *p_zytp_dev  = (struct awbl_zytp_dev *)p_fd;
    uart_com = AWBL_DEV_UNIT_GET(&p_zytp_dev->dev);

    /* 注销ty钩子函数 */
    aw_serial_ioctl(uart_com, AW_TIOPROTOHOOK, (void *)NULL);
    return 0;
}

aw_local int __zytp_ioctl (void *p_fd, int cmd, void *p_arg)
{
    char                 *p_cmd = NULL;
    aw_mprinter_params_t   *p_mp_params = NULL;
    int   i;
    struct awbl_zytp_dev *p_zytp_dev  = (struct awbl_zytp_dev *)p_fd;
    uint8_t uart_com = AWBL_DEV_UNIT_GET(&p_zytp_dev->dev);

    if (NULL == p_fd) {
        return -AW_EINVAL;
    }

    /* 有指令宏查询该指令是不是在指令列表中 */
    if (NULL != (p_cmd = __zytp_pos_find(&p_zytp_dev->dev, cmd))) {

        /* 该指令在指令列表中，且是非扩展指令，发送该指令码 */
        for(i = 0; i < __ZYTP_POS_NUM; i++) {
            if (p_cmd[i] == 0) {
                break;
            }
        }

        aw_serial_write(uart_com, p_cmd, i);
    } else if (cmd == AW_MPCALLBACKSET) {

        /* 设置回调功能指令 */
        __zytp_callback_set(p_zytp_dev, p_arg);
        return AW_OK;

    } else if (cmd == AW_MPEXPANDTYPE){

        /* 扩展指令 */
        p_mp_params = (aw_mprinter_params_t   *)p_arg;
        if (AW_OK != __zytp_expand_pos_chect(&p_zytp_dev->dev, p_mp_params)) {
            return -AW_EPERM; /* 不支持该指令 */
        }
    } else {

        if (-AW_EINVAL == aw_serial_ioctl(uart_com, cmd, p_arg)) {
            return -AW_EPERM; /* 不支持该指令 */
        }
    }

    if (NULL == p_arg) {
        return AW_OK;
    }

    p_mp_params = (aw_mprinter_params_t   *)p_arg;

    /* 判断p_mp_params参数buf1_len是否为空，如果是将数据写入 */
    if (p_mp_params->p_buf1 && p_mp_params->buf1_len > 0) {
        aw_serial_write(uart_com,
                (char *)p_mp_params->p_buf1,
                        p_mp_params->buf1_len);
    }

    /* 判断p_mp_params参数buf2_len是否为空，如果是将数据写入 */
    if (p_mp_params->p_buf2 && p_mp_params->buf2_len > 0) {
        aw_serial_write(uart_com,
                (char *)p_mp_params->p_buf2,
                        p_mp_params->buf2_len);
    }

    /* 判断是否需要回应，如果需要回应接收数据 */
    if (p_mp_params->p_rsp_buf) {
        __zytp_rece_data(&p_zytp_dev->dev, p_mp_params);
    }

    return AW_OK;
}


ssize_t __zytp_write (void *p_fd, const void *p_buf, size_t nbyte)
{
    struct awbl_zytp_dev *p_zytp_dev  = (struct awbl_zytp_dev *)p_fd;

    uint8_t uart_com = AWBL_DEV_UNIT_GET(&p_zytp_dev->dev);

    if (NULL == p_fd) {
        return -AW_EINVAL;
    }

    return aw_serial_write(uart_com, (char *)p_buf, nbyte);
}


/******************************************************************************/
void awbl_zytp_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_zytp_drv_registration);
}

/* end of file */
