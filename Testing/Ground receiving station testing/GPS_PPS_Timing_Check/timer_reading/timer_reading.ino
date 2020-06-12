/*
 * program to see how reading a timer works
 * 
 */
unsigned long pulseCount = 0;

void setup() {
  Serial.begin(115200);
    pinMode(9,OUTPUT);
    digitalWrite(9,LOW);
    
    noInterrupts();
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1 = 0; //initialise counter value to 0
    OCR1A = 65535; //sets the value
    TCCR1B |= (1<< WGM12); //turn on CTC mode 
    TCCR1B |= (1<<CS10); //prescaler set to 1 (62.5nS resolution)
    TIMSK1 |= (1 << OCIE1A); //sets up the interupt for timer1
    interrupts();
}

void loop() {
  digitalWrite(9,LOW);

}

ISR(TIMER1_COMPA_vect){//timer1 interrupt
digitalWrite(9,HIGH);
pulseCount++;
}
