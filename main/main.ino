// Including the libraries

// Library for LCD over I2C Protocol
// Reference page https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
#include <LiquidCrystal_I2C.h>
// Library to use I2C protocol
#include <Wire.h>

// // Library for Stepper
// Reference page https://www.arduino.cc/reference/en/libraries/stepper/
#include <Stepper.h>

/*
// Uncomment if need to control servo motors
// Reference page https://www.arduino.cc/reference/en/libraries/servo/
#include <Servo.h>
*/

// Library for KeyPad
#include <Keypad.h>

// Pin definitions
// Pin definitions should be added here and any references to the pins should be done through these definitions and should never be made directly to the pin address unless absolutely necessary.

// Dispensing Control Pins
#define dispenser_0_Open A0
#define dispenser_0_Close A1
#define dispenser_1_Open A2
#define dispenser_1_Close A3
#define dispenser_2_Open A4
#define dispenser_2_Close A5
#define dispenserOpenDelay 2000 // in ms
#define dispenserCloseDelay 3000 // in ms

// Sealing Unit Control
#define sealer_assembly_Step 52
#define sealer_assembly_Direction 53

// Weight Scale
#define HX711_dout 4
#define HX711_sck 5

// Library Code for Weight Scale
#include <HX711_ADC.h>
#include <EEPROM.h>

HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

// End of Library Code for Weight Scale

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

// Library Code for Keypad

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {keypad_row1, keypad_row2, keypad_row3, keypad_row4}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {keypad_col1, keypad_col2, keypad_col3, keypad_col4}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// End of Library Code for Keypad

// Buzzer

#define buzzer 13

LiquidCrystal_I2C lcd(0x27, 20, 4); // Setting up the LCD at address 0x27

// Offset values for Grams
#define Type1OffSet 0
#define Type2OffSet 0
#define Type3OffSet 0

// Custom Functions Prototypes

// Give an audio alert to the user
void beeper(int beeps);
// Get the Order details from the customer
int getdetails(char *typetodispence);
// Getts a character from the KeyPad
char getachar();
// To dispence a given amount from a given type
void dispence(char typetodispence, int weighttodispence);
// To get the mass from the scale
int getWeightFromScale();
// To open the hopper
void openhopper(char hopper);
// To close the hopper
void closehopper(char hopper);
// A helpper function to make mixtures
void dispenceMixture(int weighttodispence);
// Ending the program gracefully
void thankyou();

// Main Entry Point
void setup()
{
  // put your setup code here, to run once:

  // Setup Dispensers as output pins
  pinMode(dispenser_0_Open, OUTPUT);
  pinMode(dispenser_0_Close, OUTPUT);
  pinMode(dispenser_1_Open, OUTPUT);
  pinMode(dispenser_1_Close, OUTPUT);
  pinMode(dispenser_2_Open, OUTPUT);
  pinMode(dispenser_2_Close, OUTPUT);

  lcd.init(); // initialize the lcd
  lcd.backlight();

  lcd.setCursor(3, 0);
  lcd.print("Hardware Project");
  lcd.setCursor(2, 1);
  lcd.print("IT Group 12");

  delay(500);
  beeper(1);

  delay(10);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Starting...");
  lcd.setCursor(2, 1);
  lcd.print("Scale");

  LoadCell.begin();
  float calibrationValue;   // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch

  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;                 // set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag())
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Timeout");
    lcd.setCursor(2, 1);
    lcd.print("Check Wiring");
    while (1)
      ;
  }
  else
  {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Scale Ready");
  }
}

// Main Loop
void loop()
{
  // put your main code here, to run repeatedly:
  char typetodispence;
  int weighttodispence = getdetails(&typetodispence);
  if (typetodispence = 'D')
  {
    dispenceMixture(weighttodispence);
  }
  else
  {
    dispence(typetodispence, weighttodispence);
  }

  thankyou();
}

// Custom Functions

