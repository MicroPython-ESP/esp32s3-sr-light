#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2s.h"
#include "driver/rmt.h"
#include "led_strip.h"
#include "esp_sr.h"
#include "esp_err.h"

#define I2S_SCK_PIN     4
#define I2S_WS_PIN      5
#define I2S_SD_PIN      6
#define RGB_PIN         48
#define LED_NUM         1
#define MAX_BRIGHT      70

static led_strip_handle_t led_strip;

typedef enum {
    MODE_OFF,
    MODE_SOLID,
    MODE_BLINK,
    MODE_BREATH,
    MODE_RAINBOW
} light_mode_t;

static light_mode_t g_mode = MODE_OFF;
static uint8_t g_r=0,g_g=0,g_b=0;
static uint8_t g_bright = 50;
static uint32_t g_blink_ms = 500;

static const uint8_t color_table[8][3] = {
    {255,0,0},    //红
    {255,120,0},  //橙
    {255,255,0},  //黄
    {0,255,0},    //绿
    {0,255,200},  //青
    {0,0,255},    //蓝
    {180,0,255},  //紫
    {255,255,255} //白
};

static void set_rgb_raw(uint8_t r,uint8_t g,uint8_t b)
{
    r = r * g_bright / 100;
    g = g * g_bright / 100;
    b = b * g_bright / 100;
    led_strip_set_pixel(led_strip,0,r,g,b);
    led_strip_refresh(led_strip);
}

