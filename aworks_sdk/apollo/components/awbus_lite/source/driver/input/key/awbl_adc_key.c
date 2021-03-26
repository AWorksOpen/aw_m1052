#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_task.h"
#include "aw_adc.h"
#include "aw_mem.h"
#include "aw_input.h"
#include "string.h"
#include "driver/input/key/awbl_adc_key.h"


/*******************************************************************************
    macro operation
*******************************************************************************/
#define ADC_KEY_DEV_DECL(pdev, p_awdev)             \
        struct awbl_adc_key_dev *pdev = (struct awbl_adc_key_dev *)p_awdev


#define ADC_KEY_DEVINFO_DECL(p_info, pdev)          \
        struct awbl_adc_key_devinfo *p_info =       \
            (struct awbl_adc_key_devinfo *)AWBL_DEVINFO_GET(pdev)



/******************************************************************************/
void *__key_input_handler (void *p_arg)
{
    ADC_KEY_DEV_DECL(pdev, p_arg);
    ADC_KEY_DEVINFO_DECL(p_info, pdev);

    aw_err_t    ret = -1;
    uint32_t    keyad = 0;
    uint32_t    mvval = 0;
    uint32_t    i = 0;

    AW_FOREVER {

        mvval = 0;
        keyad = 0;

        ret = aw_adc_sync_read(p_info->adc_chan,
                               (void *)&keyad,
                               1,
                               AW_FALSE);
        if (ret == AW_OK) {
            ret = aw_adc_val_to_mv(p_info->adc_chan,
                                   (void *)&keyad,
                                   1,
                                   &mvval);
        } else {
            aw_kprintf("read key ad channel %d failed.\r\n", p_info->adc_chan);
            return NULL;
        }

        if (p_info->printf_org_val) {
            aw_kprintf("key ad: %d.\r\n", mvval);
        }

        for (i = 0; i < p_info->key_nums; i++) {

            if ((mvval >= p_info->key_cfg_tab[i].voltage_mv - p_info->offset) &&
                (mvval <= p_info->key_cfg_tab[i].voltage_mv + p_info->offset)) {

                if (p_info->key_cfg_tab[i].continuous) {

                    p_info->key_cfg_tab[i].cnt++;

                    if ( (p_info->key_cfg_tab[i].cnt == 1) ||
                         (p_info->key_cfg_tab[i].cnt > p_info->continue_cnt) ) {
                        aw_input_report_key(AW_INPUT_EV_KEY,
                                            p_info->key_cfg_tab[i].key_code,
                                            AW_TRUE);
                        p_info->key_cfg_tab[i].pressed = AW_TRUE;
//                        aw_kprintf("key %s down.\r\n", p_info->key_cfg_tab[i].key_name);
                    }

                } else {
                    /* 不支持连按模式的按键必须要是上一次为松开的情况下才能重新上报按键事件 */
                    if (p_info->key_cfg_tab[i].pressed == AW_FALSE) {
                        aw_input_report_key(AW_INPUT_EV_KEY,
                                            p_info->key_cfg_tab[i].key_code,
                                            AW_TRUE);
                        p_info->key_cfg_tab[i].pressed = AW_TRUE;
//                        aw_kprintf("key %s down.\r\n", p_info->key_cfg_tab[i].key_name);
                    }
                }

            } else {
                if (p_info->key_cfg_tab[i].pressed == AW_TRUE) {
                    aw_input_report_key(AW_INPUT_EV_KEY,
                                        p_info->key_cfg_tab[i].key_code,
                                        AW_FALSE);
                }
                p_info->key_cfg_tab[i].pressed = AW_FALSE;
                p_info->key_cfg_tab[i].cnt = 0;
            }
        }

        aw_mdelay(p_info->scan_period_ms);
    }

    return NULL;
}


/******************************************************************************/
void __adc_key_inst_init2 (struct awbl_dev *p_awdev)
{
    ADC_KEY_DEV_DECL(pdev, p_awdev);

    aw_task_id_t    tsk;
    static const char *str = "key_input_task";

    tsk = aw_task_create(str,
                         5,
                         2048,
                         __key_input_handler,
                         (void *)pdev);
    if (tsk) {
        aw_kprintf("%s create success, start it.\r\n", str);
        aw_task_startup(tsk);
    } else {
        aw_kprintf("%s create failed.\r\n", str);
    }
}


/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_adc_key = {
        NULL,
        __adc_key_inst_init2,
        NULL,
};


/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_awbl_drvinfo_adc_key = {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_PLB,                 /* bus_id */
        ADC_KEY_DRV_NAME,              /* p_drvname */
        &__g_awbl_drvfuncs_adc_key,    /* p_busfuncs */
        NULL,                           /* p_methods */
        NULL                            /* pfunc_drv_probe */
};


/** \brief register ADC key driver */
void awbl_adc_key_drv_register (void)
{
    awbl_drv_register(&__g_awbl_drvinfo_adc_key);
}




