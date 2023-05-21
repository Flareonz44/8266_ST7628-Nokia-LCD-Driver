//includes
#include <Adafruit_GFX.h> //graphics
#include <ST7628.h> //definitions and etc
//#include <limits.h> //??
//#include <pgmspace.h> //memory management
#include <SPI.h> //handle SPI interface

//swap colors
inline uint16_t swapcolor(uint16_t x) {  //inline for performance improvement
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

//constructor with HSPI (Hardware SPI)
ST7628::ST7628(int8_t CS /*Chip Select*/, int8_t RST /*ReSeT*/) : Adafruit_GFX(8266_ST7628_TFTWIDTH, 8266_ST7628_TFTHEIGHT) {
    _cs   = CS;
    _rst  = RST;
}

// send 8bits data throw hardware SPI
void spiwrite(uint8_t c) {
    SPI.transfer(c); // data transfer
}

// send command datatype
void ST7628::writecommand(uint8_t c) {
    digitalWrite(_cs, LOW); // enable LCD
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(0x0); // send first bit low, tells LCD next stream is a command
    //spiwrite(c); // main data transfer
    SPI.transfer(c);
    SPI.endTransaction();
    digitalWrite(_cs, HIGH); // disable LCD
}

// send data datatype
void ST7628::writedata(uint8_t c) {
    digitalWrite(_cs, LOW); // enable LCD
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(0x1); // send first bit high, tells LCD next stream is data
    spiwrite(c); // main data transfer
    SPI.endTransaction();
    digitalWrite(_cs, HIGH); // disable LCD
}

