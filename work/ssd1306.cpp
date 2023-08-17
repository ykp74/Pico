//SSD1306 Driver for Rasberry PICO
#include <ctype.h>
#include "ssd1306.h"
#include "charmap.h"

//#define DEBUG
#ifdef DEBUG
#define D if(1) 
#else
#define D if(0) 
#endif

 // By default these LCD display drivers are on bus address 0x27
 static int addr = SSD1306_I2C_ADDRESS;

/* Quick helper function for single byte transfers */
 void i2c_write_byte(uint8_t val) {
 //#ifdef i2c_default
     i2c_write_blocking(i2c_default, addr, &val, 1, false);
 //#endif
 }

int SSD1306_init(void) {
    // Handle hardware I2C
    
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Make the I2C pins available to picotool
    //bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    printf("Using hardware I2C with custom I2C provider. \n");
 
    return 0;
}

SSD1306::SSD1306(void) {
    for(int i=0; i<8; i++){
        for(int j=0; j<128; j++){
            displayLines[i][j] = 0;
        }
    }
}

// void SSD1306::writeI2C(unsigned char* data, int bytes) 
// {
//     for(int i=0; i<bytes; i++) { 
//         D printf("%x ", data[i]);
//     }
//     write(fd_i2c, data, bytes);
//     D printf("\n");
// }

void SSD1306::initDisplay(void) {
    D printf("initDisplay\n");
    //writeI2C(initSequence, 26);
    i2c_write_blocking(i2c_default, addr, initSequence, 26, false);
    i2cInitialised = TRUE;
}

void SSD1306::textDisplay(const char *message) {                                                                                                                                                                                                                                                                                                                                                                                                 
    int currByteCount = 0;
    
    if( !i2cInitialised ){
        initDisplay();
    }

    // and scroll up if needed (in SCROLL mode)
    if((displayMode == SSD1306::Mode::SCROLL) && needScroll){
        scrollUp(1);
    } 

    D printf("textDisplay - %s\n", message);

    for(int i=0; i<strlen(message); i++){
        int bytesAdded = addFontBytes(currByteCount, message[i]);
        if(bytesAdded > 0){
            currByteCount += bytesAdded;
        } else {
            if(wordWrap){
                // fill the rest of this line with 0s
                while(currByteCount < 128) {
                    displayLines[currentLine][currByteCount++] = 0x00;
                }
                // move it onto the next line
                currentLine++;
                // and scroll up if needed (in SCROLL mode)
                if((displayMode == SSD1306::Mode::SCROLL) && needScroll){
                    scrollUp(1);
                } 

                if(currentLine > 7){ 
                    currentLine = 0;  
                    needScroll = TRUE;                  
                }
                currByteCount=0;
                i--; // move back one so we try it again
            }
        }
    }

    // now fill up any left over with 0x00s on the current line
    while(currByteCount < 128){
        displayLines[currentLine][currByteCount++] = 0x00;
    }

    // now point to next line and set to first char
    currByteCount = 0;
    currentLine++;
    if(currentLine > 7){ 
        currentLine = 0;
        needScroll = TRUE;
    }

    updateDisplayFull();
}

void SSD1306::clearDisplay(void) {
    // blank out the line buffers
    for(int i=0; i<8; i++){
        for(int j=0; j<128; j++){
            displayLines[i][j] = 0;
        }
    }
    //reset the scroll pointer and line pointer
    currentScrollLine = 0;
    currentLine = 0;
    // write it out
    updateDisplayFull();
}

void SSD1306::setWordWrap(int b){
    wordWrap = b;
}

void SSD1306::setDisplayMode(SSD1306::Mode mode) {
    displayMode = mode;
}

// private functions
void SSD1306::scrollUp(int lines) {
    if(!i2cInitialised){
        initDisplay();
    }
    currentScrollLine += lines;
    if (currentScrollLine > 7) { 
        currentScrollLine -= 8;
    }
    scrollUpSequence[2] = currentScrollLine * 8;
    D printf("scrollUp\n");
    //writeI2C(scrollUpSequence, 3);
    i2c_write_blocking(i2c_default, addr, scrollUpSequence, 3, false);
}

int SSD1306::addFontBytes(int curr, unsigned char c) {
    D printf("addFontBytes - %i - ", c);
    c = toupper(c); // we only support UPPERCASE letters
    int letterIdx = (c - ' ');
    int letterBytes = fontData[letterIdx][0];
    
    if(letterIdx > 64){
        letterIdx = 65;
    }

    if((curr + letterBytes + 1) > 127 ){
        return 0;
        D printf("\n");
    } else {
        for(int i=0; i<letterBytes; i++){
            D printf("%x ", fontData[letterIdx][1 + i]);
            displayLines[currentLine][curr + i] = fontData[letterIdx][1 + i]; 
        }
        displayLines[currentLine][curr + letterBytes++] = 0x00;  // single byte space / seperator
        D printf("\n");
        return letterBytes;
    }
}

void SSD1306::setDisplayRange (int line = -1){
    // -1 = full range
    // 0..7 = line
    D printf("setDisplayRange (7 bytes)\n");
    if(line == -1){
        //writeI2C(setFullRange, 7);
        i2c_write_blocking(i2c_default, addr, setFullRange, 7, true);
    }
}

void SSD1306::updateDisplayFull(void){
    setDisplayRange(-1);
    for(int line=0; line<8; line++){
        unsigned char buffer[129] = {0};
        buffer[0] = 0x40;
        for(int i=0; i<128; i++){
            buffer[1 + i] = displayLines[line][i];
        }
        //writeI2C(buffer, 129);
        i2c_write_blocking(i2c_default, addr, buffer, 129, false);
    }
}