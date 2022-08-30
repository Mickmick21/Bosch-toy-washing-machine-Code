/*********Bosch Serie 8 toy washing machine Mod**********
  by @Giacgiac21, @Mickmick2012, @Wishy Washer
  Original idea by @Mickmick2012 and @Wishy Washer
  @Mickmick2012: https://www.youtube.com/channel/UCeqR3VP9sD_Yg3V-uurZH5w
  @Wishy Washer: https://www.youtube.com/channel/UCUahB2ZOtE1biDp3gDAiZVA
  @Giacgiac21: https://www.youtube.com/channel/UC6KFaBeQQt-3KtYDCugNKiA
  0.1.0
  /*in1/2/enA = motor 1 ,in3/4/enB = pump*/
// Rotary encoder declarations
#include <EEPROM.h>
void (*resetFunc)(void) = 0;  //declare reset function at address 0
int spin_counter = 0;
int spin_speed = 1200;
static int pinA = 2;                        // Our first hardware interrupt pin is digital pin 2
static int pinB = 3;                        // Our second hardware interrupt pin is digital pin 3
volatile byte aFlag = 0;                    // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0;                    // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = EEPROM.read(0);  //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0;                //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0;                  //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent
// Button reading, including debounce without delay function declarations
const byte buttonPin = 4;               // this is the Arduino pin we are connecting the push button to
byte oldButtonState = HIGH;             // assume switch open because of pull-up resistor
const unsigned long debounceTime = 10;  // milliseconds
unsigned long buttonPressTime;          // when the switch last changed state
boolean buttonPressed = 0;              // a flag variable
// Menu and submenu/setting declarations
byte Mode = 0;            // This is which menu mode we are in at any given time (top level or one of the submenus)
const byte modeMax = 19;  // This is the number of submenus/settings you want
byte setting1 = 0;        // a variable which holds the value we set
byte setting2 = 0;        // a variable which holds the value we set
byte setting3 = 0;        // a variable which holds the value we set
/* Note: you may wish to change settingN etc to int, float or boolean to suit your application.
  Remember to change "void setAdmin(byte name,*BYTE* setting)" to match and probably add some
  "modeMax"-type overflow code in the "if(Mode == N && buttonPressed)" section*/
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 ecranOLED(128, 32, &Wire, -1);
// Motor A connections
int enA = 10;
int in1 = 9;
int in2 = 8;
int enB = 5;
int in3 = 7;
int in4 = 6;
#include "pitches.h"
int start_jingle[] = {
  NOTE_D6, NOTE_A5, NOTE_A6
};
int play_jingle[] = {
  NOTE_D6, NOTE_A6
};
int end_jingle[] = {
  NOTE_D6, NOTE_A5, NOTE_E6, NOTE_A6, 0, NOTE_E6, NOTE_D6, NOTE_A5, NOTE_A5, NOTE_E6, NOTE_A6
};
int off_jingle[] = {
  NOTE_E6, NOTE_A6, NOTE_A5
};
int error_jingle[] = {
  NOTE_A5, NOTE_A5
};
int noteDurations[] = {
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};
int jingleDurations[] = {
  4, 4, 4, 4, 4
};                           //integer assignment for how long to play each tone
#define largeurDeLimage 128  // Largeur de l'image à afficher, en pixels
#define hauteurDeLimage 32   // Hauteur de l'image à afficher, en pixels
const unsigned char logo[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xf0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x90, 0x04, 0xe0, 0x00, 0x0f, 0xf8, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x3f, 0x01, 0xc0, 0x50,
  0x07, 0x30, 0x0e, 0x70, 0x00, 0x0f, 0xfe, 0x01, 0xff, 0x80, 0xff, 0x80, 0xff, 0xc3, 0xc0, 0xf8,
  0x06, 0x70, 0x0f, 0x30, 0x00, 0x0f, 0xff, 0x03, 0xff, 0xc1, 0xff, 0xc1, 0xff, 0xe3, 0xc0, 0xf8,
  0x0c, 0xf0, 0x0f, 0xb8, 0x00, 0x0f, 0xff, 0x87, 0xff, 0xc3, 0xf7, 0xc1, 0xff, 0xe3, 0xc0, 0xf0,
  0x0c, 0xf0, 0x0f, 0x98, 0x00, 0x0f, 0x0f, 0x87, 0xc3, 0xe3, 0xc1, 0x83, 0xe1, 0xc3, 0xc0, 0xf0,
  0x19, 0xff, 0xfd, 0x98, 0x00, 0x0f, 0x07, 0x8f, 0x81, 0xe3, 0xc0, 0x03, 0xc0, 0x83, 0xc0, 0xf0,
  0x19, 0xbf, 0xfd, 0xc8, 0x00, 0x0f, 0x07, 0x8f, 0x01, 0xe3, 0xe0, 0x07, 0xc0, 0x03, 0xe0, 0xf0,
  0x19, 0xb0, 0x0c, 0xcc, 0x00, 0x0f, 0x9f, 0x0f, 0x00, 0xf3, 0xfc, 0x07, 0x80, 0x03, 0xff, 0xf0,
  0x19, 0xb0, 0x0c, 0xcc, 0x00, 0x0f, 0xfe, 0x0f, 0x00, 0xf1, 0xff, 0x87, 0x80, 0x03, 0xff, 0xf0,
  0x1b, 0xb0, 0x0c, 0xcc, 0x00, 0x0f, 0xff, 0x0f, 0x00, 0xf0, 0xff, 0xc7, 0x80, 0x03, 0xff, 0xf0,
  0x19, 0xb0, 0x0c, 0xcc, 0x00, 0x0f, 0xff, 0x8f, 0x00, 0xf0, 0x1f, 0xe7, 0x80, 0x03, 0xff, 0xf0,
  0x19, 0xbf, 0xfc, 0xcc, 0x00, 0x0f, 0x07, 0xcf, 0x01, 0xf0, 0x03, 0xe7, 0x80, 0x03, 0xc0, 0xf0,
  0x19, 0xbf, 0xfd, 0x88, 0x00, 0x0f, 0x07, 0xcf, 0x81, 0xe0, 0x01, 0xe3, 0xc0, 0x03, 0xc0, 0xf0,
  0x1d, 0xf0, 0x0d, 0x98, 0x00, 0x0f, 0x07, 0xc7, 0x83, 0xe1, 0x81, 0xe3, 0xe0, 0xc3, 0xc0, 0xf0,
  0x0c, 0xf0, 0x0f, 0x98, 0x00, 0x0f, 0x0f, 0xc7, 0xff, 0xc3, 0xc3, 0xe3, 0xff, 0xe3, 0xc0, 0xf0,
  0x0e, 0x70, 0x0f, 0x30, 0x00, 0x0f, 0xff, 0x83, 0xff, 0xc3, 0xff, 0xc1, 0xff, 0xe3, 0xc0, 0xf0,
  0x06, 0x30, 0x0e, 0x70, 0x00, 0x0f, 0xff, 0x01, 0xff, 0x81, 0xff, 0x80, 0xff, 0xc3, 0xc0, 0xf0,
  0x03, 0x10, 0x0c, 0x60, 0x00, 0x0f, 0xfe, 0x00, 0xfe, 0x00, 0xff, 0x00, 0x3f, 0x83, 0xc0, 0xf0,
  0x01, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xe0, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x7c, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {
  //Rotary encoder section of setup
  pinMode(pinA, INPUT_PULLUP);       // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP);       // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  attachInterrupt(0, PinA, RISING);  // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1, PinB, RISING);  // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  // button section of setup
  pinMode(buttonPin, INPUT_PULLUP);  // setup the button pin
  // DEBUGGING section of setup
  Serial.begin(9600);  // DEBUGGING: opens serial port, sets data rate to 9600 bps
  ecranOLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  ecranOLED.clearDisplay();
  ecranOLED.drawBitmap(
    (ecranOLED.width() - largeurDeLimage) / 2,   // Position de l'extrême "gauche" de l'image (pour centrage écran, ici)
    (ecranOLED.height() - hauteurDeLimage) / 2,  // Position de l'extrême "haute" de l'image (pour centrage écran, ici)
    logo,
    largeurDeLimage,
    hauteurDeLimage,
    WHITE);
  ecranOLED.display();
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    int noteDuration = 1000 / jingleDurations[thisNote];
    tone(11, start_jingle[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(11);
  }
  delay(2500);
  if (EEPROM.read(1) == 1) {
    ecranOLED.clearDisplay();
    Serial.println("E4");
    for (int i = 0; i <= 5; i++) {
      for (int thisNote = 0; thisNote < 3; thisNote++) {
        int noteDuration = 1000 / jingleDurations[thisNote];
        tone(11, error_jingle[thisNote], noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(11);
      }
      ecranOLED.setTextSize(1);
      ecranOLED.setTextColor(WHITE);
      ecranOLED.setCursor(0, 0);
      ecranOLED.println("E4 - Le programme a été interrompu par arret de courant");
      ecranOLED.display();
      delay(500);
      ecranOLED.clearDisplay();
      ecranOLED.display();
    }
    EEPROM.write(1, 0);
  }
}


void loop() {
  rotaryMenu();
  if (encoderPos == 0) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Selectionnez votre programme");
    ecranOLED.display();
  } else if (encoderPos == 1) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Cotton");
    ecranOLED.display();
  } else if (encoderPos == 2) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Cotton ECO");
    ecranOLED.display();
  } else if (encoderPos == 3) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Easycare");
    ecranOLED.display();
  } else if (encoderPos == 4) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Schnell/Mix");
    ecranOLED.display();
  } else if (encoderPos == 5) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Chargement mixe");
    ecranOLED.display();
  } else if (encoderPos == 6) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Laine");
    ecranOLED.display();
  } else if (encoderPos == 7) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Vider tambour");
    ecranOLED.display();
  } else if (encoderPos == 8) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Selectionnez votre programme");
    ecranOLED.display();
  } else if (encoderPos == 9) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Selectionnez votre programme");
    ecranOLED.display();
  } else if (encoderPos == 10) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Selectionnez votre programme");
    ecranOLED.display();
  } else if (encoderPos == 11) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Essorage");
    ecranOLED.display();
  } else if (encoderPos == 12) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Super Rapide 5/6min");
    ecranOLED.display();
  } else if (encoderPos == 13) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Lavage Sportif");
    ecranOLED.display();
  } else if (encoderPos == 14) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Allergie +");
    ecranOLED.display();
  } else if (encoderPos == 15) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Easycare +");
    ecranOLED.display();
  } else if (encoderPos == 16) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Soft");
    ecranOLED.display();
  } else if (encoderPos == 17) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Nettoyage tambour");
    ecranOLED.display();
  } else if (encoderPos >= 18) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Selectionnez votre programme");
    ecranOLED.display();
  } else if (encoderPos >= 19) {
    unknown_error();
  }
}



