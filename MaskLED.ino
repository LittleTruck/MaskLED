#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN        6
#define NUMPIXELS 64

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500
char input = '0';

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    Serial.begin(9600);
    pixels.begin();
    pixels.setBrightness(60);
}

void loop() {
    pixels.clear();

    if (Serial.available()) {
        input = Serial.read();
    }

    switch (input)
    {
    case 's':
        Serial.print(input);
        Serial.println(": smile");
        pixels.setPixelColor(0, pixels.Color(0, 150, 0));
        pixels.show();
        break;
        
    case 'a':
        Serial.print(input);
        Serial.println(": angry");
        pixels.setPixelColor(1, pixels.Color(0, 150, 0));
        pixels.show();
        break;

    default:
        break;
    }

    


 /*   for (int i = 0; i < NUMPIXELS; i++) {

        pixels.setPixelColor(i, pixels.Color(0, 150, 0));
        pixels.show();
        delay(DELAYVAL);
    }*/
}
