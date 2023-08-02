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


//------------------DEFINES--------------------
#define NUM_KEYS_NUMPAD 40
// #define NUM_KEYS_LEFT 40
// #define NUM_KEYS_RIGHT 40  //Maybe i should just have a general NUM_KEYS as all of them are 40 lmao
//---------------------------------------------


//------------------PCA OBJECTS----------------
PCA9505_06 NUMPAD;
PCA9505_06 LEFT;
PCA9505_06 RIGHT;
//---------------------------------------------


//------------------TYPEDEFS-------------------
typedef struct{
  PCA9505_06* expander;
  uint8_t pin;
  uint8_t key;
} Key; //KEY{&EXPANDER, PIN, CODE}
//---------------------------------------------


//------------------KEYMAPS--------------------
Key NUMPADMAP[] = {
  {&NUMPAD, 0,  0xCE},
  {&NUMPAD, 1,  0xCF},
  {&NUMPAD, 2,  0xD0},
  {&NUMPAD, 3,  0xDB},
  {&NUMPAD, 4,  0xDC},
  {&NUMPAD, 5,  0xDD},
  {&NUMPAD, 6,  0xDE},
  {&NUMPAD, 7,  0xDA},

  {&NUMPAD, 8,  0xD1},
  {&NUMPAD, 9,  0xD2},
  {&NUMPAD, 10, 0xD3},
  {&NUMPAD, 11, 0xE7},
  {&NUMPAD, 12, 0xE8},
  {&NUMPAD, 13, 0xE9},
  {&NUMPAD, 14, 0xDF},
  {&NUMPAD, 15, 0xDA},

  {&NUMPAD, 16, 0xD4},
  {&NUMPAD, 17, 0xD5},
  {&NUMPAD, 18, 0xD6},
  {&NUMPAD, 19, 0xE4},
  {&NUMPAD, 20, 0xE5},
  {&NUMPAD, 21, 0xE6},
  {&NUMPAD, 22, 0xDF},
  {&NUMPAD, 23, 0xDA},

  {&NUMPAD, 24, 0xDA},
  {&NUMPAD, 25, 0xDA},
  {&NUMPAD, 26, 0xDA},
  {&NUMPAD, 27, 0xE1},
  {&NUMPAD, 28, 0xE2},
  {&NUMPAD, 29, 0xE3},
  {&NUMPAD, 30, 0xE0},
  {&NUMPAD, 31, 0xDA},

  {&NUMPAD, 32, 0xD8},
  {&NUMPAD, 33, 0xD9},
  {&NUMPAD, 34, 0xD7},
  {&NUMPAD, 35, 0xEA},
  {&NUMPAD, 36, 0xEA},
  {&NUMPAD, 37, 0xEB},
  {&NUMPAD, 38, 0xE0},
  {&NUMPAD, 39, 0xDA}
}; //NUMPAD keymap
/*
Key LEFTMAP[] = {
  {&LEFT, 0,  0xCE},
  {&LEFT, 1,  0xCF},
  {&LEFT, 2,  0xD0},
  {&LEFT, 3,  0xDB},
  {&LEFT, 4,  0xDC},
  {&LEFT, 5,  0xDD},
  {&LEFT, 6,  0xDE},
  {&LEFT, 7,  0xDA},

  {&LEFT, 8,  0xD1},
  {&LEFT, 9,  0xD2},
  {&LEFT, 10, 0xD3},
  {&LEFT, 11, 0xE7},
  {&LEFT, 12, 0xE8},
  {&LEFT, 13, 0xE9},
  {&LEFT, 14, 0xDF},
  {&LEFT, 15, 0xDA},

  {&LEFT, 16, 0xD4},
  {&LEFT, 17, 0xD5},
  {&LEFT, 18, 0xD6},
  {&LEFT, 19, 0xE4},
  {&LEFT, 20, 0xE5},
  {&LEFT, 21, 0xE6},
  {&LEFT, 22, 0xDF},
  {&LEFT, 23, 0xDA},

  {&LEFT, 24, 0xDA},
  {&LEFT, 25, 0xDA},
  {&LEFT, 26, 0xDA},
  {&LEFT, 27, 0xE1},
  {&LEFT, 28, 0xE2},
  {&LEFT, 29, 0xE3},
  {&LEFT, 30, 0xE0},
  {&LEFT, 31, 0xDA},

  {&LEFT, 32, 0xD8},
  {&LEFT, 33, 0xD9},
  {&LEFT, 34, 0xD7},
  {&LEFT, 35, 0xEA},
  {&LEFT, 36, 0xEA},
  {&LEFT, 37, 0xEB},
  {&LEFT, 38, 0xE0},
  {&LEFT, 39, 0xDA}
}; //LEFT keymap

Key RIGHTMAP[] = {
  {&RIGHT, 0,  0xCE},
  {&RIGHT, 1,  0xCF},
  {&RIGHT, 2,  0xD0},
  {&RIGHT, 3,  0xDB},
  {&RIGHT, 4,  0xDC},
  {&RIGHT, 5,  0xDD},
  {&RIGHT, 6,  0xDE},
  {&RIGHT, 7,  0xDA},

  {&RIGHT, 8,  0xD1},
  {&RIGHT, 9,  0xD2},
  {&RIGHT, 10, 0xD3},
  {&RIGHT, 11, 0xE7},
  {&RIGHT, 12, 0xE8},
  {&RIGHT, 13, 0xE9},
  {&RIGHT, 14, 0xDF},
  {&RIGHT, 15, 0xDA},

  {&RIGHT, 16, 0xD4},
  {&RIGHT, 17, 0xD5},
  {&RIGHT, 18, 0xD6},
  {&RIGHT, 19, 0xE4},
  {&RIGHT, 20, 0xE5},
  {&RIGHT, 21, 0xE6},
  {&RIGHT, 22, 0xDF},
  {&RIGHT, 23, 0xDA},

  {&RIGHT, 24, 0xDA},
  {&RIGHT, 25, 0xDA},
  {&RIGHT, 26, 0xDA},
  {&RIGHT, 27, 0xE1},
  {&RIGHT, 28, 0xE2},
  {&RIGHT, 29, 0xE3},
  {&RIGHT, 30, 0xE0},
  {&RIGHT, 31, 0xDA},

  {&RIGHT, 32, 0xD8},
  {&RIGHT, 33, 0xD9},
  {&RIGHT, 34, 0xD7},
  {&RIGHT, 35, 0xEA},
  {&RIGHT, 36, 0xEA},
  {&RIGHT, 37, 0xEB},
  {&RIGHT, 38, 0xE0},
  {&RIGHT, 39, 0xDA}
}; //RIGHT keymap
//---------------------------------------------
*/

