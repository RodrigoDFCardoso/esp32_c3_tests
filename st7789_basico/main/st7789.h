#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

/* ==============================
 * Configuração dos pinos
 * ============================== */

#define ST7789_SCK_GPIO   8
#define ST7789_MOSI_GPIO  10

#define ST7789_CS_GPIO    2
#define ST7789_DC_GPIO    3
#define ST7789_RES_GPIO   4

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

/* ==============================
 * Funções públicas
 * ============================== */

void st7789_init(void);

void st7789_fill(uint16_t color);

void st7789_draw_pixel(uint16_t x,
                       uint16_t y,
                       uint16_t color);

#endif