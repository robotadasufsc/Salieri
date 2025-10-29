/* main.c: Serieli: an Electronic Theremin.
 *
 * SPDX-FileCopyrightText: 2025 Robota UFSC
 * SPDX-License-Identifier: MIT
 */

#include <math.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <esp_timer.h>
#include <driver/gpio.h>

#include "audio.h"

#define LOG_TAG "APP"

void lock_and_report(void) {
    for(;;) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    if(!setup_audio()) {
        ESP_LOGE(LOG_TAG, "Could not start audio, halting");

        lock_and_report();
    }

    ESP_LOGI(LOG_TAG, "All systems OK!");

    for(;;) {
        for(size_t i = 0; i < 314; i++) {
            float frequency = 500.0f + 100.0f * sin(0.02 * (float) i);
            float amplitude = 0.5 * 0.2 * cos(0.007 * (float) i);

            set_audio_params(frequency, amplitude);
            process_audio();
        }
    }
}
