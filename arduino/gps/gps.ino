#include "TinyGPS++.h"

TinyGPSPlus gps;
bool done;

void setup()  {
  Serial1.begin(9600);
  Serial.begin(115200);
  done = false;
}

void loop() {
  if(!done){
    while (Serial1.available() > 0){
      gps.encode(Serial1.read());
      if(gps.date.isValid() && gps.location.isValid() && gps.time.isValid()){
        printData();
        break;    
      }
    }
  }
}

void printData(){
  Serial.println("\n--------------------------------------------"); 
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
  Serial.println("--------------------------------------------"); 
  done = true;
}
