/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**                                      
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               led.c
** Latest modified Date:    2008-09-11
** Latest Version:          1.0
** Descriptions:            CANopen参数设置表 
**
**--------------------------------------------------------------------------------------------------------
** Created by:              xiangjl
** Created date:            2008-07-14
** Version:                 1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/
#ifndef __TARGET_LED_H
#define __TARGET_LED_H

#ifdef __cplusplus
extern "C" {
#endif                                                                  /*  __cplusplus                 */       


/*********************************************************************************************************
** Function name:           PredefineCOB_ID
** Descriptions:            function init CANopen led configuration	
** input parameters:        ucInst:  当前实例号
**                         
** output parameters:       none
** Returned value:          无:  
** Created by:              xiangjl
** Created date:            2008-09-11
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
*********************************************************************************************************/
void usrCANopenLedCfg(unsigned char inst);
void usrCANopenLedRun(unsigned char inst,unsigned char onoff);
void usrCANopenLedErr(unsigned char inst,unsigned char onoff);

#ifdef __cplusplus
    }  
#endif                                                                  /*  __cplusplus                 */

#endif                                                                  /*  __LED_H                     */

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
