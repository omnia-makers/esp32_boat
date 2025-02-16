#ifndef POWER_H
#define POWER_H

#include <Arduino.h>  // Ensure Arduino functions are available

// Motor control pins
const int motorPin1 = 27;  // H-Bridge IN1
const int motorPin2 = 26;  // H-Bridge IN2

// Función map
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Initializes the motor pins
void initMotorPower(void) {
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);

    Serial.begin(115200); // Start serial communication
    Serial.println("Motor initialized.");
}

// Disables outputs to the motor (stops the motor)
void setOffMotorPower(void) {
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 0);
    Serial.println("Motor stopped.");
}

// Sets the motor to a given duty value (-255 to 255).
void setDutyMotorPower(int duty) {
    if (duty < 0) {
        int speed = map(-duty, 0, 254, 30, 235);
        analogWrite(motorPin1, speed);  // Forward speed (0-255)
        analogWrite(motorPin2, 0);
        Serial.print("Motor moving BACKWARD with duty cycle: ");
        Serial.println(speed);
    } 
    else if (duty > 0) {
        int speed = map(duty, 0, 254, 30, 235);
        analogWrite(motorPin2, speed);  // Reverse speed (0-255)
        analogWrite(motorPin1, 0);
        
        Serial.print("Motor moving FORWARD with duty cycle: ");
        Serial.println(speed);
    } 
    else {
      setOffMotorPower();
    }

}

#endif
