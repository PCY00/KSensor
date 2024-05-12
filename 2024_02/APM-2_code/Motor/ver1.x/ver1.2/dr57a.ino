#include "misumi.h"

#define RESOLUTION_200 200
#define RESOLUTION_400 400
#define RESOLUTION_800 800
#define RESOLUTION_1600 1600
#define RESOLUTION_3200 3200
#define RESOLUTION_6400 6400
#define RESOLUTION_12800 12800
#define RESOLUTION_25600 25600

misumi test_h(6,7,RESOLUTION_400);

void setup(){
  test_h.setSpeed(5000);
}

void loop(){
  test_h.move_motor_angle(true);
}
