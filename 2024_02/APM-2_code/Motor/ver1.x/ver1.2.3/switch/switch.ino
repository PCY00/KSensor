//module1
int switch1_30 = 23;
int switch1_60 = 24;
int switch1_90 = 25;

//module2
int switch2_30 = 27;
int switch2_60 = 28;
int switch2_90 = 29;

//module3
int switch3_30 = 31;
int switch3_60 = 32;
int switch3_90 = 33;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(switch1_30, INPUT_PULLUP);
  pinMode(switch1_60, INPUT_PULLUP);
  pinMode(switch1_90, INPUT_PULLUP);

  pinMode(switch2_30, INPUT_PULLUP);
  pinMode(switch2_60, INPUT_PULLUP);
  pinMode(switch2_90, INPUT_PULLUP);

  pinMode(switch3_30, INPUT_PULLUP);
  pinMode(switch3_60, INPUT_PULLUP);
  pinMode(switch3_90, INPUT_PULLUP);
  
}

void loop(){ 
  if (Serial1.available()) {
    String receivedString = Serial1.readStringUntil('\n');
    Serial.println(receivedString);
    int receivednum = receivedString.toInt();
    while(1){
      if(receivednum == 30){
        Serial.println("30_come");
        if(digitalRead(switch1_30)){
          Serial1.println("30");
          Serial.println("30");
          break;
        }
      }else if(receivednum == 60){
        Serial.println("60_come");
        if(digitalRead(switch1_60)){
          Serial1.println("60");
          Serial.println("60");
          break;
        }
      }else if(receivednum == 90){
        Serial.println("90_come");
        if(digitalRead(switch1_90)){
          Serial1.println("90");
          Serial.println("90");
          break;
        }
      }
    }
  }
}
