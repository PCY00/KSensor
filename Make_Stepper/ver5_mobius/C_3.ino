#include "m_3.h"

m_3 bottom(2, 3, 4);
m_3 top_1(5, 6, 7, 22);
m_3 top_2(8, 9, 10, 24);
m_3 top_3(11, 12, 13, 26);

void setup() {
  // put your setup code here, to run once:
  bottom.setRound(7200);
  top_1.setRound(7200);
  top_2.setRound(7200);
  top_3.setRound(7200);

  //10ms down
  bottom.setspeed(10);
  top_1.setspeed(1);
  top_2.setspeed(1);
  top_3.setspeed(1);

  //top motor setup
  top_1.TopSetup();
  top_2.TopSetup();
  top_3.TopSetup();

}

void loop() {
  if(Serial.available() > 0){
    int inputValue = Serial.parseInt();
    
  }
  bottom.moveto();
  top_1.moveto();
  top_2.moveto();
  top_3.moveto();
}
