// FastLED - Version: Latest 
#include <FastLED.h>


#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <LiquidCrystal.h>

const int OUTPUT_TYPE = SERIAL_PLOTTER;

CRGB leds[10];

const int PIN_INPUT = A0; //Heart reate sensor
const int PIN_BLINK = 13; // Pin 13 is the on-board LED
const int THRESHOLD = 570; // TODO find a good threshold
const int PIN_LED_ECG = 6;
const int NUM_ECG_LED = 10;
const int BLUE[4] = {0,1,2,3}; // LEDs that should be lighting up blue
const int RED[6] = {4,5,6,7,8,9}; // LEDs that should be lighting up red

/*
 * LCD INFO
 * The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

PulseSensorPlayground pulseSensor;

// Setting up the LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(115200);

  // Configure the PulseSensor manager.

  pulseSensor.analogInput(PIN_INPUT);
  pulseSensor.blinkOnPulse(PIN_BLINK);
  
  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Now that everything is ready, start reading the PulseSensor signal.
  pulseSensor.begin();
  
  FastLED.addLeds<NEOPIXEL, PIN_LED_ECG>(leds, NUM_ECG_LED);
  FastLED.show();
  
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("<3  BPM:       ");
}


void animateECG(CRGB color){
    for(int i=0; i < NUM_ECG_LED; i++){
      leds[i] = color; 
        FastLED.show(); 
        delay(20); 
    }
    for(int i=0; i < NUM_ECG_LED; i++){
      leds[i] = CRGB::Black; 
        FastLED.show(); 
        delay(20); 
    }
  }


void loop() {
  delay(20);

  // write the latest sample to Serial.
 pulseSensor.outputSample();

  /*
     If a beat has happened since we last checked,
     write the per-beat information to Serial.
   */
  if (pulseSensor.sawStartOfBeat()) {
   pulseSensor.outputBeat();
   
   int bpm = pulseSensor.getBeatsPerMinute();
   if (bpm < 140){
     
     lcd.setCursor(8,0);
     lcd.print("            ");
     
     lcd.setCursor(10,0);
     lcd.print(bpm);
     
     CRGB color = CRGB::White;
     
     if(60 < bpm && bpm < 100){
       lcd.setCursor(0,1);
       lcd.print("Rested :)        ");
       color = CRGB::Purple;
     } else if(bpm <= 60){
       lcd.setCursor(0,1);
       lcd.print("Wayy relaxed     ");
       color = CRGB::Green;
     } else if (bpm >=100){
       lcd.setCursor(0,1);
       lcd.print("Beating fast     ");
       color = CRGB::Orange;
     }
     
     animateECG(color);
   }
   
  }
}