// Function to get what to dispence from the customer
int getdetails(char *typetodispence)
{
  int weight = 0;
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Enter Amount");
  lcd.setCursor(3, 1);
  lcd.print("And Type");

  while (1)
  {
    char got = getachar();

    if (got == '*')
    {
      weight /= 10;
    }
    else if (got == '#')
    {
      weight = 0;
    }
    else if (got == 'A' || got == 'B' || got == 'C' || got == 'D')
    {
      *typetodispence = got;
      return weight;
    }
    else
    {
      weight *= 10;
      weight += got - '0';
    }

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Weight gramms");
    lcd.setCursor(3, 1);
    String weightDisplay = String(weight);
    lcd.print(weightDisplay);
  }
}

char getachar()
{
  while (1)
  {
    char key = keypad.getKey();

    if (key)
    {
      return (key);
    }
  }
}

// Function to give an audio alert to the user
void beeper(int beeps)
{
  for (int i = 0; i < beeps; i++)
  {
    // turn on the buzzer at 1535 frequency for 500 milliseconds
    tone(buzzer, 1535, 500);
    // add another 500 milliseconds of silence
    delay(1000);
  }
}

void dispence(char typetodispence, int weighttodispence)
{
  delay(100);
  int tare = getWeightFromScale();
  int offset;
  switch (typetodispence)
  {
  case 'A':
    offset = Type1OffSet;
    break;
  case 'B':
    offset = Type2OffSet;
    break;
  case 'C':
    offset = Type3OffSet;
    break;
  }
  int target = tare + weighttodispence - offset;
  openhopper(typetodispence);
  do
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Dispensing.....");
    int progress = (int)((((float)target - (float)tare) / (float)weighttodispence) * (float)16);
    lcd.setCursor(1, 1);
    for (int i = 0; i < progress; i++)
    {
      lcd.print("#");
    }
    delay(100);
  } while (target > getWeightFromScale());
  closehopper(typetodispence);
}

void dispenceMixture(int weighttodispence)
{
  int smallWeights = weighttodispence / 6;
  for (int i = 0; i < 2; i++)
  {
    dispence('A', smallWeights);
    delay(1000);
    dispence('B', smallWeights);
    delay(1000);
    dispence('C', smallWeights);
    delay(1000);
  }
}

void openhopper(char dispenser)
{
  switch (dispenser)
  {
  case 'A':
    digitalWrite(dispenser_0_Open, HIGH);
    delay(dispenserOpenDelay);
    digitalWrite(dispenser_0_Open, LOW);
    break;

  case 'B':
    digitalWrite(dispenser_1_Open, HIGH);
    delay(dispenserOpenDelay);
    digitalWrite(dispenser_1_Open, LOW);
    break;

  case 'C':
    digitalWrite(dispenser_2_Open, HIGH);
    delay(dispenserOpenDelay);
    digitalWrite(dispenser_2_Open, LOW);
    break;
  }
}
void closehopper(char dispenser)
{
  switch (dispenser)
  {
  case 'A':
    digitalWrite(dispenser_0_Close, HIGH);
    delay(dispenserCloseDelay);
    digitalWrite(dispenser_0_Close, LOW);
    break;

  case 'B':
    digitalWrite(dispenser_1_Close, HIGH);
    delay(dispenserCloseDelay);
    digitalWrite(dispenser_1_Close, LOW);
    break;

  case 'C':
    digitalWrite(dispenser_2_Close, HIGH);
    delay(dispenserCloseDelay);
    digitalWrite(dispenser_2_Close, LOW);
    break;
  }
}

int getWeightFromScale()
{

  static boolean newDataReady = 0;
  const int weightReadDelayInterval = 0; // increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update())
    newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady)
  {
    if (millis() > t + weightReadDelayInterval)
    {
      float i = LoadCell.getData();
      newDataReady = 0;
      t = millis();
      return i;
    }
  }
  /*
  I don't think we'll need this code, it's for taring! Which I implemented to be done automatically in my code.
    // receive command from serial terminal, send 't' to initiate tare operation:
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 't') LoadCell.tareNoDelay();
    }

    // check if last tare operation is complete:
    if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
    }
  */
}

void thankyou()
{

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Thank You!");
  lcd.setCursor(2, 1);
  lcd.print("Next Please");
  delay(2000);
}