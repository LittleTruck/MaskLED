#include <SoftwareSerial.h>
#include "Timer.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN        6
#define NUMPIXELS 64

Timer t;
SoftwareSerial debug(3, 2);  // RX, TX // make RX Arduino line is pin 4, make TX Arduino line is pin 5.
#define SSID "Galaxy Note10c85a"
#define PASS "zcnf8107"

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500
char input = '0';
char old = '0';


void setup(){
  Serial.begin(115200);
  while (!Serial) {;}
  debug.begin(115200); // your esp's baud rate might be different
//  pinMode(10,OUTPUT);
//  digitalWrite(10,LOW);
//  pinMode(11,OUTPUT);
//  digitalWrite(11,LOW);
//  pinMode(12,OUTPUT);
//  digitalWrite(12,LOW);
  t.every(500, Wifi_main);
  
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
  #endif
      Serial.begin(9600);
      pixels.begin();
      pixels.setBrightness(60);
}


void loop(){
  t.update();
}
int Wifi_State,delaytime;
String cmd;
void Wifi_main(){
  switch (Wifi_State) {
    case 0://--------------------------------------------------Test Wifi Module
      sendDebug("AT+RST");
      sendDebug("AT");
      if (Loding("Sent AT")){ 
        Wifi_State++;
      }
    break;
    case 1://--------------------------------------------------Wifi Mode
      sendDebug("AT+CWMODE=3");
      if (Loding("AT+CWMODE=3")){ 
        Wifi_State++;
      }
    break;
    case 2://--------------------------------------------------Wifi connect
      cmd="AT+CWJAP=\"";
      cmd+=SSID;
      cmd+="\",\"";
      cmd+=PASS;
      cmd+="\"";
      sendDebug(cmd);
      Wifi_State++;
      if (Loding("Wifi connect")){ 
        Wifi_State++; //next setting wifi mode
      }
      else{Wifi_State--;}
    break;
    case 3://--------------------------------------------------Get ip address 數據機"http://192.168.0.6:8888/"  手機熱點" http://172.20.10.8:8888/pin="  <=== ip 
      sendDebug("AT+CIFSR");
//      if (Loding("Get ip address")){ //delay 1 sec
        Wifi_State++; //next setting wifi mode
//      }
//      else{Wifi_State--;}
    break;
    case 4://--------------------------------------------------configure for multiple connections
      sendDebug("AT+CIPMUX=1");
//      if (Loding("Set CIPMUX")){ 
          Wifi_State++; //next setting wifi mode
//      }
//      else{Wifi_State--;}
    break;
    case 5://--------------------------------------------------turn on server on port 8888
      sendDebug("AT+CIPSERVER=1,8888");
      Wifi_State++; //next setting wifi mode
    break;
    case 6:// check if the esp is sending a message 
      if(debug.available()) {
        if(debug.find("+IPD,")){
          Serial.println("Get webpage signal, analyzing...");
          Wifi_State++;
          }
      }
    break;
    case 7:// wait for the serial buffer to fill up (read all the serial data)
        delay(10);
        int connectionId = debug.read()-48; // subtract 48 because the read() function returns 
        // the ASCII decimal value and 0 (the first decimal number) starts at 4
        debug.find("pin="); // advance cursor to "pin="
        int pinNumber = (debug.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
        pinNumber += (debug.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
        
        pixels.setPixelColor(pinNumber, pixels.Color(0, 150, 0));
        pixels.show();
        
//        digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin    
        // make close command
        String closeCommand = "AT+CIPCLOSE="; 
        closeCommand+=connectionId; // append connection id
        closeCommand+="\r\n";
        Serial.print("Turn Pin");
        Serial.print(pinNumber);
        Serial.print(":");
        Serial.print(digitalRead(pinNumber));
        Serial.println("!");
        sendDebug(closeCommand); // close connection
        Wifi_State--; //next setting wifi mode
     break;
  }
}
String get_response() {  //get esp responce without "Serial.find()".
  String response="";  
  char c; 
  while (debug.available()) {  
    c=debug.read();
    response.concat(c);   
    delay(10);  
    }
  response.trim();  
  return response;
}
boolean Loding(String state){
  String response=get_response();
  for (int timeout=0 ; timeout<30 ; timeout++)
  {
    if(response.indexOf("OK") != -1 || response.indexOf("no change") != -1) 
    {
        Serial.print(" OK ! ");
        if(timeout>=2){
          Serial.print("This commend waste ");
          Serial.print(timeout/2);
          Serial.println(" sec.");
        }
        else Serial.println("");
        return 1; 
        break;
    }
    else if(timeout==29){ //after 15 sec for wait esp without responce.
      Serial.print(state);
      Serial.println(" fail...\nExit2");
      return 0; 
    }
    else{
      response=get_response(); //reget
      if(timeout==0)
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


void controlLED()
{
    pixels.clear();
    old = input;
    
    if (Serial.available() > 0) {
      input = Serial.read();
      if(input=='0'||input=='a'||input=='s');
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
        pixels.setPixelColor(41, pixels.Color(0, 150, 0));
        pixels.setPixelColor(37, pixels.Color(0, 150, 0));
        pixels.setPixelColor(27, pixels.Color(0, 150, 0));
        pixels.setPixelColor(19, pixels.Color(0, 150, 0));
        pixels.setPixelColor(29, pixels.Color(0, 150, 0));
        pixels.setPixelColor(33, pixels.Color(0, 150, 0));
        pixels.setPixelColor(47, pixels.Color(0, 150, 0));
        pixels.show();
        break;

    case '0':
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

        
    default:
      break;
      }
} 
