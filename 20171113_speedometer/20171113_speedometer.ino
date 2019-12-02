/*
   PINS IN USE:
   SDA: A4
   SCL: A5
   Press button: 10
   MEDER MK5-1C90C REED-sensor: 2
*/

#include <DS1307RTC.h>                // clock
#include <Time.h>                     // required by DS1307RTC.h
#include <TimeLib.h>                  // contains definition for tmElements_t
#include <Wire.h>                     // I2C protocol communication on pins A4 and A5
#include <LiquidCrystal.h>            // LCD
#include <MsTimer2.h>                 // periodic tasks

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // LCD pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int menuCounterButton = 10;
int menuCounter = 1;

const float wheelSize = 2000; // wheel size in millimeters
volatile unsigned long instantNow = 0, prevInstant = 0;
volatile float trip = 0;
float instSpeed = 0, avgSpeed = 0;

tmElements_t tm, tm0;
int secondTime = 0, minuteTime = 0, hourTime = 0;

int buttonState = 0, lastButtonState = 0, startPressed = 0, endPressed = 0, timeHold = 0, timeReleased = 0;     // current state of the button
int function(int pin);

// functions
void pulse();
void updateAllValues();
void print2digits(int number);
void printFullLine(int c1, int r1, String valueName, int c2, int r2, float value, int decimalPlaces, String unit);
int longPress(int buttonPin);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(menuCounterButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), pulse, RISING);
  MsTimer2::set(1000, updateAllValues);
  MsTimer2::start();
  RTC.read(tm0);
}

void loop() {
  printFullLine(0, 0, "SPEED:", 6, 0, instSpeed, 1, " km/h");

  int x = longPress(menuCounterButton);

  if (x == 1) {
//    delay(200);                       // button debouncing
    menuCounter++;
    lcd.clear();
    if (menuCounter == 6)
      menuCounter = 1;
  }
  
  else if (x == 2) {
    trip = 0;
    RTC.read(tm0);
    avgSpeed = 0;
  }

  switch (menuCounter) {
    case 1:
      printFullLine(0, 1, "TRIP:", 6, 1, trip, 2, " km");
      break;

    case 2:
      lcd.setCursor(0, 1);
      lcd.print("TIME:");

      if (RTC.read(tm)) {

        secondTime = tm.Second - tm0.Second;
        if (secondTime < 0) {
          secondTime += 60;
          tm.Minute--;
        }

        minuteTime = tm.Minute - tm0.Minute;
        if (minuteTime < 0) {
          minuteTime += 60;
          tm.Hour--;
        }

        hourTime = tm.Hour - tm0.Hour;
        if (hourTime < 0)
          hourTime += 24;

        lcd.setCursor(7, 1);
        print2digits(hourTime);
        lcd.write(':');
        print2digits(minuteTime);
        lcd.write(':');
        print2digits(secondTime);
      }
      break;

    case 3:
      printFullLine(0, 1, "AVG:", 6, 1, avgSpeed, 1, " km/h");
      break;

    case 4:
      lcd.setCursor(0, 1);
      lcd.print("TOTAL:");
      break;

    case 5:
      lcd.setCursor(0, 1);
      lcd.print("CLOCK:");

      if (RTC.read(tm)) {
        lcd.setCursor(7, 1);
        print2digits(tm.Hour);
        lcd.write(':');
        print2digits(tm.Minute);
        lcd.write(':');
        print2digits(tm.Second);
      }
      break;
  }
}

// pulse: function called by the systeminterrupt
//        stores the duration since arduino turned on in milliseconds at which the sensor reads a signal

void pulse() {
  prevInstant = instantNow;
  instantNow = millis();
  trip += wheelSize / 1000000;
}

//updateAllValues: called at regular interval by MsTimer2
//                 calculates the instantaneous values (instantaneous speed, average...) based on the two last instant values.

void updateAllValues() {

  // Instantaneous speed
  if (millis() - instantNow > 3000)                               // speed is null if no signal for 3 seconds
    instSpeed = 0;
  else if (instantNow > prevInstant && prevInstant > 0)
    instSpeed = wheelSize / (instantNow - prevInstant) * 3.60;

  // Average speed
  avgSpeed = trip / (millis() / (1000.00 * 3600.00));
}

void print2digits(int number) {        // converts 14:5:3 to 14:05:03
  if (number >= 0 && number < 10)
    lcd.write('0');
  lcd.print(number);
}

void printFullLine(int c1, int r1, String valueName, int c2, int r2, float value, int decimalPlaces, String unit) {
  lcd.setCursor(c1, r1);
  lcd.print(valueName);
  lcd.setCursor(c2, r2);
  if (value >= 0 && value < 100) {
    lcd.write(' ');
    if (value >= 0 && value < 10)
      lcd.write(' ');
  }
  lcd.print(value, decimalPlaces);
  lcd.print(unit);
}

int longPress(int pin) {
  int x = 0;
  buttonState = digitalRead(pin);

  if (buttonState != lastButtonState) {

    if (buttonState == HIGH) {
      startPressed = millis();
      timeReleased = startPressed - endPressed;
    }
    else {
      endPressed = millis();
      timeHold = endPressed - startPressed;

      if (timeHold >= 0 && timeHold < 2000) {
        x = 1;
      }

      if (timeHold >= 2000) {
        x = 2;
      }
    }
  }
  lastButtonState = buttonState;
  return x;
}
