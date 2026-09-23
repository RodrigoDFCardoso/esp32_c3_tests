#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "st7789.h"

void app_main(void)
{
    st7789_init();

    st7789_fill(0x07E0);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}