/*
 *  author: Jorge Monzon Monedero
 *  description: This code aims to control the performance of and VHF header. The components which are going to be managed are a variable attenuator
 *  and the switching structure which selects the receiving or the transmitting branches.
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
#define UART0_TXD       1                               // TXD buffer is in the GPIO1
#define UART0_RXD       3                               // RXD buffer is in the GPIO3
#define UART0_RTS       -1                              // Not configured
#define UART0_CTS       -1                              // Not configured
#define UART0_PORT      0                               // The UART used for this aplication - UART0
#define U0BAUD_RATE     115200                          // Baudrate for the serial communications throught UART0
#define BUFFER_SIZE     1024                            // Buffer size for the memory allocation
/*GPIO pin used to set the variable attenuator*/
#define GPIO_OUTPUT_32    GPIO_NUM_32
#define GPIO_OUTPUT_33    GPIO_NUM_33
#define GPIO_OUTPUT_25    GPIO_NUM_25
#define GPIO_OUTPUT_26    GPIO_NUM_26
#define GPIO_OUTPUT_27    GPIO_NUM_27
#define GPIO_OUTPUT_14    GPIO_NUM_14
#define GPIO_OUTPUT_12    GPIO_NUM_12
#define GPIO_OUTPUT_13    GPIO_NUM_13      
/* Bit mask for the pins which are going to use as outputs*/       
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_32)|(1ULL<<GPIO_OUTPUT_33)|(1ULL<<GPIO_OUTPUT_25)|(1ULL<<GPIO_OUTPUT_26)|(1ULL<<GPIO_OUTPUT_27)|(1ULL<<GPIO_OUTPUT_14)|(1ULL<<GPIO_OUTPUT_12)|(1ULL<<GPIO_OUTPUT_13))

/** Global variables section**/
static uart_config_t uart_config;
static gpio_config_t io_conf = {};

/** Prototypes section**/
void configure_uart();
void configure_gpio();
void gpio_event(uint8_t *val);
void menu();
void enable_LE();

/** Task section**/
static void uart_rcv_event_task(void *pvParameters);

