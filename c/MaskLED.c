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
        pixels.setPixelColor(24, pixels.Color(0, 150, 0));
        pixels.setPixelColor(38, pixels.Color(0, 150, 0));
        pixels.setPixelColor(42, pixels.Color(0, 150, 0));
        pixels.setPixelColor(43, pixels.Color(0, 150, 0));
        pixels.setPixelColor(44, pixels.Color(0, 150, 0));
        pixels.setPixelColor(45, pixels.Color(0, 150, 0));
        pixels.setPixelColor(33, pixels.Color(0, 150, 0));
        pixels.setPixelColor(31, pixels.Color(0, 150, 0));
        pixels.show();
        break;

    case 'a':
        Serial.print(input);
        Serial.println(": angry");
        pixels.setPixelColor(41, pixels.Color(0, 150, 0));
        pixels.setPixelColor(37, pixels.Color(0, 150, 0));
        pixels.setPixelColor(27, pixels.Color(0, 150, 0));
        pixels.setPixelColor(19, pixels.Color(0, 150, 0));
        pixels.setPixelColor(29, pixels.Color(0, 150, 0));
        pixels.setPixelColor(33, pixels.Color(0, 150, 0));
        pixels.setPixelColor(47, pixels.Color(0, 150, 0));
        pixels.show();
        break;

    default:
        Serial.println(": default");
        pixels.setPixelColor(22, pixels.Color(0, 150, 0));
        pixels.setPixelColor(10, pixels.Color(0, 150, 0));
        pixels.setPixelColor(11, pixels.Color(0, 150, 0));
        pixels.setPixelColor(12, pixels.Color(0, 150, 0));
        pixels.setPixelColor(18, pixels.Color(0, 150, 0));
        pixels.setPixelColor(29, pixels.Color(0, 150, 0));
        pixels.setPixelColor(35, pixels.Color(0, 150, 0));
        pixels.setPixelColor(36, pixels.Color(0, 150, 0));
        pixels.setPixelColor(45, pixels.Color(0, 150, 0));
        pixels.setPixelColor(50, pixels.Color(0, 150, 0));
        pixels.setPixelColor(60, pixels.Color(0, 150, 0));
        pixels.setPixelColor(59, pixels.Color(0, 150, 0));
        pixels.setPixelColor(58, pixels.Color(0, 150, 0));
        pixels.setPixelColor(54, pixels.Color(0, 150, 0));
        pixels.show();
        break;
    }




    /*   for (int i = 0; i < NUMPIXELS; i++) {

           pixels.setPixelColor(i, pixels.Color(0, 150, 0));
           pixels.show();
           delay(DELAYVAL);
       }*/
}
