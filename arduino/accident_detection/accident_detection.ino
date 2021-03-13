//IMU e comunicazione seriale
#include "Wire.h"           
#include "MPU6050.h"

//Importazione dei moduli necessari
//GPS ublox Neo-6m
#include "TinyGPS++.h"

MPU6050 accelgyro;
TinyGPSPlus gps;


#define DEBUG 1
#define DEBUG_SERIAL if(DEBUG)Serial

//Pin utilizzati
//Il modulo GPS usa la Serial1 per comunicare -> Rx1 = 19 e Tx1 = 18
#define MPU_addr 0x68  //  I2C address of the GY-521
#define flame A15      //  Input pin per la lettura del flame sensor
#define light A14      //  Input pin per la lettura del light sensor
#define trigger 8      //  Output pin trigger ultrasonic
#define echo 9         //  Input pin per la ricezione del trigger
#define buz 10         //  Output pin del buzzer

//Funzioni per la calibrazione dell'IMU
#include "calibration.h"
//Funzioni per la lettura dei dati
#include "read_data.h"
//Funzioni per la stampa dei dati
#include "print_information.h"

//Variabile usata per controllare i dati a intervalli regolari
unsigned long prev_millis;

//Variabili
long time_passed;   //  Tempo che impieghera' il suono a percorrere una certa distanza
long distance;      //  La distanza che ha percorso il suono
int flame_val;      //  Valore del flame sensor 
int light_val;      //  Valore del light sensor

double AcX, AcY, AcZ, Tmp;      //  Varaibli per la lettura dall'IMU
int X, Y, Z;
float Roll, Pitch, alpha = 0.5; //  Angoli di rotazione (Pitch -> asse Y Roll -> asse X) 
bool no_data;



void setup()  {
  //Serial1 usata per la comunicazione con il modulo gps 9600 freq. di default
  Serial1.begin(9600);
  //Serial usata per la comunicazione con il cavo usb
  Serial.begin(115200);

  //Inizializzazione dell'accelerometro
  Wire.begin();
	accelgyro.initialize();
  setup_imu();

  //Pin setup
  pinMode(flame, INPUT);
  pinMode(light, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(buz, OUTPUT);

  digitalWrite(echo, LOW);
  digitalWrite(trigger, LOW);

  time_passed = distance = Pitch = Roll = X = Y = Z = 0;
  prev_millis = millis();
  no_data = true;
}


void loop(){
  //read_ultrasonic_data();
  //read_gps_data();
  read_imu_data();
  if(millis() - prev_millis > 100){
    prev_millis = millis();
    //print_ultrasonic_data();
    //print_gps_data();
    print_imu_data();
  }
}
