#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TRIGGER_PIN GPIO_NUM_25
#define ECHO_PIN GPIO_NUM_35

#define ADC_CHANNEL ADC1_CHANNEL_7

#define MAX_DISTANCE 500 // Max distance in cm
#define SOUND_SPEED 0.034

void ultrasonic_sensor_init(void)
{
    gpio_pad_select_gpio(TRIGGER_PIN);
    gpio_set_direction(TRIGGER_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(ECHO_PIN);
    gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT);
}

float ultrasonic_read_distance_cm(void)
{
    uint32_t start_time = 0, end_time = 0, duration = 0;
    float distance;

    // Trigger the sensor
    gpio_set_level(TRIGGER_PIN, 1);
    ets_delay_us(10);
    gpio_set_level(TRIGGER_PIN, 0);

    // Wait for the echo signal and measure the duration
    while (gpio_get_level(ECHO_PIN) == 0)
    {
        start_time = esp_timer_get_time();
    };

    while (gpio_get_level(ECHO_PIN) == 1)
    {
        duration = esp_timer_get_time() - start_time;
        if (duration > 50000)
        { // timeout after 50ms
            return -1;
        }
    };
    end_time = esp_timer_get_time();

    // Calculate the distance in meters
    duration = end_time - start_time;
    distance = duration * SOUND_SPEED / 2;

    return distance;
}

void app_main(void)
{
    // setup
    ultrasonic_sensor_init();
    // loop
    while (1)
    {
        float distance = ultrasonic_read_distance_cm();
        if(distance >= 0){
            ESP_LOGI("ultrasonic", "Distance: %.2fcm\n", distance * 10);
        } else {
            ESP_LOGI("ultrasonic", "Meassurement error");
        }
        // printf("Distance: %.2fcm\n", distance * 10);
        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 500ms before reading again
    }
}
