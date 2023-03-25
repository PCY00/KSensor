#define ContinueMode 0
#define PollingMode 1

char buffer[50];
unsigned long past = 0;
bool flag = false;
char co_print[20];
float co_ppm = 0.0;
char NO2_data[50];    //Hardware-designed no2 data archiving
char SO2_data[50];    //Hardware-designed so2 data archiving
char NO2_print[7];    //print no2 string
char SO2_print[7];    //print so2 string
float so2_ppm = 0.0;
float no2_ppm = 0.0;
char degree_print[7];
float degree = 0.0;
float direct[] = {3.84,1.98,2.25,0.41,0.45,0.32,0.90,0.62,1.40,1.19,3.08,2.93,4.62,4.04,4.33,3.43};
uint8_t wind_print = 0;

void set_Time(){
  unsigned long now = millis();
  if(now - past >= 5000) {
    past = now;
    flag = true;
  }
}

void windSpeed(){
  uint8_t wind_val = analogRead(A1);
  float wind_outvoltage = wind_val * (5.0 / 1023.0);
  wind_print = 6 * wind_outvoltage;
}

void windDirection(){
  uint8_t wind_direction_val = analogRead(A2);
  float wind_direction_outvoltage = wind_direction_val * 5 / 1023.0;

  for(uint8_t j =0; j < 16; j++){
    if(direct[j]-0.02 < wind_direction_outvoltage && direct[j]+0.02 > wind_direction_outvoltage){
       degree = j * 22.5;
       break;
    }
  }
}

void co(){
  uint8_t co_val = analogRead(0);
  co_ppm = (float)co_val / 1000;
}

void SN_Command(){
  #if PollingMode
    Serial1.write('\r');
    Serial2.write('\r');
    delay(1000);
  #else
    delay(100);
  #endif
}

void SN_data_read(){
  int i = 0;
  while (Serial1.available()) {
    NO2_data[i] = Serial1.read();
    i++;
  }

  i = 0;
  
  while (Serial2.available()) { // read from AGSM port, send to Serial port to interupt continuous output send 'c''/r' without line ending, may have to send more than once.
    SO2_data[i] = Serial2.read();
    i++;
  }
}

void SN_ppm_get(String str_so2, String str_no2){
  for(uint8_t j = 13; j < 16; j++){
     str_so2 += SO2_data[j];
     str_no2 += NO2_data[j];
  }
  //The part that changes String to Int and converts it to float to express it in ppm.
  so2_ppm = (float)str_so2.toInt() / 1000;
  no2_ppm = (float)str_no2.toInt() / 1000;
}

void sensor_convert(){
  dtostrf(so2_ppm, 5, 3, SO2_print);
  dtostrf(no2_ppm, 5, 3, NO2_print);
  dtostrf(co_ppm, 4, 2, co_print);
  dtostrf(degree, 5, 2, degree_print);
}

void setup(){
  Serial.begin(57600);
  Serial1.begin(9600);        //NO2
  Serial2.begin(9600);        //SO2
  delay(1000);
  
}

void loop(){
  set_Time();
  
  String str_so2_data = "";
  String str_no2_data = "";
  
  SN_Command();
  windSpeed();
  windDirection();
  co();
  SN_data_read();

  if(flag == true){
    SN_ppm_get(str_so2_data, str_no2_data);
    sensor_convert();
    sprintf(buffer,"%s ,%s ,%s ,%2d ,%s",SO2_print, NO2_print, co_print, wind_print, degree_print);
    Serial.println(buffer);

    flag = false;
  }
}
