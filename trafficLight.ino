/* Traffic Light Implementation
 *  This code operates a small traffic light/timer 
 *  with an Arduino Metro Mini, some I2C 7-segment
 *  displays, and various switches/LEDs.
 *  
 *  This project is licensed under the terms of the MIT license.
 */

/* ToDo:
 *  fix small display (hw) (parts ordered)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment bigDisplay = Adafruit_7segment();
Adafruit_7segment smallDisplay = Adafruit_7segment();

// LEDs
const int redLed = 13;
const int ylwLed = 12;
const int grnLed = 11;

// buttons and switches
const int button1 = 2;
const int button2 = 3;
const int modeSw = 4;

//other variables
int counter = 0;
volatile int manState = 0;
int dbM = 300;  //debounce time, ms

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup");
  bigDisplay.begin(0x70);
  //Serial.println("Big Display complete");
  smallDisplay.begin(0x71);

  // pin settings
  //Serial.println("Pin Settings");
  pinMode(redLed, OUTPUT);
  pinMode(ylwLed, OUTPUT);
  pinMode(grnLed, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(modeSw, INPUT_PULLUP);
  
  //clear the displays
  clearDisplays();
  Serial.println("Setup complete");
}

void loop() {
  // manual mode
  while (digitalRead(modeSw) == HIGH) {
    manualMode();
  }
  // timer mode
  while (digitalRead(modeSw) == LOW) {
    timerMode();
  }
}

void manualMode() {
  // manual mode setup
  Serial.println("Entering Manual Mode");
  attachInterrupt(digitalPinToInterrupt(button1), manualCounter, FALLING);
  
  clearDisplays();
  counter = 0;
  
  // manual mode loop
  while (true) { 
      switch(counter % 3) {
        case 0:
        // red light
          ledCtrl(LOW, LOW, HIGH);
        break;
        case 1:
        // green light
          ledCtrl(HIGH, LOW, LOW);
        break;
        case 2:
        // yellow light
          ledCtrl(LOW, HIGH, LOW);
        break;
        default:
          //something broke
          Serial.println("Manual Mode broke.");
        break;
      }
      if (digitalRead(modeSw) == LOW) {
        break;
      }
  }
}

void timerMode() {
  // setup for timer mode
  Serial.println("Entering Timer Mode");
  int minutes = 0;
  int seconds = 0;
  float maxSeconds = 0;
  int go = false;
  detachInterrupt(digitalPinToInterrupt(button1));
  counter = minutes*100 + seconds;

  // zero both displays
  writeBothDisplays(counter, true);

  // turn red LED on
  ledCtrl(LOW, LOW, HIGH);

  // timer mode loop
  while (true) {
    // set timer
    if (digitalRead(button2) == LOW) {
      minutes++;
      counter = minutes*100 + seconds;
      writeBothDisplays(counter, true);
      delay(dbM);
    }
    //start timer
    if (digitalRead(button1) == LOW) {
      go = true;
      maxSeconds = minutes*60;
      delay(dbM);
    }
    if (go) {
      writeBothDisplays(counter, true);
      if (counter > 0) {
        // control the traffic lights
        Serial.println(counter);
        // timer was > 1 minute and only 1 minute remains
        if (maxSeconds > 60 && minutes == 0) {
          ledCtrl(LOW, HIGH, LOW);
        }
        // timer was <= 1 minute and 10 seconds remain
        else if (maxSeconds<=60 && minutes==0 && seconds<=10) {
          ledCtrl(LOW, HIGH, LOW);
        }
        else {
          ledCtrl(HIGH, LOW, LOW);
        }
        // handle minutes/seconds conversion
        if (seconds == 0) {
          minutes--;
          seconds = 59;
        }
        else {
          seconds--;
        }
      }
      else {
        go = false;
        clearDisplays();
        ledCtrl(LOW, LOW, HIGH);
      }
      counter = minutes*100 + seconds;
      delay(1000);
    }
    if (digitalRead(modeSw) == HIGH) {
      break;
    }
  }
}

void clearDisplays() {
  writeBothDisplays(10000, false);
  ledCtrl(LOW, LOW, HIGH);
}

void writeBothDisplays(int number, int colon) {
  bigDisplay.print(number, DEC);
  smallDisplay.print(number, DEC);
  // zero padding, since the displays do not pad
  if (number < 1000) {
    smallDisplay.writeDigitNum(0,0);
    bigDisplay.writeDigitNum(0,0);
    if (number < 100) {
      smallDisplay.writeDigitNum(1,0);
      bigDisplay.writeDigitNum(1,0);
      if (number < 10) {
        smallDisplay.writeDigitNum(3,0);
        bigDisplay.writeDigitNum(3,0);
      }
    }
  }
  smallDisplay.drawColon(colon);
  bigDisplay.drawColon(colon);
  smallDisplay.writeDisplay();
  bigDisplay.writeDisplay();
}

void ledCtrl(boolean green, boolean yellow, boolean red) {
  Serial.print("G:");Serial.print(green);
  Serial.print(" Y:");Serial.print(yellow);
  Serial.print(" R:");Serial.println(red);
  digitalWrite(grnLed, green);
  digitalWrite(ylwLed, yellow);
  digitalWrite(redLed, red);
}

void manualCounter() {
  Serial.println("Button 1 Interrupt");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > dbM) {
    counter++;
  }
  last_interrupt_time = interrupt_time;
}

