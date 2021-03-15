#include "TinyGPS++.h"

TinyGPSPlus gps;
long timer;

void setup()  {
  Serial1.begin(9600);
  Serial.begin(9600);
  timer = millis();
}

void loop() {
  read_data();
  if(millis() - timer > 100){
    timer = millis();
    printData();
  }
}

void read_data(){
  while (Serial1.available() > 0){
    //Serial.println("LEGGO");
    Serial.write(Serial1.read());
    //gps.encode(Serial1.read());
    //if(gps.date.isValid() && gps.location.isValid() && gps.time.isValid()){
    //  break;    
    //}
  }
}

void printData(){
  Serial.print(gps.location.lat(), 6);
  Serial.print(","); 
  Serial.println(gps.location.lng(), 6);
  Serial.print(gps.date.day());
  Serial.print(F("/"));
  Serial.print(gps.date.month());
  Serial.print(F("/"));
  Serial.println(gps.date.year());
  Serial.print(gps.time.hour());
  Serial.print(F(":"));
  Serial.print(gps.time.minute());
  Serial.print(F(":"));
  Serial.println(gps.time.second());
}
