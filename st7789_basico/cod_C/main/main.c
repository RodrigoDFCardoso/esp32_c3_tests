#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdlib.h>
#include "st7789.h"

void app_main(void)
{
    st7789_init();

    while (1)
    {
        /* =========================
         * TESTE 1 - CORES
         * ========================= */

        st7789_fill(ST7789_RED);
        vTaskDelay(pdMS_TO_TICKS(1000));

        st7789_fill(ST7789_GREEN);
        vTaskDelay(pdMS_TO_TICKS(1000));

        st7789_fill(ST7789_BLUE);
        vTaskDelay(pdMS_TO_TICKS(1000));

        st7789_fill(ST7789_WHITE);
        vTaskDelay(pdMS_TO_TICKS(1000));

        st7789_fill(ST7789_BLACK);
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* =========================
         * TESTE 2 - CORES DIVERSAS
         * ========================= */

        st7789_fill(ST7789_YELLOW);
        vTaskDelay(pdMS_TO_TICKS(700));

        st7789_fill(ST7789_CYAN);
        vTaskDelay(pdMS_TO_TICKS(700));

        st7789_fill(ST7789_MAGENTA);
        vTaskDelay(pdMS_TO_TICKS(700));

        st7789_fill(ST7789_ORANGE);
        vTaskDelay(pdMS_TO_TICKS(700));

        /* =========================
         * TESTE 3 - PIXEL
         * ========================= */

        st7789_fill(ST7789_BLACK);

        st7789_draw_pixel(
            120,
            120,
            ST7789_WHITE
        );

        vTaskDelay(pdMS_TO_TICKS(1000));

        /* =========================
         * TESTE 4 - RETÂNGULOS
         * ========================= */

        st7789_fill(ST7789_BLACK);

        st7789_draw_rect(
            10, 10,
            220, 220,
            ST7789_WHITE
        );

        st7789_draw_rect(
            30, 30,
            180, 180,
            ST7789_RED
        );

        st7789_draw_rect(
            60, 60,
            120, 120,
            ST7789_GREEN
        );

        st7789_draw_rect(
            90, 90,
            60, 60,
            ST7789_BLUE
        );

        vTaskDelay(pdMS_TO_TICKS(2000));

        /* =========================
         * TESTE 5 - CÍRCULOS
         * ========================= */

        st7789_fill(ST7789_BLACK);

        st7789_draw_circle(
            120,
            120,
            100,
            ST7789_WHITE
        );

        st7789_draw_circle(
            120,
            120,
            70,
            ST7789_RED
        );

        st7789_draw_circle(
            120,
            120,
            40,
            ST7789_GREEN
        );

        st7789_draw_circle(
            120,
            120,
            15,
            ST7789_BLUE
        );

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}