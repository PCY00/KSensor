#include "m_2.h"

m_2 bottom(2, 3, 4);
m_2 top_1(5, 6, 7,22);
m_2 top_2(8, 9, 10,24);
m_2 top_3(11, 12, 13,26);

void setup() {
  // put your setup code here, to run once:
  bottom.setRound(7200);
  top_1.setRound(7200);
  top_2.setRound(7200);
  top_3.setRound(7200);

  bottom.setspeed(1);
  top_1.setspeed(1);
  top_2.setspeed(1);
  top_3.setspeed(1);

}

void loop() {
  bottom.moveto();
  top_1.moveto();
  top_2.moveto();
  top_3.moveto();

}
