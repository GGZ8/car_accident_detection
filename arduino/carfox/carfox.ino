/* Carfox: a device to find them all
   by: Luis Roda Sánchez
   date: October 25, 2017
   Based on Kris Winer sketch for managing MPU-9250 and modified by Brent Wilkins.
   Sketch runs on the MKR FOX 1200.

  Hardware setup:
  MPU9250 Breakout --------- Arduino MKRFOX1200
  VDD ---------------------- VCC (3.3V)
  SDA ----------------------- 11
  SCL ----------------------- 12
  GND ---------------------- GND
*/

#include <SigFox.h>
#include "MPU9250.h"

boolean SerialDebug;  // Set to true to get Serial output for debugging.

// Pin definitions:
int buzzer  = 5;  // Set up pin 5 for buzzer.
int pressure = 0;
int pressurePin = A1;  // Set up analog pin 1 for pressure sensor.
int absAx = 900;
int absAy = 900;
int absAz = 900;
int freeFall = 900;
float temperature = 25;
boolean firstTime = true;

// Limits (adjusted for testing,
// they should be modified for real case):
int accelerationLimit = 3000;
int pressureLimit = 200;  // Around 25 kg.
int freeFallLimit = 300;
uint8_t highThreshold = 29;
uint8_t extremeThreshold = 32;
boolean holdHighTemperature = false;
boolean holdExtremeTemperature = false;

// Sigfox message structure:
typedef struct __attribute__ ((packed)) sigfox_message {
  uint8_t temperatureSigfox;
  uint8_t highTemp = 0;
  uint8_t extremeTemp = 0;
  uint8_t accident = 0;
  uint8_t criticalAccident = 0;
} SigfoxMessage;

// stub for message which will be sent
SigfoxMessage msg;

MPU9250 IMU;

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  if (!SigFox.begin()) {
    // Something is really wrong, try rebooting
    // Reboot is useful if we are powering the board using an unreliable
    // power source (eg. solar panels or other energy harvesting methods).
    reboot();
  }

  // Send module to standby until we need to send a message.
  SigFox.end();
  SigFox.debug();

  SerialDebug = true; // Set to true to get Serial output for debugging.
  pinMode(buzzer, OUTPUT);

  // Read the WHO_AM_I register, this is a good test of communication.
  byte c = IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  if (SerialDebug) {
    Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
    Serial.print(" I should be "); Serial.println(0x71, HEX);
  }

  if (c == 0x71) // WHO_AM_I should always be 0x68.
  {
    if (SerialDebug) {
      Serial.println("MPU9250 is online...");
    }
    IMU.initMPU9250();
    // Initialize device for active mode read of accelerometer, gyroscope
    // and temperature.
    if (SerialDebug) {
      Serial.println("MPU9250 initialized for active data mode....");
    }
  } // if (c == 0x71)
}

