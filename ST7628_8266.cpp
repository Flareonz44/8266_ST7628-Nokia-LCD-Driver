//includes
#include <Adafruit_GFX.h>                                               // graphics
#include "ST7628_8266.h"                                                // definitions and etc
#include <pgmspace.h>                                                   // access flash memory during runtime
#include <SPI.h>                                                        // handle SPI interface

//swap colors
inline uint16_t swapcolor(uint16_t x) {                                 // inline for performance improvement
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

//constructor with HSPI (Hardware SPI)
ST7628_8266::ST7628_8266(int8_t CS /*Chip Select*/, int8_t RST /*ReSeT*/) : Adafruit_GFX(ST7628_8266_TFTWIDTH, ST7628_8266_TFTHEIGHT) {
    _cs   = CS;
    _rst  = RST;
}

// send command datatype
void ST7628_8266::writeCommand(uint8_t c) {
    digitalWrite(_cs, LOW);                                             // enable LCD
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(0x0);                                                  // send first bit low, tells LCD next stream is a command
    SPI.transfer(c);                                                    // main data transfer
    SPI.endTransaction();
    digitalWrite(_cs, HIGH);                                            // disable LCD
}

// send data datatype
void ST7628_8266::writeData(uint8_t c) {
    digitalWrite(_cs, LOW);                                             // enable LCD
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(0x1);                                                  // send first bit high, tells LCD next stream is data
    SPI.transfer(c);                                                    // main data transfer
    SPI.endTransaction();
    digitalWrite(_cs, HIGH);                                            // disable LCD
}

// So here is a command table for init screen. this way is better than writing thousands of writeCommand and writeData calls
#define CM_DELAY 0x80
static const uint8_t PROGMEM initComList[] = {                          // we use PROGMEM to store this on flash rather than RAM
    8,
    ST7628_8266_WRCNTR, 1, 0x39,
    ST7628_8266_SLPOUT, CM_DELAY, 10,
    ST7628_8266_NORON,  CM_DELAY, 10,
    ST7628_8266_INVOFF, CM_DELAY, 10,
    ST7628_8266_IDMOFF, CM_DELAY, 10,
    ST7628_8266_MADCTL , 1, 0x00,
    ST7628_8266_COLMOD, 1, 0x05,
    ST7628_8266_DISPON, CM_DELAY, 255
    // list continues...should I complete it?
};
// This function is the one that reads and executes the list above
void ST7628_8266::commandList(const uint8_t *addr){
    uint8_t  numCommands;
    uint8_t  numArgs;
    uint16_t ms;
    numCommands = pgm_read_byte(addr++);                                // Number of commands that list has, ie '8'
    while(numCommands--) {                                              // will execute numCommands times, ie '8' times
        writeCommand(pgm_read_byte(addr++));                            // read and commit command, then increment addr
        numArgs  = pgm_read_byte(addr++);                               // get number of arguments the command has
        ms       = numArgs & CM_DELAY;                                  // same as ms = (numArgs == CM_DELAY); but faster
        numArgs &= ~CM_DELAY;                                              // same as if(numArgs == CM_DELAY) numArgs = 0; but faster
        while(numArgs--) {                                              // for each argument if it was != from CM_DELAY
          writeData(pgm_read_byte(addr++));                             // commit wach argument to LCD
        }
        if(ms){                                                         // if CM_DELAY was detected
          ms = pgm_read_byte(addr++);                                   // get amount of delay time in ms
          if(ms == 255) ms = 500;                                       // since each item is 8bit long, the max number possible is 255, so if ms == 255 => set ms to 500ms
          delay(ms);                                                    // delay, just that
        }
    }
}

// common initialization of displays
void ST7628_8266::commonInit(const uint8_t *cmdList){
    _colStart = _rowStart = 0;                                          // calibrate origin, maybe
    pinMode(_cs, OUTPUT);                                               // setup chip select
    digitalWrite(_cs, HIGH);                                            // disable LCD (wont react to SPI pins status)
    if (_rst){                                                          // if user provides a reset pin, reset LCD
        pinMode(_rst, OUTPUT);                                          // -|
        digitalWrite(_rst, HIGH);                                       //  |
        delay(500);                                                     //  |
        digitalWrite(_rst, LOW);                                        //  |-> LCD RESET
        delay(500);                                                     //  |
        digitalWrite(_rst, HIGH);                                       //  |
        delay(500);                                                     // -|
    }
    if(cmdList) commandList(cmdList);                                   // run init command list
}

// initialization of ST7628 LCD
void ST7628_8266::init(void){
    commonInit(initComList);                                            // pass ouw command list we define above
}

// set workspace
void ST7628_8266::setAddrWindow(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1){
    writeCommand(ST7628_8266_CASET);                                    // Column Address SET
    writeData(x0+_colStart);                                            // XSTART 
    writeData(x1+_colStart);                                            // XEND
    writeCommand(ST7628_8266_RASET);                                    // Row Address SET
    writeData(y0+_rowStart);                                            // YSTART
    writeData(y1+_rowStart);                                            // YEND
    writeCommand(ST7628_8266_RAMWR);                                    // write to LCD RAM
}

// send color to LCD
inline void ST7628_8266::pushColor(uint16_t color){                     //pretty simple: the LCD data bus is 8bit long, and color is 16bit long,
    writeData(color >> 8);                                              //  so data color should be SPL\
    writeData(color);                                                   //                            \ITED
}

// draws exactly one pixel
void ST7628_8266::drawPixel(int16_t x, int16_t y, uint16_t color){
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;    // check pixel is within limits
    setAddrWindow(x,y,x,y);                                             // set workspace
    pushColor(color);                                                   // send color data
}

// draw a vertical line as fast as possible
void ST7628_8266::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color){
    if((x >= _width) || (y >= _height)) return;                         // check origin inside bounds
    if((y+h-1) >= _height) h = _height-y;                               // check endline inside bounds, if out, clip it
    setAddrWindow(x, y, x, y+h-1);                                      // set workspace
    while(h--){                                                         // for each pixel in height (h)
        pushColor(color);                                               // send color data
    }
}

