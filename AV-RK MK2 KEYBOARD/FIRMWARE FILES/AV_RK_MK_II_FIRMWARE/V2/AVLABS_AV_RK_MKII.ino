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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <Keyboard.h>
#include <Wire.h>
#include <pimoroniTrackball.h>

#include "Adafruit_SHTC3.h"
#include "Mouse.h"
//---------------------------------------------


//------------------DEFINES-------------------
//module addresses
#define NANO_I2C_ADDRESS 8
#define SCREEN_ADDRESS 0x3C 

// OLED screen module
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 

//delays for the key actuation
#define INITIAL_DELAY 250
#define REPEAT_DELAY 25

//custom keys for mouse functions
#define MOUSE_LEFT_CLICK 0xFF
#define MOUSE_RIGHT_CLICK 0xFE
#define MOUSE_SCROLL_UP 0xFD
#define MOUSE_SCROLL_DOWN 0xFC

//custom keys
#define KEY_FUNCTION 0xFB
#define KEY_CHEVRON_L 236
#define KEY_CHEVRON_R 236
#define KEY_SCREENSHOT 0xFA
#define KEY_FUN 0xF9

//customs keys for module control
#define KEY_MOD_1 0xF8  //deply arm (servo) for mouse charge
#define KEY_MOD_2 0xF7  //retract arm for mouse charge
#define KEY_MOD_3 0xF6  //cycle between Clock, AV-RK MK2 logo, WPM/CPM count, and Heart rate monitor (smol OLED screen)/
#define KEY_MOD_4 0xF5  //cycle between compact display mode (one line for RTC time, one for wpm, one for temperature, one for humidity), normal display mode (just the temps and humidity) and fun mode (just displays random things "hollywood" style)
#define KEY_MOD_5 0xF4  //not assigned
#define KEY_MOD_6 0xF3  //not assigned
//---------------------------------------------
//------------------PCA OBJECTS----------------
PCA9505_06 NUMPAD;
PCA9505_06 LEFT;
PCA9505_06 RIGHT;
//---------------------------------------------

//------------------I2C OBJECTS----------------
TwoWire &ServoControllerBus = Wire;
//---------------------------------------------

//------------------MODULE OBJECTS-------------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS1307 rtc;
//---------------------------------------------

//--------------------VARS---------------------
const uint8_t numKeys = 40; 
const int DEBOUNCE_DELAY = 200;

static const unsigned char PROGMEM degreeSymbol[] =
{ 
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00111000,
  0b01000100,
  0b01000100,
  0b00111000
};

