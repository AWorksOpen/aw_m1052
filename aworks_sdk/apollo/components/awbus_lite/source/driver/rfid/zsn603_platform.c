#include "apollo.h"
#include "awbus_lite.h"
#include "aw_i2c.h"
#include "awbl_i2cbus.h"
#include "aw_serial.h"
#include "aw_gpio.h"
#include "driver/rfid/zsn603_platform.h"

void am_zsn603_uart_rx_task(void *p_arg)
{
    ssize_t ret = 0;
    char data = 0;
    zsn603_platform_uart_t        *p_dev  = (zsn603_platform_uart_t*)p_arg;
    const aw_platform_uart_info_t *p_info = p_dev->p_platform_info;

    AW_FOREVER{
        ret = aw_serial_read(p_info->uart_com, &data, 1);
        if(ret == 1){
            p_dev->pfn_uart_rx(p_dev->p_arg, data);
        }
    }
}


/** \brief  串口传输函数 */
int zsn603_platform_uart_tx(zsn603_platform_uart_t *p_dev, uint8_t *p_data, uint32_t nbytes)
{
    if(aw_serial_write(p_dev->p_platform_info->uart_com,
                      (const char *)p_data,
                       nbytes) == nbytes){
        return  0;
    }else{
        return -1;
    }
}

/** \brief  I2C写函数 */
int zsn603_platform_i2c_write(zsn603_platform_i2c_t  *p_dev,
                              uint8_t                 slv_addr,
                              uint16_t                sub_addr,
                              uint8_t                *p_data,
                              uint32_t                nbytes)
{
    return awbl_i2c_write(p_dev->p_platform_info->p_awbl_i2c_dev,
                          AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_2BYTE,
                          slv_addr,
                          sub_addr,
                          p_data,
                          nbytes);
}
/** \brief  I2C读函数 */
int zsn603_platform_i2c_read(zsn603_platform_i2c_t     *p_dev,
                             uint8_t                    slv_addr,
                             uint16_t                   sub_addr,
                             uint8_t                   *p_data,
                             uint32_t                   nbytes)
{
    return awbl_i2c_read(p_dev->p_platform_info->p_awbl_i2c_dev,
                         AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_2BYTE,
                         slv_addr,
                         sub_addr,
                         p_data,
                         nbytes);
}



/** \brief  ZSN603 平台相关设备(UART方式)平台相关初始化 */
uint8_t  zsn603_platform_uart_init(zsn603_platform_uart_t                *p_dev,
                                   const zsn603_platform_uart_devinfo_t  *p_info,
                                   zsn603_uart_funcs_t                   *p_funcs,
                                   void                                  *p_arg)
{
    aw_task_id_t           task_id;
    p_dev->p_arg           = p_arg;
    p_dev->pfn_uart_rx     = p_funcs->zsn603_uart_rx;
    p_dev->p_platform_info = p_info;

    /* 中断引脚配置  */
    if(aw_gpio_pin_request("zsn603_int_pin", (const int *)&p_info->int_pin, 1) ==AW_OK){
        aw_gpio_pin_cfg(p_info->int_pin, AW_GPIO_INPUT);
        aw_gpio_trigger_connect(p_info->int_pin,
                                p_funcs->zsn603_int_pin_cb,
                                p_arg);
        aw_gpio_trigger_cfg(p_info->int_pin, AW_GPIO_TRIGGER_FALL);
        aw_gpio_trigger_on(p_info->int_pin);
    }

    aw_serial_ioctl(p_info->uart_com, SIO_BAUD_SET, (void *)p_info->baud_rate);

    task_id = aw_task_create("uart_task",
                             2,
                             2048,
                             am_zsn603_uart_rx_task,
                             (void *)p_dev);

    return aw_task_startup(task_id);
}
/** \brief  ZSN603 平台相关设备(I2C方式)相关初始化 */
uint8_t  zsn603_platform_i2c_init(zsn603_platform_i2c_t                 *p_dev,
                                  const zsn603_platform_i2c_devinfo_t   *p_info,
                                  void(*int_pin_callback_function)      (void *),
                                  void                                  *p_arg)
{

    p_dev->p_platform_info  = p_info;

    aw_gpio_pin_request("zsn603_int_pin", (const int *)&p_info->int_pin, 1);
    aw_gpio_pin_cfg(p_info->int_pin, AW_GPIO_INPUT);
    /*  连接并使能中断 */
    aw_gpio_trigger_connect(p_info->int_pin,
                            int_pin_callback_function,
                            p_arg);
    aw_gpio_trigger_cfg(p_info->int_pin, AW_GPIO_TRIGGER_FALL);
    aw_gpio_trigger_on(p_info->int_pin);

    return 0;

}

/** \brief  ZSN603 平台设备(I2C方式)解初始化 */
uint8_t  zsn603_platform_i2c_deinit(zsn603_platform_i2c_t    *p_dev)
{

    return 0;
}
/** \brief  ZSN603 平台设备(UART方式)解初始化 */
uint8_t  zsn603_platform_uart_deinit(zsn603_platform_uart_t    *p_dev)
{
    return 0;
}
