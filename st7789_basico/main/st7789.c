#include "st7789.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>

static const char *TAG = "ST7789";

/* ==============================
 * Handle do barramento SPI
 * ============================== */

static spi_device_handle_t spi;

/* ==============================
 * Função para controlar DC
 * ============================== */

static void st7789_dc(uint8_t level)
{
    gpio_set_level(ST7789_DC_GPIO, level);
}

/* ==============================
 * Função para controlar CS
 * ============================== */

static void st7789_cs(uint8_t level)
{
    gpio_set_level(ST7789_CS_GPIO, level);
}

/* ==============================
 * Envia comando para o ST7789
 * ============================== */

static void st7789_write_command(uint8_t command)
{
    spi_transaction_t transaction;

    memset(&transaction, 0, sizeof(transaction));

    transaction.length = 8;
    transaction.tx_buffer = &command;

    st7789_dc(0);
    st7789_cs(0);

    spi_device_polling_transmit(spi, &transaction);

    st7789_cs(1);
}

/* ==============================
 * Envia dados para o ST7789
 * ============================== */

static void st7789_write_data(const uint8_t *data, size_t length)
{
    spi_transaction_t transaction;

    memset(&transaction, 0, sizeof(transaction));

    transaction.length = length * 8;
    transaction.tx_buffer = data;

    st7789_dc(1);
    st7789_cs(0);

    spi_device_polling_transmit(spi, &transaction);

    st7789_cs(1);
}

/* ==============================
 * Reset do display
 * ============================== */

static void st7789_reset(void)
{
    gpio_set_level(ST7789_RES_GPIO, 0);

    vTaskDelay(pdMS_TO_TICKS(100));

    gpio_set_level(ST7789_RES_GPIO, 1);

    vTaskDelay(pdMS_TO_TICKS(100));
}

/* ==============================
 * Configuração GPIO
 * ============================== */

static void st7789_gpio_init(void)
{
    gpio_config_t config = {
        .pin_bit_mask =
            (1ULL << ST7789_CS_GPIO) |
            (1ULL << ST7789_DC_GPIO) |
            (1ULL << ST7789_RES_GPIO),

        .mode = GPIO_MODE_OUTPUT,

        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,

        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&config);

    gpio_set_level(ST7789_CS_GPIO, 1);
    gpio_set_level(ST7789_DC_GPIO, 1);
    gpio_set_level(ST7789_RES_GPIO, 1);
}

/* ==============================
 * Inicialização SPI
 * ============================== */

static void st7789_spi_init(void)
{
    spi_bus_config_t bus_config = {
        .mosi_io_num = ST7789_MOSI_GPIO,
        .miso_io_num = -1,

        .sclk_io_num = ST7789_SCK_GPIO,

        .quadwp_io_num = -1,
        .quadhd_io_num = -1,

        .max_transfer_sz =
            ST7789_WIDTH * ST7789_HEIGHT * 2
    };

    spi_bus_initialize(
        SPI2_HOST,
        &bus_config,
        SPI_DMA_CH_AUTO
    );

    spi_device_interface_config_t device_config = {
        .clock_speed_hz = 10 * 1000 * 1000,

        .mode = 0,

        .spics_io_num = -1,

        .queue_size = 1
    };

    spi_bus_add_device(
        SPI2_HOST,
        &device_config,
        &spi
    );
}

/* ==============================
 * Define área de desenho
 * ============================== */

static void st7789_set_window(
    uint16_t x0,
    uint16_t y0,
    uint16_t x1,
    uint16_t y1)
{
    uint8_t data[4];

    /* CASET */

    st7789_write_command(0x2A);

    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;

    st7789_write_data(data, 4);

    /* RASET */

    st7789_write_command(0x2B);

    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;

    st7789_write_data(data, 4);

    /* RAMWR */

    st7789_write_command(0x2C);
}

/* ==============================
 * Inicialização do ST7789
 * ============================== */

void st7789_init(void)
{
    ESP_LOGI(TAG, "Inicializando ST7789");

    st7789_gpio_init();

    st7789_spi_init();

    st7789_reset();

    /* Sleep Out */

    st7789_write_command(0x11);

    vTaskDelay(pdMS_TO_TICKS(120));

    /* Color Mode */

    st7789_write_command(0x3A);

    uint8_t color_mode = 0x55;

    st7789_write_data(&color_mode, 1);

    /* Memory Access Control */

    st7789_write_command(0x36);

    uint8_t madctl = 0x00;

    st7789_write_data(&madctl, 1);

    /* Inversion ON */

    st7789_write_command(0x21);

    /* Display ON */

    st7789_write_command(0x29);

    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "ST7789 inicializado");
}

/* ==============================
 * Preenche tela
 * ============================== */

void st7789_fill(uint16_t color)
{
    uint8_t pixel[2];

    pixel[0] = color >> 8;
    pixel[1] = color & 0xFF;

    st7789_set_window(
        0,
        0,
        ST7789_WIDTH - 1,
        ST7789_HEIGHT - 1
    );

    /*
     * Envia pixel por pixel.
     *
     * Não é a versão otimizada ainda.
     * Estamos fazendo assim propositalmente
     * para entender a comunicação.
     */

    for (uint32_t i = 0;
         i < ST7789_WIDTH * ST7789_HEIGHT;
         i++)
    {
        st7789_write_data(pixel, 2);
    }
}

/* ==============================
 * Desenha um pixel
 * ============================== */

void st7789_draw_pixel(
    uint16_t x,
    uint16_t y,
    uint16_t color)
{
    if (x >= ST7789_WIDTH ||
        y >= ST7789_HEIGHT)
    {
        return;
    }

    st7789_set_window(
        x,
        y,
        x,
        y
    );

    uint8_t pixel[2];

    pixel[0] = color >> 8;
    pixel[1] = color & 0xFF;

    st7789_write_data(pixel, 2);
}