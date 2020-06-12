float sensorValue = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}


void loop() {

  for(int i = 0;i<100;i++)
  {
  sensorValue = sensorValue + (analogRead(A0) * (3.3 / 1023.0));
  //Serial.println(analogRead(A0));
  }
  sensorValue = sensorValue/100;
  Serial.print("voltage is:");
  Serial.println(sensorValue);
   Serial.print("Total voltage is:");
  Serial.println(2*sensorValue);
  delay(1000);
  sensorValue=0;
}
