//IMU e comunicazione seriale
#include "Wire.h"           
#include "MPU6050.h"

//Importazione dei moduli necessari
//GPS ublox Neo-6m
#include "TinyGPS++.h"

//Funzioni per la calibrazione dell'IMU
#include "calibration.h"
//Funzioni per la lettura dei dati
#include "read_data.h"
//Funzioni per la stampa dei dati
#include "print_information.h"
//Funzioni per l'invio dei dati sulla seriale
#include "send_data.h"

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

//Variabile usata per controllare i dati a intervalli regolari
unsigned long prev_millis;

//Variabili
long time_passed;   //  Tempo che impieghera' il suono a percorrere una certa distanza
long distance;      //  La distanza che ha percorso il suono
int flame_val;      //  Valore del flame sensor 
int light_val;      //  Valore del light sensor

double AcX, AcY, AcZ, Tmp;      //  Varaibli per la lettura dall'IMU
float Roll, Pitch, alpha = 0.5; //  Angoli di rotazione (Pitch -> asse Y Roll -> asse X) 
int serial_state, f_serial_state;


void setup()  {
  //Serial1 usata per la comunicazione con il modulo gps 9600 freq. di default
  Serial1.begin(9600);
  //Serial usata per la comunicazione con il cavo usb
  Serial.begin(9600);

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
  pinMode(13, OUTPUT);

  digitalWrite(echo, LOW);
  digitalWrite(trigger, LOW);
  noTone(buz);

  time_passed = distance = Pitch = Roll = serial_state = f_serial_state = 0;
  prev_millis = millis();
}


void loop(){
  if(millis() - prev_millis > 100){
    prev_millis = millis();
    update_imu_data();
    update_flame_light_data();
    update_ultrasonic_data(); //Devono passare almeno 50ms perchè se no le onde interferiscono
    //print_ultrasonic_data();
    /*if(distance <= 2){
      accident_detection();
    }*/
    /*digitalWrite(13, HIGH);
    delay(100);*/
    print_imu_data();
    //digitalWrite(13, LOW);
    
  }
}


bool check_condition(){
  
}

void accident_detection(){
  tone(buz, 500);
  //Continuo a leggere i dati finchè non sono validi
  while(!update_gps_data()){}
  send_data();
  //Controllo che il bridge abbia ricevuto i dati
  while(f_serial_state != 3){
    if(Serial.available() > 0){
      uint8_t c = Serial.read();
      if(c == 'R' && serial_state == 0)  f_serial_state = 1;
      if(c == 'E' && serial_state == 1)  f_serial_state = 2;
      if(c == 'C' && serial_state == 2)  f_serial_state = 3;
    }
    else{
      //aspetto un secondo e rinvio i dati
      delay(1000);
      send_data();
    }
    serial_state = f_serial_state;
  }
  noTone(buz);
  while(true){} //INCIDENTE SEGNALATO E RICEVUTO CORRETTAMENTE ASPETTO RESET
  f_serial_state = serial_state = 0;
}