static void sr_cb(esp_sr_result_t *res, void *arg)
{
    if(!res || !res->text) return;
    printf("识别：%s\n",res->text);
    const char *txt = res->text;

    if(strstr(txt,"关灯")){
        g_mode = MODE_OFF;
        set_rgb_raw(0,0,0);
    }
    else if(strstr(txt,"红灯亮")) {g_mode=MODE_SOLID; g_r=color_table[0][0];g_g=color_table[0][1];g_b=color_table[0][2]; set_rgb_raw(g_r,g_g,g_b);}
    else if(strstr(txt,"橙灯亮")) {g_mode=MODE_SOLID; g_r=color_table[1][0];g_g=color_table[1][1];g_b=color_table[1][2]; set_rgb_raw(g_r,g_g,g_b);}
    else if(strstr(txt,"黄灯亮")) {g_mode=MODE_SOLID; g_r=color_table[2][0];g_g=color_table[2][1];g_b=color_table[2][2]; set_rgb_raw(g_r,g_g,g_b);}
    else if(strstr(txt,"绿灯亮")) {g_mode=MODE_SOLID; g_r=color_table[3][0];g_g=color_table[3][1];g_b=color_table[3][2]; set_rgb_raw(g_r,g_g,g_b);}
    else if(strstr(txt,"青灯亮")) {g_mode=MODE_SOLID; g_r=color_table[4][0];g_g=color_table[4][1];g_b=color_table[4][2]; set_rgb_raw(g_r,g_g,g_b);}
    else if(strstr(txt,"蓝灯亮")) {g_mode=MODE_SOLID; g_r=color_table[5][0];g_g=color_table[5][1];g_b=color_table[5][2]; set_rgb_raw(g_r,g_g,g_b);}
    else if(strstr(txt,"紫灯亮")) {g_mode=MODE_SOLID; g_r=color_table[6][0];g_g=color_table[6][1];g_b=color_table[6][2]; set_rgb_raw(g_r,g_g,g_b);}
    else if(strstr(txt,"白灯亮")) {g_mode=MODE_SOLID; g_r=color_table[7][0];g_g=color_table[7][1];g_b=color_table[7][2]; set_rgb_raw(g_r,g_g,g_b);}

    else if(strstr(txt,"亮一点")){
        if(g_bright <= 90) g_bright +=10;
        if(g_bright>MAX_BRIGHT) g_bright=MAX_BRIGHT;
        if(g_mode==MODE_SOLID) set_rgb_raw(g_r,g_g,g_b);
    }
    else if(strstr(txt,"暗一点")){
        if(g_bright >=10) g_bright -=10;
        if(g_mode==MODE_SOLID) set_rgb_raw(g_r,g_g,g_b);
    }

    else if(strstr(txt,"红灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[0][0];g_g=color_table[0][1];g_b=color_table[0][2];}
    else if(strstr(txt,"橙灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[1][0];g_g=color_table[1][1];g_b=color_table[1][2];}
    else if(strstr(txt,"黄灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[2][0];g_g=color_table[2][1];g_b=color_table[2][2];}
    else if(strstr(txt,"绿灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[3][0];g_g=color_table[3][1];g_b=color_table[3][2];}
    else if(strstr(txt,"青灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[4][0];g_g=color_table[4][1];g_b=color_table[4][2];}
    else if(strstr(txt,"蓝灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[5][0];g_g=color_table[5][1];g_b=color_table[5][2];}
    else if(strstr(txt,"紫灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[6][0];g_g=color_table[6][1];g_b=color_table[6][2];}
    else if(strstr(txt,"白灯闪烁")) {g_mode=MODE_BLINK; g_r=color_table[7][0];g_g=color_table[7][1];g_b=color_table[7][2];}

    else if(strstr(txt,"快一点")) g_blink_ms=250;
    else if(strstr(txt,"慢一点")) g_blink_ms=1000;

    else if(strstr(txt,"红灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[0][0];g_g=color_table[0][1];g_b=color_table[0][2];}
    else if(strstr(txt,"橙灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[1][0];g_g=color_table[1][1];g_b=color_table[1][2];}
    else if(strstr(txt,"黄灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[2][0];g_g=color_table[2][1];g_b=color_table[2][2];}
    else if(strstr(txt,"绿灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[3][0];g_g=color_table[3][1];g_b=color_table[3][2];}
    else if(strstr(txt,"青灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[4][0];g_g=color_table[4][1];g_b=color_table[4][2];}
    else if(strstr(txt,"蓝灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[5][0];g_g=color_table[5][1];g_b=color_table[5][2];}
    else if(strstr(txt,"紫灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[6][0];g_g=color_table[6][1];g_b=color_table[6][2];}
    else if(strstr(txt,"白灯呼吸")) {g_mode=MODE_BREATH; g_r=color_table[7][0];g_g=color_table[7][1];g_b=color_table[7][2];}

    else if(strstr(txt,"彩虹渐变")) g_mode=MODE_RAINBOW;
}

static void light_task(void *arg)
{
    uint8_t phase=0;
    uint8_t breath_dir=1;
    uint8_t breath_val=0;
    while(1){
        switch(g_mode){
            case MODE_OFF:
                set_rgb_raw(0,0,0);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            case MODE_SOLID:
                set_rgb_raw(g_r,g_g,g_b);
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            case MODE_BLINK:
                set_rgb_raw(g_r,g_g,g_b);
                vTaskDelay(pdMS_TO_TICKS(g_blink_ms));
                set_rgb_raw(0,0,0);
                vTaskDelay(pdMS_TO_TICKS(g_blink_ms));
                break;
            case MODE_BREATH:
                if(breath_dir){
                    breath_val += 2;
                    if(breath_val >= g_bright) breath_dir=0;
                }else{
                    breath_val -=2;
                    if(breath_val==0) breath_dir=1;
                }
                {
                    uint8_t rr = g_r * breath_val / 100;
                    uint8_t gg = g_g * breath_val / 100;
                    uint8_t bb = g_b * breath_val / 100;
                    led_strip_set_pixel(led_strip,0,rr,gg,bb);
                    led_strip_refresh(led_strip);
                }
                vTaskDelay(pdMS_TO_TICKS(30));
                break;
            case MODE_RAINBOW:
                {
                    uint8_t r,g,b;
                    uint8_t h=phase;
                    if(h<85){ r=255-h*3; g=h*3; b=0; }
                    else if(h<170){ h-=85; r=0;g=255-h*3;b=h*3; }
                    else {h-=170; r=h*3;g=0;b=255-h*3;}
                    r=r*g_bright/100; g=g*g_bright/100; b=b*g_bright/100;
                    led_strip_set_pixel(led_strip,0,r,g,b);
                    led_strip_refresh(led_strip);
                }
                phase++;
                vTaskDelay(pdMS_TO_TICKS(40));
                break;
        }
    }
}

static void led_init(void)
{
    led_strip_config_t strip_cfg={
        .strip_gpio_num=RGB_PIN,
        .max_leds=LED_NUM,
        .led_pixel_format=LED_PIXEL_FORMAT_GRB,
        .led_model=LED_MODEL_WS2812,
        .flags.invert_out=false
    };
    led_strip_rmt_config_t rmt_cfg={
        .clk_src=RMT_CLK_SRC_DEFAULT,
        .resolution_hz=10*1000*1000,
        .flags.with_dma=true
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_cfg,&rmt_cfg,&led_strip));
    led_strip_clear(led_strip);
    led_strip_refresh(led_strip);
}

static void i2s_mic_init(void)
{
    i2s_driver_install(I2S_NUM_0, &(i2s_config_t){
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 512,
    },0,NULL);
    i2s_pin_config_t pin={
        .bck_io_num=I2S_SCK_PIN,
        .ws_io_num=I2S_WS_PIN,
        .data_out_num=-1,
        .data_in_num=I2S_SD_PIN
    };
    i2s_set_pin(I2S_NUM_0,&pin);
}

void app_main(void)
{
    led_init();
    i2s_mic_init();
    xTaskCreate(light_task,"light",2048,NULL,5,NULL);

    esp_sr_config_t sr_cfg={
        .i2s_port=I2S_NUM_0,
        .sample_rate=16000,
        .callback=sr_cb
    };
    ESP_ERROR_CHECK(esp_sr_start(&sr_cfg));
}
