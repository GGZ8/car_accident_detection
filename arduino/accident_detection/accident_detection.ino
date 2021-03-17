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

void(* reboot) (void) = 0;  //Funzione che permette il riavvio

/*//-----------------------------------------------------------------------------------

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


//-----------------------------------------------------------------------------------
*/

#define DEBUG 1
#define DEBUG_SERIAL if(DEBUG)Serial

//Pin utilizzati
//Il modulo GPS usa la Serial1 per comunicare -> Rx1 = 19 e Tx1 = 18
#define MPU_addr 0x68  //  I2C address of the GY-521
#define flame_pin A15      //  Input pin per la lettura del flame sensor
#define light_pin A14      //  Input pin per la lettura del light sensor
#define trigger_pin 8      //  Output pin trigger ultrasonic
#define echo_pin 9         //  Input pin per la ricezione del trigger
#define buz_pin 10         //  Output pin del buzzer

//Variabile usata per controllare i dati a intervalli regolari
unsigned long prev_millis;

//Variabili ultrasonic
long time_passed;   //  Tempo che impieghera' il suono a percorrere una certa distanza
long distance;      //  La distanza che ha percorso il suono
int flame_val;      //  Valore del flame sensor 
int light_val;      //  Valore del light sensor

//Variabili accelerometro
float AcX, AcY, AcZ, Tmp;            //  Varaibli per la lettura dall'IMU
float Roll, Pitch;                   //  Angoli di rotazione (Pitch -> asse Y Roll -> asse X)
float prec_acx, prec_acy, prec_acz;  //  Variabili utilizzata per il controllo degli incidenti laterali

int serial_state, f_serial_state;
bool frontal, tilt, fire, fall, detection;

//Struttura dati contenente le soglie di attivazione
struct threshold_t {
  int temp;
  int distance;
  int tilt;
  int fire;
  int light;
  float fall;
}threshold;


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
  pinMode(flame_pin, INPUT);
  pinMode(light_pin, INPUT);
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(buz_pin, OUTPUT);

  digitalWrite(echo_pin, LOW);
  digitalWrite(trigger_pin, LOW);
  noTone(buz_pin);

  time_passed = distance = Pitch = Roll = serial_state = f_serial_state = 0;
  prev_millis = millis();
  frontal = tilt = fire = fall = detection = false;
  //Soglie di attivazioni, devono essere modificate per funzionare in casi reali
  threshold.temp = 40;
  threshold.tilt = 45;
  threshold.fall = 0.1;
  threshold.distance = 2;
  threshold.light = 1000;
  threshold.fire = 500;
  /*accelgyro.setIntFreefallEnabled(1);
  accelgyro.setFreefallDetectionThreshold(0x10);
  accelgyro.setFreefallDetectionDuration(0x05);*/
}


void loop(){
  if(millis() - prev_millis > 100){
    prev_millis = millis();
    update_imu_data();
    update_flame_light_data();
    update_ultrasonic_data(); //Devono passare almeno 50ms perchè se no le onde interferiscono
    if(check_condition()){
      #ifdef DEBUG
        print_data();
        tone(buz_pin, 500);
        delay(500);
        noTone(buz_pin);
        print_imu_data();
        reset_fun();
        delay(2000);
      #else
        accident_detection();
      #endif
    }
    //print_flame_light_data();
    //plotter_imu_data();
  }
}

bool check_condition(){
  //Incidente frontale
  if(distance <= threshold.distance)  frontal = true;

  //Caduta libera
  if(AcZ + AcX + AcY > -threshold.fall && AcZ + AcX + AcY < threshold.fall) fall = true;

  //Rilevazione di ribaltamento
  if(Pitch >= threshold.tilt || AcZ < 0) tilt = true;

  //Incidente laterale
  if(AcX - prec_acx > 1.2) detection = true;

  //Tamponamento
  if(AcY - prec_acy > 1.2) detection = true;

  //Fuoco
  //Se non viene rilevata luce allora c'è sicuramente del fuoco
  if(light_val < threshold.light && flame_val > threshold.fire){
    DEBUG_SERIAL.println("FLAME & LIGHT");
    fire = true;
  }
  //Se c'è luce, oltre a controllare il valore del flame sensor considero la temperatura
  else if(light_val >= threshold.light && flame_val > threshold.fire){
    if(Tmp >= threshold.temp){
      DEBUG_SERIAL.println("FLAME & LIGHT & TEMP");
      fire = true;
    }
  }

  
  prec_acx = AcX;
  prec_acy = AcY;
  prec_acz = AcZ;
  
  return (frontal | tilt | fall | detection);
}

void accident_detection(){
  tone(buz_pin, 500);
  //Continuo a leggere i dati finchè non sono validi
  //while(!update_gps_data()){}
  #ifdef DEBUG
    print_all();
  #else
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
        //aspetto 10 secondi e rinvio i dati
        delay(10000);
        send_data();
      }
      serial_state = f_serial_state;
    }
  #endif
  noTone(buz_pin);
  //INCIDENTE SEGNALATO E RICEVUTO CORRETTAMENTE ASPETTO RESET
  delay(60000); //aspetto un minuto 
  reset_fun(); //Riavvio l'arduino
}

void reset_fun(){
  frontal = tilt = fire = fall = detection = false;
  serial_state = f_serial_state = 0;
}

void print_data(){
  DEBUG_SERIAL.print("Frontal = ");
  DEBUG_SERIAL.print(frontal);
  DEBUG_SERIAL.print(" | Fire = ");
  DEBUG_SERIAL.print(fire);
  DEBUG_SERIAL.print(" | Tilt = ");
  DEBUG_SERIAL.print(tilt);
  DEBUG_SERIAL.print(" | Fall = ");
  DEBUG_SERIAL.print(fall);
  DEBUG_SERIAL.print(" | Detection = ");
  DEBUG_SERIAL.print(detection);
  DEBUG_SERIAL.println();
}
