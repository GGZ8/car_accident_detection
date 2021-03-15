

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
  Serial.write(1);
  //Tilt
  Serial.write(0);
  //Fire
  Serial.write(0);
  //Free Fall
  Serial.write(1);
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



void send_imu_data(){
  byte * Az = (byte *) &AcZ;
  byte * Ay = (byte *) &AcY;
  byte * Ax = (byte *) &AcX;
  Serial.write(0x7F);
  Serial.write(0x0C);
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
  Serial.write(0x7E);
}
