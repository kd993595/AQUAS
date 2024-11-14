#define InputPIN 3

void setup(){
  // Set up serial monitor
  Serial.begin(115200);
  
  pinMode(InputPIN, INPUT);
}


void loop() {
  
  int val = digitalRead(InputPIN);
  Serial.println(val);
  
  delay(500);
}