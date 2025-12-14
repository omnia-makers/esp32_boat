#include <Arduino.h>
#include "servo.h"

// Logging macro that mimics ESP_LOGI(TAG, ...)
#define ESP_LOGI(tag, format, ...) Serial.printf("[%s] " format "\n", tag, ##__VA_ARGS__)

// If you want to mimic ESP_OK for “success”
#ifndef ESP_OK
  #define ESP_OK 0
#endif

#define SERVO_GPIO 2
#define SERVO_MIN_PULSEWIDTH  500  // Minimum pulse width in microseconds
#define SERVO_MAX_PULSEWIDTH 2500  // Maximum pulse width in microseconds
#define SERVO_MAX_DEGREE     255

static const char *Motor_TAG = "Motor_Control";

// Global timer/channel objects for demonstration
ledc_timer_config_t Timer_1;         // LEDC timer config
ledc_channel_config_t ledc_channel;  // LEDC channel config

// Map function to replace the custom “mapservo()”
static uint16_t mapservo(int value, int in_min, int in_max, int out_min, int out_max)
{
  return (uint16_t)(((value - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min);
}

void pwm_timer_init(ledc_timer_config_t *timer_conf,
                    ledc_mode_t speed_mode,
                    ledc_timer_bit_t duty_resolution,
                    ledc_timer_t timer_num,
                    uint32_t freq_hz,
                    ledc_clk_cfg_t clk_conf)
{
  timer_conf->speed_mode      = speed_mode;
  timer_conf->duty_resolution = duty_resolution;
  timer_conf->timer_num       = timer_num;
  timer_conf->freq_hz         = freq_hz;
  timer_conf->clk_cfg         = clk_conf;

  esp_err_t err = ledc_timer_config(timer_conf);
  if (err != ESP_OK) {
    ESP_LOGI(Motor_TAG, "Timer Config Error...");
  }
}

void pwm_channel_init(ledc_channel_config_t *channel_conf,
                      int gpio_num,
                      ledc_channel_t channel,
                      ledc_intr_type_t intr_type,
                      ledc_timer_t timer_sel,
                      uint32_t duty,
                      int hpoint)
{
  channel_conf->gpio_num   = gpio_num;
  channel_conf->channel    = channel;
  channel_conf->intr_type  = intr_type;
  channel_conf->timer_sel  = timer_sel;
  channel_conf->duty       = duty;
  channel_conf->hpoint     = hpoint;
  channel_conf->speed_mode = LEDC_LOW_SPEED_MODE;

  esp_err_t err = ledc_channel_config(channel_conf);
  if (err != ESP_OK) {
    ESP_LOGI(Motor_TAG, "Channel Config Error...");
  }
}

// This initializes our Servo_conf_S struct with relevant data
void Servo_init(Servo_conf_S *Servo_conf,
                ledc_timer_config_t *timer_conf,
                ledc_channel_t channel_conf,
                int Servo_Gpio_Num,
                uint16_t Servo_Min_Pulse_Width,
                uint16_t Servo_Max_Pulse_Width,
                uint8_t Servo_Max_Degree)
{
  Servo_conf->timer_conf           = *timer_conf;
  Servo_conf->channel              = channel_conf;
  Servo_conf->Servo_Gpio_Num       = Servo_Gpio_Num;
  Servo_conf->Servo_Min_Pulse_Width= Servo_Min_Pulse_Width;
  Servo_conf->Servo_Max_Pulse_Width= Servo_Max_Pulse_Width;
  Servo_conf->Servo_Max_Degree     = Servo_Max_Degree;
}

void Servo_Task(void *pvParameters)
{
  // We cast the task’s argument back to a Servo_conf_S pointer
  Servo_conf_S *Servo = (Servo_conf_S *) pvParameters;

  ESP_LOGI("Servo_Task", "Servo->Angle %d ", Servo->Angle);

  // Example mapping of angle to duty. Here the code is set up
  // to take angles from -254..254 → some duty range from 58..93, etc.
  // You can adjust these values to your servo or usage scenario.
  uint16_t pulse_width = mapservo(Servo->Angle, -254, 254, 26, 128);

  // For a 10-bit timer (0..1023), we might do:
  // duty = (pulse_width * 1024) / 2000, etc.
  // But below, the code actually sets the “pulse_width” directly as duty
  // on LEDC_CHANNEL_2. Adjust as needed for your servo timing.
  uint16_t duty = (pulse_width * 1024) / 2000;

  ESP_LOGI("Servo_Task", "pulse_width %u ", pulse_width);
  ESP_LOGI("Servo_Task", "duty %u ", duty);

  // Now set the duty on the channel. LEDC_CHANNEL_2 was configured in pwm_channel_init.
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, pulse_width);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);

  // Delay for demonstration. Then delete the task so we don’t block forever.
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  vTaskDelete(NULL);
}

// Example function that creates multiple tasks with different angles
void servoTest(void)
{
  // In your original code, you sometimes used servoInit or pwm_timer_init, etc.
  // Make sure servoInit() is called before servoTest() if you haven’t already done so.

  // Here is an example of creating tasks with different angles:
  Servo_conf_S Servo1;
  Servo1.Angle = 180;  // Not actually used below except for first reference
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Servo1.Angle = 25;
  xTaskCreate(Servo_Task, "Servo task", 2048, &Servo1, 1, NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Servo1.Angle = 124;
  xTaskCreate(Servo_Task, "Servo task", 2048, &Servo1, 1, NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Servo1.Angle = 22;
  xTaskCreate(Servo_Task, "Servo task", 2048, &Servo1, 1, NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Servo1.Angle = 121;
  xTaskCreate(Servo_Task, "Servo task", 2048, &Servo1, 1, NULL);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Servo1.Angle = 72;
  xTaskCreate(Servo_Task, "Servo task", 2048, &Servo1, 1, NULL);
}

// A basic init function to configure the timer, channel, and store them in Servo_conf_S
void servoInit(void)
{
  // 1) Configure LEDC timer (50 Hz for many servos, 10-bit resolution, etc.)
  pwm_timer_init(
    &Timer_1,
    LEDC_LOW_SPEED_MODE,
    LEDC_TIMER_10_BIT,
    LEDC_TIMER_1,
    50,
    LEDC_AUTO_CLK
  );

  // 2) Configure LEDC channel. We’re using LEDC_CHANNEL_2 and SERVO_GPIO pin
  pwm_channel_init(
    &ledc_channel,
    SERVO_GPIO,
    LEDC_CHANNEL_2,
    LEDC_INTR_DISABLE,
    LEDC_TIMER_1,
    0,
    0
  );

  // 3) Optionally create a Servo_conf_S struct to store config.
  Servo_conf_S Servo1;
  Servo1.Angle = 0;  // default angle
  Servo_init(
    &Servo1,
    &Timer_1,
    LEDC_CHANNEL_2,
    SERVO_GPIO,
    SERVO_MIN_PULSEWIDTH,
    SERVO_MAX_PULSEWIDTH,
    SERVO_MAX_DEGREE
  );
}

// Creates a temporary Servo_conf_S and spawns a task that sets the servo
void setServoAngle(int angle)
{
  ESP_LOGI("Servo", "setServoAngle %d ", angle);
  Servo_conf_S *Servo1 = new Servo_conf_S; // dynamic allocate so it persists
  Servo1->Angle = angle;

  xTaskCreate(Servo_Task, "Servo task", 2048, Servo1, 1, NULL);

  // Allow the task’s code to run for at least some ticks before returning.
  vTaskDelay(50 / portTICK_PERIOD_MS);
}
