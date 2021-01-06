#include <SoftwareSerial.h>
#include "Timer.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN        6
#define NUMPIXELS 64

Timer t;
SoftwareSerial debug(3, 2);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500
char input = '0';
char old = '0';
int mode = 1;

const int FLEX_PIN = A0; // Pin connected to voltage divider output
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 47500.0;

const float STRAIGHT_RESISTANCE = 37300.0; // resistance when straight
const float BEND_RESISTANCE = 90000.0; // resistance at 90 deg

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  debug.begin(115200); // your esp's baud rate might be different
  t.every(500, Wifi_main);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  //  Serial.begin(9600);
  pixels.begin();
  pixels.setBrightness(60);

  pinMode(FLEX_PIN, INPUT);
}

void loop() {
  t.update();
}


int Wifi_State = 2, delaytime;
String cmd;

void Wifi_main() {
  Serial.print(Wifi_State);
  switch (Wifi_State) {
    case 0://--------------------------------------------------Test Wifi Module
      {
        sendDebug("AT+RST");
        sendDebug("AT");
        if (Loding("Sent AT")) {
          Wifi_State++;
        }
        break;
      }

    case 1://--------------------------------------------------Wifi Mode
      {
        sendDebug("AT+CWMODE=3");
        if (Loding("AT+CWMODE=3")) {
          Wifi_State++;
        }
        break;
      }

    case 2://--------------------------------------------------Wifi connect
      {
        cmd = "AT+CWJAP=\"";
        cmd += "Galaxy Note10c85a";
        cmd += "\",\"";
        cmd += "zcnf8107";
        cmd += "\"";
        sendDebug(cmd);
        Wifi_State++;
        if (Loding("Wifi connect")) {
          Wifi_State++; //next setting wifi mode
        }
        else {
          Wifi_State--;
        }
        break;
      }
    case 3://--------------------------------------------------Get ip address 數據機"http://192.168.0.6:8888/"  手機熱點" http://172.20.10.8:8888/pin="  <=== ip
      {
        sendDebug("AT+CIFSR");
        Wifi_State++; 
        break;
      }

    case 4://--------------------------------------------------configure for multiple connections
      {
        sendDebug("AT+CIPMUX=1");
        Wifi_State++; //next setting wifi mode
        break;
      }

    case 5://--------------------------------------------------turn on server on port 8888
      {
        sendDebug("AT+CIPSERVER=1,8888");
        Wifi_State += 2; //next setting wifi mode
        break;
      }

    case 6:// check if the esp is sending a message
      {
        if (debug.available()) {
          if (debug.find("+IPD,")) {
            if (debug.find("mode=")) {
              Serial.println(F("****************Switch mode**************"));
              mode = debug.read() - 48;
              Serial.println(mode);
              pixels.clear();
              pixels.show();
            }
          }
        }
        if (mode == 1) {
          Wifi_State++;
        } else {
          Wifi_State++;
          Wifi_State++;
        }
        break;
      }

    case 7://sensor控制
      {
        Serial.println(F("~ ~ ~ ~ ~ ~ ~ ~sensor~ ~ ~ ~ ~ ~ ~ ~"));

        int flexADC = analogRead(FLEX_PIN);
        float flexV = flexADC * VCC / 1023.0;
        float flexR = R_DIV * (VCC / flexV - 1.0);
        Serial.println("Resistance: " + String(flexR) + " ohms");

        float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,
                          0, 90.0);
        Serial.print(F("-------------Bend: "));
        Serial.print(String(angle));
        Serial.println(F(" degrees-------------"));
        Serial.println();
        if (angle < 160) {
          Serial.print("no face");
          controlLED('0');
        } else {
          Serial.print("smile");
          controlLED('s');
        }
        delay(50);
        Wifi_State--;
        break;
      }

    case 8://web控制
      {
        if (debug.available()) {
          if (debug.find("+IPD,")) {
            Serial.println("Get webpage signal, analyzing...");
            delay(10);
            Serial.println(debug.read());
            debug.find("pin="); // advance cursor to "pin="

            int pinNumber = (debug.read() - 48) * 10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
            pinNumber += (debug.read() - 48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
            
            Serial.println(pinNumber);

            if (pinNumber == 64) {
              pixels.clear();
              pixels.show();
              Wifi_State -= 2;
              break;
            }

             if (pinNumber == 65) {
              controlLED('c');
              break;
            }

             if (pinNumber == 66) {
              controlLED('h');
              break;
            }

            if (pinNumber == 67) {
              controlLED('f');
              break;
            }

            if (pinNumber == 68) {
              controlLED('a');
              break;
            }

            pixels.setPixelColor(pinNumber, pixels.Color(0, 150, 0));
            pixels.show();

            String closeCommand = "AT+CIPCLOSE=";
            closeCommand += pinNumber; // append connection id
            closeCommand += "\r\n";
            Serial.print("Turn Pin");
            Serial.println(pinNumber);
          }
        }

        delay(500);
        break;
      }
  }
}


