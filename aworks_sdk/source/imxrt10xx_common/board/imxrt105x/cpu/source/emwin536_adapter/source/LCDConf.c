/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2014  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.24 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
 * \file
 * \brief emwin LCD configuration.
 *
 * \par 使用说明
 *
 * \internal
 * \par modification history
 * - 2.00 19-04-30 cat, redesign frame buffer set up flow.
 * - 1.00 14-08-04 ops, based on the original file of emwin.
 * \endinternal
 */

#include "LCDConf.h"
#include "LCD.h"

#include <string.h>

#include "GUI.h"
#include "GUIConf.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"
#include "GUIDRV_FlexColor.h"

#include "aw_fb.h"
#include "aw_ts.h"
#include "aw_int.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_cache.h"
#include "aw_mem.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************/

aw_local void *__gp_fb = NULL;

/**
 * \breif 缓冲区复制函数
 *
 * \param  layer_index    层索引
 * \param  index_src      前置缓冲区
 * \param  index_dst      后置缓冲区
 *
 */
aw_local void __copy_buffer(int layer_index, int index_src, int index_dst)
{
    uint32_t buf_size = 0, addr_src = 0, addr_dst = 0;

    aw_fb_fix_info_t fix_screen_info;
    aw_fb_var_info_t var_screen_info;
    aw_fb_ioctl(__gp_fb, AW_FB_CMD_GET_FINFO, &fix_screen_info);
    aw_fb_ioctl(__gp_fb, AW_FB_CMD_GET_VINFO, &var_screen_info);

    buf_size = (var_screen_info.res.x_res * var_screen_info.res.y_res * var_screen_info.bpp.word_lbpp) / 8;

    addr_src = fix_screen_info.vram_addr + buf_size * (index_src);
    addr_dst = fix_screen_info.vram_addr + buf_size * (index_dst);

    memcpy((void *)addr_dst, (void *)addr_src, buf_size);
}

/**
 * \breif  配置指定层显示驱动及颜色转换关系
 *
 * \param  layer_index  指定层索引
 * \param  dis_drv      显示驱动
 * \param  color_conv   调色板模式(每一层都需要配置调色板)
 *
 * \return
 */
aw_local int __lcd_layer_config(int layer_index, const GUI_DEVICE_API* dis_drv, const LCD_API_COLOR_CONV* (*color_conv))
{
    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;

    aw_fb_ioctl(__gp_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(__gp_fb, AW_FB_CMD_GET_VINFO, &var_info);

    /* 设置显示驱动及颜色转换 */
    GUI_DEVICE_CreateAndLink(dis_drv, color_conv[layer_index], 0, layer_index);

    /* 设置给定层的位置 */
    LCD_SetPosEx   (layer_index, 0, 0);
    /* 设定给定层的物理尺寸 */
    LCD_SetSizeEx  (layer_index, var_info.res.x_res, var_info.res.y_res);
    /* 设置给定层的虚拟显示区域尺寸 */
    LCD_SetVSizeEx (layer_index, var_info.res.x_res, var_info.res.y_res);

    /* 设置屏幕显示方向(默认横屏) */
    if (var_info.orientation) {

        /* 横屏显示变竖屏显示 */
        GUI_SetOrientationEx(GUI_MIRROR_X | GUI_SWAP_XY, layer_index);
    }

    /* 设置显存地址 */
    LCD_SetVRAMAddrEx(layer_index, (void *)fix_info.vram_addr);

    LCD_SetDevFunc(layer_index, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))__copy_buffer);

    return AW_OK;
}

/**
 * \breif 设置校准
 *
 * \param  info   帧缓冲设备信息
 */
//aw_local void __gui_touch_calibration(aw_fb_info_t *info, int layer_num)
aw_local void __gui_touch_calibration(void *info, int layer_num)
{
    uint32_t TouchOrientation;

    TouchOrientation = (GUI_MIRROR_X * LCD_GetMirrorXEx(0)) |
                       (GUI_MIRROR_Y * LCD_GetMirrorYEx(0)) |
                       (GUI_SWAP_XY  * LCD_GetSwapXYEx(0)) ;

//    TouchOrientation = (GUI_MIRROR_X * LCD_GetMirrorX()) |
//                       (GUI_MIRROR_Y * LCD_GetMirrorY()) |
//                       (GUI_SWAP_XY  * LCD_GetSwapXY()) ;

    GUI_TOUCH_SetOrientation(TouchOrientation);
}

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/

