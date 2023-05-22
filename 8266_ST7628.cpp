//includes
#include <Adafruit_GFX.h>                                               // graphics
#include <ST7628.h>                                                     // definitions and etc
//#include <limits.h> //??
#include <pgmspace.h>                                                   // access flash memory during runtime
#include <SPI.h>                                                        // handle SPI interface

//swap colors
inline uint16_t swapcolor(uint16_t x) {                                 // inline for performance improvement
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

//constructor with HSPI (Hardware SPI)
8266_ST7628::8266_ST7628(int8_t CS /*Chip Select*/, int8_t RST /*ReSeT*/) : Adafruit_GFX(8266_ST7628_TFTWIDTH, 8266_ST7628_TFTHEIGHT) {
    _cs   = CS;
    _rst  = RST;
}

// send command datatype
void 8266_ST7628::writeCommand(uint8_t c) {
    digitalWrite(_cs, LOW);                                             // enable LCD
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(0x0);                                                  // send first bit low, tells LCD next stream is a command
    SPI.transfer(c);                                                    // main data transfer
    SPI.endTransaction();
    digitalWrite(_cs, HIGH);                                            // disable LCD
}

// send data datatype
void 8266_ST7628::writeData(uint8_t c) {
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
    8266_ST7628_WRCNTR, 1, 0x39,
    8266_ST7628_SLPOUT, CM_DELAY, 10,
    8266_ST7628_NORON,  CM_DELAY, 10,
    8266_ST7628_INVOFF, CM_DELAY, 10,
    8266_ST7628_IDMOFF, CM_DELAY, 10,
    8266_ST7628_MADCTL , 1, 0x00,
    8266_ST7628_COLMOD, 1, 0x05,
    8266_ST7628_DISPON, CM_DELAY, 255
    // list continues...should I complete it?
};
// This function is the one that reads and executes the list above
void 8266_ST7628::commandList(const uint8_t *addr){
    uint8_t  numCommands;
    uint8_t  numArgs;
    uint16_t ms;
    numCommands = pgm_read_byte(addr++);                                // Number of commands that list has, ie '8'
    while(numCommands--) {                                              // will execute numCommands times, ie '8' times
        writecommand(pgm_read_byte(addr++));                            // read and commit command, then increment addr
        numArgs  = pgm_read_byte(addr++);                               // get number of arguments the command has
        ms       = numArgs & CM_DELAY;                                  // same as ms = (numArgs == CM_DELAY); but faster
        numArgs &= ~DELAY;                                              // same as if(numArgs == CM_DELAY) numArgs = 0; but faster
        while(numArgs--) {                                              // for each argument if it was != from CM_DELAY
          writedata(pgm_read_byte(addr++));                             // commit wach argument to LCD
        }
        if(ms){                                                         // if CM_DELAY was detected
          ms = pgm_read_byte(addr++);                                   // get amount of delay time in ms
          if(ms == 255) ms = 500;                                       // since each item is 8bit long, the max number possible is 255, so if ms == 255 => set ms to 500ms
          delay(ms);                                                    // delay, just that
        }
    }
}

// common initialization of displays
void 8266_ST7628::commonInit(const uint8_t *cmdList){
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
void 8266_ST7628::init(void){
    commonInit(initComList);                                            // pass ouw command list we define above
}

// set workspace
void 8266_ST7628::setAddrWindow(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1){
    writeCommand(8266_ST7628_CASET);                                    // Column Address SET
    writeData(x0+_colStart);                                            // XSTART 
    writeData(x1+_colStart);                                            // XEND
    writeCommand(8266_ST7628_RASET);                                    // Row Address SET
    writeData(y0+_rowStart);                                            // YSTART
    writeData(y1+_rowStart);                                            // YEND
    writeCommand(8266_ST7628_RAMWR);                                    // write to LCD RAM
}

// send color to LCD
inline void 8266_ST7628::pushColor(uint16_t color){                     //pretty simple: the LCD data bus is 8bit long, and color is 16bit long,
    writeData(color >> 8);                                              //  so data color should be SPL\
    writeData(color);                                                   //                            \ITED
}

// draws exactly one pixel
void 8266_ST7628::drawPixel(int16_t x, int16_t x, uint16_t color){
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;    // check pixel is within limits
    setAddrWindow(x,y,x,y);                                             // set workspace
    pushColor(color);                                                   // send color data
}

// draw a vertical line as fast as possible
void 8266_ST7628::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color){
    if((x >= _width) || (y >= _height)) return;                         // check origin inside bounds
    if((y+h-1) >= _height) h = _height-y;                               // check endline inside bounds, if out, clip it
    setAddrWindow(x, y, x, y+h-1);                                      // set workspace
    while(h--){                                                         // for each pixel in height (h)
        pushColor(color);                                               // send color data
    }
}