/** Main section**/
void app_main(void)
{
    configure_uart(); 
    configure_gpio();
    menu();
    xTaskCreate(uart_rcv_event_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}
/** Functions definitions**/
void menu(){
    char *menu = "\r\nAttenuations in dB\n1) IL\n2) 0.5\n3) 1\n4) 2\n5) 4\n6) 8\n7) 16\n8)31.5\n\r";
    int len = strlen(menu);
    uart_write_bytes(UART0_PORT,menu,len);
}
 
void configure_gpio(){
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 1;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    char *msg = "\r\n Setting LE to 0 and P/S to 0\n\r";
    int len_msg = strlen(msg);
    uart_write_bytes(UART0_PORT,msg,len_msg);
    gpio_set_level(GPIO_OUTPUT_12, 0);
    gpio_set_level(GPIO_OUTPUT_13, 0);
    vTaskDelay(0.01 / portTICK_PERIOD_MS);
}

void gpio_event(uint8_t *val){
    // Attenuator non active and select parallel mode
    char *msg = "\r\n Setting LE to 0 and P/S to 0\n\r";
    int len_msg = strlen(msg);
    uart_write_bytes(UART0_PORT,msg,len_msg);
    gpio_set_level(GPIO_OUTPUT_12, 0);
    gpio_set_level(GPIO_OUTPUT_13, 0);
    vTaskDelay(0.01 / portTICK_PERIOD_MS);
    // Set attenuador valies
    switch (*val)
    {
    case '1':                              //11 1101 
        gpio_set_level(GPIO_OUTPUT_32, 1);
        gpio_set_level(GPIO_OUTPUT_33, 1);
        gpio_set_level(GPIO_OUTPUT_25, 1);
        gpio_set_level(GPIO_OUTPUT_26, 1);
        gpio_set_level(GPIO_OUTPUT_27, 1);
        gpio_set_level(GPIO_OUTPUT_14, 1);
        enable_LE();
        break;
    case '2':                               //11 1100
        gpio_set_level(GPIO_OUTPUT_32, 1);
        gpio_set_level(GPIO_OUTPUT_33, 1);
        gpio_set_level(GPIO_OUTPUT_25, 1);
        gpio_set_level(GPIO_OUTPUT_26, 1);
        gpio_set_level(GPIO_OUTPUT_27, 1);
        gpio_set_level(GPIO_OUTPUT_14, 0);
        enable_LE();
        break;
    case '3':                               //11 1011
        gpio_set_level(GPIO_OUTPUT_32, 1);
        gpio_set_level(GPIO_OUTPUT_33, 1);
        gpio_set_level(GPIO_OUTPUT_25, 1);
        gpio_set_level(GPIO_OUTPUT_26, 1);
        gpio_set_level(GPIO_OUTPUT_27, 0);
        gpio_set_level(GPIO_OUTPUT_14, 1);
        enable_LE();
        break;
    case '4':                               //11 1010
        gpio_set_level(GPIO_OUTPUT_32, 1);
        gpio_set_level(GPIO_OUTPUT_33, 1);
        gpio_set_level(GPIO_OUTPUT_25, 1);
        gpio_set_level(GPIO_OUTPUT_26, 0);
        gpio_set_level(GPIO_OUTPUT_27, 1);
        gpio_set_level(GPIO_OUTPUT_14, 1);
        enable_LE();
        break;
    case '5':                               //11 1001
        gpio_set_level(GPIO_OUTPUT_32, 1);
        gpio_set_level(GPIO_OUTPUT_33, 1);
        gpio_set_level(GPIO_OUTPUT_25, 0);
        gpio_set_level(GPIO_OUTPUT_26, 1);
        gpio_set_level(GPIO_OUTPUT_27, 1);
        gpio_set_level(GPIO_OUTPUT_14, 1);
        enable_LE();
        break;
    case '6':                               //11 1000
        gpio_set_level(GPIO_OUTPUT_32, 1);
        gpio_set_level(GPIO_OUTPUT_33, 0);
        gpio_set_level(GPIO_OUTPUT_25, 1);
        gpio_set_level(GPIO_OUTPUT_26, 1);
        gpio_set_level(GPIO_OUTPUT_27, 1);
        gpio_set_level(GPIO_OUTPUT_14, 1);
        enable_LE();
        break;
    case '7':                               // 11 0111
        gpio_set_level(GPIO_OUTPUT_32, 0);
        gpio_set_level(GPIO_OUTPUT_33, 1);
        gpio_set_level(GPIO_OUTPUT_25, 1);
        gpio_set_level(GPIO_OUTPUT_26, 1);
        gpio_set_level(GPIO_OUTPUT_27, 1);
        gpio_set_level(GPIO_OUTPUT_14, 1);
        enable_LE();
        break;
    case '8':                               // 11 1111
        gpio_set_level(GPIO_OUTPUT_32, 0);
        gpio_set_level(GPIO_OUTPUT_33, 0);
        gpio_set_level(GPIO_OUTPUT_25, 0);
        gpio_set_level(GPIO_OUTPUT_26, 0);
        gpio_set_level(GPIO_OUTPUT_27, 0);
        gpio_set_level(GPIO_OUTPUT_14, 0);
        enable_LE();
        break;
    default:
        char *messg = "\r\nInvalid option!\n\r";
        int len = strlen(messg);
        uart_write_bytes(UART0_PORT,messg,len);
        break;
    }
    menu();
}

void enable_LE(){
    char *msgg = "\r\n Setting LE to 1\n\r";
    int len_msg = strlen(msgg);
    uart_write_bytes(UART0_PORT,msgg,len_msg);
    gpio_set_level(GPIO_OUTPUT_12, 1);
}

void configure_uart(){  
    /* Set the UART configuration*/      
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
    /* Apply the configuration*/
    ESP_ERROR_CHECK(uart_driver_install(UART0_PORT, BUFFER_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART0_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART0_PORT, UART0_TXD, UART0_RXD, UART0_RTS, UART0_CTS));
}

static void uart_rcv_event_task(void *pvParameters){
    uint8_t *data = (uint8_t *) malloc(BUFFER_SIZE);    // Allocate memory to variable data
    while (1)
    {   
        // Read data from the RXD buffer 
        const int data_len = uart_read_bytes(UART0_PORT, data, (BUFFER_SIZE - 1), 20 / portTICK_PERIOD_MS);
        if (data_len > 0)
        {   
            uart_write_bytes(UART0_PORT, data, data_len);   // Make an echo of the data received
            gpio_event(data);                               // Set the attenuator configuration                  
            data[data_len] = '\0'; 
        }
        
    }
    free(data); // Free up the memory space assigned to variable data
}