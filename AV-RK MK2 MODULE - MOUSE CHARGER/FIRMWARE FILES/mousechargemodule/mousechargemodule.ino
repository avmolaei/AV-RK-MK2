/*-------------------------------------------------------------------------
AVLABS_AV_RK_MKII_ArmModule.ino, I2C slave firmware for the charging arm.
    Copyright (C) 2025  AVlabs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/


//------------------LIBRARIES------------------
//Wire:   I2C slave communications
//Servo:  Standard hobby servo control
//EEPROM: Persist last known arm position (angle)
#include <Wire.h>
#include <Servo.h>
#include <EEPROM.h>
//--------------------------------------------------------------------------------------------------------


//------------------DEFINES------------------------------------------------------
//I2C slave address (must match master NANO_I2C_ADDRESS)
#define ARM_I2C_ADDRESS         8

//Pins
#define SERVO_PIN               9       // PWM-capable pin for servo signal
#define STATUS_LED_PIN          LED_BUILTIN

//Servo and motion config
#define ANGLE_MIN               3       // degrees
#define ANGLE_MAX               250     // degrees

//Set true for eased movement; false for immediate set
#define USE_SMOOTH_MOTION       false
#define SMOOTH_STEP_DEG         2       // degrees per step when smoothing
#define SMOOTH_STEP_DELAY_MS    15      // ms between steps

//Auto-detach to avoid buzzing/heat when idle
#define DETACH_AFTER_IDLE_MS    600

//EEPROM address for persisted angle (1 byte)
#define EEPROM_ADDR_ANGLE       0
//--------------------------------------------------------------------------------------------------------


//--------------------VARS---------------------
//Servo control
Servo servoArm;
bool  servoAttached           = false;

//Angles
uint8_t currentAngle          = 0;      // physical angle last written to servo
uint8_t targetAngle           = 0;      // requested angle from master

//Timers
unsigned long lastMoveTime    = 0;      // last time we moved or received a command
unsigned long lastStepTime    = 0;      // last time we performed a smooth step
//---------------------------------------------


//------------------PROTOTYPES------------------
void onI2CReceive(int numBytes);
void attachIfNeeded();
void detachIfIdle();
uint8_t clampAngle(int v);
void setAngleImmediate(uint8_t deg);
void setAngleSmooth(uint8_t deg);
void saveAngleToEEPROM(uint8_t deg);
uint8_t loadAngleFromEEPROM();
//---------------------------------------------


//------------------FUNCTIONS------------------
/**
 * @brief setup(), init function
*/
void setup() {
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  //Restore last known angle from EEPROM and apply once
  currentAngle = loadAngleFromEEPROM();
  targetAngle  = currentAngle;

  attachIfNeeded();
  servoArm.write(currentAngle);
  lastMoveTime = millis();

  //I2C slave init
  Wire.begin(ARM_I2C_ADDRESS);
  Wire.onReceive(onI2CReceive);

  //Blink to signal ready
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(60);
  digitalWrite(STATUS_LED_PIN, LOW);
}


/**
 * @brief loop(), infinite loop function
*/
void loop() {
  //Handle smooth motion if enabled
  if (USE_SMOOTH_MOTION && currentAngle != targetAngle) {
    unsigned long now = millis();
    if (now - lastStepTime >= SMOOTH_STEP_DELAY_MS) {
      lastStepTime = now;

      int diff = (int)targetAngle - (int)currentAngle;
      int step = (diff > 0) ? SMOOTH_STEP_DEG : -SMOOTH_STEP_DEG;

      if (abs(diff) <= SMOOTH_STEP_DEG) {
        currentAngle = targetAngle;     // snap to target when close
      } else {
        currentAngle = clampAngle((int)currentAngle + step);
      }

      attachIfNeeded();
      servoArm.write(currentAngle);
      lastMoveTime = now;

      if (currentAngle == targetAngle) {
        saveAngleToEEPROM(currentAngle);
      }
    }
  }

  //Detach after inactivity to reduce noise and power
  detachIfIdle();
}


/**
 * @brief I2C receive callback, reads last byte as target angle
 * 
 * The keyboard master sends a single byte command (0..180).
 * We clamp to [ANGLE_MIN..ANGLE_MAX], ignore duplicates (idempotent),
 * and move the servo immediately (or smoothly if enabled).
*/
void onI2CReceive(int numBytes) {
  if (numBytes <= 0) return;

  int lastVal = -1;
  while (Wire.available()) {
    lastVal = Wire.read(); // consume all, keep last
  }
  if (lastVal < 0) return;

  uint8_t requested = clampAngle(lastVal);

  //Update activity time so auto-detach doesn't trigger immediately
  lastMoveTime = millis();

  //Idempotent: if same as current target, no redundant motion
  if (requested == targetAngle) return;

  targetAngle = requested;

  //Status flash
  digitalWrite(STATUS_LED_PIN, HIGH);

  if (USE_SMOOTH_MOTION) {
    setAngleSmooth(targetAngle);  // loop() takes care of steps
  } else {
    setAngleImmediate(targetAngle);
  }

  digitalWrite(STATUS_LED_PIN, LOW);
}


/**
 * @brief Attach servo if not already attached
*/
void attachIfNeeded() {
  if (!servoAttached) {
    servoArm.attach(SERVO_PIN, 500, 2600);
    servoAttached = true;
  }
}



/**
 * @brief Detach servo after an idle timeout to avoid buzzing/heat
*/
void detachIfIdle() {
  if (!servoAttached) return;
  unsigned long now = millis();
  if (now - lastMoveTime >= DETACH_AFTER_IDLE_MS) {
    servoArm.detach();
    servoAttached = false;
  }
}


/**
 * @brief Clamp helper for angles
*/
uint8_t clampAngle(int v) {
  if (v < ANGLE_MIN) return ANGLE_MIN;
  if (v > ANGLE_MAX) return ANGLE_MAX;
  return (uint8_t)v;
}


/**
 * @brief Set servo angle immediately and persist
*/
void setAngleImmediate(uint8_t deg) {
  attachIfNeeded();
  servoArm.write(deg);
  currentAngle = deg;
  lastMoveTime = millis();
  saveAngleToEEPROM(currentAngle);
}


/**
 * @brief Prepare a smooth movement to a target angle
 * 
 * loop() will handle stepping at SMOOTH_STEP_DELAY_MS intervals.
*/
void setAngleSmooth(uint8_t deg) {
  targetAngle  = clampAngle(deg);
  attachIfNeeded();
  lastStepTime = millis();
  lastMoveTime = millis();
}


/**
 * @brief Save current angle to EEPROM (wear-safe)
*/
void saveAngleToEEPROM(uint8_t deg) {
  if (EEPROM.read(EEPROM_ADDR_ANGLE) != deg) {
    EEPROM.update(EEPROM_ADDR_ANGLE, deg);
  }
}


/**
 * @brief Load last angle from EEPROM (validated)
*/
uint8_t loadAngleFromEEPROM() {
  uint8_t v = EEPROM.read(EEPROM_ADDR_ANGLE);
  return clampAngle(v);
}
//END
