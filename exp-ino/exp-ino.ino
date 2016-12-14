#include <Dynamixel_Serial.h> // MX all but 12

#define SERVO_ID_1 0x01
#define SERVO_ID_2 0x02
#define SERVO_ID_3 0x03
#define SERVO_ID_4 0x04
#define SPEED 50

void setup() {
  Serial.begin(57600);

  Dynamixel.begin(59000, 2);
  Dynamixel.setMode(SERVO_ID_1, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_2, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_3, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_4, SERVO, 1024, 3072);

  stopAll();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

int state = 0;
unsigned int p;
unsigned int target = 2000;
unsigned int start;
long t0 = millis();

void loop() {
  if (state == 0) {
    start = readPosition(SERVO_ID_4);
    Serial.println(start);
    if (abs(start - 1000) < 20 && millis() - t0 > 5000) 
      state = 1;
  } else if (state == 1) {
    p = readPosition(SERVO_ID_4);
    unsigned int value = f(start, target, p, SPEED);
    position_mx(SERVO_ID_4, target, value);
    if (p > 1200)
      state = 2;
  } else if (state == 2) {
    
  }
}

unsigned int f(unsigned int start, unsigned int target, unsigned int p, unsigned int topSpeed) {
  float slice = 50;
  float x = abs(p - start) + 1;
  unsigned int value = (unsigned int) (topSpeed / slice * x);
  if (value == 0)
    value = 1;
  Serial.print(x);
  Serial.print(", ");
  Serial.print(p);
  Serial.print(", ");
  Serial.println(value);
  Serial.print(", ");
  Serial.println(topSpeed / slice * x);
  
  if (x > slice)
    return topSpeed;
  else
    return value;
}

void position_mx(unsigned int id, unsigned int position, unsigned int speed) {
  Dynamixel.begin(59000, 2);
  Dynamixel.servo(id, position, speed);
  Serial.print(".");
}

unsigned int readPosition(unsigned int id) {
  Dynamixel.begin(59000, 2);
  return Dynamixel.readPosition(id);
  Serial.print("*");
}

void stopAll() {
  position_mx(SERVO_ID_1, 2048, SPEED);
  position_mx(SERVO_ID_2, 2048, SPEED);
  position_mx(SERVO_ID_3, 4095-2048, SPEED);
  position_mx(SERVO_ID_4, 1000, SPEED);
}
