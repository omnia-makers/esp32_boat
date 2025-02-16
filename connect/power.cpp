#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/ledc.h>
#include "power.h"

// Simple log macro that mimics ESP_LOGI(TAG, ...)
#define ESP_LOGI(tag, format, ...)  Serial.printf("[%s] " format "\n", tag, ##__VA_ARGS__)

// You can define ESP_OK for consistency, or just use int return types
#define ESP_OK 0

static const char *TAG = "MotorPower";

// GPIO setup (some pins might need to be changed for your board)
#define LEDC_OUTPUT_0  18   // Which pin for channel 0
#define LEDC_OUTPUT_1  19  // Which pin for channel 1

// LEDC config
#define LEDC_MODE        LEDC_LOW_SPEED_MODE
#define LEDC_TIMER       LEDC_TIMER_0
#define LEDC_CHANNEL_0   LEDC_CHANNEL_0
#define LEDC_CHANNEL_1   LEDC_CHANNEL_1
#define LEDC_DUTY_RES    LEDC_TIMER_8_BIT
#define LEDC_FREQUENCY   5000
#define LEDC_DUTY        0
#define LEDC_HPOINT      0

// Task stack
#define STACK_SIZE 2048

// A helper function that mimics Arduino's map()
static int mapValue(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Sets up both LEDC channels and the timer, so we can output PWM on channel 0 & 1
static int power_set_pwm(void)
{
  // Prepare channel config for channel 0
  ledc_channel_config_t ledc_channel_0 = {
    .gpio_num   = LEDC_OUTPUT_0,
    .speed_mode = LEDC_MODE,
    .channel    = LEDC_CHANNEL_0,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER,
    .duty       = LEDC_DUTY,
    .hpoint     = LEDC_HPOINT
  };

  // Prepare channel config for channel 1
  ledc_channel_config_t ledc_channel_1 = {
    .gpio_num   = LEDC_OUTPUT_1,
    .speed_mode = LEDC_MODE,
    .channel    = LEDC_CHANNEL_1,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER,
    .duty       = LEDC_DUTY,
    .hpoint     = LEDC_HPOINT
  };

  // Configure channels
  ledc_channel_config(&ledc_channel_0);
  ledc_channel_config(&ledc_channel_1);

  // Prepare timer config
  ledc_timer_config_t ledc_timer = {
    .speed_mode      = LEDC_MODE,
    .duty_resolution = LEDC_DUTY_RES,
    .timer_num       = LEDC_TIMER,
    .freq_hz         = LEDC_FREQUENCY,
    .clk_cfg         = LEDC_AUTO_CLK
  };

  // Configure timer
  ledc_timer_config(&ledc_timer);

  return ESP_OK;
}

int power_set_pwm_duty(int duty)
{
  // In your original code, this function started a task that sets duty to 4000
  // in a loop. If you want that same behavior, we’ll create the task here:
  static uint8_t ucParameterToPass;
  TaskHandle_t xHandle = NULL;

  xTaskCreate(
    vTaskSetPwmMotor,
    "vTaskCode",
    STACK_SIZE,
    &ucParameterToPass,
    1,            // Priority
    &xHandle
  );

  // You could also directly set the duty here if you prefer.
  return ESP_OK;
}

// Example task that repeatedly sets duty cycle to 4000 on channel 0
void vTaskSetPwmMotor(void *pvParameters)
{
  ESP_LOGI("PWM", "Task Set PWM Motor - starting");
  while (1) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, 4000);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setOffMotorPower(void)
{
  ESP_LOGI(TAG, "Set Off Motor Power");
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);

  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);
}

void setDutyMotorPower(int duty)
{
  // Example usage: map negative “duty” to one channel, positive to the other
  ESP_LOGI(TAG, "Set Duty Motor Power %d", duty);

  if (duty < 0) {
    int speed = mapValue(-duty, 0, 254, 30, 235);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, speed);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);

    ESP_LOGI(TAG, "Set MAP Motor Power Inverso %d", speed);
  } else {
    int speed = mapValue(duty, 0, 254, 30, 235);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, speed);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);

    ESP_LOGI(TAG, "Set MAP Motor Power %d", speed);
  }
}

void initMotorPower(void)
{
  // Initialize the PWM channels/timers
  ESP_LOGI(TAG, "Init Motor Power");
  power_set_pwm();
}
