int driverPUL = 7;    // PUL- pin
int driverDIR = 6;    // DIR- pin
int delayspeed = 16000;

// Variables
boolean setdir = HIGH; // Set Direction

// Interrupt Handler
void revmotor (){
  setdir = !setdir;
}


void setup() {
  Serial.begin(9600);
  pinMode (driverPUL, OUTPUT);
  pinMode (driverDIR, OUTPUT);
  digitalWrite(driverDIR,setdir);
}

void loop() {
  if(Serial.available() > 0){
    char input = Serial.read();

    if(input == '1'){
      delayspeed = 101;
    }else if(input == '2'){
      delayspeed = 5000;
    }else if(input == '3'){
      delayspeed = 10000;
    }else if(input == '4'){
      for(int i = 0; i < 400; i++){
        digitalWrite(driverPUL,HIGH);
        delayMicroseconds(delayspeed);
        digitalWrite(driverPUL,LOW);
        delayMicroseconds(delayspeed);
      }
    }
  }
}
