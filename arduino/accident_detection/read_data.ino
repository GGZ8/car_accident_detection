void update_imu_data(){
  // Inizio trasmissione con il dispositivo I2C
  Wire.beginTransmission(MPU_ADD);
  // Indirizzo di inizio della lettura
  Wire.write(0x3B);
  Wire.endTransmission(false);
  // Chiedo di leggere 8 indirizzi perchè sono 2 per ogni dato 
  Wire.requestFrom(MPU_ADD, 8, true);

  //Leggo
  int16_t acx = (uint8_t)Wire.read() << 8 | (uint8_t)Wire.read();
  int16_t acy = (uint8_t)Wire.read() << 8 | (uint8_t)Wire.read();
  int16_t acz = (uint8_t)Wire.read() << 8 | (uint8_t)Wire.read();
  int16_t tmp = (uint8_t)Wire.read() << 8 | (uint8_t)Wire.read();

  //Normalizing data
  float alpha = 0.5;
  float X = acx * alpha + (acx * (1.0 - alpha));
  float Y = acy * alpha + (acy * (1.0 - alpha)); 
  float Z = acz * alpha + (acz * (1.0 - alpha));
  AcX = (float)acx / 16384.0;
  AcY = (float)acy / 16384.0;
  AcZ = (float)acz / 16384.0;
  Tmp = tmp / 340.00 + 36.53;

  //Calcolo rotazioni su asse X e Y
  Roll = abs((atan2(X, sqrt(Y*Y + Z*Z)) * 180.0) / M_PI);
  Pitch = abs((atan2(Y, sqrt(X*X + Z*Z)) * 180.0) / M_PI);
}

void update_flame_light_data(){
  flame_val = map(analogRead(flame_pin), 0, 1024, 1024, 0);
  light_val = analogRead(light_pin);
}

bool update_gps_data(){
  //Finchè ci sono dati sulla seriale li leggo
  while (Serial1.available() > 0){
    gps.encode(Serial1.read());
    if(gps.date.isValid() && gps.location.isValid() && gps.time.isValid() && gps.time.isUpdated()){
      //Quando sono riuscito a leggere correttamente i dati esco
      return true;
    }
  }
  return false;
}

void update_ultrasonic_data(){
  //Invio un impulso HIGH sul pin del trigger
  digitalWrite(trigger_pin, HIGH);
  //lo lascio al valore HIGH per 10 microsecondi
  delayMicroseconds(10);
  //lo riporto allo stato LOW
  digitalWrite(trigger_pin, LOW);

  //ottengo il numero di microsecondi per i quali il PIN echo e' rimasto allo stato HIGH
  //per fare questo utilizzo la funzione pulseIn()
  time_passed = pulseIn(echo_pin, HIGH);

  // La velocita' del suono e' di 340 metri al secondo, o 29 microsecondi al centimetro.
  // il nostro impulso viaggia in andata e ritorno, quindi per calcoalre la distanza
  // tra il sensore e il nostro ostacolo occorre fare:
  distance = time_passed / 58.3;
}
