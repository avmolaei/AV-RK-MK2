#include <Keyboard.h>
#include <PCA9505_9506.h>

#define INITIAL_DELAY 500
#define REPEAT_DELAY 100

PCA9505_06 GPIO;

const uint8_t numKeys = 35;
char keyMap[numKeys] = {
  '0xCE', 'W', 'E', 'R', // Replace these with your actual keys
  'T', 'Y', 'U', 'I',
  'O', 'P', 'A', 'S',
  'D', 'F', 'G', 'H'
};

// Array to keep track of which keys are currently pressed
bool keyState[numKeys] = {0};
unsigned long lastPressTime;
uint8_t lastKeyPressed = 255; // Invalid key code to start with
bool initialDelayPassed = false; // No initial delay has passed at the beginning

void setup() {
  GPIO.begin(0x23); 
  for (uint8_t i = 0; i < numKeys; i++) {
    GPIO.pinMode(i, INPUT);
  }
  Keyboard.begin();
}

void loop() {
  for (uint8_t i = 0; i < numKeys; i++) {
    bool isPressed = (GPIO.digitalRead(i) == 0);
    if (isPressed != keyState[i]) {
      keyState[i] = isPressed;
      if (isPressed) {
        Keyboard.press(keyMap[i]);
        lastPressTime = millis();
        lastKeyPressed = i;
        initialDelayPassed = false; // Reset the initial delay for the new key
      } else {
        Keyboard.release(keyMap[i]);
        if (i == lastKeyPressed) {
          lastKeyPressed = 255; // No key is repeating now
        }
      }
    }
  }

  // If a key is in the repeating state and it's time for the next repeat
  if (lastKeyPressed != 255 && keyState[lastKeyPressed] &&
      ((millis() - lastPressTime > INITIAL_DELAY && !initialDelayPassed) ||
       (initialDelayPassed && millis() - lastPressTime > REPEAT_DELAY))) {
    Keyboard.release(keyMap[lastKeyPressed]);
    Keyboard.press(keyMap[lastKeyPressed]);
    lastPressTime = millis();
    initialDelayPassed = true;
  }
}
