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
//Wire, Mouse and Pimoroni libs for the trackball
#include <PCA9505_9506.h>
#include <Keyboard.h>
#include <Wire.h>
#include "Mouse.h"
#include <pimoroniTrackball.h> 
//---------------------------------------------


//------------------DEFINES-------------------
#define INITIAL_DELAY 150
#define REPEAT_DELAY 25
#define MOUSE_LEFT_CLICK  0xFF
#define MOUSE_RIGHT_CLICK 0xFE
#define MOUSE_SCROLL_UP   0xFD
#define MOUSE_SCROLL_DOWN 0xFC
#define FUNCTION_KEY 0xFB
#define KEY_CHEVRON_L 236
#define KEY_CHEVRON_R 236
#define KEY_SCREENSHOT 0xFA

//---------------------------------------------

//------------------PCA OBJECTS----------------
PCA9505_06 NUMPAD;
PCA9505_06 LEFT;
PCA9505_06 RIGHT;
//---------------------------------------------


//--------------------VARS---------------------
const uint8_t numKeys = 40;

char keyMapNUMPAD[numKeys] = { 
  KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE,        KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK,  KEY_KP_MINUS,           KEY_F13, 
  KEY_INSERT,       KEY_HOME,        KEY_PAGE_UP,      KEY_KP_7,     KEY_KP_8,      KEY_KP_9,        KEY_KP_PLUS,            KEY_F13,
  KEY_DELETE,       KEY_END,         KEY_PAGE_DOWN,    KEY_KP_4,     KEY_KP_5,      KEY_KP_6,        KEY_KP_PLUS,            KEY_F13,
  KEY_F13,          KEY_UP_ARROW,    KEY_F13,          KEY_KP_1,     KEY_KP_2,      KEY_KP_3,        KEY_KP_ENTER,           KEY_F13,
  KEY_LEFT_ARROW,   KEY_DOWN_ARROW,  KEY_RIGHT_ARROW,  KEY_KP_0,     KEY_KP_0,      KEY_KP_ENTER,    KEY_KP_DOT,             KEY_F13
};

char keyMapNUMPADALT[numKeys] = {0};

char keyMapLEFT[numKeys] = {
  KEY_ESC,           '1',            '2',              '3',          '4',           '5',             '6',                    KEY_F13,
  KEY_TAB,           'q',            'w',              'e',          'r',           't',             '`'/*KEY_SUPERSCRIPT_TWO*/,    KEY_F13, 
  KEY_CAPS_LOCK,     'a',            's',              'd',          'f',           'g',              MOUSE_LEFT_CLICK/*'CLIC GAUCHE'*/,          KEY_F13, 
  KEY_LEFT_SHIFT,    KEY_CHEVRON_L,      'z',              'x',          'c',           'v',             MOUSE_RIGHT_CLICK/*'CLIC DROIT'*/,           KEY_F13, 
  KEY_LEFT_CTRL,      KEY_LEFT_GUI,  KEY_LEFT_ALT,   KEY_F13/* FONCTION*/,     KEY_SCREENSHOT/*SCREENSHOT*/,    ' ',             KEY_RETURN,           KEY_F13
}; 

char keyMapLEFTALT[numKeys] = {
  KEY_ESC,           KEY_F1,            KEY_F2,              KEY_F3,          KEY_F4,           KEY_F5,             KEY_F6,                    KEY_F13,
  KEY_TAB,           'q',            'w',              'e',          'r',           't',             '`'/*KEY_SUPERSCRIPT_TWO*/,    KEY_F13, 
  KEY_CAPS_LOCK,     'a',            's',              'd',          'f',           'g',              MOUSE_SCROLL_UP/*'CLIC GAUCHE'*/,          KEY_F13, 
  KEY_LEFT_SHIFT,    KEY_F13/*'chevron'*/,      'z',              'x',          'c',           'v',             MOUSE_SCROLL_DOWN/*'CLIC DROIT'*/,           KEY_F13, 
  KEY_LEFT_CTRL,      KEY_LEFT_GUI,  KEY_LEFT_ALT,   KEY_F13/* FONCTION*/,     KEY_F13/*SCREENSHOT*/,    ' ',             KEY_RETURN,           KEY_F13
};

