#ifndef POWER_H
#define POWER_H

// Initializes the motor (sets up LEDC channels and timers).
void initMotorPower(void);

// Disables outputs to the motor.
void setOffMotorPower(void);

// Sets the motor to a given duty value.
void setDutyMotorPower(int duty);

// Internal duty cycle setter task (optional if you want that background task).
void vTaskSetPwmMotor(void *pvParameters);

#endif
