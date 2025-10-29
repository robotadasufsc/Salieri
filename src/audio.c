/* audio.c: Real-time audio generation API.
 *
 * SPDX-FileCopyrightText: 2025 Robota UFSC
 * SPDX-License-Identifier: MIT
 */

#include "audio.h"

#include <math.h>

#include <esp_log.h>
#include <driver/i2s_std.h>
#include <driver/gpio.h>

#define LOG_TAG "AUDIO"

#define PI_F 3.14159265358979323626433832795028841f
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 441

#define PIN_BCLK GPIO_NUM_3
#define PIN_WSEL GPIO_NUM_4
#define PIN_DOUT GPIO_NUM_2

typedef uint16_t sample_t;

typedef struct {
    float a;
    float theta;
    float omega;
} AudioState;

AudioState astate = {
    .a = 1.0,
    .theta = 0.0,
    .omega = 2.0f * PI_F / ((float) BUFFER_SIZE)
};

i2s_chan_handle_t i2s_handle;

bool setup_audio(void) {
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ((1 << PIN_BCLK) | (1 << PIN_WSEL) | (1 << PIN_DOUT)),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    {
        i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
        esp_err_t result = i2s_new_channel(&chan_cfg, &i2s_handle, NULL);
        if(result != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Could not start I²S channel");
            return false;
        }
    }

    {
        i2s_std_config_t i2s_config = {
            .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
            .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
            .gpio_cfg = {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = PIN_BCLK,
                .ws = PIN_WSEL,
                .dout = PIN_DOUT,
                .din = I2S_GPIO_UNUSED,
                .invert_flags = {
                    .mclk_inv = false,
                    .bclk_inv = false,
                    .ws_inv = false
                }
            }
        };
        esp_err_t result = i2s_channel_init_std_mode(i2s_handle, &i2s_config);
        if(result != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Could not initialise standard mode");
            return false;
        }
    }

    {
        esp_err_t result = i2s_channel_enable(i2s_handle);
        if(result != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Could not enable I²S channel");
            return false;
        }
    }

    return true;
}

void process_audio(void) {
    sample_t samples[BUFFER_SIZE * 2];

    float theta = astate.theta;
    float omega = astate.omega;

    for(size_t sample_idx = 0; sample_idx < BUFFER_SIZE; sample_idx++) {
        float x = theta;
        if(x >= (0.5f * PI_F)) {
            if(x < (1.5f * PI_F)) x = PI_F - x;
            else x -= 2.0f * PI_F;
        }

        float xx = x * x; 
        // max error: 0.000004 (+)
        // float calc = x * (1.0f - xx * (1.0f - xx * (1 - xx * (1.0f - xx / 72.0f) / 42.0f) / 20.0f) / 6.0f);
        // max error: 0.000157 (-)
        float sinx = x * (1.0f - xx * (1.0f - xx * (1 - xx / 42.0f) / 20.0f) / 6.0f);

        samples[sample_idx << 1] = samples[(sample_idx << 1) | 1] = 0x8000 + (int16_t) (0x3FFF * sinx);
        theta = fmodf(theta + omega, 2.0f * PI_F);
    }

    astate.theta = theta;

    esp_err_t result = i2s_channel_write(i2s_handle, samples, BUFFER_SIZE * 2 * sizeof(sample_t), NULL, 100000);
    if(result != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Bad");
    }
}

void set_audio_params(float frequency, float amplitude) {
    astate.a = amplitude;
    astate.omega = (2.0 * PI_F / SAMPLE_RATE) * frequency;
}