char keyMapRIGHT[numKeys] = {
  '7',                '8',            '9',              '0',          '-',           '=',            '[',                    KEY_F13,
  KEY_F13,          'y',            'u',              'i',          'o',           'p',            ']',                    KEY_F13, 
  KEY_BACKSPACE,     'h',            'j',              'k',          'l',           ';',             '\'',                    KEY_F13, 
  KEY_DELETE,        'b',            'n',              'm',          ',',           '.',            '/',                    KEY_F13, 
  KEY_RETURN,             ' ',          KEY_RIGHT_ALT,           KEY_RIGHT_GUI,       KEY_MENU,          KEY_RIGHT_CTRL,          '\\',                    KEY_F13

};  

char keyMapRIGHTALT[numKeys] = {
  KEY_F7,                KEY_F8,            KEY_F9,              KEY_F10,          KEY_F11,           KEY_F12,            '[',                    KEY_F13,
  KEY_F13,          'y',            'u',              KEY_UP_ARROW,          'o',           'p',            ']',                    KEY_F13, 
  KEY_BACKSPACE,     'h',            KEY_LEFT_ARROW,              KEY_DOWN_ARROW,          KEY_RIGHT_ARROW,           ';',             '\'',                    KEY_F13, 
  KEY_DELETE,        'b',            'n',              'm',          ',',           '.',            '/',                    KEY_F13, 
  KEY_RETURN,             ' ',          KEY_RIGHT_ALT,           KEY_F13,       KEY_F13,          KEY_F13,          '\\',                    KEY_F13
};

bool keyStateNP[numKeys] = {0};
bool keyStateL[numKeys] = {0};
bool keyStateR[numKeys] = {0};   

bool isShiftPressed = false;
bool isCapsLockOn = false;

unsigned long lastPressTime;
uint8_t lastKeyPressed = 255; 
bool initialDelayPassed = false;

uint8_t mouseSpeed =7; 
int16_t x = 0;
int16_t y = 0;
//---------------------------------------------


//------------------FUNCTIONS------------------.................;;;;;....;;;;...........................
/**
 * @brief void setup(), init function
*/
void setup() {
  NUMPAD.begin(0x23);
  LEFT.begin(0x22);
  RIGHT.begin(0x21); 
  trackball.begin(0x0A);

  for (uint8_t i = 0; i < numKeys; i++) {
    NUMPAD.pinMode(i, INPUT);
    LEFT.pinMode(i, INPUT);
    RIGHT.pinMode(i, INPUT);
  }
  
  Keyboard.begin();

  if(trackball.isConnected()) {
    Mouse.begin();
  }
  else {
    while(true)
    {
      delay(1000);
    }
  }
}


/**
 * @brief void loop(), infinite loop function
*/
void loop() { 
  bool funcKeyPressed = ((LEFT.digitalRead(35)==0) || (RIGHT.digitalRead(8))==0)  ;
  checkKeys(NUMPAD, keyMapNUMPAD, keyMapNUMPADALT, keyStateNP, funcKeyPressed);
  checkKeys(LEFT, keyMapLEFT, keyMapLEFTALT, keyStateL, funcKeyPressed);
  checkKeys(RIGHT, keyMapRIGHT, keyMapRIGHTALT, keyStateR, funcKeyPressed);

  if(trackball.changed()){
    y = (trackball.right() - trackball.left()) * mouseSpeed;
    x = (trackball.down() - trackball.up())*(-1)* mouseSpeed;
  if(x != 0 || y != 0){
      Mouse.move(x, y, 0);
  }
  if(trackball.click()){
      Mouse.press(MOUSE_LEFT);
    }
    else if(trackball.release()){
      if(Mouse.isPressed(MOUSE_LEFT)){
        Mouse.release(MOUSE_LEFT);
      }
    }
  }
}


