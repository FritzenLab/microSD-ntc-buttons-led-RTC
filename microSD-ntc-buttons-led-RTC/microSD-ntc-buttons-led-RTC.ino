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
#include <Wire.h>
#include <RTClib.h>

const int pinTermistor = A0;
#define NTC 0
#define button1 2
#define button2 3
#define LED 6
const int chipSelect = 7;

RTC_DS1307 rtc;

bool button1State= false;
long button1Time= 0;
bool button2State= false;
long button2Time= 0;
String caller= " ";
double sensorValue= 0.00;
bool start= false;
int counter= 0;
long ledTimer= 0;
String myDate;
String myTime;

const double beta = 3950.0;
const double r0 = 10000.0;
const double t0 = 273.0 + 25.0;
const double rx = r0 * exp(-beta/t0);

const double vcc = 3.3;
const double R = 9810.0;
 
// Numero de amostras na leitura
const int nSamples = 5;

void setup() {
  Serial.begin(9600);
  analogReadResolution(12);
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
  Wire.begin();
 
  if (!rtc.begin()) {
    Serial.println("Did no find RTC module!");
    while (1);
  }
 
  if (!rtc.isrunning()) {
    Serial.println("Configuring time and date");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Ajusta o horário para o do computador
  }
  
}

void loop() {

  // put your main code here, to run repeatedly:
  if(digitalRead(button1) && (millis() - button1Time > 400)){
    button1State= true;
    button1Time= millis();
    caller= "button1";
    ledBlink("on");
    myDate= getCurrentDate();
    myTime= getCurrentTime();
    saveToSD(button1State, caller, myDate, myTime);
  }
  if(digitalRead(button2) && (millis() - button2Time > 400)){
    button2State= true;
    button2Time= millis();
    caller= "button2";
    ledBlink("on");
    myDate= getCurrentDate();
    myTime= getCurrentTime();
    saveToSD(button2State, caller, myDate, myTime);
  }
  ledBlink("none");


}

void saveToSD(bool buttonState, String caller, String myDate, String myTime){
  
  File dataFile = SD.open("mydata.txt", FILE_WRITE);
  
  sensorValue= readSensor();
  if (dataFile){
    dataFile.print(myDate);
    dataFile.print(",");
    dataFile.print(myTime);
    dataFile.print(",");
    dataFile.print(caller);
    dataFile.print(",");
    dataFile.print(sensorValue);
    dataFile.println(",");
    dataFile.close();
    Serial.println(caller);
  }
  button1State= false;
}

double readSensor(){

  int sum = 0;
  for (int i = 0; i < nSamples; i++) {
    sum += analogRead(pinTermistor);
    delay (20);
  }
 
  // Determina a resistência do termistor
  double v = (vcc*sum)/(nSamples*4096.0);
  double rt = (vcc*R)/v - R;
 
  // Calcula a temperatura
  double t = beta / log(rt/rx);
  Serial.println (t-273.0);
  return t-273.0;
}
void ledBlink(String blinkState){
  if(blinkState == "on"){
    start= true;
    counter++;
  }
  if(millis() - ledTimer > 300){
    ledTimer= millis();
    if(start == true && counter < 5){
    digitalWrite(LED, !digitalRead(LED));
    counter++;
    }else{
      digitalWrite(LED, LOW);
      counter= 0;
      start= false;
    }
  }  
}
String getCurrentDate(){
  DateTime now = rtc.now();
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" ");
  myDate= String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  return myDate;
}
String getCurrentTime(){
  DateTime now = rtc.now();
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  String myTime= String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  //Serial.println(myTime);
  return myTime;
}