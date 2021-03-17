void print_gps_data(){
  DEBUG_SERIAL.print("Lat = ");
  DEBUG_SERIAL.print(gps.location.lat(), 6);
  DEBUG_SERIAL.print(" | Lng = "); 
  DEBUG_SERIAL.println(gps.location.lng(), 6);
  
  DEBUG_SERIAL.print(gps.date.day());
  DEBUG_SERIAL.print(F("/"));
  DEBUG_SERIAL.print(gps.date.month());
  DEBUG_SERIAL.print(F("/"));
  DEBUG_SERIAL.println(gps.date.year());
  
  DEBUG_SERIAL.print(gps.time.hour());
  DEBUG_SERIAL.print(F(":"));
  DEBUG_SERIAL.print(gps.time.minute());
  DEBUG_SERIAL.print(F(":"));
  DEBUG_SERIAL.println(gps.time.second());
}

void print_imu_data(){
  DEBUG_SERIAL.print("AcX = ");
  DEBUG_SERIAL.print(AcX);
  DEBUG_SERIAL.print(" | AcY = ");
  DEBUG_SERIAL.print(AcY);
  DEBUG_SERIAL.print(" | AcZ = ");
  DEBUG_SERIAL.print(AcZ);
  DEBUG_SERIAL.print(" | Tmp = ");
  DEBUG_SERIAL.print(Tmp);
  //DEBUG_SERIAL.print(" | Roll = ");
  //DEBUG_SERIAL.print(Roll);
  DEBUG_SERIAL.print(" | Pitch = ");
  DEBUG_SERIAL.println(Pitch);
}


void print_flame_light_data(){
  DEBUG_SERIAL.print("Flame = "); 
  DEBUG_SERIAL.print(flame_val); 
  DEBUG_SERIAL.print(" | ");
  DEBUG_SERIAL.print("Light = ");
  DEBUG_SERIAL.println(light_val);
}

void print_ultrasonic_data(){
  DEBUG_SERIAL.print("Distanza = ");
  DEBUG_SERIAL.println(distance);
}

void print_all(){
  DEBUG_SERIAL.println();
  print_gps_data();
  print_imu_data();
  print_flame_light_data();
  print_ultrasonic_data();
}

void plotter_imu_data(){
  DEBUG_SERIAL.print(AcX);
  DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.print(AcY);
  DEBUG_SERIAL.print(" ");
  DEBUG_SERIAL.println(AcZ);
}
