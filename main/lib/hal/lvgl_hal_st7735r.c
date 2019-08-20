#include <string.h>
#include <memory.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include "lvgl_hal_st7735r.h"

#define  LOG_TAG "st7735"

// ST7735 specific commands used in init
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B // PASET
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
//Add
#define ST7735_VSCRDEF 0x33
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_VSCRSADD 0x37

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

static spi_device_handle_t device_handle;

typedef struct {
    uint8_t reg;
    uint8_t data[16];
    uint8_t len;
} st7735s_init_t;

/**
 * From: https://github.com/m5stack/M5StickC/blob/2431efeff0c65f4c6ac7b26a1c9827d4598b7950/src/utility/ST7735_Init.h
 *       https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST7735.cpp
 * Combination of Rcmd1 + Rcmd2Green + Rcmd3
 */
static const st7735s_init_t st7735s_init_seq[] = {
    { ST7735_SWRESET, {  }, 0  },   // Software RESET
    { ST7735_SLPOUT, {  }, 0  },    // Disable Sleep mode
    { ST7735_FRMCTR1, { 0x01, 0x2c, 0x2d }, 3 },
    { ST7735_FRMCTR2, { 0x01, 0x2c, 0x2d }, 3 },
    { ST7735_FRMCTR3, { 0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d }, 6 },
    { ST7735_INVCTR, { 0x07 }, 1 },
    { ST7735_PWCTR1, { 0xa2, 0x02, 0x84 }, 3 }, // Power control #1: 4.6v, auto
    { ST7735_PWCTR2, { 0xc5 }, 1 }, // Power control #2
    { ST7735_PWCTR3, { 0x0a, 0x00 }, 2 },
    { ST7735_PWCTR4, { 0x8a, 0x2a }, 2 },
    { ST7735_PWCTR5, { 0x8a, 0xee }, 2 },
    { ST7735_VMCTR1, { 0x0e }, 1 },
    { ST7735_INVOFF, {  }, 0 },
    { ST7735_MADCTL, { 0xc8 }, 1 },
    { ST7735_COLMOD, { 0x05 }, 1 },
};

static void st7735r_spi_send_bytes(const uint8_t *payload, size_t len, bool is_cmd)
{
    if(!payload) {
        ESP_LOGE(LOG_TAG, "Payload is null!");
        return;
    }

    spi_transaction_t spi_tract;
    memset(&spi_tract, 0, sizeof(spi_tract));

    spi_tract.tx_buffer = payload;
    spi_tract.length = len * 8;
    spi_tract.rxlength = 0;

    // ESP_LOGD(LOG_TAG, "Sending SPI payload, length : %d, is_cmd: %s", len, is_cmd ? "TRUE" : "FALSE");
    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LVGL_IO_DC, is_cmd ? 0 : 1)); // Low for CMD, High for DATA
    ESP_ERROR_CHECK(spi_device_polling_transmit(device_handle, &spi_tract)); // Use blocking transmit for now (easier to debug)
    // ESP_LOGD(LOG_TAG, "SPI payload sent!");
}

static void st7735r_send_init_seq(st7735s_init_t *seq)
{
    if(!seq) {
        ESP_LOGE(LOG_TAG, "Sequence is null!");
        return;
    }

    ESP_LOGD(LOG_TAG, "Writing to register 0x%02X, length: %u", seq->reg, seq->len);
    st7735r_spi_send_bytes(&seq->reg, 1, true);

    if(seq->len > 0) {
        st7735r_spi_send_bytes(seq->data, seq->len, false);
    }
}

void lvgl_st7735r_init()
{
    ESP_LOGI(LOG_TAG, "Performing GPIO init...");
    ESP_ERROR_CHECK(gpio_set_direction(CONFIG_LVGL_IO_DC, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CONFIG_LVGL_IO_RST, GPIO_MODE_OUTPUT));
    ESP_LOGI(LOG_TAG, "GPIO initialization finished, resetting OLED...");

    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LVGL_IO_RST, 0));
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(gpio_set_level(CONFIG_LVGL_IO_RST, 1));
    vTaskDelay(pdMS_TO_TICKS(100));

    spi_bus_config_t bus_config = {
            .mosi_io_num = CONFIG_LVGL_SPI_MOSI,
            .sclk_io_num = CONFIG_LVGL_SPI_SCLK,
            .miso_io_num = -1, // We don't need to get any bullshit from the panel, so no MISO needed.
            .quadhd_io_num = -1,
            .quadwp_io_num = -1,
            .max_transfer_sz = 80 * 160 * 3
    };

    spi_device_interface_config_t device_config = {
#ifndef CONFIG_LVGL_SPI_CLK_DEBUG
            .clock_speed_hz = SPI_MASTER_FREQ_40M,
#else
            .clock_speed_hz = SPI_MASTER_FREQ_8M,
#endif
            .mode = 0, // CPOL = 0, CPHA = 0???
            .spics_io_num = CONFIG_LVGL_SPI_CS,
            .queue_size = 7
    };

    ESP_LOGI(LOG_TAG, "Performing SPI init...");
    ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &bus_config, 1));
    ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &device_config, &device_handle));
    ESP_LOGI(LOG_TAG, "SPI initialization finished, sending init sequence to IPS panel...");

    //
}