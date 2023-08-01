int en_B =  2;
int step_B = 3;
int dir_B = 4;

int en_T1 = 5;
int step_T1 = 6;
int dir_T1 = 7;

int en_T2 = 8;
int step_T2 = 9;
int dir_T2 = 10;

int en_T3 = 11;
int step_T3 = 12;
int dir_T3 = 13;

int switch_1 = 22;
int switch_2 = 24;
int switch_3 = 26;

//모터 속도 단위 마이크로초
#define speeds_B 1000
#define speeds_T 100
#define spedds_T_setup 100
/*
 * 여기서 모터 한바퀴는 1600스텝이다.
 * bottom일 경우 APM 한바퀴가 몇인지 알아야함
 * top일 경우 초기화후 위로 올라가는게 몇 까지 올라가야하는지 알아야함
 * 
 * 위를 토대로 rounds를 설정해줘야함
 * 기본값으로 4800이 한바퀴라고 설정을 해둠 (조정필요)
*/
#define rounds_B 7200
#define rounds_T 7200

//function
//setup top motor postion
void motorTopSetup(int sw, int M_step);

//setting pinMode bottom & top
void setting(int M_en, int M_step, int M_dir);
void setting(int M_en, int M_step, int M_dir, int sw);

//move to motor -> steppin, dirpin, motor speed, 1step check, count sppin
void M_run(int M_step, int M_dir, int M_speeds, int M_step_count, int M_rounds);

//1step_count
int step_B_count = 0;
int step_T1_count = 0;
int step_T2_count = 0;
int step_T3_count = 0;

void setup(){
  //bottom setting
  setting(en_B, step_B, dir_B);
  //top setting
  setting(en_T1, step_T1, dir_T1, switch_1);
  setting(en_T2, step_T2, dir_T2, switch_2);
  setting(en_T3, step_T3, dir_T3, switch_3);

  //top motor setup
  motorTopSetup(switch_1, step_T2);
  motorTopSetup(switch_2, step_T3);
  motorTopSetup(switch_3, step_T1);
  
  delay(1000);
}

void loop(){
  //bottom control
  M_run(step_B, dir_B, speeds_B, step_B_count, rounds_B);
  
  //top control
  M_run(step_T1, dir_T1, speeds_T, step_T1_count, rounds_T);
  M_run(step_T2, dir_T2, speeds_T, step_T2_count, rounds_T);
  M_run(step_T3, dir_T3, speeds_T, step_T3_count, rounds_T);
}

void motorTopSetup(int sw, int M_step){
  if(digitalRead(sw) == HIGH){
    digitalWrite(M_step, HIGH);
    delayMicroseconds(spedds_T_setup);
    digitalWrite(M_step, LOW);
    delayMicroseconds(spedds_T_setup);
  }
}
void setting(int M_en, int M_step, int M_dir){
  pinMode(M_en, OUTPUT);
  pinMode(M_step, OUTPUT);
  pinMode(M_dir, OUTPUT);

  digitalWrite(M_en, LOW);
  digitalWrite(M_dir, LOW);
}

void setting(int M_en, int M_step, int M_dir, int sw){
  pinMode(M_en, OUTPUT);
  pinMode(M_step, OUTPUT);
  pinMode(M_dir, OUTPUT);

  pinMode(sw, INPUT_PULLUP);

  digitalWrite(M_en, LOW);
  digitalWrite(M_dir, LOW);
}
void M_run(int M_step, int M_dir, int M_speeds, int M_step_count, int M_rounds){
  if(M_step_count == M_rounds){
    if(digitalRead(M_dir) == HIGH){
       digitalWrite(M_dir, LOW);
       M_step_count = 0;
    }else{
        digitalWrite(M_dir, HIGH);
        M_step_count = 0;
    }
  }else if(M_step_count == 0){
    digitalWrite(M_dir, HIGH);
  }
  
  digitalWrite(M_step, HIGH);
  delayMicroseconds(M_speeds);
  digitalWrite(M_step, LOW);
  delayMicroseconds(M_speeds);

  M_step_count++;
}
