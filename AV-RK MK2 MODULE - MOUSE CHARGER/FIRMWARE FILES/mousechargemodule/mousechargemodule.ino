#include <Wire.h>
#include <Servo.h>

#define I2C_ADDRESS 0x08

Servo myservo;
int pos = 0;
int howmany = 0;

void setup() {
  myservo.attach(9);
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent);

  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Setup complete. Waiting for I2C command...");
}

void loop() {
  delay(100);
}

void receiveEvent(int howmany) {
  while (Wire.available()) {
    char c = Wire.read();
    pos = c;
    Serial.print("Received position command: ");
    Serial.println(pos);

    if (pos == '90') {
      Serial.println("Moving servo to 90 degrees...");
      myservo.write(90);
    } else if (pos == '0') {
      Serial.println("Moving servo to 0 degrees...");
      myservo.write(0);
    } else {
      Serial.println("Received unrecognized command. Ignoring...");
    }
  }
}
