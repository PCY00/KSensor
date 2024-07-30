void setup() {
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  Serial.begin(9600);

}

void loop() {
  if(Serial.available()){
    String input = Serial.readStringUntil('\n');
    if(input == "start"){
      digitalWrite(2,HIGH);
      Serial.println("in");
    }else{
      digitalWrite(2,LOW);
      Serial.println("in");
    }
  }

}
