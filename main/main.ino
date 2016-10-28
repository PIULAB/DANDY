
#include <Dynamixel_Serial.h>

#define SERVO_ID_1 0x01             // ID of which we will set Dynamixel too
#define SERVO_ID_2 0x02
#define SERVO_ID_3 0x03
#define SERVO_ID_4 0x04
#define SERVO_ID_5 0x05

#define SERVO_ControlPin 0x02       // Control pin of buffer chip, NOTE: this does not matter becasue we are not using a half to full contorl buffer.
#define SERVO_SET_Baudrate 57600    // Baud rate speed which the Dynamixel will be set too (57600)

#define CW_LIMIT_ANGLE_1 1024        // lowest clockwise angle is 1, as when set to 0 it set servo to wheel mode
#define CCW_LIMIT_ANGLE_1 3072      // Highest anit-clockwise angle is 0XFFF, as when set to 0 it set servo to wheel mode

#define CW_LIMIT_ANGLE_4 1024
#define CCW_LIMIT_ANGLE_4 3072


#define SPEED_1 0x30
#define ACCEL_GOAL_1 25
#define CARROT_1 25
unsigned int grado_1 = 2048;

#define SPEED_4 0x30
#define ACCEL_GOAL_4 25
#define CARROT_4 25
unsigned int grado_4 = 2048;

int asse_x = A0;  // Pin analogico del potenziometro asse x
int asse_y = A1;  // Pin analogico del potenziometro asse y


void setup() {
  Serial.begin(115200);
  //Ax12.begin(SERVO_SET_Baudrate, SERVO_ControlPin);
  Dynamixel.begin(SERVO_SET_Baudrate, SERVO_ControlPin);
  
  delay(1000);

  pinMode(asse_x, INPUT);
  pinMode(asse_y, INPUT);

  Dynamixel.setMode(SERVO_ID_1, SERVO, CW_LIMIT_ANGLE_1, CCW_LIMIT_ANGLE_1);
  Dynamixel.ledState(SERVO_ID_1, OFF);
  //Dynamixel.setAccelerationGoal( SERVO_ID_1, ACCEL_GOAL_1 );
  Dynamixel.servo( SERVO_ID_1, grado_1, SPEED_1 );

  Dynamixel.setMode(SERVO_ID_4, SERVO, CW_LIMIT_ANGLE_4, CCW_LIMIT_ANGLE_4);
  Dynamixel.ledState(SERVO_ID_4, OFF);
 // Dynamixel.setAccelerationGoal( SERVO_ID_4, ACCEL_GOAL_4 );
  Dynamixel.servo( SERVO_ID_4, grado_4, SPEED_4 );

  Serial.println("ready");
}

void loop() {
  
  unsigned int val_asse_x = abs(map(analogRead(asse_x), 0, 1023, CW_LIMIT_ANGLE_1, CCW_LIMIT_ANGLE_1));
  
  Serial.println(val_asse_x);
  Dynamixel.servo( SERVO_ID_1, val_asse_x, SPEED_1 );
  
  delay(100);
  
  unsigned int val_asse_y = abs(map(analogRead(asse_y), 0, 1023, CW_LIMIT_ANGLE_4, CCW_LIMIT_ANGLE_4));
  
  Serial.println(val_asse_y);
  Dynamixel.servo( SERVO_ID_4, val_asse_y, SPEED_4 );
  
  delay(100);
}

