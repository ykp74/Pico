#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#ifndef BOOL
    #define BOOL
    #define FALSE 0
    #define TRUE  1
#endif

#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

// Constants
#define   SSD1306_I2C_ADDRESS  0x3C	// 011110+SA0+RW - 0x3C or 0x3D
#define   SSD1306_SETCONTRAST  0x81
#define   SSD1306_DISPLAYALLON_RESUME  0xA4
#define   SSD1306_DISPLAYALLON  0xA5
#define   SSD1306_NORMALDISPLAY  0xA6
#define   SSD1306_INVERTDISPLAY  0xA7
#define   SSD1306_DISPLAYOFF  0xAE
#define   SSD1306_DISPLAYON  0xAF
#define   SSD1306_SETDISPLAYOFFSET  0xD3
#define   SSD1306_SETCOMPINS  0xDA
#define   SSD1306_SETVCOMDETECT  0xDB
#define   SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define   SSD1306_SETPRECHARGE  0xD9
#define   SSD1306_SETMULTIPLEX  0xA8
#define   SSD1306_SETLOWCOLUMN  0x00
#define   SSD1306_SETHIGHCOLUMN  0x10
#define   SSD1306_SETSTARTLINE  0x40
#define   SSD1306_MEMORYMODE  0x20
#define   SSD1306_COLUMNADDR  0x21
#define   SSD1306_PAGEADDR  0x22
#define   SSD1306_COMSCANINC  0xC0
#define   SSD1306_COMSCANDEC  0xC8
#define   SSD1306_SEGREMAP  0xA0
#define   SSD1306_CHARGEPUMP  0x8D
#define   SSD1306_EXTERNALVCC  0x1
#define   SSD1306_SWITCHCAPVCC  0x2

// Scrolling constants
#define   SSD1306_ACTIVATE_SCROLL  0x2F
#define   SSD1306_DEACTIVATE_SCROLL  0x2E
#define   SSD1306_SET_VERTICAL_SCROLL_AREA  0xA3
#define   SSD1306_RIGHT_HORIZONTAL_SCROLL  0x26
#define   SSD1306_LEFT_HORIZONTAL_SCROLL  0x27
#define   SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL  0x29
#define   SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL   0x2A

#define   SSD1306_WIDTH   128
#define   SSD1306_HEIGHT   64

class SSD1306
{
    public:
        enum Mode
        {
            SCROLL,
            WRAP
        };

        SSD1306();
        void clearDisplay();
        void initDisplay();
        void textDisplay(const char *message);
        void setWordWrap(int);
        void setDisplayMode(SSD1306::Mode);

    private:
        // line buffers (128 chars * 8 lines)
    	unsigned char displayLines[8][128];
    	int currentLine = 0;
        int wordWrap = FALSE;
        int displayMode = Mode::SCROLL;
        int currentScrollLine = 0;
        int needScroll = FALSE;

        // i2c handles
        int i2cHandle = -1;
        int i2cInitialised = FALSE;
    	// int i2cAddress = 0x3C;

        // ssd1306 command sequences
        unsigned char initSequence[26] = {0x00,0xAE,0xA8,0x3F,0xD3,0x00,0x40,0xA1,0xC8,0xDA,0x12,0x81,0x7F,
                                          0xA4,0xA6,0xD5,0x80,0x8D,0x14,0xD9,0x22,0xD8,0x30,0x20,0x00,0xAF};

    	unsigned char setFullRange[7] = {0x00,0x21,0x00,0x7F,0x22,0x00,0x07};

        unsigned char scrollUpSequence[3] = {0x00,0xD3,0x08};

        // helper functions
        int addFontBytes(int curr, unsigned char c);
        void setDisplayRange(int line);
        void updateDisplayFull();
        void writeI2C(unsigned char* data, int bytes); 
        void scrollUp(int); 
};

int SSD1306_init(void);

#endif