char keyMapNUMPAD[numKeys] = {
  KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE, KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK, KEY_KP_MINUS, KEY_F13,
  KEY_INSERT, KEY_HOME, KEY_PAGE_UP, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_PLUS, KEY_F13,
  KEY_DELETE, KEY_END, KEY_PAGE_DOWN, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_PLUS, KEY_F13,
  KEY_F13, KEY_UP_ARROW, KEY_F13, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_ENTER, KEY_F13,
  KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, KEY_KP_0, KEY_KP_0, KEY_KP_ENTER, KEY_KP_DOT, KEY_F13
};
char keyMapNUMPADALT[numKeys] = {
  KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE, KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK, KEY_KP_MINUS, KEY_F13,
  KEY_INSERT, KEY_HOME, KEY_PAGE_UP, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_PLUS, KEY_F13,
  KEY_DELETE, KEY_END, KEY_PAGE_DOWN, KEY_MOD_4, KEY_MOD_5, KEY_MOD_6, KEY_KP_PLUS, KEY_F13,
  KEY_F13, KEY_UP_ARROW, KEY_F13, KEY_MOD_1, KEY_MOD_2, KEY_MOD_3, KEY_KP_ENTER, KEY_F13,
  KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, KEY_KP_0, KEY_KP_0, KEY_KP_ENTER, KEY_KP_DOT, KEY_F13
};
char keyMapLEFT[numKeys] = {
  KEY_ESC, '1', '2', '3', '4', '5', '6', KEY_F13,
  KEY_TAB, 'q', 'w', 'e', 'r', 't', '`' /*KEY_SUPERSCRIPT_TWO*/, KEY_F13,
  KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', MOUSE_LEFT_CLICK /*'CLIC GAUCHE'*/, KEY_F13,
  KEY_LEFT_SHIFT, KEY_CHEVRON_L, 'z', 'x', 'c', 'v', MOUSE_RIGHT_CLICK /*'CLIC DROIT'*/, KEY_F13,
  KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_F13 /* FONCTION*/, KEY_SCREENSHOT /*SCREENSHOT*/, ' ', KEY_RETURN, KEY_F13
};
char keyMapLEFTALT[numKeys] = {
  KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F13,
  KEY_TAB, 'q', 'w', 'e', 'r', 't', '`' /*KEY_SUPERSCRIPT_TWO*/, KEY_F13,
  KEY_CAPS_LOCK, 'a', 's', 'd', KEY_FUN, 'g', MOUSE_SCROLL_UP /*'CLIC GAUCHE'*/, KEY_F13,
  KEY_LEFT_SHIFT, KEY_F13 /*'chevron'*/, 'z', 'x', 'c', 'v', MOUSE_SCROLL_DOWN /*'CLIC DROIT'*/, KEY_F13,
  KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_F13 /* FONCTION*/, KEY_F13 /*SCREENSHOT*/, ' ', KEY_RETURN, KEY_F13
};
char keyMapRIGHT[numKeys] = {
  '7', '8', '9', '0', '-', '=', '[', KEY_F13,
  KEY_F13, 'y', 'u', 'i', 'o', 'p', ']', KEY_F13,
  KEY_BACKSPACE, 'h', 'j', 'k', 'l', ';', '\'', KEY_F13,
  KEY_DELETE, 'b', 'n', 'm', ',', '.', '/', KEY_F13,
  KEY_RETURN, ' ', KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_MENU, KEY_RIGHT_CTRL, '\\', KEY_F13

};
char keyMapRIGHTALT[numKeys] = {
  KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, '[', KEY_F13,
  KEY_F13, 'y', 'u', KEY_UP_ARROW, 'o', 'p', ']', KEY_F13,
  KEY_BACKSPACE, 'h', KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, ';', '\'', KEY_F13,
  KEY_DELETE, 'b', 'n', 'm', ',', '.', '/', KEY_F13,
  KEY_RETURN, ' ', KEY_RIGHT_ALT, KEY_F13, KEY_F13, KEY_F13, '\\', KEY_F13
};

bool keyStateNP[numKeys] = { 0 };
bool keyStateL[numKeys] = { 0 };
bool keyStateR[numKeys] = { 0 };

bool isShiftPressed = false;
bool isCapsLockOn = false;
bool initialDelayPassed = false;
bool funTypingModeActive = false; 

unsigned long lastPressTime;
unsigned long lastToggleTime = 0;
uint8_t lastKeyPressed = 255;

uint8_t mouseSpeed = 7;
int16_t x = 0;
int16_t y = 0;

uint8_t countServoMod1 = 0;
int keyMod2Counter = 0;
unsigned long lastPressTimeMod1 = 0;
unsigned long lastPressTimeMod2 = 0;
const long interval = 1000; 

int currentPage = 1;
int pageChangeCounter = 0; 
int lastPinState = HIGH; 
unsigned long lastPageChangeMillis = 0; 
const unsigned long debounceTime = 50; 
//---------------------------------------------


//------------------PROTOTYPES------------------
void checkKeys(PCA9505_06& expander, char* keyMap, char* funcKeyMap, bool* keyState, bool funcKeyPressed);
void chargingArmDeploy(bool servoCommand);
void displayPage1();
void displayPage2();
void displayPage3();
//---------------------------------------------


//------------------FUNCTIONS------------------
/**
 * @brief void setup(), init function
*/
void setup() {
  Serial.begin(9600);
  NUMPAD.begin(0x23);
  LEFT.begin(0x22);
  RIGHT.begin(0x21);
  trackball.begin(0x0A);
  ServoControllerBus.begin();
  trackball.setRGBW(0, 0, 0, 255);

  for (uint8_t i = 0; i < numKeys; i++) {
    NUMPAD.pinMode(i, INPUT);
    LEFT.pinMode(i, INPUT);
    RIGHT.pinMode(i, INPUT);
  }

  Keyboard.begin();

  if (trackball.isConnected()) {
    Mouse.begin();
  } else {
    while (true) {
      delay(1000);
    }
  }

  //SENSOR MODULE: uncomment when RTC is present on the I2C bus

  /*
  if (!rtc.begin()) {
    //Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //Serial.println("RTC is NOT running!");
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
   // Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  */
}


