#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

/* ==============================
 * Configuração dos pinos
 * ============================== */

#define ST7789_SCK_GPIO   4
#define ST7789_MOSI_GPIO  6

#define ST7789_CS_GPIO    8
#define ST7789_DC_GPIO    9
#define ST7789_RES_GPIO   21

/* ==============================
 * Resolução
 * ============================== */

#define ST7789_WIDTH   240
#define ST7789_HEIGHT  240

/* ==============================
 * Cores RGB565
 * ============================== */

#define ST7789_BLACK   0x0000
#define ST7789_WHITE   0xFFFF
#define ST7789_RED     0xF800
#define ST7789_GREEN   0x07E0
#define ST7789_BLUE    0x001F
#define ST7789_YELLOW  0xFFE0
#define ST7789_CYAN    0x07FF
#define ST7789_MAGENTA 0xF81F
#define ST7789_ORANGE  0xFD20
#define ST7789_PURPLE  0x780F
#define ST7789_GRAY    0x8410

/* ==============================
 * Funções públicas
 * ============================== */

void st7789_init(void);

void st7789_fill(uint16_t color);

void st7789_draw_pixel(uint16_t x,
                       uint16_t y,
                       uint16_t color);


void st7789_draw_rect(uint16_t x,
                      uint16_t y,
                      uint16_t w,
                      uint16_t h,
                      uint16_t color);

void st7789_draw_line(int x0,
                      int y0,
                      int x1,
                      int y1,
                      uint16_t color);

void st7789_draw_circle(int cx,
                        int cy,
                        int radius,
                        uint16_t color);
#endif