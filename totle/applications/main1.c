///*
// * Copyright (c) 2006-2022, RT-Thread Development Team
// * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
// *
// * SPDX-License-Identifier: Apache-2.0
// *
// * Change Logs:
// * Date           Author       Notes
// * 2022-04-28     CDT          first version
// */
//
//#include <rtthread.h>
//#include <rtdevice.h>
//#include <board.h>
//
///* defined the LED_GREEN pin: PD4 */
//#define LED_RED_PIN GET_PIN(D, 3)
//#define LED_GREEN_PIN GET_PIN(D, 4)
//#define LED_ORANGE_PIN GET_PIN(D, 5)
//#define LED_BLUE_PIN GET_PIN(D, 6)
//
//#include "sensor_asair_aht10.h"
//#define AHT10_I2C_BUS "i2c2"
//#include <rtdbg.h>
//
//extern int onenet_publish_string(int argc, char **argv);
//extern void sensor(int argc, char **argv);
//extern rt_err_t onenet_mqtt_upload_digit(const char *ds_name, const double digit);
//extern void sensor_show_data(rt_size_t num, rt_sensor_t sensor, struct rt_sensor_data *sensor_data);
//extern int onenet_publish_digit(int argc, char **argv);
//extern int onenet_mqtt_init(void);
//
//int rt_hw_aht10_port(void)
//{
//    struct rt_sensor_config cfg;
//    cfg.intf.dev_name = AHT10_I2C_BUS;
//    cfg.intf.user_data = (void *)AHT10_I2C_ADDR;
//    rt_hw_aht10_init("aht10", &cfg);
//    return RT_EOK;
//}
//INIT_ENV_EXPORT(rt_hw_aht10_port);
//
//
//int main(void)
//{
//    char *argv0[4]={"sensor","probe","temp_aht10","humi_aht10"};
//    char *argv1[4]={"sensor","read"};
//    char *argv2[4]={"humi","temperature","12"};//char *argv2[4]={"onenet_mqtt_publish_digit","temperature","12"};
//    char *argv3[4]={"sensor","read"};
//    rt_uint8_t reg1 = 0xFF,reg2 = 0xFF;
//    rt_size_t res1,res2;
//    rt_sensor_t sensor1,sensor2;
//    rt_device_t dev1=RT_NULL,dev2=RT_NULL;
//    dev1 = rt_device_find(argv0[2]);
//    dev2 = rt_device_find(argv0[3]);
//    onenet_mqtt_init();
//    if (dev1 == RT_NULL)
//    {
//        LOG_E("Can't find device:%s", argv0[2]);
//        return;
//    }
//    if (rt_device_open(dev2, RT_DEVICE_FLAG_RDWR) != RT_EOK)
//    {
//        LOG_E("open device1 failed!");
//        return;
//    }
//    if (dev2 == RT_NULL)
//        {
//            LOG_E("Can't find device:%s", argv0[3]);
//            return;
//        }
//    if (rt_device_open(dev2, RT_DEVICE_FLAG_RDWR) != RT_EOK)
//        {
//            LOG_E("open device2 failed!");
//            return;
//        }
//    rt_device_control(dev1, RT_SENSOR_CTRL_GET_ID, &reg1);
//    LOG_I("device id: 0x%x!", reg1);
//    rt_device_control(dev2, RT_SENSOR_CTRL_GET_ID, &reg2);
//    LOG_I("device id: 0x%x!", reg2);
//    sensor1 = (rt_sensor_t)dev1;
//    sensor2 = (rt_sensor_t)dev2;
//    struct rt_sensor_data data1,data2;
//
//    while(1)
//    {
//        if (dev1 == RT_NULL)
//        {
//            LOG_W("Please probe sensor device first!");
//            return;
//        }
//        res1 = rt_device_read(dev1, 0, &data1, 1);
//        if (res1 != 1)
//        {
//            LOG_E("read data failed!size is %d", res1);
//        }
//        else
//        {
//            sensor_show_data(0,sensor1,&data1);//显示温度数据->data.humi / 10
//        }
//        rt_thread_mdelay(100);
//        if (dev2 == RT_NULL)
//       {
//           LOG_W("Please probe sensor device first!");
//           return;
//       }
//       res2 = rt_device_read(dev2, 0, &data2, 1);
//       if (res2 != 1)
//       {
//           LOG_E("read data failed!size is %d", res2);
//       }
//       else
//       {
//           sensor_show_data(0,sensor2,&data2);//显示
//       }
//
//        rt_thread_mdelay(500);
////        onenet_publish_digit(3,argv2);
//        if(onenet_mqtt_upload_digit(argv2[1],(double)(data1.data.temp)/10) < 0)
//        {
//            LOG_E("upload digit data has an error!\n");
//        }
//        rt_thread_mdelay(100);
//        if(onenet_mqtt_upload_digit(argv2[0],(double)(data2.data.humi)/10) < 0)
//        {
//            LOG_E("upload digit data has an error!\n");
//        }
//        rt_thread_mdelay(500);
//       // sensor_data->data.temp / 10, (rt_uint32_t)sensor_data->data.temp % 10
//    }
//}
//
