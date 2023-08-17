#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "ssd1306.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;
SSD1306 myDisplay;  /* Define Local dispaly value */
const uint8_t ds3231Address = 0x68;
const uint8_t sdaPin = 14;
const uint8_t sclPin = 15;

typedef struct  {
    uint8_t year = 0;
    uint8_t month = 0;
    uint8_t date =0;
    uint8_t hour = 0;
    uint8_t min = 0;
    uint8_t sec =0;
} tTime;

static tTime timeData;

uint8_t bcdToDec(uint8_t val)
{
  return( (val/16*10) + (val%16) );
}

uint8_t decToBcd(uint8_t value)
{   //10진수로 이루어진 시간값을 DS3231에서 다루는 2진형 값으로 변환시켜 주는 함수
    uint8_t bcd = (((value/10)<<4) + (value%10));
    return bcd;
}

void ds3231GetTime(uint8_t* buf)
{
    timeData.year  = bcdToDec(buf[6]);
    timeData.month = bcdToDec(buf[5]&0x1F);
    timeData.date = bcdToDec(buf[4]&0x3F);

    timeData.hour = bcdToDec(buf[2]&0x3F);
    timeData.min = bcdToDec(buf[1]&0x7F);
    timeData.sec = bcdToDec(buf[0]&0x7F);

//    printf("%2d/%2d/%2d\n",bcdToDec(buf[6]),bcdToDec(buf[5]&0x1F), bcdToDec(buf[4]&0x3F) );
//    printf("%2d:%2d:%2d\n",bcdToDec(buf[2]&0x3F),bcdToDec(buf[1]&0x7F), bcdToDec(buf[0]&0x7F) );
}

int ds3231SetTime()
{
	uint8_t buf[2];
	//set second
	buf[0]=0x00;
	buf[1]=0x50;
	i2c_write_blocking(i2c_default,ds3231Address,buf,2,false);
	//set minute
	buf[0]=0x01;
	buf[1]=0x50;
	i2c_write_blocking(i2c_default,ds3231Address,buf,2,false);
	//set hour
	buf[0]=0x02;
	buf[1]=0x04;
	i2c_write_blocking(i2c_default,ds3231Address,buf,2,false);
	//set weekday
	buf[0]=0x03;
	buf[1]=0x04;
	i2c_write_blocking(i2c_default,ds3231Address,buf,2,false);
	//set day
	buf[0]=0x04;
	buf[1]=0x04;
	i2c_write_blocking(i2c_default,ds3231Address,buf,2,false);
	//set month
	buf[0]=0x05;
	buf[1]=0x03;
	i2c_write_blocking(i2c_default,ds3231Address,buf,2,false);
	//set year
	buf[0]=0x06;
	buf[1]=0x23;
	i2c_write_blocking(i2c_default,ds3231Address,buf,2,false);
	return 0;
}

void input_task(void *p)
{
    int hour,min,sec;

    while(true){
        while(scanf("%d %d %d",&hour, &min,&sec) == -1){
            vTaskDelay(pdMS_TO_TICKS(500));
        }        
        printf("Get data : %d %d %d \n",hour, min, sec);      
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void text_task(void *p)
{
    char timeText[13];
    int numRead = 0;
    static uint8_t buf[8];

    while(true){
        /* Get low data of date and time from DS3231 */
        i2c_write_blocking(i2c_default, ds3231Address, 0x00, 1, false);
        numRead = i2c_read_blocking(i2c_default, ds3231Address, buf, 7, false);

        /* Convert real time data */
        ds3231GetTime(buf);
        sprintf(timeText,"%2d:%2d:%2d",timeData.hour,timeData.min,timeData.sec );
        
        /* Display to LCD */
        myDisplay.textDisplay(timeText);
        //sleep_ms(2000);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void led_task(void *p)
{
    int numRead = 0;
    static uint8_t buf[8];

    while(true){
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
      
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main() {
    //setup_default_uart();
    stdio_init_all();  //use usb serial
    printf("Hello, world!\n");
    
    SSD1306_init(); //OLED Driver init
    
    //i2c_init(i2c1, 100*1000);
    //gpio_set_function(sdaPin, GPIO_FUNC_I2C);
    //gpio_set_function(sclPin, GPIO_FUNC_I2C); 
    //gpio_pull_up(sdaPin);
    //gpio_pull_up(sclPin);
    //ds3231SetTime();

    myDisplay.setDisplayMode(SSD1306::Mode::SCROLL);
    myDisplay.setWordWrap(TRUE);
    myDisplay.textDisplay("THIS IS THE FIRST LINE.");

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    xTaskCreate(led_task, "LED_Task 1", 256, NULL, 1, NULL);

    xTaskCreate(text_task, "TEXT_Task 2", 256, NULL, 1, NULL);

    xTaskCreate(input_task, "INPUT_Task 3", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true) {
        ;
    }

    return 0;
}