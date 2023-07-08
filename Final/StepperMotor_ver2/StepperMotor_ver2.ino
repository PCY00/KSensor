int en_1 = 2;
int step_1 = 3;
int dir_1 = 4;

int en_2 = 5;
int step_2 = 6;
int dir_2 = 7;

int en_3 = 8;
int step_3 = 9;
int dir_3 = 10;

void setup(){
  pinMode(en_1, OUTPUT);
  pinMode(step_1, OUTPUT);
  pinMode(dir_1, OUTPUT);

  pinMode(en_2, OUTPUT);
  pinMode(step_2, OUTPUT);
  pinMode(dir_2, OUTPUT);

  pinMode(en_3, OUTPUT);
  pinMode(step_3, OUTPUT);
  pinMode(dir_3, OUTPUT);


  digitalWrite(en_1, LOW);
  digitalWrite(step_1, LOW);

  digitalWrite(en_2, LOW);
  digitalWrite(step_2, LOW);

  digitalWrite(en_3, LOW);
  digitalWrite(step_3, LOW);

  delay(1000);
}

void loop(){
  digitalWrite(dir_1, LOW);
  digitalWrite(dir_2, LOW);
  digitalWrite(dir_3, LOW);
  for(uint16_t i = 0; i < 1600; i++){
    //1
    digitalWrite(step_1, HIGH);
    delay(1);
    digitalWrite(step_1, LOW);
    delay(1);
    //2
    digitalWrite(step_2, HIGH);
    delay(1);
    digitalWrite(step_2, LOW);
    delay(1);
    //3
    digitalWrite(step_3, HIGH);
    delay(1);
    digitalWrite(step_3, LOW);
    delay(1);
  }
  delay(1000);

  digitalWrite(dir_1, HIGH);
  digitalWrite(dir_2, HIGH);
  digitalWrite(dir_3, HIGH);
  for(uint16_t i = 0; i < 1600; i++){
    //1
    digitalWrite(step_1, HIGH);
    delay(1);
    digitalWrite(step_1, LOW);
    delay(1);
    //2
    digitalWrite(step_2, HIGH);
    delay(1);
    digitalWrite(step_2, LOW);
    delay(1);
    //3
    digitalWrite(step_3, HIGH);
    delay(1);
    digitalWrite(step_3, LOW);
    delay(1);
  }
  delay(1000);
}
