void print_gps_data(){
  DEBUG_SERIAL.print("Lat = ");
  DEBUG_SERIAL.print(gps.location.lat(), 6);
  DEBUG_SERIAL.print(" | Lng = "); 
  DEBUG_SERIAL.println(gps.location.lng(), 6);
  
  DEBUG_SERIAL.print(gps.date.day());
  DEBUG_SERIAL.print("/");
  DEBUG_SERIAL.print(gps.date.month());
  DEBUG_SERIAL.print("/");
  DEBUG_SERIAL.println(gps.date.year());
  
  DEBUG_SERIAL.print(gps.time.hour());
  DEBUG_SERIAL.print(":");
  DEBUG_SERIAL.print(gps.time.minute());
  DEBUG_SERIAL.print(":");
  DEBUG_SERIAL.println(gps.time.second());
}

void print_imu_data(){
  DEBUG_SERIAL.print("AcX = " + String(AcX));
  DEBUG_SERIAL.print(" | AcY = " + String(AcY));
  DEBUG_SERIAL.print(" | AcZ = " + String(AcZ));
  DEBUG_SERIAL.print(" | Tmp = " + String(Tmp));
  //DEBUG_SERIAL.print(" | Roll = ");
  //DEBUG_SERIAL.print(Roll);
  DEBUG_SERIAL.print(" | Pitch = " + String(Pitch));
}


void print_flame_light_data(){
  DEBUG_SERIAL.print("Flame = " + String(flame_val)); 
  DEBUG_SERIAL.print(" | Light = " + String(light_val));
}

void print_ultrasonic_data(){
  DEBUG_SERIAL.print("Distance = " + String(distance));
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
