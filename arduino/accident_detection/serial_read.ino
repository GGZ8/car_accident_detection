bool fire_read(){
  bool ret = false;
  while(Serial.available() > 0){
    uint8_t c = Serial.read();
    if(c == 'F' && ser_state == 0)  f_ser_state = 1;
    if(c == 'I' && ser_state == 1)  f_ser_state = 2;
    if(c == 'R' && ser_state == 2)  f_ser_state = 3;
    if(c == 'E' && ser_state == 3)  f_ser_state = 4;
    ser_state = f_ser_state ;
  }
  if(ser_state == 4) {
    ret = true;
    f_ser_state = ser_state = 0;
  }
  return ret;
}


void bridge_ack(){
  while(ser_state != 3){
    if(Serial.available() > 0){
      uint8_t c = Serial.read();
      if(c == 'A' && ser_state == 0)  f_ser_state = 1;
      if(c == 'C' && ser_state == 1)  f_ser_state = 2;
      if(c == 'K' && ser_state == 2)  f_ser_state = 3;
    }
    else{ 
      //Se il bridge non mi conferma la ricezione aspetto 
      //10 secondi e rinvio i dati
      delay(10000);
      send_data();
    }
    ser_state = f_ser_state;
  }
}

bool near_accident_led(){
  bool ret;
  while(Serial.available() > 0){
    uint8_t c = Serial.read();
    if(ser_state == 0 && c == 'O') f_ser_state = 1;
    if(ser_state == 1 && c == 'N') f_ser_state = 2;
    if(ser_state == 1 && c == 'F') f_ser_state = 3;
    if(ser_state == 3 && c == 'F') f_ser_state = 4;
    if(ser_state == 4 && c == 'O') f_ser_state = 1;
    if(ser_state == 2 && c == 'O') f_ser_state = 1;

    if(f_ser_state == 2 && ser_state == 1) ret = true;
    if(f_ser_state == 4 && ser_state == 3) ret = false;

    f_ser_state = ser_state;
  }
  f_ser_state = ser_state = 0;

  return ret;
}
