//bottom
int en_1 = 2;
int step_1 = 3;
int dir_1 = 4;

//top
int en_2 = 5;
int step_2 = 6;
int dir_2 = 7;

int en_3 = 8;
int step_3 = 9;
int dir_3 = 10;

int en_4 = 11;
int step_4 = 12;
int dir_4 = 13;

int swich_1 = 14;
int swich_2 = 15;
int swich_3 = 16;

void setup(){
  //bottom pin setting
  pinMode(en_1, OUTPUT);
  pinMode(step_1, OUTPUT);
  pinMode(dir_1, OUTPUT);

  ///top pin setting
  pinMode(en_2, OUTPUT);
  pinMode(step_2, OUTPUT);
  pinMode(dir_2, OUTPUT);

  pinMode(en_3, OUTPUT);
  pinMode(step_3, OUTPUT);
  pinMode(dir_3, OUTPUT);

  pinMode(en_4, OUTPUT);
  pinMode(step_4, OUTPUT);
  pinMode(dir_4, OUTPUT);

  //swich setting
  pinMode(swich_1, INPUT_PULLUP);
  pinMode(swich_2, INPUT_PULLUP);
  pinMode(swich_3, INPUT_PULLUP);

  // en = ONHIGH
  digitalWrite(en_1, HIGH);
  digitalWrite(en_2, HIGH);
  digitalWrite(en_3, HIGH);
  digitalWrite(en_4, HIGH);

  //top motor initialization
  digitalWrite(dir_2, LOW);
  digitalWrite(dir_3, LOW);
  digitalWrite(dir_4, LOW);
  while(1){
    if(digitalRead(swich_1) == LOW && digitalRead(swich_2) == LOW && digitalRead(swich_3) == LOW){
      break;
    }else{
      digitalWrite(step_2, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_2, LOW);
      delayMicroseconds(100);
    
      digitalWrite(step_3, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_3, LOW);
      delayMicroseconds(100);
    
      digitalWrite(step_4, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_4, LOW);
      delayMicroseconds(100);
    }
  }
  delay(1000);
}

void loop(){
  digitalWrite(dir_1, LOW);
  digitalWrite(dir_2, LOW);
  digitalWrite(dir_3, LOW);
  digitalWrite(dir_4, LOW);
  for(int i = 0; i < 6400; i++){
    digitalWrite(step_1, HIGH);
    delay(1);
    digitalWrite(step_1, LOW);
    delay(1);
  
    digitalWrite(step_2, HIGH);
    delay(1);
    digitalWrite(step_2, LOW);
    delay(1);
  
    digitalWrite(step_3, HIGH);
    delay(1);
    digitalWrite(step_3, LOW);
    delay(1);
  
    digitalWrite(step_4, HIGH);
    delay(1);
    digitalWrite(step_4, LOW);
    delay(1); 
  }
  delay(1);
  
  digitalWrite(dir_1, HIGH);
  digitalWrite(dir_2, HIGH);
  digitalWrite(dir_3, HIGH);
  digitalWrite(dir_4, HIGH);
  for(int i = 0; i < 6400; i++){
    digitalWrite(step_1, HIGH);
    delay(1);
    digitalWrite(step_1, LOW);
    delay(1);
  
    digitalWrite(step_2, HIGH);
    delay(1);
    digitalWrite(step_2, LOW);
    delay(1);
  
    digitalWrite(step_3, HIGH);
    delay(1);
    digitalWrite(step_3, LOW);
    delay(1);
  
    digitalWrite(step_4, HIGH);
    delay(1);
    digitalWrite(step_4, LOW);
    delay(1); 
  }
}
