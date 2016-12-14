
#include <Dynamixel_Serial.h>

#define SERVO_ID_1 0x01
#define SERVO_ID_2 0x02
#define SERVO_ID_3 0x03
#define SERVO_ID_4 0x04
#define SERVO_ID_5 0x05
//#define SERVO_ID_6 0x06

#define SERVO_ControlPin 0x02
#define SERVO_Baudrate 57600

#define CW_LIMIT 0x3ff
#define CCW_LIMIT 0xbff
//#define CW_LIMIT 0x0
//#define CCW_LIMIT 0xfff

String pc_data = "";
boolean pc_data_complete = false;

void setup() {
  Serial.begin(115200);
  Dynamixel.begin(SERVO_Baudrate, SERVO_ControlPin);
  delay(1000);

  Dynamixel.setMode(SERVO_ID_1, SERVO, CW_LIMIT, CCW_LIMIT);
  Dynamixel.setMode(SERVO_ID_2, SERVO, CW_LIMIT, CCW_LIMIT);
  Dynamixel.setMode(SERVO_ID_3, SERVO, CW_LIMIT, CCW_LIMIT);
  Dynamixel.setMode(SERVO_ID_4, SERVO, CW_LIMIT, CCW_LIMIT);
//  Dynamixel.setMode(SERVO_ID_5, SERVO, CW_LIMIT, CCW_LIMIT);
//  Dynamixel.setMode(SERVO_ID_6, SERVO, CW_LIMIT, CCW_LIMIT);

  Dynamixel.ledState(SERVO_ID_1, ON);

  Serial.println("{}");
//  Dynamixel.servo(SERVO_ID_3, CW_LIMIT, 0x30);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

int x, y;
void loop() {
  int new_x = analogRead(A0);
  int new_y = analogRead(A1);

  if (abs(new_x - x) > 5 || abs(new_y - y) > 5) {
    x = new_x; y = new_y;
    Serial.println("{\"x\": " + String(x) + ", \"y\": " + String(y) + "}");
  }

  // this handles a simple protocol 'key:value\n'
  if (pc_data_complete) {
    int sep = pc_data.indexOf(':');
    if (sep == -1) {
      Serial.println("{\"error\": \"no key terminator\"}");
    } else {
      String key = pc_data.substring(0, sep);
      String value = pc_data.substring(sep+1);
      Serial.println("{\"key\": \"" + key + "\", \"value\": \"" + value + "\" }");
      execute(key, value);
    }
    // clear the string:
    pc_data = "";
    pc_data_complete = false;
  }

  check_serial();
}

unsigned int speed_1 = 30;
unsigned int speed_2 = 30;
unsigned int speed_3 = 30;
unsigned int speed_4 = 0;
unsigned int limit_1 = CW_LIMIT;
unsigned int limit_2 = CW_LIMIT;
unsigned int limit_3 = CCW_LIMIT;
unsigned int limit_4 = CW_LIMIT;
void execute(String key, String value) {
  if (key == "angle_A") {
    limit_1 = abs(value.toInt());
    Dynamixel.servo(SERVO_ID_1, limit_1, speed_1);
    Serial.println("{\"debug\": \"" + String(speed_1) + "\"}");
  } else if (key == "angle_B") {
    limit_2 = abs(value.toInt());
    Dynamixel.servo(SERVO_ID_2, limit_2, speed_2);
    Dynamixel.servo(SERVO_ID_3, abs(4096 - limit_2), speed_2);
    Serial.println("{\"debug\": \"" + String(speed_2) + "\"}");
  } else if (key == "angle_C") {
    limit_3 = abs(value.toInt());
    Dynamixel.servo(SERVO_ID_4, limit_3, speed_3);
    Serial.println("{\"debug\": \"" + String(speed_3) + "\"}");
  } else if (key == "speed_A") {
    speed_1 = abs(value.toInt());
    Dynamixel.servo(SERVO_ID_1, limit_1, speed_1);
    Serial.println("{\"debug\": \"" + String(speed_1) + "\"}");
  } else if (key == "speed_B") {
    speed_2 = speed_3 = abs(value.toInt());
    Dynamixel.servo(SERVO_ID_2, limit_2, speed_2);
    Dynamixel.servo(SERVO_ID_3, limit_3, speed_3);
    Serial.println("{\"debug\": \"" + String(speed_2) + "\"}");
  } else if (key == "speed_C") {
    speed_4 = abs(value.toInt());
    Dynamixel.servo(SERVO_ID_4, limit_4, speed_4);
    Serial.println("{\"debug\": \"" + String(speed_4) + "\"}");
  } else if (key == "speed_D") {
//    speed = abs(value.toInt());
//    id = SERVO_ID_4;
//    Dynamixel.servo(id, limit, speed);
//    Serial.println("{\"debug\": \"" + String(speed_4) + "\"}");
  } else if (key == "direction_A") {
    if (value == "CCW") {
      limit_1 = CCW_LIMIT;
    } else {
      limit_1 = CW_LIMIT;
    }
    Serial.println("{\"limit\":\"" + String(limit_1) + "\"}");
    Dynamixel.servo(SERVO_ID_1, limit_1, speed_1);
  } else if (key == "direction_B") {
    if (value == "CCW") {
      limit_2 = CCW_LIMIT;
      limit_3 = CW_LIMIT;
    } else {
      limit_2 = CW_LIMIT;
      limit_3 = CCW_LIMIT;
    }
    Serial.println("{}");
    Dynamixel.servo(SERVO_ID_2, limit_2, speed_2);
    Dynamixel.servo(SERVO_ID_3, limit_3, speed_3);
  } else if (key == "direction_C") {
    if (value == "CCW") {
      limit_4 = CCW_LIMIT;
    } else {
      limit_4 = CW_LIMIT;
    }
    Serial.println("{}");
    Dynamixel.servo(SERVO_ID_4, limit_4, speed_4);
  } else if (key == "direction_D") {
//    limit = value == "CCW" ?CCW_LIMIT :CW_LIMIT;
//    id = SERVO_ID_4;
    Serial.println("{}");
//    Dynamixel.servo(id, limit, speed);
  }
}

// TODO check excess size of pc_data
void check_serial() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar != '\n') {
      pc_data += inChar;
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      pc_data_complete = true;
      break;
    }
  }
}



