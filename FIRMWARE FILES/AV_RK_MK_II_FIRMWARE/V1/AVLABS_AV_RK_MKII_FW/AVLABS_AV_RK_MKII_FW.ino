/*-------------------------------------------------------------------------
AVLABS_AV_RK_MKII.ino, the firmware of the AV-RK MK II keyboard.
    Copyright (C) 2023  AVlabs (Avesta MOLAEI)


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
//PCA9505_9506.h: Lib for the NXP PCA9505 IO Expander through I2C
//Keyboard.h:     Classic Arduino Keyboard lib
#include <PCA9505_9506.h>
#include <Keyboard.h>
//---------------------------------------------


//------------------DEFINES-------------------
#define INITIAL_DELAY 500
#define REPEAT_DELAY 100

//---------------------------------------------


//------------------PCA OBJECTS----------------
PCA9505_06 NUMPAD;
PCA9505_06 LEFT;
PCA9505_06 RIGHT;
//---------------------------------------------


//--------------------VARS---------------------
const uint8_t numKeys = 40;

char keyMapNUMPAD[numKeys] = {
  KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE,        KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK,  KEY_KP_MINUS,   KEY_F13, 
  KEY_INSERT,       KEY_HOME,        KEY_PAGE_UP,      KEY_KP_7,     KEY_KP_8,      KEY_KP_9,        KEY_KP_PLUS,    KEY_F13,
  KEY_DELETE,       KEY_END,         KEY_PAGE_DOWN,    KEY_KP_4,     KEY_KP_5,      KEY_KP_6,        KEY_KP_PLUS,    KEY_F13,
  KEY_F13,          KEY_UP_ARROW,    KEY_F13,          KEY_KP_1,     KEY_KP_2,      KEY_KP_3,        KEY_KP_ENTER,   KEY_F13,
  KEY_LEFT_ARROW,   KEY_DOWN_ARROW,  KEY_RIGHT_ARROW,  KEY_KP_0,     KEY_KP_0,      KEY_KP_ENTER,    KEY_KP_DOT,     KEY_F13
};

char keyMapLEFT[numKeys] = {/*TODO: complete map*/};
char keyMapRIGHT[numKeys] = {/*TODO: complete map*/};

bool keyStateNP[numKeys] = {0};
bool keyStateL[numKeys] = {0};
bool keyStateR[numKeys] = {0};

unsigned long lastPressTime;
uint8_t lastKeyPressed = 255; 
bool initialDelayPassed = false;
//---------------------------------------------


//------------------FUNCTIONS------------------
/**
 * void setup(), init function
*/
void setup() {
  NUMPAD.begin(0x23);
  LEFT.begin(0x22);
  RIGHT.begin(0x21); 

  for (uint8_t i = 0; i < numKeys; i++) {
    NUMPAD.pinMode(i, INPUT);
    LEFT.pinMode(i, INPUT);
    RIGHT.pinMode(i, INPUT);
  }
  
  Keyboard.begin();
}


/**
 * void loop(), infinite loop function
*/
void loop() {
  checkKeys(NUMPAD, keyMapNUMPAD, keyStateNP);
  checkKeys(LEFT, keyMapLEFT, keyStateL);
  checkKeys(RIGHT, keyMapRIGHT, keyStateR);
}


/**
 * @brief Checks the state of each key connected to a specific PCA9505 chip and triggers key press/release actions accordingly.
 * 
 * This function iterates over each key connected to the PCA9505 chip, represented by a PCA9505_06 object. It checks whether the state of the key has changed 
 * (i.e., it has been pressed or released) and triggers the appropriate action. If a key is pressed, the function triggers a "key press" action and sets 
 * up the necessary variables for the key repeat functionality. If a key is released, it triggers a "key release" action. It also handles the repeating 
 * of keys if a key remains pressed for longer than the initial delay.
 *
 * @param expander Reference to the PCA9505_06 object representing the PCA9505 chip to be checked.
 * @param keyMap Pointer to the array containing the keymap for the PCA9505 chip.
 * @param keyState Pointer to the array containing the current state of each key connected to the PCA9505 chip.
 */
void checkKeys(PCA9505_06& expander, char* keyMap, bool* keyState) {
  for (uint8_t i = 0; i < numKeys; i++) {
    bool isPressed = (expander.digitalRead(i) == 0);
    if (isPressed != keyState[i]) {
      keyState[i] = isPressed;
      if (isPressed) {
        Keyboard.press(keyMap[i]);
        lastPressTime = millis();
        lastKeyPressed = i;
        initialDelayPassed = false;
      } else {
        Keyboard.release(keyMap[i]);
        if (i == lastKeyPressed) {
          lastKeyPressed = 255;
        }
      }
    }
  }

  if (lastKeyPressed != 255 && keyState[lastKeyPressed] &&
      ((millis() - lastPressTime > INITIAL_DELAY && !initialDelayPassed) ||
       (initialDelayPassed && millis() - lastPressTime > REPEAT_DELAY))) {
    Keyboard.release(keyMap[lastKeyPressed]);
    Keyboard.press(keyMap[lastKeyPressed]);
    lastPressTime = millis();
    initialDelayPassed = true;
  }
}
//---------------------------------------------


//END
