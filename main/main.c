/*
 *  description: ***********
 *  author: Jorge Monzon Monedero
 *  date:   27/04/2023
 */

/** Libraries inclusion section **/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "string.h"

/** Macros section **/
#define UART0_TXD       1
#define UART0_RXD       3
#define UART0_RTS       -1
#define UART0_CTS       -1
#define UART0_PORT      0
#define U0BAUD_RATE     115200
#define BUFFER_SIZE     1024     
#define GPIO_OUTPUT_IO_0    GPIO_NUM_2
#define GPIO_OUTPUT_PIN_SEL  1ULL<<GPIO_OUTPUT_IO_0

/** Global variables section**/
static uart_config_t uart_config;
static gpio_config_t io_conf = {};
static uint8_t att_value = 0;
//static QueueHandle_t uart0_queue;
//static const char *TAG = "UART TEST";

/** Prototypes section**/
void configure_uart();
void configure_gpio();
//void menu();
//int sendData(const char* logName, const char* data);
//void echo_uart();
/** Task section**/
static void uart_rcv_event_task(void *pvParameters);
//static void uart_tx_event_task(void *arg);
static void gpio_event_task(uint8_t *val);

/** Main section**/
void app_main(void)
{
    configure_uart(); 
    configure_gpio();
    xTaskCreate(uart_rcv_event_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    //xTaskCreate(uart_tx_event_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(gpio_event_task, "gpio_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
}
/** Functions definitions**/
void configure_gpio(){
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

static void gpio_event_task(){
    while (1)
    {
        if (att_value == '1')
        {
            gpio_set_level(GPIO_OUTPUT_IO_0, 1);
        } else if(att_value == '0'){
            gpio_set_level(GPIO_OUTPUT_IO_0, 0);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void configure_uart(){        
    uart_config.baud_rate = U0BAUD_RATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity    = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;
    int intr_alloc_flags = 0;
    #if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif
    ESP_ERROR_CHECK(uart_driver_install(UART0_PORT, BUFFER_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART0_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART0_PORT, UART0_TXD, UART0_RXD, UART0_RTS, UART0_CTS));
}

static void uart_rcv_event_task(void *pvParameters){
    uint8_t *data = (uint8_t *) malloc(BUFFER_SIZE);
    while (1)
    {
        const int data_len = uart_read_bytes(UART0_PORT, data, (BUFFER_SIZE - 1), 20 / portTICK_PERIOD_MS);
        if (data_len > 0)
        {
            uart_write_bytes(UART0_PORT, data, data_len);
            att_value = *data;
            data[data_len] = '\0'; 
        }
        
    }
    free(data);
}
/*
static void uart_tx_event_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX_TASK";
    static const char *TX_TASK_MENU = "\nIntroduce a value for the attenuador\n";
    //while (1) {
        sendData(TX_TASK_TAG, TX_TASK_MENU);
       // vTaskDelay(2000 / portTICK_PERIOD_MS);
    //}
}

int sendData(const char* logName, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART0_PORT, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}*/