//--------------------VARS---------------------
unsigned long lastPressTime;
uint8_t lastKey;
const int keyDelay = 500; 
//---------------------------------------------


//-----------------PROTOTYPES------------------
void pressKey(PCA9505_06 &expander, uint8_t pin, uint8_t key, uint8_t delay);
//---------------------------------------------


//------------------FUNCTIONS------------------
/**
 * void setup(), init function
*/
void setup() {
  NUMPAD.begin(0x23);  //0100 011      
  //LEFT.begin(0x22);  //0100 010
  //RIGHT.begin(0x21); //0100 001

  for(uint8_t i = 0; i<=NUM_KEYS_NUMPAD; i++){
    NUMPAD.pinMode(i, INPUT);
  }
  /*
  for(uint8_t i = 0; i<=NUM_KEYS_LEFT; i++){
    LEFT.pinMode(i, INPUT);
  }
  for(uint8_t i = 0; i<=NUM_KEYS_RIGHT; i++){
    RIGHT.pinMode(i, INPUT);
  }
  */
  
  Keyboard.begin();
}

/**
 * void loop(), infinite loop function
*/
void loop() {
  for (uint8_t i = 0; i < NUM_KEYS_NUMPAD; i++) {
    pressKey(NUMPADMAP[i].expander, NUMPADMAP[i].pin);
  }
  /*
  for (uint8_t i = 0; i < NUM_KEYS_LEFT; i++) {
    pressKey(LEFTMAP[i].expander, LEFTMAP[i].pin, LEFTMAP[i].key, keyRepeatDelay);
  }
  for (uint8_t i = 0; i < NUM_KEYS_RIGHT; i++) {
    pressKey(RIGHTMAP[i].expander, RIGHTMAP[i].pin, RIGHTMAP[i].key, keyRepeatDelay);
  }
  */
}

/**
 * void presskey(), handler function to, well, duh, press keys.
 * @param expander the PCA9505 IO expander name used (NUMPAD, LEFT or RIGHT)
 * @param pin the pin number of the PCA (0 through 39)
*/
void pressKey(PCA9505_06* expander, uint8_t pin) {
  uint8_t key = expander->digitalRead(pin) == LOW;
  if (key != lastKey) {
    lastPressTime = millis();
    lastKey = key;
    
    if (key != 0) {
      Keyboard.press(key);
    } else {
      Keyboard.releaseAll();
    }
  } else if (key != 0 && millis() - lastPressTime > keyDelay) {
    Keyboard.write(key);
    lastPressTime = millis();
  }
}
//---------------------------------------------