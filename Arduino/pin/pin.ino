void reading(void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(30, INPUT_PULLUP);
  pinMode(31, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(31, HIGH);
  reading();
  delay(1000);
  digitalWrite(31, LOW);
  reading();
}

void reading()
{
  if(digitalRead(30) == HIGH)
    Serial.println("High.");
  if(digitalRead(30) == LOW)
    Serial.println("Low.");
}
