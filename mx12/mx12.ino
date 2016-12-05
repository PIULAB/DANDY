#include <Dynamixel_Serial.h> // MX all but 12
#include <DynamixelSerial1.h> // MX-12

#define SERVO_ID_1 0x01
#define SERVO_ID_2 0x02
#define SERVO_ID_3 0x03
#define SERVO_ID_4 0x04
#define SERVO_ID_5 0x05
#define SERVO_ID_6 0x06

void setup() {
}

void loop() {
  position_mx12(SERVO_ID_5, 1000, 3000, 1000, 1023);
  position_mx12(SERVO_ID_6, 1000, 3000, 1000, 1023);
  position_mx(SERVO_ID_1, 1000, 3000, 1000, 1023);
  position_mx(SERVO_ID_2, 1000, 3000, 1000, 1023);
  position_mx(SERVO_ID_3, 1000, 3000, 4095-1000, 1023);
  position_mx(SERVO_ID_4, 1000, 3000, 1000, 1023);
  delay(4000);

  position_mx12(SERVO_ID_5, 1000, 3000, 3000, 1023);
  position_mx12(SERVO_ID_6, 1000, 3000, 3000, 1023);
  position_mx(SERVO_ID_1, 1000, 3000, 3000, 1023);
  position_mx(SERVO_ID_2, 1000, 3000, 3000, 1023);
  position_mx(SERVO_ID_3, 1000, 3000, 4095-3000, 1023);
  position_mx(SERVO_ID_4, 1000, 3000, 3000, 1023);
  delay(4000);
}

void position_mx12(unsigned int id, unsigned int cw_limit, unsigned int ccw_limit, unsigned int position, unsigned int speed) {
  DynamixelMX12.begin(850000, 2);
//  DynamixelMX12.setAngleLimit(id, cw_limit, ccw_limit);
  DynamixelMX12.moveSpeed(id, position, speed);
}

void position_mx(unsigned int id, unsigned int cw_limit, unsigned int ccw_limit, unsigned int position, unsigned int speed) {
  Dynamixel.begin(59000, 2);
  Dynamixel.setMode(id, SERVO, cw_limit, ccw_limit);
  Dynamixel.servo(id, position, speed);
}

