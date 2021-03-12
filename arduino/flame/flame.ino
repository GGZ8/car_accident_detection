int flame = A15;
int light = A14;
int flame_val;
int light_val;
long timer = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(flame, INPUT);
  pinMode(light, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - timer > 200){
    timer = millis();
    flame_val = analogRead(flame);
    light_val = analogRead(light); 
    Serial.print(flame_val);
    Serial.print("\t");
    Serial.println(light_val);
  }

}
