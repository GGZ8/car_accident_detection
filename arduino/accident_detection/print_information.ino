void start(char* sensor){
    DEBUG_SERIAL.print("-------------------");
    DEBUG_SERIAL.print(sensor);
    DEBUG_SERIAL.println("------------------------");
}

void end(){
    DEBUG_SERIAL.println("----------------------------------------------\n");
}


void print_gps_data(){
  char sensor_name[] = "GPS";
  //start(&sensor_name[0]);
  DEBUG_SERIAL.print(gps.location.lat(), 6);
  DEBUG_SERIAL.print(","); 
  DEBUG_SERIAL.println(gps.location.lng(), 6);
  DEBUG_SERIAL.println(gps.date.value());
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
  //end();
}

void print_imu_data(){
  char sensor_name[] = "IMU";
  //start(&sensor_name[0]);
  DEBUG_SERIAL.print("AcX = ");
  DEBUG_SERIAL.print(AcX);
  DEBUG_SERIAL.print(" | AcY = ");
  DEBUG_SERIAL.print(AcY);
  DEBUG_SERIAL.print(" | AcZ = ");
  DEBUG_SERIAL.print(AcZ);
  DEBUG_SERIAL.print(" | Tmp = ");
  DEBUG_SERIAL.print(Tmp);
  
  DEBUG_SERIAL.print(" | Roll = ");
  DEBUG_SERIAL.print((int)Roll);
  DEBUG_SERIAL.print(" | Pitch = ");
  DEBUG_SERIAL.println((int)Pitch);
  //end();
}


void print_flame_light_data(){
    char sensor_name[] = "F_L";
    start(&sensor_name[0]);
    DEBUG_SERIAL.print(flame_val + "\t" + light_val);
    end();
}

void print_ultrasonic_data(){
    char sensor_name[] = "SON";
    //start(&sensor_name[0]);
    DEBUG_SERIAL.print("Distanza :");
    DEBUG_SERIAL.println(distance);
    //end();
}
