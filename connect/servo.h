#ifndef SERVO_H
#define SERVO_H

#include <driver/ledc.h>  // For LEDC functions
#include <esp_err.h>      // For esp_err_t (optional; or you can just use "int")

// Simple struct to hold servo configuration
typedef struct {
    ledc_timer_config_t timer_conf;
    ledc_channel_t      channel;
    int                 Servo_Gpio_Num;         // Arduino pin or GPIO number
    uint16_t           Servo_Min_Pulse_Width;
    uint16_t           Servo_Max_Pulse_Width;
    uint8_t            Servo_Max_Degree;
    int                Angle;
} Servo_conf_S;

// Initializes the servo’s timer config and channel settings.
void servoInit(void);

// An example function that demos creating tasks to move the servo
void servoTest(void);

// Sets the servo to a given angle by creating a short-lived task.
void setServoAngle(int angle);

// Internal function that configures the LEDC timer
void pwm_timer_init(ledc_timer_config_t *timer_conf,
                    ledc_mode_t speed_mode,
                    ledc_timer_bit_t duty_resolution,
                    ledc_timer_t timer_num,
                    uint32_t freq_hz,
                    ledc_clk_cfg_t clk_conf);

// Internal function that configures the LEDC channel
void pwm_channel_init(ledc_channel_config_t *channel_conf,
                      int gpio_num,
                      ledc_channel_t channel,
                      ledc_intr_type_t intr_type,
                      ledc_timer_t timer_sel,
                      uint32_t duty,
                      int hpoint);

// FreeRTOS task that actually sets the servo by writing the duty cycle
void Servo_Task(void *pvParameters);

#endif // SERVO_H
