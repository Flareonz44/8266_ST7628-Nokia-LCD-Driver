//don't include twice
#ifndef 8266_ST7628_HEADER
#define 8266_ST7628_HEADER

//graphic core
#include <Adafruit_GFX.h>

//some definitions
#define 8266_ST7628_TFTWIDTH  97
#define 8266_ST7628_TFTHEIGHT 69

#define 8266_ST7628_NOP        0x00
#define 8266_ST7628_SWRESET    0x01
#define 8266_ST7628_RDDID      0x04
#define 8266_ST7628_RDDST      0x09

#define 8266_ST7628_SLPIN      0x10
#define 8266_ST7628_SLPOUT     0x11
#define 8266_ST7628_PTLON      0x12
#define 8266_ST7628_NORON      0x13

#define 8266_ST7628_INVOFF     0x20
#define 8266_ST7628_INVON      0x21
#define 8266_ST7628_WRCNTR     0x25
#define 8266_ST7628_DISPOFF    0x28
#define 8266_ST7628_DISPON     0x29
#define 8266_ST7628_CASET      0x2A
#define 8266_ST7628_RASET      0x2B
#define 8266_ST7628_RAMWR      0x2C
#define 8266_ST7628_RAMRD      0x2E

#define 8266_ST7628_PTLAR      0x30
#define 8266_ST7628_COLMOD     0x3A
#define 8266_ST7628_MADCTL     0x36
#define 8266_ST7628_IDMOFF     0x38

#define 8266_ST7628_FRMCTR1    0xB1
#define 8266_ST7628_FRMCTR2    0xB2
#define 8266_ST7628_FRMCTR3    0xB3
#define 8266_ST7628_INVCTR     0xB4
#define 8266_ST7628_NLIS       0xB5
#define 8266_ST7628_DISSET5    0xB6

#define 8266_ST7628_PWCTR1     0xC0
#define 8266_ST7628_PWCTR2     0xC1
#define 8266_ST7628_PWCTR3     0xC2
#define 8266_ST7628_PWCTR4     0xC3
#define 8266_ST7628_PWCTR5     0xC4
#define 8266_ST7628_VMCTR1     0xC5
#define 8266_ST7628_VSS        0xCB
#define 8266_ST7628_ID1S       0xCC
#define 8266_ST7628_ID3S       0xCE

#define 8266_ST7628_ANASET     0xD0
#define 8266_ST7628_ALS        0xD7
#define 8266_ST7628_RDID1      0xDA
#define 8266_ST7628_RDID2      0xDB
#define 8266_ST7628_RDID3      0xDC
#define 8266_ST7628_RDID4      0xDD

#define 8266_ST7628_PWCTR6     0xFC
#define 8266_ST7628_THYS       0xF7

#define 8266_ST7628_GMCTRP1    0xE0
#define 8266_ST7628_GMCTRN1    0xE1
#define 8266_ST7628_EPMRD      0xE3

// some colors
#define	8266_ST7628_BLACK      0x0000
#define	8266_ST7628_BLUE       0x001F
#define	8266_ST7628_RED        0xF800
#define	8266_ST7628_GREEN      0x07E0
#define 8266_ST7628_CYAN       0x07FF
#define 8266_ST7628_MAGENTA    0xF81F
#define 8266_ST7628_YELLOW     0xFFE0  
#define 8266_ST7628_WHITE      0xFFFF