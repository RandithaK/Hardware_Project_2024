#define buzzer 6

void setup() {

   pinMode(buzzer, OUTPUT);

      tone(buzzer,1535,500);

      //add another 500 milliseconds of silence

      delay(1000);
      noTone(buzzer);
      digitalWrite(buzzer, HIGH);


}

 
void loop() {

     //turn on the buzzer at 1535 frequency for 500 milliseconds

}