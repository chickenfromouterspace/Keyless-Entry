void setup(){
  Serial.begin(115200);
  Serial3.begin(115200);
}

void loop(){
  Serial.println(Serial1.read());
}
