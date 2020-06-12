/*
 * Ground_Reciever - Ground Station GPS PPS Timing check
 * 
 * This program is designed to test the timing and accuracy of the GPS PPS signal. 
 *  
 *  
 * Author:  Travis McKee 
 * Date:    21 Feb 2020
 * 
 */

volatile unsigned long pulseCount = 0, count = 0;
volatile unsigned long pulseNum,clockNum;
volatile unsigned long diffTime = 0;
volatile byte state = LOW;

void setup() {
  Serial.begin(115200);

  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1 = 0; //initialise counter value to 0
  OCR1A = 65535; //sets the value
  TCCR1B |= (1<< WGM12); //turn on CTC mode 
  TCCR1B |= (1<<CS10); //prescaler set to 1 (62.5nS resolution)
  TIMSK1 |= (1 << OCIE1A); //sets up the interupt for timer1
  interrupts();

  attachInterrupt(digitalPinToInterrupt(3), stamp, RISING);

}

void loop() {
  if (state == HIGH)
  {
    Serial.print((pulseNum*65535)+clockNum); Serial.print(",");
    state = LOW;
  }

}

void stamp()
{
  noInterrupts();
  clockNum = TCNT1;
  pulseNum = pulseCount;
  state = HIGH;
  pulseCount = 0;
  TCNT1 = 0;
  interrupts();
}

ISR(TIMER1_COMPA_vect){//perform interupt when timer1 hits 65535 or 4,096us (top of register)
pulseCount++;
}