void LCD_X_Config(void) {

    const GUI_DEVICE_API *display_drv = NULL;
    const LCD_API_COLOR_CONV *color_conv[3];

    aw_fb_fix_info_t fix_screen_info;
    aw_fb_var_info_t var_screen_info;


    /* frame buffer 设备打开 */
    __gp_fb = aw_fb_open("imx1050_fb", 0);

    /* frame buffer 初始化 */
    aw_fb_init(__gp_fb);

    /* frame buffer 设备信息获取 */
    aw_fb_ioctl(__gp_fb, AW_FB_CMD_GET_FINFO, &fix_screen_info);
    aw_fb_ioctl(__gp_fb, AW_FB_CMD_GET_VINFO, &var_screen_info);

    /* frame buffer 开启 */
    aw_fb_start(__gp_fb);

    /* 打开背光 */
    aw_fb_backlight(__gp_fb, 100);

    if (var_screen_info.bpp.word_lbpp == 24 || var_screen_info.bpp.word_lbpp == 18) {

        display_drv = GUIDRV_LIN_24;
        color_conv[0] = GUICC_M888;
        color_conv[1] = GUICC_M888;
        color_conv[2] = GUICC_M888;
    } else if (var_screen_info.bpp.word_lbpp == 16) {

        display_drv = GUIDRV_LIN_16;
        color_conv[0] = GUICC_M565;
        color_conv[1] = GUICC_M565;
        color_conv[2] = GUICC_M565;
    }

    /* 设置缓冲数目 */
    GUI_MULTIBUF_Config(var_screen_info.buffer.buffer_num);

    /* 设置指定层 */
    __lcd_layer_config(0, display_drv, color_conv);

    /* 触摸屏校准 (后设置触屏校准)*/
    __gui_touch_calibration(__gp_fb, 0);

    //
    // Set user palette data (only required if no fixed palette is used)
    //
    #if defined(PALETTE)
        LCD_SetLUTEx(0, PALETTE);
    #endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {

    int r;

    switch (Cmd) {

    case LCD_X_INITCONTROLLER : {

        return 0;
    }

    case LCD_X_SETVRAMADDR : {

//        LCD_X_SHOWBUFFER_INFO * p;
//        p = (LCD_X_SHOWBUFFER_INFO *)pData;

        return 0;
    }

    case LCD_X_SETORG : {

        return 0;
    }

    case LCD_X_SHOWBUFFER : {

        LCD_X_SHOWBUFFER_INFO * p;
        p = (LCD_X_SHOWBUFFER_INFO *)pData;

        aw_fb_swap_buf(__gp_fb);

        GUI_MULTIBUF_Confirm(p->Index);
        return 0;
    }

    default:

        r = -1;
        break;
    }
    return r;
}

/*********************************************************************
*
*       Global functions for GUI touch
*
**********************************************************************
*/

#if GUI_SUPPORT_TOUCH  // Used when touch screen support is enabled

/*********************************************************************
*
*       GUI_TOUCH_X_ActivateX()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Switches on voltage on X-axis,
*   prepares measurement for Y-axis.
*   Voltage on Y-axis is switched off.
*/
void GUI_TOUCH_X_ActivateX(void) {
}

/*********************************************************************
*
*       GUI_TOUCH_X_ActivateY()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Switches on voltage on Y-axis,
*   prepares measurement for X-axis.
*   Voltage on X-axis is switched off.
*/
void GUI_TOUCH_X_ActivateY(void) {
}

/*********************************************************************
*
*       GUI_TOUCH_X_MeasureX()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Measures voltage of X-axis.
*/
int  GUI_TOUCH_X_MeasureX(void) {

    return 0;
}

/*********************************************************************
*
*       GUI_TOUCH_X_MeasureY()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Measures voltage of Y-axis.
*/
int  GUI_TOUCH_X_MeasureY(void) {

    return 0;
}

#endif  // GUI_SUPPORT_TOUCH
/*************************** End of file ****************************/
