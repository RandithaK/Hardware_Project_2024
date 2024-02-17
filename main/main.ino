// Including the libraries
#include <LiquidCrystal_I2C.h>
// Reference page https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
#include <Stepper.h>
// Reference page https://www.arduino.cc/reference/en/libraries/stepper/
#include <Servo.h>
// Reference page https://www.arduino.cc/reference/en/libraries/servo/

// Pin definitions
// Pin definitions should be added here and any references to the pins should be done through these definitions and should never be made directly to the pin address unless absolutely necessary.

#define dispenser_0 A0
#define dispenser_1 A1
#define dispenser_2 A2
#define weight_cell A3
// Pins A4 and A5 are used as communication pins SCL and SDA with the LCD
// Pins 0 and 1 used for serial communication and hence not used for any other purpose for stabilty
#define sealer_assembly 2
#define servo 3
#define mode_switch 4
#define keypad_col4 5
#define keypad_col3 6
#define keypad_col2 7
#define keypad_col1 8
#define keypad_row4 9
#define keypad_row3 10
#define keypad_row2 11
#define keypad_row1 12
#define error_pin 13


// Setting up devices
LiquidCrystal_I2C lcd(0x27,20,4); // Setting up the LCD at address 0x27


//Function Prototypes
void get_keypad_input();
void dispense();
void error();
void mode_switcher();



// This is the entry point of the program
void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();

}


// This is the loop section of the program.
// Will be executed till the machine is turned off.
void loop() {
  // put your main code here, to run repeatedly:

}





void get_keypad_input(){
  // put code to get keypad input
}

void dispense(){
  // put code to dispense gram
}

void error(){
  pinMode(error_pin , OUTPUT);
  while (1)
  {
    digitalWrite(error_pin, HIGH);
    delay(350);
    digitalWrite(error_pin, LOW);
    delay(150);
  }
  
  
}

void mode_switcher(){
  // switch to dispensing mode from input mode
  if (digitalRead(mode_switch) == HIGH)
  {
    digitalWrite(mode_switch,LOW);
  }
  else
  {
    digitalWrite(mode_switch,HIGH);
  }
}