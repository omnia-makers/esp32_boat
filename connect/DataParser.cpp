#include <Arduino.h>       // Needed for Serial
#include "DataParser.h"    // Include its own header
#include "power.h"
#include "servo.h"

void parseDataComunication(const char* data) {
  Serial.print("Parsing data: ");
  Serial.println(data);

  int x, y;
  if (strcmp(data, "on") == 0)
  {
      initMotorPower();
  }

  if (strcmp(data, "off") == 0)
  {
      setOffMotorPower();
  }

  if (strcmp(data, "testservo") == 0)
  {
      setServoAngle(100);
      setServoAngle(50);
      setServoAngle(0);
      setServoAngle(130);
      setServoAngle(20);
      setServoAngle(0);
  }

  if (strcmp(data, "testpower") == 0)
  {

      setDutyMotorPower(255);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(180);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(200);
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      setDutyMotorPower(100);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(180);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(240);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(10);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(180);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(150);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(254);
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      setDutyMotorPower(220);
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      setDutyMotorPower(200);
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      setDutyMotorPower(150);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(254);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(180);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(150);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      setDutyMotorPower(10);
      vTaskDelay(4000 / portTICK_PERIOD_MS);

      setOffMotorPower();
  }

  if (sscanf(data, "x %i y %i", &x, &y) == 2)
  {
      // printf("VARS: x: %i, y: %i\n", x, y);

      // enviamos el valor de X al motor
      // int int_value = atoi(x);
      setDutyMotorPower(y);
      setServoAngle(x);
  }

  if (sscanf(data, "y %i", &y) == 1)
  {
      // printf("VARS: y: %i\n", y);

      // enviamos el valor de X al motor
      // int int_value = atoi(x);
      setDutyMotorPower(y);
  }

  if (sscanf(data, "x %i", &x) == 1)
  {
      setServoAngle(x);
  }
}
