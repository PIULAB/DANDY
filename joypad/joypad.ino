#include <Dynamixel_Serial.h>

#define SERVO_ID_1 0x01

#define SERVO_ControlPin 0x02
#define SERVO_Baudrate 57600

#define CW_LIMIT_1 1024
#define CCW_LIMIT_1 3072

String pc_data = "";
boolean pc_data_complete = false;

void setup() {
  Serial.begin(115200);
  Dynamixel.begin(SERVO_Baudrate, SERVO_ControlPin);
  delay(1000);

  Dynamixel.setMode(SERVO_ID_1, SERVO, CW_LIMIT_1, CCW_LIMIT_1);

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

unsigned int speed_1 = 0;
unsigned int limit_1 = CW_LIMIT_1;
void execute(String key, String value) {
  if (key == "speed_A") {
    speed_1 = abs(value.toInt());
    Serial.println("{\"debug\": \"" + String(speed_1) + "\"}");
  } else if (key == "direction_A") {
    if (value == "CCW") {
      limit_1 = CCW_LIMIT_1;
      Serial.println("{\"debug\": \"ccw\"}");
    } else {
      limit_1 = CW_LIMIT_1;
      Serial.println("{\"debug\": \"cw\"}");
    }
  }
  Dynamixel.servo(SERVO_ID_1, limit_1, speed_1);
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



