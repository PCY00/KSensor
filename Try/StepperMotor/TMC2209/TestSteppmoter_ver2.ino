int en_1 = 2;
int step_1 = 3;
int dir_1 = 4;

void setup(){
  pinMode(en_1, OUTPUT);
  pinMode(step_1, OUTPUT);
  pinMode(dir_1, OUTPUT);


  digitalWrite(en_1, LOW);
  digitalWrite(step_1, LOW);

  delay(1000);
}

void loop(){
  digitalWrite(dir_1, LOW);
  for(uint16_t i = 0; i < 1600; i++){
    digitalWrite(step_1, HIGH);
    delay(1);
    digitalWrite(step_1, LOW);
    delay(1);
  }
  delay(1000);

  digitalWrite(dir_1, HIGH);
  for(uint16_t i = 0; i < 1600; i++){
    digitalWrite(step_1, HIGH);
    delay(1);
    digitalWrite(step_1, LOW);
    delay(1);
  }
  delay(1000);
}