/**
 * @brief Checks the state of each key connected to a specific PCA9505 chip, triggers key press/release actions, and handles mouse clicks.
 *
 * This function iterates over each key connected to the PCA9505 chip, represented by a PCA9505_06 object, and checks whether the state of the key has changed
 * (i.e., it has been pressed or released). It triggers the appropriate key press/release actions based on the current state of the keys and supports handling
 * mouse clicks. If a "function" key is pressed, the function switches to an alternative keymap layer defined by the function keymap.
 *
 * @param expander Reference to the PCA9505_06 object representing the PCA9505 chip to be checked.
 * @param keyMap Pointer to the array containing the primary keymap for the PCA9505 chip.
 * @param keyMapFunction Pointer to the array containing the function keymap for the PCA9505 chip.
 * @param keyState Pointer to the array containing the current state of each key connected to the PCA9505 chip.
 *
 * @note The function keymap is activated when the "function" key, represented by FUNCTION_KEY, is pressed.
 * @note MOUSE_LEFT_CLICK and MOUSE_RIGHT_CLICK are special constants used to represent mouse click actions.
 */
void checkKeys(PCA9505_06& expander, char* keyMap, char* funcKeyMap, bool* keyState, bool funcKeyPressed) {
  for (uint8_t i = 0; i < numKeys; i++) {
    bool isPressed = (expander.digitalRead(i) == 0);
    char key = funcKeyPressed ? funcKeyMap[i] : keyMap[i];
    if (isPressed != keyState[i]) {
      keyState[i] = isPressed;
      switch (key) {
        case KEY_SCREENSHOT:
          if (isPressed) {
            Keyboard.press(KEY_LEFT_GUI);
            Keyboard.press(KEY_LEFT_SHIFT);
            Keyboard.press('s');
            delay(50); 
            Keyboard.releaseAll();
            lastPressTime = millis();
            lastKeyPressed = i;
            initialDelayPassed = false;
          }
          break;

        case KEY_CHEVRON_L:
          if (isPressed) {
            if (isCapsLockOn || isShiftPressed) {
              Keyboard.press(KEY_LEFT_SHIFT); 
              Keyboard.press(KEY_CHEVRON_L); 
              Keyboard.releaseAll(); 
            } else {
              Keyboard.press(KEY_CHEVRON_R); 
              Keyboard.release(KEY_CHEVRON_R); 
            }
            lastPressTime = millis();
            lastKeyPressed = i;
            initialDelayPassed = false;
          } 
          break;
        case MOUSE_LEFT_CLICK:
          isPressed ? Mouse.press(MOUSE_LEFT) : Mouse.release(MOUSE_LEFT);
          break;
        case MOUSE_RIGHT_CLICK:
          isPressed ? Mouse.press(MOUSE_RIGHT) : Mouse.release(MOUSE_RIGHT);
          break;
        case MOUSE_SCROLL_UP: 
          if (isPressed) Mouse.move(0, 0, 1); 
          break;
        case MOUSE_SCROLL_DOWN:
          if (isPressed) Mouse.move(0, 0, -1); 
          break;
        default:
          if (isPressed) {
            Keyboard.press(key);
            lastPressTime = millis();
            lastKeyPressed = i;
            initialDelayPassed = false;
          } else {
            Keyboard.release(key);
            if (i == lastKeyPressed) lastKeyPressed = 255;
          }
          break;
      }
    }
  }

  if (lastKeyPressed != 255 && keyState[lastKeyPressed] &&
      ((millis() - lastPressTime > INITIAL_DELAY && !initialDelayPassed) ||
       (initialDelayPassed && millis() - lastPressTime > REPEAT_DELAY))) {
    char key = funcKeyPressed ? funcKeyMap[lastKeyPressed] : keyMap[lastKeyPressed];
    if (key != MOUSE_LEFT_CLICK && key != MOUSE_RIGHT_CLICK && key != MOUSE_SCROLL_UP && key != MOUSE_SCROLL_DOWN) {
      Keyboard.release(key);
      Keyboard.press(key);
      lastPressTime = millis();
      initialDelayPassed = true;
    }
  }
}
//---------------------------------------------


//END
