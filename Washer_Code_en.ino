/*********Bosch Serie 8 toy washing machine Mod**********
  by @Giacgiac21, @Mickmick2012, @Wishy Washer
  Original idea by @Mickmick2012 and @Wishy Washer
  @Mickmick2012 (a.k.a Michelangelo Chaume): https://www.youtube.com/channel/UCeqR3VP9sD_Yg3V-uurZH5w
  @Wishy Washer: https://www.youtube.com/channel/UCUahB2ZOtE1biDp3gDAiZVA
  @Giacgiac21: https://www.youtube.com/channel/UC6KFaBeQQt-3KtYDCugNKiA
  0.5.4 Beta Build
  /*in1/2/enA = motor 1*/
// Rotary encoder declarations
#include <EEPROM.h>
char welcome_msg[] = "Welcome, sample!";  // the welcome text after the logo (and error) put whatever you want here
static int welcome_text_size = 1;           //  change this if it is too big or too small
void (*resetFunc)(void) = 0;                //declare reset function at address 0
int spin_speed = EEPROM.read(3);
unsigned long start_delay = 0;              //
static int pinA = 2;                        // Our first hardware interrupt pin is digital pin 2
static int pinB = 3;                        // Our second hardware interrupt pin is digital pin 3
volatile byte aFlag = 0;                    // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0;                    // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = EEPROM.read(0);  //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0;                //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0;                  //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent
int led = 9;                                //red led pin
int spk = 11;                               //speaker pin
// Button reading, including debounce without delay function declarations
const byte buttonPin = 4;               // this is the Arduino pin we are connecting the push button to
byte oldButtonState = HIGH;             // assume switch open because of pull-up resistor
const unsigned long debounceTime = 10;  // milliseconds
unsigned long buttonPressTime;          // when the switch last changed state
boolean buttonPressed = 0;              // a flag variable
int dots = 0;
// Menu and submenu/setting declarations
byte Mode = 0;            // This is which menu mode we are in at any given time (top level or one of the submenus)
const byte modeMax = 19;  // This is the number of submenus/settings you want
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 ecranOLED(128, 32, &Wire, -1);
#include "pitches.h"
int enA = 5;  //Power for the motor
int in1 = 6;  //Direction 1
int in2 = 7;  //Direction 2
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
#define largeurDeLimage 128  // Largeur de l'image � afficher, en pixels
#define hauteurDeLimage 32   // Hauteur de l'image � afficher, en pixels
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
// 'path12691', 128x32px
const unsigned char epd_bitmap_path12691[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x06,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xf0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x08,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0xc2,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0xc2,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x82,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0c,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'path12811', 128x32px
const unsigned char epd_bitmap_path12811[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x38,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// 'baseline_dark_mode_black_18dp', 128x32px
const unsigned char epd_bitmap_baseline_dark_mode_black_18dp[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 528)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
  epd_bitmap_baseline_dark_mode_black_18dp
};


void setup() {
  //Rotary encoder section of setup
  pinMode(pinA, INPUT_PULLUP);  // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP);  // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  attachInterrupt(0, PinA, RISING);  // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1, PinB, RISING);  // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  // button section of setup
  pinMode(buttonPin, INPUT_PULLUP);  // setup the button pin
  // DEBUGGING section of setup
  Serial.begin(9600);  // DEBUGGING: opens serial port, sets data rate to 9600 bps
  ecranOLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (EEPROM.read(5) == 0) {
    start_delay = 0;
  } else if (EEPROM.read(5) == 1) {
    start_delay = 3600000;
  } else if (EEPROM.read(5) == 2) {
    start_delay = 7200000;
  } else if (EEPROM.read(5) == 3) {
    start_delay = 10800000;
  } else if (EEPROM.read(5) == 4) {
    start_delay = 14400000;
  } else if (EEPROM.read(5) == 5) {
    start_delay = 1800000;
  }
  Serial.println(start_delay);
  Serial.println(EEPROM.read(3));
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(led, OUTPUT);
  analogWrite(led, 0);
  analogWrite(led, 0);
  ecranOLED.clearDisplay();
  ecranOLED.drawBitmap(
    (ecranOLED.width() - largeurDeLimage) / 2,   // Position de l'extr�me "gauche" de l'image (pour centrage �cran, ici)
    (ecranOLED.height() - hauteurDeLimage) / 2,  // Position de l'extr�me "haute" de l'image (pour centrage �cran, ici)
    logo,
    largeurDeLimage,
    hauteurDeLimage,
    WHITE);
  ecranOLED.display();
  if (EEPROM.read(6) == 0) {
    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000 / jingleDurations[thisNote];
      tone(spk, start_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(spk);
    }
  }
  if (EEPROM.read(6) == 0) {
    for (int i = 0; i <= 255; i++) {
      analogWrite(led, 0 + i);
      delay(5);
    }
  } else {
    analogWrite(led, 0);
  }
  delay(1500);
  if (EEPROM.read(1) == 1) {
    ecranOLED.clearDisplay();
    Serial.println("E4");
    if (EEPROM.read(6) == 0) {
      for (int i = 0; i <= 5; i++) {
        for (int thisNote = 0; thisNote < 3; thisNote++) {
          int noteDuration = 1000 / jingleDurations[thisNote];
          tone(spk, error_jingle[thisNote], noteDuration);
          int pauseBetweenNotes = noteDuration * 1.30;
          delay(pauseBetweenNotes);
          noTone(spk);
        }
        ecranOLED.setTextSize(1);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("E4 - The program stopped because of an power outage");
        ecranOLED.display();
        delay(500);
        ecranOLED.clearDisplay();
        ecranOLED.display();
      }
    }
  }
  EEPROM.write(1, 0);
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(welcome_text_size);
  ecranOLED.setTextColor(WHITE);
  ecranOLED.setCursor(0, 0);
  ecranOLED.println(welcome_msg);
  ecranOLED.display();
  delay(3000);
}


void loop() {
  rotaryMenu();
  if (encoderPos == 0) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Select your program");
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
    ecranOLED.println("Mixed Loading");
    ecranOLED.display();
  } else if (encoderPos == 6) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Wool");
    ecranOLED.display();
  } else if (encoderPos == 7) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Empty Drum");
    ecranOLED.display();
  } else if (encoderPos == 8) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Night Mode");
    ecranOLED.display();
  } else if (encoderPos == 9) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Spin Speed de l'Spin");
    ecranOLED.display();
  } else if (encoderPos == 10) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Select your program");
    ecranOLED.display();
  } else if (encoderPos == 11) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Spin");
    ecranOLED.display();
  } else if (encoderPos == 12) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Super Fast 5/6min");
    ecranOLED.display();
  } else if (encoderPos == 13) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Sportswear");
    ecranOLED.display();
  } else if (encoderPos == 14) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Allergy +");
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
    ecranOLED.println("Drum Cleaning");
    ecranOLED.display();
  } else if (encoderPos >= 18) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Select your program");
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
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        wash();
        dots = 3;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 4;
        refresh_steps();
        test();
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
        dots = 1;
        refresh_steps();
        wash();
        dots = 2;
        refresh_steps();
        interim_spin();
        final_spin();
        final_spin();
        final_spin_slow_down();
        dots = 3;
        refresh_steps();
        test();
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
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        wash();
        dots = 3;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 4;
        refresh_steps();
        test();
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
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        wash();
        dots = 3;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 4;
        refresh_steps();
        test();
        End();
      }
      if (Mode == 5) {
        Serial.println("Mixed Loading");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Mixed Loading");
        ecranOLED.display();
        start();
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        wash();
        dots = 3;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 4;
        refresh_steps();
        test();
        End();
      }
      if (Mode == 6) {
        Serial.println("Wool");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Wool");
        ecranOLED.display();
        start();
        dots = 1;
        ecranOLED.drawBitmap(
          (ecranOLED.width() - largeurDeLimage) / 2,   // Position de l'extr�me "gauche" de l'image (pour centrage �cran, ici)
          (ecranOLED.height() - hauteurDeLimage) / 2,  // Position de l'extr�me "haute" de l'image (pour centrage �cran, ici)
          epd_bitmap_path12811,
          largeurDeLimage,
          hauteurDeLimage,
          WHITE);
        refresh_steps();
        wool_wash();
        wool_wash();
        wool_wash();
        End();
      }
      if (Mode == 7) {
        Serial.println("Empty Drum");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Empty Drum");
        ecranOLED.display();
        start();
        End();
      }
      if (Mode == 8) {
        if (encoderPos == 8) {
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(2);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Night Mode: Off");
          ecranOLED.display();
          EEPROM.write(6, 0);
          delay(5000);
        }
        if (encoderPos == 9) {
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(2);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Night Mode: On");
          ecranOLED.display();
          EEPROM.write(6, 1);
          delay(5000);
        }
        resetFunc();
      }
      if (Mode == 9) {
        if (encoderPos == 9) {
          EEPROM.write(3, 230);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Spin Speed: 800");
          ecranOLED.display();
          Serial.println("Spin Speed: 800");
          delay(5000);
        }
        if (encoderPos == 10) {
          EEPROM.write(3, 240);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Spin Speed: 1000");
          ecranOLED.display();
          Serial.println("Spin Speed: 1000");
          delay(5000);
        }
        if (encoderPos == 11) {
          EEPROM.write(3, 255);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Spin Speed: 1200");
          ecranOLED.display();
          Serial.println("Spin Speed: 1200");
          delay(5000);
        }
        resetFunc();  //call reset
      }
      if (Mode == 10) {
        if (encoderPos == 10) {
          EEPROM.write(5, 0);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Spin Speed: 0h");
          ecranOLED.display();
          Serial.println("Delay: 0h");
          delay(5000);
        }
        if (encoderPos == 11) {
          EEPROM.write(5, 1);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Delay: 1h");
          ecranOLED.display();
          Serial.println("Delay: 1h");
          delay(5000);
        }
        if (encoderPos == 12) {
          EEPROM.write(5, 2);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Delay: 2h");
          ecranOLED.display();
          Serial.println("Delay: 2h");
          delay(5000);
        }
        if (encoderPos == 13) {
          EEPROM.write(5, 3);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Delay: 3h");
          ecranOLED.display();
          Serial.println("Delay: 3h");
          delay(5000);
        }
        if (encoderPos == 14) {
          EEPROM.write(5, 4);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Delay: 4h");
          ecranOLED.display();
          Serial.println("Delay: 4h");
          delay(5000);
        }
        if (encoderPos == 15) {
          EEPROM.write(5, 5);
          ecranOLED.clearDisplay();
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(WHITE);
          ecranOLED.setCursor(0, 0);
          ecranOLED.println("Delay: 30m");
          ecranOLED.display();
          Serial.println("Delay: 30m");
          delay(5000);
        }
        resetFunc();  //call reset
      }
      if (Mode == 11) {
        Serial.println("Spin");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Spin");
        ecranOLED.display();
        start();
        dots = 1;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 2;
        refresh_steps();
        test();
        End();
      }
      if (Mode == 12) {
        Serial.println("Super Fast 5/6min");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Super Fast 5/6min");
        ecranOLED.display();
        start();
        dots = 1;
        refresh_steps();
        wash();
        dots = 2;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 3;
        refresh_steps();
        test();
        End();
      }
      if (Mode == 13) {
        Serial.println("Sportswear");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Sportswear");
        ecranOLED.display();
        start();
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        wash();
        dots = 3;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 4;
        refresh_steps();
        test();
        End();
      }
      if (Mode == 14) {
        Serial.println("AllergiePlus");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Allergy +");
        ecranOLED.display();
        start();
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        wash();
        dots = 3;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 4;
        refresh_steps();
        test();
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
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        wash();
        dots = 3;
        refresh_steps();
        interim_spin();
        final_spin_speed_up();
        final_spin();
        final_spin_slow_down();
        dots = 4;
        refresh_steps();
        test();
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
        dots = 1;
        refresh_steps();
        regular_wash();
        dots = 2;
        refresh_steps();
        uni_toss_wash();
        dots = 3;
        refresh_steps();
        bi_toss_wash();
        dots = 4;
        refresh_steps();
        wool_wash();
        dots = 5;
        refresh_steps();
        uni_toss_wash();
        dots = 6;
        refresh_steps();
        bi_toss_wash();
        dots = 7;
        refresh_steps();
        End();
      }
      if (Mode == 17) {
        Serial.println("Drum Cleaning");  //DEBUGGING: print which mode has been selected
        ecranOLED.clearDisplay();
        ecranOLED.setTextSize(2);
        ecranOLED.setTextColor(WHITE);
        ecranOLED.setCursor(0, 0);
        ecranOLED.println("Drum Cleaning");
        ecranOLED.display();
        start();
        dots = 1;
        refresh_steps();
        wash();
        dots = 2;
        refresh_steps();
        wool_wash();
        dots = 3;
        refresh_steps();
        bi_toss_wash();
        dots = 4;
        refresh_steps();
        uni_toss_wash();
        End();
      }
      if (Mode == 18) {
      }
    }
  }
}
// Carry out common activities each time a setting is changed
void start() {
  if (EEPROM.read(6) == 0) {
    for (int thisNote = 0; thisNote < 2; thisNote++) {
      int noteDuration = 1000 / jingleDurations[thisNote];
      tone(spk, play_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 0.8;
      delay(pauseBetweenNotes);
      noTone(spk);
    }
  } else {
    ecranOLED.drawBitmap(
      (ecranOLED.width() - largeurDeLimage) / 2,   // Position de l'extr�me "gauche" de l'image (pour centrage �cran, ici)
      (ecranOLED.height() - hauteurDeLimage) / 2,  // Position de l'extr�me "haute" de l'image (pour centrage �cran, ici)
      epd_bitmap_baseline_dark_mode_black_18dp,
      largeurDeLimage,
      hauteurDeLimage,
      WHITE);
  }
  EEPROM.write(1, 1);
  delay(1000);
  delay(start_delay);
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
    analogWrite(enA, 40);
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
void regular_wash() {
  // 1 minute
  for (int i = 0; i <= 1; i++) {
    analogWrite(enA, 45);
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
}
void wool_wash() {
  for (int i = 0; i <= 40; i++) {
    analogWrite(enA, 35);
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
void interim_spin() {
  ecranOLED.drawBitmap(
    (ecranOLED.width() - largeurDeLimage) / 2,   // Position de l'extr�me "gauche" de l'image (pour centrage �cran, ici)
    (ecranOLED.height() - hauteurDeLimage) / 2,  // Position de l'extr�me "haute" de l'image (pour centrage �cran, ici)
    epd_bitmap_path12691,
    largeurDeLimage,
    hauteurDeLimage,
    WHITE);
  ecranOLED.display();
  for (int i = 10; i <= 230; i++) {
    analogWrite(enA, i);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(100);
  }
  delay(19900);
  for (int i = 1; i <= 230; i++) {
    analogWrite(enA, 255 - i);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(100);
  }
}

void final_spin_speed_up() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  for (int i = 80; i <= 180; i++) {
    analogWrite(enA, i);
    delay(100);
  }
  analogWrite(enA, 181);
  delay(59000);
}
void final_spin() {
  for (int i = 181; i <= EEPROM.read(3); i++) {
    analogWrite(enA, i);
    delay(150);
  }
}
void final_spin_slow_down() {
  for (int i = 0; i <= EEPROM.read(3); i++) {
    analogWrite(enA, EEPROM.read(3) - i);
    delay(120);
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}
void End() {
  EEPROM.write(1, 0);
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(4);
  ecranOLED.setTextColor(WHITE);
  ecranOLED.setCursor(0, 0);
  ecranOLED.println("End");
  ecranOLED.display();
  if (EEPROM.read(6) == 0) {
    for (int thisNote = 0; thisNote < 19; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(spk, end_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration;
      delay(pauseBetweenNotes);
      noTone(spk);
    }
  }
  delay(7500);
  ecranOLED.clearDisplay();
  ecranOLED.display();
  if (EEPROM.read(6) == 0) {
    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000 / jingleDurations[thisNote];
      tone(spk, off_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(spk);
    }
  }
  for (int i = 0; i <= 255; i++) {
    analogWrite(led, 255 - i);
    delay(10);
  }
  delay(1600);
  resetFunc();  //call reset
}
void unknown_error() {
  Serial.println("EU");
  ecranOLED.clearDisplay();
  for (int i = 0; i <= 7; i++) {
    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000 / jingleDurations[thisNote];
      tone(spk, error_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(spk);
    }
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("Unknown error, please reset the rotary encodeur (prg selector)");
    ecranOLED.display();
    delay(500);
    ecranOLED.clearDisplay();
    ecranOLED.display();
  }
  resetFunc();  //call reset
}
void refresh_steps() {
  if (0 == 1) {
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 15);
    if (dots == 1) {
      ecranOLED.println(".");
    } else if (dots == 2) {
      ecranOLED.println(" .");
    } else if (dots == 3) {
      ecranOLED.println("  .");
    } else if (dots == 4) {
      ecranOLED.println("   .");
    } else if (dots == 5) {
      ecranOLED.println("    .");
    } else if (dots == 6) {
      ecranOLED.println("     .");
    } else if (dots == 7) {
      ecranOLED.println("      .");
    }
    ecranOLED.display();
  }
}
void eeprom_full() {
  Serial.println("E8");
  ecranOLED.clearDisplay();
  for (int i = 0; i <= 7; i++) {
    for (int thisNote = 0; thisNote < 3; thisNote++) {
      int noteDuration = 1000 / jingleDurations[thisNote];
      tone(spk, error_jingle[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(spk);
    }
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(WHITE);
    ecranOLED.setCursor(0, 0);
    ecranOLED.println("E8 - The EEPROM (Save System) is full");
    ecranOLED.display();
    delay(500);
    ecranOLED.clearDisplay();
    ecranOLED.display();
  }
  resetFunc();  //call reset
}
void test() {
  for (int i = 0; i <= 10; i++) {
    analogWrite(enA, 40);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(3000);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    delay(1000);
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
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
/*........................................................................................................................................................................................................
........................................................................................................................................................................................................
.........................................................''''''''..............................'''''''''...............................'''''''..........................................................
.....',,,',,',,'''',,,',,,,,'''.....................'',,,,,,,,,,,,,,''....................',,,',,,,,,,,,',,,''...................'',,,,,,,,,,,,,,'''...........'''',,,,,,.............',,,,'',,,........
.....',,,,,,,,,,,,,,,,,,,,,,,,,,''...............',,,,,,,,,,,,,,,,,,,,,''..............''',,,,,,,,,,,,,,,,,,,,,''.............'',,,,,,,,,,,,,,,,,,,,,'.........',,,,,,,,,.............',,,,,,,,,........
.....',,,,,,,,,,,,,,,,,',,,,,,,,,,'............',,,,,,,,,,'',,,',,,,,,,,,''...........'',,,,,,,,,,,,,,,,,,,,,,,,''..........'',,,,,,,,,',,,,,'',,,,,,,,'.......'',,,,,,,,.............',,,,,,,,,........
.....',,,,,,,,''''''''''',,,,,,,,,,'..........',,,,,,,,,,''''''',,,,,,,,,,,'.........',,,,,,,,''........'',,,,''...........',,,,,,,,,,,''''''',,,,,,,,'''......',,,,,,,,,.............',,,,,,,,,........
.....',',,,,,,'...........',,,,,,,,,.........'',,,,,,,,'..........',,,,,,,,,'.......',,,,,,,,'.............'''............'',,,,,,,,''..........',,,,'.........'',,,,,,,,.............',,,,,,,,,........
.....',,,,,,,,'...........'',,,,,,,,........',,,,,,,,'..............',,,,,,,,'......',,,,,,,,'...........................',,,,,,,,''.............''............',,,,,,,,,.............'',,,,,,,,........
.....',,,,,,,,'...........',,,,,,,,'.......',,,,,,,,'................',,,,,,,,'.....',',,,,,,,'..........................,,,,,,,,,'............................',,,,,,,,,.............'',,,,,,,,........
.....',,,,,,,''..........',,,,,,,,'........,,,,,,,,,.................',,,,,,','......',,,,,,,,,,'''.....................',,,,,,,,'.............................',,,,,,,,,'''''''''''''',,,,,,,,,........
.....',,,,,,,,,,,,,',,,,,,,,,,,''..........,,,,,,,,'..................,,,,,,,,,.......',,,,,,,,,,,,,,,,,''''............',,,,,,,,..............................',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,........
.....',,,,,,,,,,,,,,,,,,,,,,,,,'...........',,,,,,,'..................',,,,',,,.........'',',,,,,,,,,,,,,,,,,,'.........,,,,,,,,,..............................',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,........
.....',,,,,,,,,,,,,,,,,,,,,,,,,,''.........,',,,,,,'..................,,,,,,,,,............'',,',,,,,,,,,,,,,,,''.......',,,,,,,,..............................',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,........
.....',,,,,,',''........'',,,,,,,,,'.......,,,,,,,,,..................,,,,,,,,,..................''''',,,,,,,,,,,,'.....',,,,,,,,'.............................',,,,,,,,,'''''''''''''',,,,,,,,,........
.....',,,,,,,''............',,,,,,,,'......',,,,,,,''................',,,,,'','.........................',,,,,,,,,'.....',,,,,,,,'.............................',,,,,,,,,.............',,,,,,,,,........
.....',,,,,,,,..............,,,,,,,,''......',,,,,,,'...............',,,,,,',,...........................',,,,,,,,,......',,,,,,,,'............................',,,,,,,,,.............',,,,,,,,,........
.....',,,,,,,,.............',,,,,,,,,'......',,,,,,,,,'............',,,,,,,'''.........'''...............'',,',,,''.......,,,,,,,,,''............',''..........',,,,,,,,,.............',,,,,,,,,........
.....',,,,,,,''...........'',,,,,,,','.......'',,,,,,,,,'.......'',,,,,,,,,,'........',,,''''...........'''',,,,,,'........',,,,,,,,,'''......'',,,,,,''.......',,,,,,,,,.............',,,,,,,,,........
.....',,,,,,,,,',,,,,,,,,,,,',,,,,,,'.........'',,,,,,,,,,',,,,,,,,,,,,,,,''.......',,,,,,,,,,,'''''''',,,,,,,,,,'..........',,,,,,,,,,,,,,,,',,,,,,,,,,'......',,,,,,,,,.............',,,,,,,,,........
.....',,,,,,,',,,,,,,,,,,,,,,,,,,,''............',,,,,,,,,,,,,,,,,,,,,,,,'..........',,,,,,,,,,,,,,,''',,,,,,,,,'.............',,,,,,,,,,,,,,,,,,,,,,,''.......',,,,,,,,,.............'',,,,,,,,........
.....',,,,,,'',,,,,,,,',,,,,,,,''.................'',,,,,,,,,,,,,,,,,,''..............'',,,,,,,,,,,,,,,,,,,,,''.................'',,,,,,,,,,,,,,,,,,''.........',,,,,,,,,.............',,,,,,,,,........
.....''''''''''''''''''''''''.........................'',,,,,,,,,,''......................''',,,,,,,,,,,'''........................''',,,,,,,,,'''..............'''''''''..............'''''''''........
........................................................................................................................................................................................................*/