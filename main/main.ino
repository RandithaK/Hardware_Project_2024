// Including the libraries

// Library for LCD over I2C Protocol
// Reference page https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
#include <LiquidCrystal_I2C.h>
// Library to use I2C protocol
#include <Wire.h>

// Library for KeyPad
#include <Keypad.h>

// Pin definitions
// Pin definitions should be added here and any references to the pins should be done through these definitions and should never be made directly to the pin address unless absolutely necessary.

// Dispensing Control Pins
#define dispenser_0_Open A0
#define dispenser_0_Close A1
#define dispenser_1_Open A7
#define dispenser_1_Close A6
#define dispenser_2_Open A4
#define dispenser_2_Close A5
#define dispenserOpenDelay 500  // in ms
#define dispenserCloseDelay 500 // in ms

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
#define keypad_row1 22
#define keypad_row2 24
#define keypad_row3 26
#define keypad_row4 28

#define keypad_col1 30
#define keypad_col2 32
#define keypad_col3 34
#define keypad_col4 36

// Library Code for Keypad

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
/*
Default Key layout
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'#', '0', '*', 'D'}};
*/

// Custom Layout
char keys[ROWS][COLS] = {
    {'D', '*', '0', '#'},
    {'C', '9', '8', '7'},
    {'B', '6', '5', '4'},
    {'A', '3', '2', '1'}};
byte rowPins[ROWS] = {keypad_row1, keypad_row2, keypad_row3, keypad_row4}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {keypad_col1, keypad_col2, keypad_col3, keypad_col4}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// End of Library Code for Keypad

// Buzzer

#define buzzer 13

#define LCDColumns 16
#define LCDRows 2
LiquidCrystal_I2C lcd(0x27, LCDColumns, LCDRows); // Setting up the LCD at address 0x27
#define TextScrollSpeed 300
#define TextScrollRest 400
// Sealing Unit
#define dirPin 2
#define stepPin 3
#define sealerSteps 240 // Change this value as need to adjest the movement of the sealer
#define sealersetupsteps 60

// Offset values for Grams
#define Type1OffSet 0
#define Type2OffSet 0
#define Type3OffSet 0

#define MaxTimetoDispence 20000
#define shakeInterval 500

// Custom Functions Prototypes
// Greeting Message
void greetText();
// Initialises the Hoppers
void starthoppers();
// Initialise Sealer
void resetsealer();
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
// To seal the bag using the stepper motor
void seal();
// Ending the program gracefully
void thankyou();
// Reset Function
void (*resetFunc)(void) = 0; // declare reset function at address 0
// Handelling Errors Gracefully
void systemerror();

// Main Entry Point
void setup()
{
  // Code to run once:

  // Setup Dispensers as output pins
  pinMode(dispenser_0_Open, OUTPUT);
  pinMode(dispenser_0_Close, OUTPUT);
  pinMode(dispenser_1_Open, OUTPUT);
  pinMode(dispenser_1_Close, OUTPUT);
  pinMode(dispenser_2_Open, OUTPUT);
  pinMode(dispenser_2_Close, OUTPUT);

  // Setup Buzzer as output
  pinMode(buzzer, OUTPUT);

  // Setup Stepper Motor Pins as output pins
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  lcd.init(); // initialize the lcd
  lcd.backlight();

  lcd.setCursor(0, 0);
  greetText();

  delay(500);
  //  beeper(1);

  //  delay(10);
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
    starthoppers();
    resetsealer();
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("System Ready");
  }
}

// Main Loop
void loop()
{
  // put your main code here, to run repeatedly:
  char typetodispence;
  int weighttodispence = getdetails(&typetodispence);
  if (typetodispence == 'D')
  {
    dispenceMixture(weighttodispence);
  }
  else
  {
    dispence(typetodispence, weighttodispence);
  }
  seal();
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
    // Stop the buzzer
    noTone(buzzer);
    digitalWrite(buzzer, HIGH);
  }
}

void dispence(char typetodispence, int weighttodispence)
{
  delay(100);
  unsigned long StartTime = millis();
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
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Dispensing.....");
  int shaketime = millis();
  do
  {
    int progress = (int)(((float)weighttodispence / ((float)target - (float)tare)) * (float)16);
    lcd.setCursor(1, 1);
    for (int i = 0; i < progress; i++)
    {
      lcd.print("#");
    }
    delay(100);
    {
      if ((millis() - StartTime) >= MaxTimetoDispence)
      {
        systemerror();
      }
    }
    {
      if ((millis() - shaketime) > shakeInterval)
      {
        openhopper(typetodispence);
        closehopper(typetodispence);
        shaketime = millis();
      }
    }
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
  const int weightReadDelayInterval = 0; // increase value to slow down return activity

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
}

void seal()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sealing");
  lcd.setCursor(0, 2);
  lcd.print("############");

  digitalWrite(dirPin, LOW);

  // Spin motor slowly
  for (int x = 0; x < sealerSteps; x++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
    delay(2);
  }
  delay(1000); // Wait a second

  // Set motor direction counterclockwise (reverse)
  digitalWrite(dirPin, HIGH);

  // Spin motor quickly
  for (int x = 0; x < sealerSteps; x++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
    delay(1);
  }
  resetsealer();
}

void thankyou()
{

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Hardware Project");
  lcd.setCursor(2, 1);
  lcd.print("IT Group 12");

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Thank You!");
  lcd.setCursor(2, 1);
  lcd.print("Next Please");
  delay(2000);
}
void starthoppers()
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Starting Hoppers");

  digitalWrite(dispenser_0_Close, HIGH);
  delay(dispenserCloseDelay);
  digitalWrite(dispenser_0_Close, LOW);

  digitalWrite(dispenser_1_Close, HIGH);
  delay(dispenserCloseDelay);
  digitalWrite(dispenser_1_Close, LOW);

  digitalWrite(dispenser_2_Close, HIGH);
  delay(dispenserCloseDelay);
  digitalWrite(dispenser_2_Close, LOW);
}

void resetsealer()
{

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Initialising");
  lcd.setCursor(2, 0);
  lcd.print("Sealer...");
  digitalWrite(dirPin, HIGH);
  for (int x = 0; x < sealersetupsteps; x++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
    delay(1);
  }
}

void greetText()
{
  String text = "Spicy Bites";
  int text_Len = text.length();
  //  int j = 0;
  for (int i = 0; i < (LCDColumns - text_Len + 1); i++)
  {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(text);
    delay(TextScrollSpeed);
  }

  delay(TextScrollRest);

  for (int i = (LCDColumns - text_Len); i > -1; i--)
  {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(text);
    delay(TextScrollSpeed);
  }

  delay(TextScrollRest);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  delay(TextScrollSpeed);

  delay(TextScrollRest);
}

void systemerror()
{

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("System Error");
  lcd.setCursor(0, 1);
  lcd.print("Check Hoppers");
  delay(10000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Check Hoppers");
  lcd.setCursor(0, 1);
  lcd.print("Rebooting...");
  delay(2000);
  resetFunc();
}