String get_response() {  //get esp responce without "Serial.find()".
  String response = "";
  char c;
  while (debug.available()) {
    c = debug.read();
    response.concat(c);
    delay(10);
  }
  response.trim();
  return response;
}

boolean Loding(String state) {
  String response = get_response();
  for (int timeout = 0 ; timeout < 30 ; timeout++)
  {
    if (response.indexOf("OK") != -1 || response.indexOf("no change") != -1)
    {
      Serial.print(" OK ! ");
      if (timeout >= 2) {
        Serial.print("This commend waste ");
        Serial.print(timeout / 2);
        Serial.println(" sec.");
      }
      else Serial.println("");
      return 1;
      break;
    }
    else if (timeout == 29) { //after 15 sec for wait esp without responce.
      Serial.print(state);
      Serial.println(" fail...\nExit2");
      return 0;
    }
    else {
      response = get_response(); //reget
      if (timeout == 0)
        Serial.print("Wifi Loading.");
      else
        Serial.print(".");
      delay(500);
    }
  }
}

void sendDebug(String sent_cmd)
{
  Serial.print("SEND: ");
  Serial.println(sent_cmd);
  debug.println(sent_cmd);
}


void controlLED(char input)
{
  pixels.clear();
  old = input;

  if (Serial.available() > 0) {
    input = Serial.read();
    if (input == '0' || input == 'a' || input == 's');
    else
      input = old;
  }

  Serial.println(input);
  switch (input)
  {
    case 's':
      Serial.print(input);
      Serial.println(": smile");

      pixels.clear();
      pixels.setPixelColor(24, pixels.Color(0, 150, 0));
      pixels.setPixelColor(38, pixels.Color(0, 150, 0));
      pixels.setPixelColor(42, pixels.Color(0, 150, 0));
      pixels.setPixelColor(43, pixels.Color(0, 150, 0));
      pixels.setPixelColor(44, pixels.Color(0, 150, 0));
      pixels.setPixelColor(45, pixels.Color(0, 150, 0));
      pixels.setPixelColor(33, pixels.Color(0, 150, 0));
      pixels.setPixelColor(31, pixels.Color(0, 150, 0));
      pixels.show();
      delay(500);

      pixels.setPixelColor(24, pixels.Color(0, 150, 0));
      pixels.setPixelColor(25, pixels.Color(0, 150, 0));
      pixels.setPixelColor(26, pixels.Color(0, 150, 0));
      pixels.setPixelColor(27, pixels.Color(0, 150, 0));
      pixels.setPixelColor(28, pixels.Color(0, 150, 0));
      pixels.setPixelColor(29, pixels.Color(0, 150, 0));
      pixels.setPixelColor(30, pixels.Color(0, 150, 0));
      pixels.setPixelColor(31, pixels.Color(0, 150, 0));

      pixels.setPixelColor(39, pixels.Color(0, 150, 0));
      pixels.setPixelColor(41, pixels.Color(0, 150, 0));
      pixels.setPixelColor(53, pixels.Color(0, 150, 0));
      pixels.setPixelColor(52, pixels.Color(0, 150, 0));
      pixels.setPixelColor(51, pixels.Color(0, 150, 0));
      pixels.setPixelColor(50, pixels.Color(0, 150, 0));
      pixels.setPixelColor(46, pixels.Color(0, 150, 0));
      pixels.setPixelColor(32, pixels.Color(0, 150, 0));

      pixels.setPixelColor(38, pixels.Color(146, 220, 206));
      pixels.setPixelColor(37, pixels.Color(146, 220, 206));
      pixels.setPixelColor(36, pixels.Color(146, 220, 206));
      pixels.setPixelColor(35, pixels.Color(146, 220, 206));
      pixels.setPixelColor(34, pixels.Color(146, 220, 206));
      pixels.setPixelColor(33, pixels.Color(146, 220, 206));
      pixels.setPixelColor(42, pixels.Color(146, 220, 206));
      pixels.setPixelColor(43, pixels.Color(146, 220, 206));
      pixels.setPixelColor(44, pixels.Color(146, 220, 206));
      pixels.setPixelColor(45, pixels.Color(146, 220, 206));
      pixels.show();
      delay(2000);

      break;

    case 'a':
      Serial.print(input);
      Serial.println(": angry");

      pixels.clear();
      pixels.setPixelColor(41, pixels.Color(240, 67, 67));
      pixels.setPixelColor(37, pixels.Color(240, 67, 67));
      pixels.setPixelColor(27, pixels.Color(240, 67, 67));
      pixels.setPixelColor(19, pixels.Color(240, 67, 67));
      pixels.setPixelColor(29, pixels.Color(240, 67, 67));
      pixels.setPixelColor(33, pixels.Color(240, 67, 67));
      pixels.setPixelColor(47, pixels.Color(240, 67, 67));

      pixels.show();
      delay(500);
      //      pixels.show();
      break;

    case '0':
      pixels.setPixelColor(25, pixels.Color(246, 201, 134));
      pixels.setPixelColor(26, pixels.Color(246, 201, 134));
      pixels.setPixelColor(27, pixels.Color(255, 188, 188));
      pixels.setPixelColor(28, pixels.Color(255, 188, 188));
      pixels.setPixelColor(29, pixels.Color(188, 255, 255));
      pixels.setPixelColor(30, pixels.Color(188, 255, 255));
      pixels.setPixelColor(38, pixels.Color(246, 201, 134));
      pixels.setPixelColor(37, pixels.Color(246, 201, 134));
      pixels.setPixelColor(36, pixels.Color(255, 188, 188));
      pixels.setPixelColor(35, pixels.Color(255, 188, 188));
      pixels.setPixelColor(34, pixels.Color(188, 255, 255));
      pixels.setPixelColor(33, pixels.Color(188, 255, 255));
      pixels.show();
      break;

    case 'h':
      pixels.setPixelColor(9, pixels.Color(245, 193, 193));
      pixels.setPixelColor(10, pixels.Color(245, 193, 193));
      pixels.setPixelColor(20, pixels.Color(245, 193, 193));
      pixels.setPixelColor(19, pixels.Color(245, 193, 193));
      pixels.setPixelColor(13, pixels.Color(245, 193, 193));
      pixels.setPixelColor(14, pixels.Color(245, 193, 193));
      pixels.setPixelColor(16, pixels.Color(245, 193, 193));
      pixels.setPixelColor(31, pixels.Color(245, 193, 193));
      pixels.setPixelColor(33, pixels.Color(245, 193, 193));
      pixels.setPixelColor(51, pixels.Color(245, 193, 193));
      pixels.setPixelColor(52, pixels.Color(245, 193, 193));
      pixels.setPixelColor(42, pixels.Color(245, 193, 193));
      pixels.setPixelColor(38, pixels.Color(245, 193, 193));
      pixels.setPixelColor(24, pixels.Color(245, 193, 193));
      pixels.setPixelColor(33, pixels.Color(245, 193, 193));
      pixels.show();
      break;

      
    case 'c':
      pixels.setPixelColor(22, pixels.Color(189, 96, 218));
      pixels.setPixelColor(25, pixels.Color(189, 96, 218));
      pixels.setPixelColor(10, pixels.Color(189, 96, 218));
      pixels.setPixelColor(11, pixels.Color(189, 96, 218));
      pixels.setPixelColor(12, pixels.Color(189, 96, 218));
      pixels.setPixelColor(18, pixels.Color(189, 96, 218));
      pixels.setPixelColor(29, pixels.Color(189, 96, 218));
      pixels.setPixelColor(35, pixels.Color(189, 96, 218));
      pixels.setPixelColor(36, pixels.Color(189, 96, 218));
      pixels.setPixelColor(43, pixels.Color(189, 96, 218));
      pixels.setPixelColor(59, pixels.Color(189, 96, 218));
      pixels.show();
      break;


    case 'f':
      pixels.setPixelColor(5, pixels.Color(81, 161, 153));
      pixels.setPixelColor(4, pixels.Color(81, 161, 153));
      pixels.setPixelColor(3, pixels.Color(81, 161, 153));
      pixels.setPixelColor(10, pixels.Color(81, 161, 153));
      pixels.setPixelColor(12, pixels.Color(81, 161, 153));
      pixels.setPixelColor(21, pixels.Color(81, 161, 153));
      pixels.setPixelColor(19, pixels.Color(81, 161, 153));
      pixels.setPixelColor(26, pixels.Color(81, 161, 153));
      pixels.setPixelColor(39, pixels.Color(81, 161, 153));
      pixels.setPixelColor(38, pixels.Color(81, 161, 153));
      pixels.setPixelColor(37, pixels.Color(81, 161, 153));
      pixels.setPixelColor(35, pixels.Color(81, 161, 153));
      pixels.setPixelColor(34, pixels.Color(81, 161, 153));
      pixels.setPixelColor(33, pixels.Color(81, 161, 153));
      pixels.setPixelColor(40, pixels.Color(81, 161, 153));
      pixels.setPixelColor(42, pixels.Color(81, 161, 153));
      pixels.setPixelColor(44, pixels.Color(81, 161, 153));
      pixels.setPixelColor(46, pixels.Color(81, 161, 153));
      pixels.setPixelColor(55, pixels.Color(81, 161, 153));
      pixels.setPixelColor(53, pixels.Color(81, 161, 153));
      pixels.setPixelColor(51, pixels.Color(81, 161, 153));
      pixels.setPixelColor(49, pixels.Color(81, 161, 153));
      pixels.setPixelColor(56, pixels.Color(81, 161, 153));
      pixels.setPixelColor(57, pixels.Color(81, 161, 153));
      pixels.setPixelColor(58, pixels.Color(81, 161, 153));
      pixels.setPixelColor(59, pixels.Color(81, 161, 153));
      pixels.setPixelColor(60, pixels.Color(81, 161, 153));
      pixels.setPixelColor(61, pixels.Color(81, 161, 153));
      pixels.setPixelColor(62, pixels.Color(81, 161, 153));
      pixels.show();
      break;

    default:
      break;
  }
}
