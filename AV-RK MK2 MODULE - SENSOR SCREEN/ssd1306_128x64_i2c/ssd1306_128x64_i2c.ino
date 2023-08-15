
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include "Adafruit_SHTC3.h"

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS1307 rtc;

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

int currentPage = 1;
int pageChangeCounter = 0; 
int lastPinState = HIGH; 
unsigned long lastPageChangeMillis = 0;  // Time of the last page change
const unsigned long debounceTime = 50; 

void setup() {
  Serial.begin(9600);
      pinMode(13, INPUT_PULLUP); 
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
   // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC is NOT running!");
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
}

void loop() {
     unsigned long currentMillis = millis(); 

       int currentPinState = digitalRead(13);  // Read the current state of pin 13

   // If the pin state has changed from HIGH to LOW (button pressed or state changed)
    // and enough time has passed since the last button press (debouncing)
    if (lastPinState == HIGH && currentPinState == LOW && currentMillis - lastPageChangeMillis > debounceTime) {
        currentPage = (currentPage % 3) + 1;  // Cycle through the pages
        lastPageChangeMillis = currentMillis;  // Update the last change time
    }

    // Remember the last pin state for the next loop iteration
    lastPinState = currentPinState;
     switch (currentPage) {
        case 1:
            displayPage1();
            break;
        case 2:
            displayPage2();
            break;
        case 3:
            displayPage3();
            break;
    }  // Wait for 1 second before updating again
    
}

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
