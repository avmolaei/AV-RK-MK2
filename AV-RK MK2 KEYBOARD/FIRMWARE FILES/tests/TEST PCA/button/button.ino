#include <PCA9505_9506.h>

/*
 *  Input detection for the PCA9505/06 I/O expander
 *
 *  Note:
 *		PCA9506 has no pull up resistors.
 *  	The 100k pullup resistors on the PCA9505 aren't configurable
 *		All IO is default as INPUT
 *  
 *  Addressing 0x20 plus:
 *    A0: 001 - 0x01
 *    A1: 010 - 0x02
 *    A2: 100 - 0x04
 *    
 */

PCA9505_06 GPIO;

void setup() {
  GPIO.begin(0x23);   //  Default address 0x20
  GPIO.pinMode(0, INPUT);

  pinMode(P1_15, OUTPUT);
}

void loop() {
  digitalWrite(P1_15, GPIO.digitalRead(0));
}
