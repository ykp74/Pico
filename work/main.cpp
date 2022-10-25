#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "ssd1306.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
SSD1306 myDisplay;  /* Define Local dispaly value */

void text_task(void *p)
{
    while(true){
        myDisplay.textDisplay("LED_ON()");
        //sleep_ms(2000);
        vTaskDelay(pdMS_TO_TICKS(2000));
        myDisplay.textDisplay("LED_OFF()");
        //sleep_ms(2000);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void led_task(void *p)
{
    while(true){
        gpio_put(LED_PIN, 1);
        printf("Hello, world!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_put(LED_PIN, 0);
        printf("Hello, world!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    //setup_default_uart();
    stdio_init_all();  //use usb serial
    printf("Hello, world!\n");
    
    SSD1306_init(); //OLED Driver init
  
    myDisplay.setDisplayMode(SSD1306::Mode::SCROLL);
    myDisplay.setWordWrap(TRUE);
    myDisplay.textDisplay("THIS IS THE FIRST LINE.");

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    xTaskCreate(led_task, "LED_Task 1", 256, NULL, 1, NULL);

    xTaskCreate(text_task, "TEXT_Task 2", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true) {
        ;
    }

    return 0;
}