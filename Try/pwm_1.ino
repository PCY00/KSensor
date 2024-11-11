#define fanCount 3

const uint8_t pwmPins[fanCount] = {8,9,10};
const uint8_t minPwm = 30;
uint8_t currentPWMs[fanCount] = {30,30,30};

bool startReceived = false;

void setup() {
  Serial.begin(9600);

  for(int i = 0; i < fanCount; i++){
    pinMode(pwmPins[i], OUTPUT);
    analogWrite(pwmPins[i], minPwm);
  }

  delay(100);
}

void loop() {

  if(Serial.available() > 0){
    String input = Serial.readStringUntil('\n');
    input.trim();

    if(input.startsWith("M1_P")){
      int fanIndex = input.substring(4,5).toInt() - 1;
      int fanSpeed = input.substring(6).toInt();
      if(fanSpeed == 1000){
        currentPWMs[0] = 200;
        currentPWMs[1] = 200;
        currentPWMs[2] = 200;
      }else if(fanSpeed == 2000){
        currentPWMs[0] = 0;
        currentPWMs[1] = 0;
        currentPWMs[2] = 0;
      }else{
        currentPWMs[0] = 100;
        currentPWMs[1] = 100;
        currentPWMs[2] = 100;
      }
    }else if(input == "start"){
      startReceived = true;
    }
  }
  if(startReceived == true){
    for(int i = 0; i < fanCount; i++){
      Serial.print(currentPWMs[i]);
      if(i < fanCount - 1){
        Serial.print(",");
      }
    }
    Serial.println();
  }

  for(int j = 0; j < 3; j++){
   analogWrite(pwmPins[j], currentPWMs[j]);
  }
}
