//IMU e comunicazione seriale
#include "Wire.h"
#include "MPU6050.h"

//Importazione dei moduli necessari
//GPS ublox Neo-6m
#include "TinyGPS++.h"

#define DEBUG 1
#define CALIBRATION 0
#define DEBUG_SERIAL if(DEBUG)Serial


MPU6050 accelgyro;
TinyGPSPlus gps;

void(* reboot) (void) = 0;  //Funzione che permette il riavvio


//Pin utilizzati
//Il modulo GPS usa la Serial1 per comunicare -> Rx1 = 19 e Tx1 = 18
#define MPU_ADD 0x68      //  I2C address of the GY-521
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

int ser_state, f_ser_state;
bool frontal, tilt, fire, fall, detection;

//Struttura dati contenente le soglie di attivazione
struct threshold_t {
  int distance;
  int tilt;
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
  Wire.setWireTimeout(3000,true); 
  Wire.clearWireTimeoutFlag();

  //Pin setup
  pinMode(flame_pin, INPUT);
  pinMode(light_pin, INPUT);
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(buz_pin, OUTPUT);

  digitalWrite(echo_pin, LOW);
  digitalWrite(trigger_pin, LOW);
  noTone(buz_pin);

  time_passed = distance = Pitch = Roll = ser_state = f_ser_state = 0;
  prev_millis = millis();
  frontal = tilt = fire = fall = detection = false;
  
  //Soglie di attivazioni, devono essere modificate per funzionare in casi reali
  threshold.tilt = 60;
  threshold.fall = 0.01;
  threshold.distance = 2;
}


void loop(){
  if(millis() - prev_millis > 200){
    prev_millis = millis();
    //Devono passare almeno 50ms perchè se no le onde interferiscono
    update_ultrasonic_data();
    //Aggiorno i dati dei sensori (imu e flame/light)
    update_imu_data();
    update_flame_light_data();
    send_flame_light_temp_data();
    delay(70);
    /*if(Tmp > 30 && AcZ < 0.2){
      tone(buz_pin, 500);
      print_imu_data();
      delay(500);
      noTone(buz_pin);
      delay(5000);
    }*/

    if(check_condition()){
      if(DEBUG){
        //while(!update_gps_data()){}
        print_all();
        print_value();
        tone(buz_pin, 500);
        delay(500);
        noTone(buz_pin);
        reset_fun();
        delay(2000);
      }
      else{
        accident_detected();
      }
    }
  }
}

/*
 * Funzione che viene eseguita per l'analisi dei valori dei sensori
 * determina se è avvenuto un incidente e di che tipo è l'incidente 
 */
bool check_condition(){
  //Incidente frontale o tamponamento
  if(distance <= threshold.distance)  frontal = true;

  //Caduta libera
  if(AcZ + AcX + AcY > -threshold.fall && AcZ + AcX + AcY < threshold.fall) fall = true;

  //Rilevazione di ribaltamento
  if(Pitch >= threshold.tilt || AcZ < threshold.fall*30) tilt = true;

  //Incidente laterale
  if(AcX - prec_acx > 1.2) detection = true;

  //Tamponamento subito
  if(AcY - prec_acy > 1.2) detection = true;

  //Controllo se è stato rilevato un fuoco
  while(Serial.available() > 0){
    uint8_t c = Serial.read();
    if(c == 'F' && ser_state == 0)  f_ser_state = 1;
    if(c == 'I' && ser_state == 1)  f_ser_state = 2;
    if(c == 'R' && ser_state == 2)  f_ser_state = 3;
    if(c == 'E' && ser_state == 3)  f_ser_state = 4;
    ser_state = f_ser_state ;
  }
  if(ser_state == 4) {
    fire = true;
    f_ser_state = ser_state = 0;
  }
 
  prec_acx = AcX;
  prec_acy = AcY;
  prec_acz = AcZ;
  
  return (frontal | tilt | fall | fire | detection);
}

/*
 * Funzione che segnala l'incidente al bridge:
 * - Leggo i dati dal modulo GPS finchè non ottengo dati validi
 * - Invio tutti i dati (posizione, dati della vettura)
 * - Aspetto che il bridge confermi la ricezione dei dati, altrimenti continuo a inviarli ad intervalli di 10 secondi
 * - Resetto le variabili 
 */
void accident_detected(){
  tone(buz_pin, 500);
  //Leggo i dati finchè non sono validi
  while(!update_gps_data()){}
  send_data();
  delay(70); //Aspetto 70ms per dare il tempo al bridge di rispondere
  
  //Controllo che il bridge abbia ricevuto i dati
  while(ser_state != 3){
    if(Serial.available() > 0){
      uint8_t c = Serial.read();
      if(c == 'A' && ser_state == 0)  f_ser_state = 1;
      if(c == 'C' && ser_state == 1)  f_ser_state = 2;
      if(c == 'K' && ser_state == 2)  f_ser_state = 3;
    }
    else{
      //aspetto 10 secondi e rinvio i dati
      delay(10000);
      send_data();
    }
    ser_state = f_ser_state;
  }
  noTone(buz_pin);
  //INCIDENTE SEGNALATO E RICEVUTO CORRETTAMENTE ASPETTO RESET
  delay(60000); //aspetto un minuto 
  reset_fun(); //Resetto le variabili l'arduino
}

/*
 * Funzione di reset delle variabili dopo la segnalazione di un incidente
 */
void reset_fun(){
  frontal = tilt = fire = fall = detection = false;
  ser_state = f_ser_state = 0;
}


void print_value(){
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
