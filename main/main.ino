// Including the libraries
#include <LiquidCrystal_I2C.h>
// Reference page https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
#include <Stepper.h>
// Reference page https://www.arduino.cc/reference/en/libraries/stepper/
#include <Servo.h>
// Reference page https://www.arduino.cc/reference/en/libraries/servo/

// Pin definitions
// Pin definitions should be added here and any references to the pins should be done through these definitions and should never be made directly to the pin address unless absolutely necessary.
#define error_pin 13
#define dispenser_0 A0
#define dispenser_1 A1
#define dispenser_2 A2
#define weight_cell A3
#define sealer_assembly 2
#define servo 3
#define mode_switch 4
#define keypad_row1 12
#define keypad_row2 11
#define keypad_row3 10
#define keypad_row4 9
#define keypad_col1 8
#define keypad_col2 7
#define keypad_col3 6
#define keypad_col4 5



// Setting up devices
LiquidCrystal_I2C lcd(0x27,20,4); // Setting up the LCD at address 0x27


//Function Prototypes
void get_keypad_input();
void dispense();
void error();
void mode_switch();

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();

}

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
  // activate buzzer to give cricket alarm
}

void mode_switch(){
  // switch to dispensing mode from input mode
}