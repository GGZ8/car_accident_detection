void read_imu_data(){
  // Inizio trasmissione con il dispositivo I2C
  Wire.beginTransmission(MPU_addr);
  // Indirizzo di inizio della lettura
  Wire.write(0x3B);
  Wire.endTransmission(false);
  // Chiedo di leggere 8 indirizzi perchè sono 2 per ogni dato 
  Wire.requestFrom(MPU_addr, 8, true);

  //Leggo
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();

  //Normalizing data
  X = AcX * alpha + (AcX * (1.0 - alpha));
  Y = AcY * alpha + (AcY * (1.0 - alpha)); 
  Z = AcZ * alpha + (AcZ * (1.0 - alpha));
  AcX = (float)AcX / 16384.0;
  AcY = (float)AcY / 16384.0;
  AcZ = (float)AcZ / 16384.0;
  Tmp = Tmp / 340.00 + 36.53;

  //Calcolo rotazioni su asse X e Y
  Roll = (atan2(-Y, Z) * 180.0) / M_PI;
  Pitch = (atan2(-X, Z) * 180.0) / M_PI;
}

void read_flame_light_data(){
  flame_val = analogRead(flame);
  light_val = analogRead(light);
}

void read_gps_data(){
  //Finchè ci sono dati sulla seriale li leggo
  while (Serial1.available() > 0){
    gps.encode(Serial1.read());
    if(gps.date.isValid() && gps.location.isValid() && gps.time.isValid()){
      //Quando sono riuscito a leggere correttamente i dati esco
      no_data = false;
      break;    
    }
  }
}

void read_ultrasonic_data(){
  //Invio un impulso HIGH sul pin del trigger
  digitalWrite(trigger, HIGH);
  //lo lascio al valore HIGH per 10 microsecondi
  delayMicroseconds(10);
  //lo riporto allo stato LOW
  digitalWrite(trigger, LOW);

  //ottengo il numero di microsecondi per i quali il PIN echo e' rimasto allo stato HIGH
  //per fare questo utilizzo la funzione pulseIn()
  time_passed = pulseIn(echo, HIGH);

  // La velocita' del suono e' di 340 metri al secondo, o 29 microsecondi al centimetro.
  // il nostro impulso viaggia in andata e ritorno, quindi per calcoalre la distanza
  // tra il sensore e il nostro ostacolo occorre fare:
  distance = time_passed / 58.3;
}
