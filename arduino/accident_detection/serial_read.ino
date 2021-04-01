bool fire_read(){
  while(Serial.available()){
    uint8_t c = Serial.read();
    f_ser_state = 0;
    if(c == 'F' && ser_state == 0)  f_ser_state = 1;
    if(c == 'I' && ser_state == 1)  f_ser_state = 2;
    if(c == 'R' && ser_state == 2)  f_ser_state = 3;
    if(c == 'E' && ser_state == 3)  f_ser_state = 4;
    ser_state = f_ser_state ;
  }
  if(ser_state == 4) {
    f_ser_state = ser_state = 0;
    return true;
  }
  return false;
}


void bridge_ack(){
  while(ser_state != 3){
    if(Serial.available()){
      uint8_t c = Serial.read();
      f_ser_state = 0;
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
  bool ret = false;
  while(Serial.available()){
    uint8_t c = Serial.read();
    f_ser_state = 0;
    if(c == 'O' && ser_state == 0) f_ser_state = 1;
    if(c == 'N' && ser_state == 1) f_ser_state = 2;
    
    if(f_ser_state == 2) ret = true;
    else ret = false;
    
    ser_state = f_ser_state;
  }
  f_ser_state = ser_state = 0;

  return ret;
}
