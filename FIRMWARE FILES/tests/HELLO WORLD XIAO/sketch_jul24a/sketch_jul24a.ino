void setup() {
  // put your setup code here, to run once:
  pinMode(P1_15, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(P1_15, HIGH);
  delay(1000);
  digitalWrite(P1_15, LOW); 
  delay(1000);
}
