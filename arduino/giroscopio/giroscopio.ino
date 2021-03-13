#include "Wire.h"
#include "MPU6050.h"
#define DEBUG 1

MPU6050 accelgyro;
#include "calibration.h"

// I2C address of the GY-521
const int MPU_addr = 0x68;
double AcX, AcY, AcZ, Tmp;
int X, Y, Z;
float Roll, Pitch, alpha = 0.5;
//Pitch su asse Y
//Roll su asse X


long interval = 0;

void setup(){
  Serial.begin(115200);
	Wire.begin();
	accelgyro.initialize(); //Initialize the accelerometer
    accelgyro.setXAccelOffset(2757);
    accelgyro.setYAccelOffset(766);
    accelgyro.setZAccelOffset(1702);
    accelgyro.setXGyroOffset(34);
    accelgyro.setYGyroOffset(23);
    accelgyro.setZGyroOffset(37);
    //Calibration of MPU6050
//    if(!mpu_init()){ 
//      Serial.print("CHECK CONNECTION");
//      while(1);
//    }
}

void loop(){
  if(millis() - interval > 100){
    acquisisco_dati();
    #ifdef DEBUG
      stampa_dati();
      //invio_dati();
      //plotter();
    #endif
    interval = millis();
  }
}

void invio_dati(){
  int8_t X = (int)Roll;
  int8_t Y = (int)Pitch;
  byte * Az = (byte *) &AcZ;
  byte * Ay = (byte *) &AcY;
  byte * Ax = (byte *) &AcX;
  Serial.write(0x7F);
  Serial.write(0x0E);
  Serial.write(Ax[0]);
  Serial.write(Ax[1]);
  Serial.write(Ax[2]);
  Serial.write(Ax[3]);
  Serial.write(Ay[0]);
  Serial.write(Ay[1]);
  Serial.write(Ay[2]);
  Serial.write(Ay[3]);
  Serial.write(Az[0]);
  Serial.write(Az[1]);
  Serial.write(Az[2]);
  Serial.write(Az[3]);
  Serial.write(X);
  Serial.write(Y);
  Serial.write(0x7E);
  
}

void acquisisco_dati(){
  Wire.beginTransmission(MPU_addr);
  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 8, true);
  //Data read
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();

  X = AcX * alpha + (AcX * (1.0 - alpha));
  Y = AcY * alpha + (AcY * (1.0 - alpha)); 
  Z = AcZ * alpha + (AcZ * (1.0 - alpha));
  //Normalizing data
  AcX = (float)AcX / 16384.0;
  AcY = (float)AcY / 16384.0;
  AcZ = (float)AcZ / 16384.0;
  Tmp = Tmp / 340.00 + 36.53;

  //Calculate X,Y rotation angle
  Roll = (atan2(-Y, Z) * 180.0) / M_PI;
  Pitch = (atan2(-X, Z) * 180.0) / M_PI;
}

void stampa_dati(){
  Serial.print("AcX = ");
  Serial.print(AcX);
  Serial.print("\t| AcY = ");
  Serial.print(AcY);
  Serial.print("\t| AcZ = ");
  Serial.print(AcZ);
  Serial.print("\t| Tmp = ");
  Serial.print(Tmp);
  
  Serial.print("\t| Roll = ");
  Serial.print((int)Roll);
  Serial.print("\t| Pitch = ");
  Serial.print((int)Pitch);
  Serial.println();
}

void plotter(){
  Serial.print(AcX);
  Serial.print(" ");
  Serial.print(AcY);
  Serial.print(" ");
  Serial.println(AcZ);
}
