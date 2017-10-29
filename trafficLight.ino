/* Traffic Light Implementation
 *  This code operates a small traffic light/timer 
 *  with an Arduino Metro Mini, some I2C 7-segment
 *  displays, and various switches/LEDs.
 *  
 *  This project is licensed under the terms of the MIT license.
 */

/* ToDo:
 *  change yellow light timing
 *  fix small display (hw)
 *  while loops & breaking
 *  change order so termination ends this loop not next
 *  implement non-hacky debounce
 *  turn red LED on when entering timer mode
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment bigDisplay = Adafruit_7segment();
Adafruit_7segment smallDisplay = Adafruit_7segment();

const int redLed = 13;
const int ylwLed = 12;
const int grnLed = 11;
const int button1 = 2;
const int button2 = 3;
const int modeSw = 4;
int counter = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup");
  bigDisplay.begin(0x70);
  Serial.println("Big Display complete");
  smallDisplay.begin(0x71);

  // pin settings
  Serial.println("Pin Settings");
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
  clearDisplays();
  counter = 0;
  
  // manual mode loop
  while (true) { 
    if (digitalRead(button1) == LOW) {
        counter++;
        // lazy debounce, there are others
        delay(500);
      }
      switch(counter % 3) {
        case 0:
        // red light
          redLedOn();
        break;
        case 1:
        // green light
          greenLedOn();
        break;
        case 2:
        // yellow light
          yellowLedOn();
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
  LedsOff();
  counter = minutes*100 + seconds;

  // zero both displays
  writeBothDisplays(counter, true);

  //timer mode loop
  while (true) {
    // set timer
    if (digitalRead(button2) == LOW) {
      minutes++;
      counter = minutes*100 + seconds;
      writeBothDisplays(counter, true);
      delay(500);
    }
    //start timer
    if (digitalRead(button1) == LOW) {
      go = true;
      maxSeconds = minutes*60;
      delay(500);
    }
    if (go) {
      if (counter > 0) {
        // control the traffic lights
        Serial.println(counter);
        // 10% of time remaining
        if ((float)(minutes*60+seconds)/maxSeconds <= 0.1) {
          yellowLedOn();
        }
        else {
          greenLedOn();
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
        redLedOn();
      }
      counter = minutes*100 + seconds;
      writeBothDisplays(counter, true);
      delay(1000);
    }
    if (digitalRead(modeSw) == HIGH) {
      break;
    }
  }
}

void clearDisplays() {
  writeBothDisplays(10000, false);
  LedsOff();
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

void greenLedOn() {
  Serial.println("Green light.");
  digitalWrite(grnLed, HIGH);
  digitalWrite(ylwLed, LOW);
  digitalWrite(redLed, LOW);
}

void yellowLedOn() {
  Serial.println("Yellow light.");
  digitalWrite(grnLed, LOW);
  digitalWrite(ylwLed, HIGH);
  digitalWrite(redLed, LOW);
}

void redLedOn() {
  Serial.println("Red light.");
  digitalWrite(grnLed, LOW);
  digitalWrite(ylwLed, LOW);
  digitalWrite(redLed, HIGH);
}

void LedsOff() {
  Serial.println("Lights out.");
  digitalWrite(grnLed, LOW);
  digitalWrite(ylwLed, LOW);
  digitalWrite(redLed, LOW);
}