/**
 * @brief void loop(), infinite loop function
*/
void loop() {
  bool funcKeyPressed = ((LEFT.digitalRead(35) == 0) || (RIGHT.digitalRead(8)) == 0);

  checkKeys(NUMPAD, keyMapNUMPAD, keyMapNUMPADALT, keyStateNP, funcKeyPressed);
  checkKeys(LEFT, keyMapLEFT, keyMapLEFTALT, keyStateL, funcKeyPressed);
  checkKeys(RIGHT, keyMapRIGHT, keyMapRIGHTALT, keyStateR, funcKeyPressed);
 // trackball.setRGBW(0, 0, 0, 255);
  if (trackball.changed()) {
    y = (trackball.right() - trackball.left()) * mouseSpeed;
    x = (trackball.down() - trackball.up()) * (-1) * mouseSpeed;
    if (x != 0 || y != 0) {
      Mouse.move(x, y, 0);
    }
    if (trackball.click()) {
      Mouse.press(MOUSE_LEFT);
      // trackball.setRGBW(0, 0, 255, 255);
    } else if (trackball.release()) {
      if (Mouse.isPressed(MOUSE_LEFT)) {
        Mouse.release(MOUSE_LEFT);
        //   trackball.setRGBW(0, 0, 255, 0);
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
 * @note The function keymap is activated when the "function" key, represented by KEY_FUNCTION, is pressed.
 * @note MOUSE_LEFT_CLICK and MOUSE_RIGHT_CLICK are special constants used to represent mouse click actions.
 */
void checkKeys(PCA9505_06& expander, char* keyMap, char* funcKeyMap, bool* keyState, bool funcKeyPressed) {
  for (uint8_t i = 0; i < numKeys; i++) {
    unsigned long currentPressTime = millis();
    bool isPressed = (expander.digitalRead(i) == 0);
    char key = funcKeyPressed ? funcKeyMap[i] : keyMap[i];
    if (isPressed != keyState[i]) {
      keyState[i] = isPressed;
      switch (key) {
        case KEY_FUN:
          funTypingModeActive = !funTypingModeActive;
          break;
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
          // trackball.setRGBW(0, 255, 0, 255);
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

        case KEY_MOD_1:   
          if (currentPressTime - lastPressTimeMod1 > DEBOUNCE_DELAY) {
            countServoMod1++;
            lastPressTimeMod1 = currentPressTime;

            if(countServoMod1 % 2) {
              //DEPLOY ARM
              chargingArmDeploy(true);
            } else {
              //RETRACT ARM
              chargingArmDeploy(false);
            }
          }
          break;
        case KEY_MOD_2:
          if (currentPressTime - lastPressTimeMod2 > DEBOUNCE_DELAY) {
            keyMod2Counter++;
            lastPressTimeMod2 = currentPressTime;
            switch(keyMod2Counter) {
              case 1:
                displayPage1();
                trackball.setRGBW(255, 0, 0, 0);
                break;
              case 2:
                displayPage2();
                trackball.setRGBW(0, 255, 0, 0);
                break;
              case 3:
                displayPage3();
                trackball.setRGBW(0, 0, 255, 0);
                break;
              default:
                trackball.setRGBW(0, 0, 0, 255);
                keyMod2Counter = 0;
                break;
              }
            } 
          break;
        case KEY_MOD_3:
          break;
        case KEY_MOD_4: 
          //trackball.setRGBW(255, 0, 0, 0);
          break;
        case KEY_MOD_5: 
          //trackball.setRGBW(0, 255, 255, 0);
          break;
        case KEY_MOD_6: 
          trackball.setRGBW(0, 0, 0, 255);
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

  if (funTypingModeActive) {
    if (millis() - lastToggleTime > 150) {
      Keyboard.press(KEY_CAPS_LOCK);
      Keyboard.release(KEY_CAPS_LOCK);
      lastToggleTime = millis();
    }
  }

  if (lastKeyPressed != 255 && keyState[lastKeyPressed] && ((millis() - lastPressTime > INITIAL_DELAY && !initialDelayPassed) || (initialDelayPassed && millis() - lastPressTime > REPEAT_DELAY))) {
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

/**
 * @brief changingArmDeploy(), Moves a servo motor to deploy or retract a charging arm.
 * 
 * The function commands a slave device over I2C to move the servo to the desired position.
 *
 * @param servoCommand When true, the servo moves to 90 degrees (deploy). When false, the servo moves to 0 degrees (retract).
 * @note error handling: Trackball LED blinks red when transmission triggers an error. It reverts to normal when a succesful transmission is sent
 */
void chargingArmDeploy(bool servoCommand){
  trackball.setRGBW(0, 0, 0, 255);
  //START TRANSMISSION
  ServoControllerBus.beginTransmission(NANO_I2C_ADDRESS);
  if(servoCommand==true){
    ServoControllerBus.write(90);//DEPLOY ARM
  }
  else{
    ServoControllerBus.write(0);//RETRACT ARM
  }
  byte error = ServoControllerBus.endTransmission();//GET ERROR
  if (error) {//LIGHT RED IF ERROR
    trackball.setRGBW(255, 0, 0, 0); 
    } 
  else { //GREEN OTHERWISE
    trackball.setRGBW(0, 255, 0, 0); 
  }
}

/**
 * @brief Displays the content for Page 1 on the OLED screen.
 * 
 * This function clears the display and updates it with the following content:
 * - A centered title "AV-RK MK II".
 * - A horizontal line under the title.
 * - A centered indicator "Page 1/3".
 * - The current RTC time in the format "RTC Time: hh:mm:ss".
 * - WPM information.
 * - Relative Humidity in percentage.
 * - Ambient Temperature in Celsius.
 * 
 * The data for temperature and humidity are currently placeholders and can be replaced with real sensor values.
 * 
 * @note Call this function in the main loop to continuously update the displayed time.
 */
void displayPage1() {
    static int lastSecond = -1;
    float temperature = 25;
    float humidity = 20;

    DateTime now = rtc.now();
    if (lastSecond == now.second()) return;
    lastSecond = now.second();

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Title - AV-RK MK II
    display.setTextSize(1.5);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("AV-RK MK II", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, (16 - h) / 2);
    display.print("AV-RK MK II");
    display.setTextSize(1);

    // Horizontal line under title
    int lineY = y1 + h + 4 + 2;
    display.drawLine(0, lineY, SCREEN_WIDTH, lineY, SSD1306_WHITE);

    // Display "Page 1/3" centered
    int y_offset = lineY + 6;
    display.getTextBounds("Page 1/3", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, y_offset);
    display.println("Page 1/3");
  
    // Display RTC Time
    y_offset += 12;
    char heure[20];
    sprintf(heure, "RTC Time: %02d:%02d:%02d", now.hour(), now.minute(), now.second());
    display.setCursor(0, y_offset);
    display.println(heure);

    // Display WPM
    display.println("WPM: 120wpm");

    // Display Relative Humidity
    display.print("RH%: ");
    display.print(humidity, 1);
    display.println("%");

    // Display Ambient Temperature
    display.print("ambiant temp: ");
    display.print(temperature, 1);
    display.drawBitmap(display.getCursorX(), display.getCursorY() - 8, degreeSymbol, 8, 8, SSD1306_WHITE);
    display.println(" C");

    display.display();
}

/**
 * @brief Displays the content for Page 2 on the OLED screen.
 * 
 * This function clears the display and updates it with placeholder content specific to Page 2. 
 * The current implementation has a title "Page 2/3" and a few lines of placeholder text.
 * 
 * @note currently placeholder.
 */
void displayPage2() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1.5);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("AV-RK MK II", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, (16 - h) / 2);
    display.print("AV-RK MK II");
    display.setTextSize(1);

    // Horizontal line under title
    int lineY = y1 + h + 4 + 2;
    display.drawLine(0, lineY, SCREEN_WIDTH, lineY, SSD1306_WHITE);

    // Display "Page 1/3" centered
    int y_offset = lineY + 6;
    display.getTextBounds("Page 1/3", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, y_offset);
    display.println("Page 2/3");
    // Placeholder content for Page 2
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.println("Content for");
    display.println("Page 2 goes");
    display.println("here...");

    display.display();
}

/**
 * @brief Displays the content for Page 3 on the OLED screen.
 * 
 * This function clears the display and updates it with placeholder content specific to Page 3.
 * The current implementation has a title "Page 3/3" and a few lines of placeholder text.
 * 
 * @note currently placeholder.
 */
void displayPage3() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1.5);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("AV-RK MK II", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, (16 - h) / 2);
    display.print("AV-RK MK II");
    display.setTextSize(1);

    // Horizontal line under title
    int lineY = y1 + h + 4 + 2;
    display.drawLine(0, lineY, SCREEN_WIDTH, lineY, SSD1306_WHITE);

    // Display "Page 1/3" centered
    int y_offset = lineY + 6;
    display.getTextBounds("Page 3/3", 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, y_offset);
    display.println("Page 3/3");

    // Placeholder content for Page 3
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.println("Content for");
    display.println("Page 3 goes");
    display.println("here...");

    display.display();
}


//END
