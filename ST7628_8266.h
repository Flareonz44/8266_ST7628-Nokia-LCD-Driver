//don't include twice
#ifndef ST7628_8266_H
#define ST7628_8266_H

//graphic core
#include <Adafruit_GFX.h>

//some definitions
#define ST7628_8266_TFTWIDTH   97        // 98 is max
#define ST7628_8266_TFTHEIGHT  69        // 70 is max

#define ST7628_8266_NOP        0x00
#define ST7628_8266_SWRESET    0x01
#define ST7628_8266_RDDID      0x04
#define ST7628_8266_RDDST      0x09

#define ST7628_8266_SLPIN      0x10
#define ST7628_8266_SLPOUT     0x11
#define ST7628_8266_PTLON      0x12
#define ST7628_8266_NORON      0x13

#define ST7628_8266_INVOFF     0x20
#define ST7628_8266_INVON      0x21
#define ST7628_8266_WRCNTR     0x25
#define ST7628_8266_DISPOFF    0x28
#define ST7628_8266_DISPON     0x29
#define ST7628_8266_CASET      0x2A
#define ST7628_8266_RASET      0x2B
#define ST7628_8266_RAMWR      0x2C
#define ST7628_8266_RAMRD      0x2E

#define ST7628_8266_PTLAR      0x30
#define ST7628_8266_COLMOD     0x3A
#define ST7628_8266_MADCTL     0x36
#define ST7628_8266_IDMOFF     0x38

#define ST7628_8266_FRMCTR1    0xB1
#define ST7628_8266_FRMCTR2    0xB2
#define ST7628_8266_FRMCTR3    0xB3
#define ST7628_8266_INVCTR     0xB4
#define ST7628_8266_NLIS       0xB5
#define ST7628_8266_DISSET5    0xB6

#define ST7628_8266_PWCTR1     0xC0
#define ST7628_8266_PWCTR2     0xC1
#define ST7628_8266_PWCTR3     0xC2
#define ST7628_8266_PWCTR4     0xC3
#define ST7628_8266_PWCTR5     0xC4
#define ST7628_8266_VMCTR1     0xC5
#define ST7628_8266_VSS        0xCB
#define ST7628_8266_ID1S       0xCC
#define ST7628_8266_ID3S       0xCE

#define ST7628_8266_ANASET     0xD0
#define ST7628_8266_ALS        0xD7
#define ST7628_8266_RDID1      0xDA
#define ST7628_8266_RDID2      0xDB
#define ST7628_8266_RDID3      0xDC
#define ST7628_8266_RDID4      0xDD

#define ST7628_8266_PWCTR6     0xFC
#define ST7628_8266_THYS       0xF7

#define ST7628_8266_GMCTRP1    0xE0
#define ST7628_8266_GMCTRN1    0xE1
#define ST7628_8266_EPMRD      0xE3

// some colors
#define	ST7628_8266_BLACK      0x0000
#define	ST7628_8266_BLUE       0x001F
#define	ST7628_8266_RED        0xF800
#define	ST7628_8266_GREEN      0x07E0
#define ST7628_8266_CYAN       0x07FF
#define ST7628_8266_MAGENTA    0xF81F
#define ST7628_8266_YELLOW     0xFFE0  
#define ST7628_8266_WHITE      0xFFFF

class ST7628_8266 : public Adafruit_GFX {
    public:
    ST7628_8266(int8_t CS /*Chip Select*/, int8_t RST /*ReSeT*/);
    void      init(void);
    void      setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void      pushColor(uint16_t color);
    void      fillScreen(uint16_t color);
    void      drawPixel(int16_t x, int16_t y, uint16_t color);
    void      drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void      drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void      fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void      setRotation(uint8_t r);
    void      invertDisplay(boolean i);
    uint16_t  Color565(uint8_t r, uint8_t g, uint8_t b);

    private:
    uint8_t  tabcolor;
    void     writeCommand(uint8_t c);
    void     writeData(uint8_t d);
    void     commandList(const uint8_t *addr);
    void     commonInit(const uint8_t *cmdList);
    uint8_t  _cs, _rst, _colStart, _rowStart;
};

#endif // for the entire header