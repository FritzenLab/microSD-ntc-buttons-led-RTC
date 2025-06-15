/*
D0 ntc
D1
D2 push button
D3 push button
D4 SDA RTC
D5 SCL RTC
D6 LED
D7 CS SDcard
D8 SCK SDcard
D9 MISO SDcard
D10 MOSI SDcard
*/
#include <SPI.h>
#include <SD.h>

#define NTC 0
#define button1 2
#define button2 3
#define LED 6

bool button1State= false;
long button1Time= 0;

const int chipSelect = 7;

void setup() {
  // put your setup code here, to run once:
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(LED, OUTPUT);

  if (!SD.begin(chipSelect))
  {
    Serial.println("SD card failed !");
    Serial.println("Verify connections");
    return;
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(button1) && (millis() - button1Time > 400)){
    button1State= true;
    button1Time= millis();
    saveToSD(button1State);
  }


}

void saveToSD(bool buttonState){
  
  File dataFile = SD.open("mydata.txt", FILE_WRITE);
  
  if (dataFile){
    dataFile.print("Button1:");
    dataFile.print(buttonState);
    dataFile.close();
  }
  button1State= false;
}