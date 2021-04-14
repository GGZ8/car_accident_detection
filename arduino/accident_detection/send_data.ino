/*
 * Funzione che invia i dati relativi all'incidente se vienen rilevato
 * Questa funzione invia un pacchetto dati sulla seriale al bridge con i sequenti dati:
 * - LAT e LNG
 * - DATA e ORA
 * - Tipo di incidente 
 * - Temperatura del veiocolo
 * - Targa
 */
void send_data(){
  uint32_t hour_tmp = gps.time.value();
  uint32_t date_tmp = gps.date.value();
  float lat_tmp = gps.location.lat();
  float lng_tmp = gps.location.lng();
  byte * lat = (byte *) &lat_tmp;
  byte * lng = (byte *) &lng_tmp;
  byte * hour = (byte *) &hour_tmp;
  byte * date = (byte *) &date_tmp;
  byte * tmp = (byte *) &Tmp;
  char license_plate[] = "AA111AA";
  
  Serial.write(0x7F);
  Serial.write(0x1F);
  //Latitutine
  Serial.write(lat[0]);
  Serial.write(lat[1]);
  Serial.write(lat[2]);
  Serial.write(lat[3]);
  //Longitudine
  Serial.write(lng[0]);
  Serial.write(lng[1]);
  Serial.write(lng[2]);
  Serial.write(lng[3]);
  //Ora
  Serial.write(hour[0]);
  Serial.write(hour[1]);
  Serial.write(hour[2]);
  Serial.write(hour[3]);
  //Data
  Serial.write(date[0]);
  Serial.write(date[1]);
  Serial.write(date[2]);
  Serial.write(date[3]);
  //Frontal
  Serial.write(int(frontal));
  //Tilt
  Serial.write(int(tilt));
  //Fire
  Serial.write(int(fire));
  //Free Fall
  Serial.write(int(fall));
  //Temperature
  Serial.write(tmp[0]);
  Serial.write(tmp[1]);
  Serial.write(tmp[2]);
  Serial.write(tmp[3]);
  //Targa
  for (uint8_t i = 0; i < sizeof(license_plate) - 1; i++) {
    Serial.write(license_plate[i]);
  }
  Serial.write(0x7E);
}


/*
 * Funzione che i invia i dati relativi agli incidendi, i dati del
 * flame sensor e del photoresistor vengono mappati per permettere
 * un'invio corretto dei dati, e infine viene inviata la temperatura
 * come uint8_t e un ulteriore byte per segnalare se la temperatura 
 * è negativa
 */
void send_flame_light_temp_data(){
  uint8_t flame = map(flame_val, 0, 1024, 0, 253);
  uint8_t light = map(light_val, 0, 1024, 0, 253);
  uint8_t temp = abs(int(Tmp));
  uint8_t data[] = {0xFE, 0x04, flame, light, temp, 0x00, 0xFF};
  if(temp < 0){
    data[5] = 0x01;
  }
  for (uint8_t i = 0; i < sizeof(data); i++) {
    Serial.write(data[i]);
  }
}


/*
 * Funzione che invia dati relativi alla posizione e alla velocità
 * della vettura per evrificare se sono presenti incidenti nelle vicinanze
 */
void send_position_data(){
  float lat_tmp = gps.location.lat();
  float lng_tmp = gps.location.lng();
  uint8_t car_speed = gps.speed.kmph();
  byte * lat = (byte *) &lat_tmp;
  byte * lng = (byte *) &lng_tmp;

  Serial.write(0xFD);
  Serial.write(0x09);

  //Latitudine
  Serial.write(lat[0]);
  Serial.write(lat[1]);
  Serial.write(lat[2]);
  Serial.write(lat[3]);
  //Longitudine
  Serial.write(lng[0]);
  Serial.write(lng[1]);
  Serial.write(lng[2]);
  Serial.write(lng[3]);
  //Velocità
  Serial.write(car_speed);
  

  Serial.write(0xFC);
  //Ragionevole pensare che le coordinate non siano 255 e 253
}
