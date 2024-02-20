#include "misumi.h"

misumi test_h(6,7);

void setup(){
  test_h.setSpeed(5000);
}

void loop(){
  test_h.test_function(400);
  test_h.revmotor();
}
