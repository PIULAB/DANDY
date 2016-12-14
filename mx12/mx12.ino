#include <Dynamixel_Serial.h> // MX all but 12
#include <DynamixelSerial1.h> // MX-12

#define SERVO_ID_1 0x01
#define SERVO_ID_2 0x02
#define SERVO_ID_3 0x03
#define SERVO_ID_4 0x04
#define SERVO_ID_5 0x05
#define SERVO_ID_6 0x06

#define SPEED 20

void setup() {
  Serial.begin(57600);
  
  DynamixelMX12.begin(850000, 2);
  DynamixelMX12.setAngleLimit(SERVO_ID_5, 1024, 3072);
  DynamixelMX12.setAngleLimit(SERVO_ID_6, 1024, 3072);

  Dynamixel.begin(59000, 2);
  Dynamixel.setMode(SERVO_ID_1, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_2, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_3, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_4, SERVO, 1024, 3072);

//  stopAll();
}

void loop() {
//  return;
  
  position_mx12(SERVO_ID_5, 2000, SPEED);
  position_mx12(SERVO_ID_6, 2000, SPEED);
  position_mx(SERVO_ID_1, 2000, SPEED);
  position_mx(SERVO_ID_2, 2000, SPEED);
  position_mx(SERVO_ID_3, 4095-2000, SPEED);
  position_mx(SERVO_ID_4, 2000, SPEED);
  delay(2000);
  Serial.println("uno");

  position_mx12(SERVO_ID_5, 2100, SPEED);
  position_mx12(SERVO_ID_6, 2100, SPEED);
  position_mx(SERVO_ID_1, 2100, SPEED);
  position_mx(SERVO_ID_2, 2000, SPEED);
  position_mx(SERVO_ID_3, 4095-2000, SPEED);
  position_mx(SERVO_ID_4, 2100, SPEED);
  delay(2000);
  
  Serial.println("due");
}

void position_mx12(unsigned int id, unsigned int position, unsigned int speed) {
  DynamixelMX12.begin(850000, 2);
  DynamixelMX12.moveSpeed(id, position, speed);
}

void position_mx(unsigned int id, unsigned int position, unsigned int speed) {
  Dynamixel.begin(57600, 2);
  Dynamixel.servo(id, position, speed);
  Serial.println("mx");
}

void stopAll() {
  position_mx12(SERVO_ID_5, 2048, SPEED);
  position_mx12(SERVO_ID_6, 2048, SPEED);
  position_mx(SERVO_ID_1, 2048, SPEED);
  position_mx(SERVO_ID_2, 2048, SPEED);
  position_mx(SERVO_ID_3, 4095-2048, SPEED);
  position_mx(SERVO_ID_4, 2048, SPEED);

}