void rotaryMenu() {  //This handles the bulk of the menu functions without needing to install/include/compile a menu library
  //DEBUGGING: Rotary encoder update display if turned
  if (oldEncPos != encoderPos) {  // DEBUGGING
    Serial.println(encoderPos);   // DEBUGGING. Sometimes the serial monitor may show a value just outside modeMax due to this function. The menu shouldn't be affected.
    oldEncPos = encoderPos;       // DEBUGGING
  }                               // DEBUGGING
  // Button reading with non-delay() debounce - thank you Nick Gammon!
  byte buttonState = digitalRead(buttonPin);
  if (buttonState != oldButtonState) {
    if (millis() - buttonPressTime >= debounceTime) {  // debounce
      buttonPressTime = millis();                      // when we closed the switch
      oldButtonState = buttonState;                    // remember for next time
      if (buttonState == LOW) {
        Serial.println("Button closed");  // DEBUGGING: print that button has been closed
        buttonPressed = 1;
      } else {
        Serial.println("Button opened");  // DEBUGGING: print that button has been opened
        buttonPressed = 0;
      }
    }  // end if debounce time up
  }    // end of state change

  //Main menu section
  if (Mode == 0) {
    if (encoderPos > (modeMax + 10)) encoderPos = modeMax;  // check we haven't gone out of bounds below 0 and correct if we have
    else if (encoderPos > modeMax)
      encoderPos = 0;  // check we haven't gone out of bounds above modeMax and correct if we have
    if (buttonPressed) {
      Mode = encoderPos;                // set the Mode to the current value of input if button has been pressed
      Serial.print("Mode selected: ");  //DEBUGGING: print which mode has been selected
      Serial.println(Mode);             //DEBUGGING: print which mode has been selected
      buttonPressed = 0;                // reset the button status so one press results in one action
      if (Mode == 1) {
        Serial.println("Cottons");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Cotton");
        ecranOLED.display();
        start();
        wash();
        drain();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 2) {
        Serial.println("Cotton ECO");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Cotton ECO");
        ecranOLED.display();
        start();
        wash();
        drain();
        interim_spin();
        final_spin();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 3) {
        Serial.println("Easy Care");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Easy Care");
        ecranOLED.display();
        start();
        wash();
        drain();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 4) {
        Serial.println("Schnell/Mix");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Schnell/Mix");
        ecranOLED.display();
        start();
        wash();
        drain();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 5) {
        Serial.println("Chargement mixe");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Chargement mixe");
        ecranOLED.display();
        start();
        wash();
        drain();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 6) {
        Serial.println("Laine");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Laine");
        ecranOLED.display();
        start();
        wool_wash();
        wool_wash();
        wool_wash();
        wool_drain();
        End();
      }
      if (Mode == 7) {
        Serial.println("Vider Tambour");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Vider Tambour");
        ecranOLED.display();
        start();
        initial_drain();
        drain();
        End();
      }
      if (Mode == 11) {
        Serial.println("Essorage");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Essorage");
        ecranOLED.display();
        start();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 12) {
        Serial.println("Super Rapide 5/6min");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Super Rapide 5/6min");
        ecranOLED.display();
        start();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 13) {
        Serial.println("Sportswear");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Lavage Sportif");
        ecranOLED.display();
        start();
        wash();
        drain();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 14) {
        Serial.println("AllergiePlus");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Allergie +");
        ecranOLED.display();
        start();
        wash();
        drain();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 15) {
        Serial.println("Easycare +");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Easycare +");
        ecranOLED.display();
        start();
        wash();
        drain();
        wash();
        drain();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        End();
      }
      if (Mode == 16) {
        Serial.println("Soft");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Soft");
        ecranOLED.display();
        start();
        uni_toss_wash();
        bi_toss_wash();
        wool_wash();
        wool_drain();
        uni_toss_wash();
        bi_toss_wash();
        End();
      }
      if (Mode == 17) {
        Serial.println("Netoyage Tambour");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Netoyage Tambour");
        ecranOLED.display();
        start();
        wash();
        wool_wash();
        bi_toss_wash();
        uni_toss_wash();
        drain();
        End();
      }
    }
  }
}
// Carry out common activities each time a setting is changed
void start() {
  for (int thisNote = 0; thisNote < 2; thisNote++) {
    int noteDuration = 1000 / jingleDurations[thisNote];
    tone(11, play_jingle[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 0.8;
    delay(pauseBetweenNotes);
    noTone(11);
  }
  EEPROM.write(1, 1);
  delay(1000);
}
void uni_toss_wash() {
  for (int i = 0; i <= 12; i++) {
    analogWrite(enA, 40);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(350);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    delay(50);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(150);
  }
  delay(4000);
}
void wash() {
  for (int i = 0; i <= 3; i++) {
    analogWrite(enA, 40;
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(10000);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(5000);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    delay(10000);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(5000);
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}
void wool_wash() {
  for (int i = 0; i <= 40; i++) {
    analogWrite(enA, 35;
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(300);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(300);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    delay(300);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(300);
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}
void initial_drain() {
  digitalWrite(in3, HIGH);
  delay(5000);
  digitalWrite(in4, LOW);
  delay(2000);
}
void drain() {
  analogWrite(enB, 30);  //ENB pin
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(10000);
  analogWrite(enA, 35);  //ENB pin
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(2000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(3000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(2000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(3000);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(2000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(3000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(2000);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(3000);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
void bi_toss_wash() {
  for (int i = 0; i <= 11; i++) {
    analogWrite(enA, 40);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(450);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(100);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    delay(420);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(100);
  }
  delay(4000);
}
void wool_drain() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(10000);
  analogWrite(10, 35);  //ENB pin
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(300);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(300);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(300);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(300);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(300);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(300);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(300);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(17900);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
void interim_spin() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  for (int i = 10; i <= 230; i++) {
    analogWrite(10, i);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(100);
  }
  delay(19900);
  for (int i = 1; i <= 255; i++) {
    analogWrite(10, 255 - i);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(100);
  }
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void final_spin_speed_up() {
  int spin = 1200;
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  for (int i = 80; i <= 180; i++) {
    analogWrite(10, i);
    delay(100);
  }
  analogWrite(10, 181);
  delay(59000);
}
void final_spin() {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  if (true) {
    for (int i = 181; i <= 255; i++) {
      analogWrite(10, i);
      delay(150);
    }

  } else if (spin_counter == 0) {
    for (int i = 182; i <= 210; i++) {
      analogWrite(10, i);
      delay(150);
    }
  } else if (spin_counter == 1) {
    for (int i = 182; i <= 220; i++) {
      analogWrite(10, i);
      delay(150);
    }
  }
}
void final_spin_slow_down() {
  if (true) {
    for (int i = 0; i <= 255; i++) {
      analogWrite(10, 255 - i);
      delay(120);
    }
  } else if (spin_counter == 0) {
    for (int i = 0; i <= 210; i++) {
      analogWrite(10, 255 - i);
      delay(120);
    }
  } else if (spin_counter == 1) {
    for (int i = 0; i <= 220; i++) {
      analogWrite(10, 255 - i);
      delay(120);
    }
  }
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
void End() {
  EEPROM.write(1, 0);
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(4);
  ecranOLED.setTextColor(WHITE);
  ecranOLED.setCursor(0, 0);
  ecranOLED.println("Fin");
  ecranOLED.display();
  for (int thisNote = 0; thisNote < 19; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(11, end_jingle[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration;
    delay(pauseBetweenNotes);
    noTone(11);
  }
  delay(7500);
  ecranOLED.clearDisplay();
  ecranOLED.display();
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    int noteDuration = 1000 / jingleDurations[thisNote];
    tone(11, off_jingle[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(11);
  }
  delay(3500);
  resetFunc();  //call reset
}
void not_finished() {
  Serial.println("E3");
  ecranOLED.clearDisplay();
  for (int i = 0; i <= 5; i++) {
    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000 / jingleDurations[thisNote];
      tone(11, error_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(11);
    }
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("E3 - Le programme n'est pas termine de developpement");
    ecranOLED.display();
    delay(500);
    ecranOLED.clearDisplay();
    ecranOLED.display();
  }
  resetFunc();  //call reset
}
void unknown_error() {
  Serial.println("EU");
  ecranOLED.clearDisplay();
  for (int i = 0; i <= 7; i++) {
    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000 / jingleDurations[thisNote];
      tone(11, error_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(11);
    }
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Erreur inconnu, veuillez reinitialiser l'encodeur rotatif (selecteur de programe)");
    ecranOLED.display();
    delay(500);
    ecranOLED.clearDisplay();
    ecranOLED.display();
  }
  resetFunc();  //call reset
}
//Rotary encoder interrupt service routine for one encoder pin
void PinA() {
  cli();                                //stop interrupts happening before we read pin values
  reading = PIND & 0xC;                 // read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && aFlag) {  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos--;                       //decrement the encoder's position count
    bFlag = 0;                          //reset flags for the next turn
    aFlag = 0;                          //reset flags for the next turn
    EEPROM.write(0, encoderPos);
  } else if (reading == B00000100)
    bFlag = 1;  //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei();        //restart interrupts
}

//Rotary encoder interrupt service routine for the other encoder pin
void PinB() {
  cli();                                //stop interrupts happening before we read pin values
  reading = PIND & 0xC;                 //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) {  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos++;                       //increment the encoder's position count
    bFlag = 0;                          //reset flags for the next turn
    aFlag = 0;                          //reset flags for the next turn
    EEPROM.write(0, encoderPos);
  } else if (reading == B00001000)
    aFlag = 1;  //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei();        //restart interrupts
}
// end of sketch!