// same as previous, but horizontal
void ST7628_8266::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color){
    if((x >= _width) || (y >= _height)) return;                         // check origin inside bounds
    if((x+w-1) >= _width) w = _width-x;                                 // check endline inside bounds, if out, clip it
    setAddrWindow(x, y, x+w-1, y);                                      // set workspace
    while(w--){                                                         // for each pixel in width (w)
        pushColor(color);                                               // send color data
    }
}

// fill the entire screen
void ST7628_8266::fillScreen(uint16_t color){
    fillRect(0, 0, _width, _height, color);
}

// fill the specified rectangle
void ST7628_8266::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
    if((x >= _width) || (y >= _height)) return;                         // check inside bounds
    if((x + w - 1) >= _width)  w = _width  - x;                         // check width bounds
    if((y + h - 1) >= _height) h = _height - y;                         // check height bounds
    setAddrWindow(x, y, x+w, y+h);
    for(y=h; y>=0; y--){                                                // interesting way of for loop
        for(x=w; x>=0; x--){                                            // doesnt use extra memory, just recicle previous variables
            pushColor(color);                                           // send color data
        }
    }
}

// converts 24bit color to 16bit color
uint16_t ST7628_8266::Color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// some constants to define rotation data
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

// function to update variables based on rotation
// 0 -> no rotation
// 1 -> 90  deg CW
// 2 -> 180 deg CW
// 3 -> 90  deg CCW
void ST7628_8266::setRotation(uint8_t m) {
    writeCommand(ST7628_8266_MADCTL);
    rotation = m % 4;                                                   // can't be higher than 3 options
    switch (rotation) {
        case 0:
            writeData(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
            _width  = ST7628_8266_TFTWIDTH;
            _height = ST7628_8266_TFTHEIGHT;
            break;
        case 1:
            writeData(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
            _width  = ST7628_8266_TFTHEIGHT;
            _height = ST7628_8266_TFTWIDTH;
            break;
        case 2:
            writeData(MADCTL_RGB);
            _width  = ST7628_8266_TFTWIDTH;
            _height = ST7628_8266_TFTHEIGHT;
            break;
        case 3:
            writeData(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
            _width  = ST7628_8266_TFTHEIGHT;
            _height = ST7628_8266_TFTWIDTH;
            break;
    }
}

// this will invert the display
void ST7628_8266::invertDisplay(boolean i) {
    writeCommand(i ? ST7628_8266_INVON : ST7628_8266_INVOFF);
}