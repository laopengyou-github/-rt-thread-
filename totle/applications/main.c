#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "sensor_asair_aht10.h"
#include <rtdbg.h>

/* defined the LED pin: PD */
#define LED_RED_PIN GET_PIN(D, 3)
#define LED_GREEN_PIN GET_PIN(D, 4)
#define LED_ORANGE_PIN GET_PIN(D, 5)
#define LED_BLUE_PIN GET_PIN(D, 6)
/*define semaphone paramater(����)*/
#define STACT_SIZE 2048
#define PRIORITY 25
#define TICK 5

/*define motor pin*/
#define ENA GET_PIN(D, 10)
#define IN1 GET_PIN(D, 8)
#define IN2 GET_PIN(D, 9)

/*define SHUIBENG pin*/
#define WATER_PUMP GET_PIN(D, 11)


#define AHT10_I2C_BUS "i2c2"


/***********引用函数声明*************************************************/
extern void sensor(int argc, char **argv);
extern rt_err_t onenet_mqtt_upload_digit(const char *ds_name, const double digit);
extern void sensor_show_data(rt_size_t num, rt_sensor_t sensor, struct rt_sensor_data *sensor_data);
extern int onenet_publish_digit(int argc, char **argv);
extern int onenet_mqtt_init(void);
extern int onenet_set_cmd_rsp(int argc, char **argv);
extern void onenet_set_cmd_rsp_cb(void (*cmd_rsp_cb)(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size));
extern void onenet_cmd_rsp_cb(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size);
extern void OLED_SHOW_TEMP(rt_int32_t temp,rt_int32_t humi);
extern void OLED_SHOW_HUMI(rt_int32_t value);
extern void OLED_Init();
extern void UI_init();
int program_sample();
/*******定义全局变量*****************************************************/
char *argv0[3]={"temp_aht10","humi_aht10"};
char *argv2[3]={"temp","humi"};//char *argv2[4]={"onenet_mqtt_publish_digit","temperature","12"};

rt_uint8_t reg1 = 0xFF,reg2 = 0xFF;
rt_size_t res1,res2;
rt_sensor_t sensor1,sensor2;
rt_device_t dev1=RT_NULL,dev2=RT_NULL;
struct rt_sensor_data data1,data2;

/* 邮箱控制块 */
static struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[128];

static struct rt_mailbox mb1;
/* 用于放邮件的内存池 */
static char mb1_pool[128];

/************************************************************/
//初始化AHT10
int rt_hw_aht10_port(void)
{
    struct rt_sensor_config cfg;
    cfg.intf.dev_name = AHT10_I2C_BUS;
    cfg.intf.user_data = (void *)AHT10_I2C_ADDR;
    rt_hw_aht10_init("aht10", &cfg);
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_hw_aht10_port);

/************************************************************/

int main(void)
{
    //配置LED相应引脚输出模式
    rt_pin_mode(LED_RED_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_GREEN_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_ORANGE_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_BLUE_PIN, PIN_MODE_OUTPUT);
    //配置电机相关引脚输出模式
    rt_pin_mode(ENA, PIN_MODE_OUTPUT);
    rt_pin_mode(IN1, PIN_MODE_OUTPUT);
    rt_pin_mode(IN2, PIN_MODE_OUTPUT);
    //查找AHT10设备
    dev1 = rt_device_find(argv0[0]);
    dev2 = rt_device_find(argv0[1]);
    //onenet 初始化
    onenet_mqtt_init();
    //定义测温，测湿度设备
    sensor1 = (rt_sensor_t)dev1;
    sensor2 = (rt_sensor_t)dev2;
    //云命令回调
    onenet_set_cmd_rsp_cb(onenet_cmd_rsp_cb);
    //OLED相关初始化
    OLED_Init();
    UI_init();
    //启动线程
    program_sample();
}

/************************************************************/

//线程句柄定义
rt_thread_t tid1,tid2,tid3;
//线程1入口函数
void temp_check_entry(void* parameter)
{

    double temp;

    while(1)
    {
        rt_device_open(dev1, RT_DEVICE_FLAG_RDWR);
        sensor1 = (rt_sensor_t)dev1;
        rt_device_read(dev1, 0, &data1, 1);
        sensor_show_data(0,sensor1,&data1);

        rt_mb_send(&mb,data1.data.temp);

        temp=(double)(data1.data.temp)/10;
        onenet_mqtt_upload_digit(argv2[0],temp);
         rt_thread_mdelay(1000);

        if(temp>33.5)
        {
            //���罵��
            rt_pin_write(ENA, PIN_HIGH);
            rt_pin_write(IN1, PIN_LOW);
            rt_pin_write(IN2, PIN_HIGH);
        }
        else
        {
            rt_pin_write(ENA, PIN_LOW);
        }

        rt_device_close(dev1);

        rt_thread_mdelay(1000);
    }
}
//线程2入口函数
void humi_check_entry(void* parameter)
{

    double humi=70;

    while(1)
    {
        rt_device_open(dev2, RT_DEVICE_FLAG_RDWR);
        sensor2 = (rt_sensor_t)dev2;
        rt_device_read(dev2, 0, &data2, 1);
        sensor_show_data(0,sensor2,&data2);

        rt_mb_send(&mb1,data2.data.humi);


        humi=(double)(data2.data.humi)/10;
        onenet_mqtt_upload_digit(argv2[1],humi);
         rt_thread_mdelay(1000);
        if(humi<40)
        {
            rt_pin_write(WATER_PUMP, PIN_HIGH);
        }
        else
        {
            rt_pin_write(WATER_PUMP, PIN_LOW);
        }

        rt_device_close(dev2);

        rt_thread_mdelay(1000);
    }
}
//线程3入口函数
void OLED_entry(void* parameter)
{
    rt_ubase_t temp,humi;
    while(1)
    {
        rt_mb_recv(&mb, &temp, RT_WAITING_FOREVER);
        rt_mb_recv(&mb1, &humi, RT_WAITING_FOREVER);
        OLED_SHOW_TEMP(temp,humi);
        //OLED_SHOW_HUMI(700);
        rt_thread_mdelay(200);
    }
}
//线程初始化
int program_sample()
{

   tid1 = rt_thread_create("temp_check",
            temp_check_entry,
            RT_NULL,
            STACT_SIZE,
            PRIORITY,
            TICK);
   if(tid1!=RT_NULL)
       rt_thread_startup(tid1);
//   -------------------------------------
   tid2 = rt_thread_create("humi_check",
               humi_check_entry,
               RT_NULL,
               STACT_SIZE,
               PRIORITY,
               TICK);
    if(tid2!=RT_NULL)
          rt_thread_startup(tid2);
//   -------------------------------------
    tid3 = rt_thread_create("OLED",
            OLED_entry,
                RT_NULL,
                STACT_SIZE,
                PRIORITY,
                TICK);
       if(tid3!=RT_NULL)
           rt_thread_startup(tid3);


       rt_err_t result;

          /* 初始化一个 mailbox */
          result = rt_mb_init(&mb,
                              "mbt",                      /* 名称是 mbt */
                              &mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                              sizeof(mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                              RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
          if (result != RT_EOK)
          {
              rt_kprintf("init mailbox failed.\n");
              return -1;
          }

      rt_err_t result1;

       /* 初始化一个 mailbox */
       result1 = rt_mb_init(&mb1,
                           "mbt1",                      /* 名称是 mbt */
                           &mb1_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                           sizeof(mb1_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                           RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
       if (result1 != RT_EOK)
       {
           rt_kprintf("init mailbox failed.\n");
           return -1;
       }
}



