int buffersize = 1000; //Numero di letture, più è alto e più ottengo precisione, ma sarà più lento il processo di calibrazione (default:1000)
int acel_deadzone = 8; //Errore di tolleranza per l'accelerometro, più è basso più si ottiene precisione, ma può non convergere (default:8)
int giro_deadzone = 1; //Errore di tolleranza per il giroscopio, più è basso più si ottiene precisione, ma può non convergere (default:1)

int16_t ax, ay, az, gx, gy, gz;

int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz, state = 0;
int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;

void meansensors(){
	long i = 0, buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;

	while (i < (buffersize + 101))
	{
		// read raw accel/gyro measurements from device
		accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

		if (i > 100 && i <= (buffersize + 100))
		{ //First 100 measures are discarded
			buff_ax = buff_ax + ax;
			buff_ay = buff_ay + ay;
			buff_az = buff_az + az;
			buff_gx = buff_gx + gx;
			buff_gy = buff_gy + gy;
			buff_gz = buff_gz + gz;
		}
		if (i == (buffersize + 100))
		{
			mean_ax = buff_ax / buffersize;
			mean_ay = buff_ay / buffersize;
			mean_az = buff_az / buffersize;
			mean_gx = buff_gx / buffersize;
			mean_gy = buff_gy / buffersize;
			mean_gz = buff_gz / buffersize;
		}
		i++;
		delay(2); //Needed so we don't get repeated measures
	}
}

void calibration()
{
	ax_offset = -mean_ax / 8;
	ay_offset = -mean_ay / 8;
	az_offset = (16384 - mean_az) / 8;

	gx_offset = -mean_gx / 4;
	gy_offset = -mean_gy / 4;
	gz_offset = -mean_gz / 4;
	while (1)
	{
		int ready = 0;
		accelgyro.setXAccelOffset(ax_offset);
		accelgyro.setYAccelOffset(ay_offset);
		accelgyro.setZAccelOffset(az_offset);

		accelgyro.setXGyroOffset(gx_offset);
		accelgyro.setYGyroOffset(gy_offset);
		accelgyro.setZGyroOffset(gz_offset);

		meansensors();
		DEBUG_SERIAL.print("... ");

		if (abs(mean_ax) <= acel_deadzone)
			ready++;
		else
			ax_offset = ax_offset - mean_ax / acel_deadzone;

		if (abs(mean_ay) <= acel_deadzone)
			ready++;
		else
			ay_offset = ay_offset - mean_ay / acel_deadzone;

		if (abs(16384 - mean_az) <= acel_deadzone)
			ready++;
		else
			az_offset = az_offset + (16384 - mean_az) / acel_deadzone;

		if (abs(mean_gx) <= giro_deadzone)
			ready++;
		else
			gx_offset = gx_offset - mean_gx / (giro_deadzone + 1);

		if (abs(mean_gy) <= giro_deadzone)
			ready++;
		else
			gy_offset = gy_offset - mean_gy / (giro_deadzone + 1);

		if (abs(mean_gz) <= giro_deadzone)
			ready++;
		else
			gz_offset = gz_offset - mean_gz / (giro_deadzone + 1);

		if (ready == 6)
			break;
	}
}


void mpu_init(){
	DEBUG_SERIAL.println("\nMPU6050 Calibration");
	delay(2000);
	DEBUG_SERIAL.println("\nYour MPU6050 should be placed in horizontal position.\n");
	delay(3000);
	// verify connection
	DEBUG_SERIAL.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
	delay(1000);
	
	// reset offsets
	accelgyro.setXAccelOffset(0);
	accelgyro.setYAccelOffset(0);
	accelgyro.setZAccelOffset(0);
	accelgyro.setXGyroOffset(0);
	accelgyro.setYGyroOffset(0);
	accelgyro.setZGyroOffset(0);

	while (true){
		if (state == 0){
			DEBUG_SERIAL.println("\nReading sensors for first time...");
			meansensors();
			state++;
			delay(1000);
		}

		if (state == 1){
			DEBUG_SERIAL.print("\nCalculating offsets... ");
			calibration();
			state++;
			delay(1000);
		}

		if (state == 2){
			meansensors();
			DEBUG_SERIAL.println("\nFINISHED!");
      #ifdef DEBUG
  			Serial.print("\nSensor readings with offsets:\t");
  			Serial.print(mean_ax);
  			Serial.print("\t");
  			Serial.print(mean_ay);
  			Serial.print("\t");
  			Serial.print(mean_az);
  			Serial.print("\t");
  			Serial.print(mean_gx);
  			Serial.print("\t");
  			Serial.print(mean_gy);
  			Serial.print("\t");
  			Serial.println(mean_gz);
        Serial.print("Your offsets:\t");
        Serial.print(ax_offset); 
        Serial.print("\t");
        Serial.print(ay_offset); 
        Serial.print("\t");
        Serial.print(az_offset); 
        Serial.print("\t");
        Serial.print(gx_offset); 
        Serial.print("\t");
        Serial.print(gy_offset); 
        Serial.print("\t");
        Serial.println(gz_offset); 
      #endif
      delay(1000);
      //Imposto i valori calcolati
			accelgyro.setXAccelOffset(ax_offset);
			accelgyro.setYAccelOffset(ay_offset);
			accelgyro.setZAccelOffset(az_offset);
			accelgyro.setXGyroOffset(gx_offset);
			accelgyro.setYGyroOffset(gy_offset);
			accelgyro.setZGyroOffset(gz_offset);
      break;
		}
	}
}

void setup_imu(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
 
  if(!accelgyro.testConnection()){
    DEBUG_SERIAL.println("ERROR CONNETCING TO IMU: CHECK WIRING");
    DEBUG_SERIAL.println("TRYING REBOOTING");
    reboot();
  }
  else{
    #ifdef DEBUG
      //Default setting dopo diverse calibrazioni
      accelgyro.setXAccelOffset(2643);
      accelgyro.setYAccelOffset(752);
      accelgyro.setZAccelOffset(1675);
      accelgyro.setXGyroOffset(29);
      accelgyro.setYGyroOffset(28);
      accelgyro.setZGyroOffset(35);
    #else
      //Funzione di calibrazione dell'IMU
      mpu_init();
    #endif
  }
}
