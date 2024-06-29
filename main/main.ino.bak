// Including the libraries
#include <LiquidCrystal_I2C.h>
// Reference page https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
#include <Stepper.h>
// Reference page https://www.arduino.cc/reference/en/libraries/stepper/
#include <Servo.h>
// Reference page https://www.arduino.cc/reference/en/libraries/servo/


// Pin definitions
// Pin definitions should be added here and any references to the pins should be done through these definitions and should never be made directly to the pin address unless absolutely necessary.

// Dispensing Control Pins
#define dispenser_0_Open A0
#define dispenser_0_Close A1
#define dispenser_1_Open A2
#define dispenser_1_Close A3
#define dispenser_2_Open A4
#define dispenser_2_Close A5

// Sealing Unit Control
#define sealer_assembly_Step 52
#define sealer_assembly_Direction 53

// Weight Scale
#define weight_cell_DATA 4
#define weight_cell_CLK 5

// Pins A4 and A5 are used as communication pins SCL and SDA with the LCD
// Pins 0 and 1 used for serial communication and hence not used for any other purpose for stabilty

// Keypad Connections
#define keypad_col1 30
#define keypad_col2 32
#define keypad_col3 34
#define keypad_col4 36

#define keypad_row1 31
#define keypad_row2 33
#define keypad_row3 35
#define keypad_row4 37

// Buzzer
#define Buzzer 13

// Setting up devices
LiquidCrystal_I2C lcd(0x27, 20, 4); // Setting up the LCD at address 0x27

// Offset values for Grams
#define Type_1 0
#define Type_2 0
#define Type_3 0

void setup()
{
  // put your setup code here, to run once:
}

void loop()
{
  // put your main code here, to run repeatedly:
}

/*
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

*/