/*
  ReadAnalogVoltage

  Reads an analog input on pin 0, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(5,OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue=0;
  digitalWrite(5,HIGH);
  delay(1000);
  for (int i=0;i<50;i++)
  {
  sensorValue = sensorValue+analogRead(A0);
  }
  sensorValue = sensorValue/50;
  float voltage = sensorValue * (3.3 / 1023.0);
  Serial.println(voltage);
  digitalWrite(5,LOW);
  delay(1000);
}