void loop()
{
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {
    IMU.readAccelData(IMU.accelCount);  // Read the x/y/z accelerometer values.
    IMU.getAres();

    // Now we'll calculate the acceleration value into actual g's
    // This depends on scale being set, in this case the maximum range (+/- 16 g) (modified in library).
    IMU.ax = (float)IMU.accelCount[0] * IMU.aRes * 1000;
    IMU.ay = (float)IMU.accelCount[1] * IMU.aRes * 1000;
    IMU.az = (float)IMU.accelCount[2] * IMU.aRes * 1000;

    IMU.readGyroData(IMU.gyroCount);  // Read the x/y/z gyroscope values.
    IMU.getGres();

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set, in this case +/- 2000 DPS (modified in library).
    IMU.gx = (float)IMU.gyroCount[0] * IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1] * IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2] * IMU.gRes;

    pressure = analogRead(pressurePin);
    float pressureKgs = (float)pressure / 357 * 45.3592;

    IMU.tempCount = IMU.readTempData();  // Read the adc values.
    IMU.temperature = ((float) IMU.tempCount) / 333.87 + 21.0;

    if (SerialDebug) {
      // Pressure in kilograms.
      Serial.print("Pressure is ");
      Serial.print(pressureKgs);
      Serial.print(" kg");
      Serial.println();

      // Temperature in degrees Centigrade.
      Serial.print("Temperature is ");  Serial.print(IMU.temperature, 1);
      Serial.println(" degrees C");

      // Accelerations in mg.
      Serial.println("Accelerations:");
      Serial.print("X: "); Serial.print(IMU.ax);
      Serial.print(" mg ");
      Serial.print("Y: "); Serial.print(IMU.ay);
      Serial.print(" mg ");
      Serial.print("Z: "); Serial.print(IMU.az);
      Serial.println(" mg ");
      Serial.println();

      // Gyro values in degrees/sec.
      Serial.println("Gyro rates:");
      Serial.print("X: "); Serial.print(IMU.gx, 3);
      Serial.print(" degrees/sec ");
      Serial.print("Y: "); Serial.print(IMU.gy, 3);
      Serial.print(" degrees/sec ");
      Serial.print("Z: "); Serial.print(IMU.gz, 3);
      Serial.println(" degrees/sec");
      Serial.println();

      // Set alarms to false.
      msg.highTemp = 0;
      msg.extremeTemp = 0;
      msg.accident = 0;
      msg.criticalAccident = 0;
    } // if (SerialDebug)
  } // if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  absAx = abs(IMU.ax);  // Absolute values.
  absAy = abs(IMU.ay);
  absAz = abs(IMU.az);

  msg.temperatureSigfox = (uint8_t)IMU.temperature; // Copy temperature to sigfox message.

  if (firstTime) { // Sending to refresh data at the beginning.
    firstTime = false;
    // Sending message via Sigfox.
    send2Sigfox();
  }

  if (msg.temperatureSigfox > highThreshold) {  // High Temperature.
    if (!holdHighTemperature) {  // To avoid sending repeated messages.
      holdExtremeTemperature = false;
      msg.highTemp = 1;
      holdHighTemperature = true;
      // Sending message via Sigfox.
      send2Sigfox();
    }
    else {
      if (msg.temperatureSigfox > extremeThreshold) {  // Extreme Temperature.
        if (!holdExtremeTemperature) {
          msg.extremeTemp = 1;
          holdExtremeTemperature = true;
          // Sending message via Sigfox.
          send2Sigfox();
          if (!SerialDebug) {
            do {
              delay(720000);  // Resend every 12 minutes.
              send2Sigfox();
            } while (1);
          }
        }
        else {
        }
      }
      else {
        if (msg.temperatureSigfox < (extremeThreshold - 1)) {  // Hysteresis to avoid errors and too many sendings.
          holdExtremeTemperature = false;
        }
      }
    }
  }
  else {
    if (msg.temperatureSigfox < (highThreshold - 1) && (holdHighTemperature == true || holdExtremeTemperature == true)) {  // Hysteresis to avoid errors and too many sendings.
      holdHighTemperature = false;
      holdExtremeTemperature = false;
      msg.highTemp = 0;
      msg.extremeTemp = 0;
      // Sending message via Sigfox to refresh data.
      send2Sigfox();
    }
  }

  if (absAx > accelerationLimit || absAy > accelerationLimit || absAz > accelerationLimit) {
    alarm(1);
  }
  if (IMU.az < 0) {
    alarm(4);
  }
  if (pressure > pressureLimit) {
    alarm(2);
  }
  freeFall = absAx + absAy + absAz;
  if (freeFall < freeFallLimit) {
    alarm(3);
  }
}

void alarm(unsigned char option) {  // Alarm goes off.
  int index = 9;
  switch (option) {
    case 1:
      if (SerialDebug) {
        Serial.println("ACCEL ALARM");
      }
      msg.accident = 1;
      send2Sigfox();
      do {
        analogWrite(buzzer, 50);
        delay(100);
        analogWrite(buzzer, 0);
        delay(200);
        if (SerialDebug) {
          index--;
        }
      } while (index > 7);
      break;

    case 2:
      if (SerialDebug) {
        Serial.println("PRESSURE ALARM");
      }
      msg.accident = 1;
      send2Sigfox();
      do {
        analogWrite(buzzer, 50); // Pin 5 to high.
        delay(300);
        analogWrite(buzzer, 0); // Pin 5 to low.
        delay(200);
        if (SerialDebug) {  // Infinite loop if a real case.
          index--;
        }
      } while (index > 6);
      break;

    case 3:
      if (SerialDebug) {
        Serial.println("FREEFALL ALARM");
      }
      msg.criticalAccident = 1;
      send2Sigfox();
      do {
        analogWrite(buzzer, 90);
        delay(100);
        analogWrite(buzzer, 0);
        delay(200);
        if (SerialDebug) {
          index--;
        }
      } while (index > 5);
      break;

    case 4:
      if (SerialDebug) {
        Serial.println("ROLL-OVER ALARM");
      }
      msg.criticalAccident = 1;
      send2Sigfox();
      do {
        analogWrite(buzzer, 50);
        delay(100);
        analogWrite(buzzer, 0);
        delay(200);
        if (SerialDebug) {
          index--;
        }
      } while (index > 2);
      break;
  }
}

void send2Sigfox() {
  // Start the module
  SigFox.begin();
  // Wait at least 30ms after first configuration (100ms before)
  delay(100);
  // Clear all pending interrupts
  SigFox.status();
  delay(1);
  SigFox.beginPacket();
  SigFox.write((uint8_t*)&msg, 5);
  SigFox.endPacket();
}

void reboot() {
  NVIC_SystemReset();
  while (1);
}