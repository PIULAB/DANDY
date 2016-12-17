#include <Dynamixel_Serial.h> // MX all but 12
#include <DynamixelSerial1.h> // MX-12

class Watch {
    long t0;
  public:
    Watch ();
    boolean after (long ms) {return millis() - t0 >= ms;}
    void reset () {t0 = millis();}
};

Watch::Watch () {
  t0 = millis();
}

#define SERVO_ID_1 0x01
#define SERVO_ID_2 0x02
#define SERVO_ID_3 0x03
#define SERVO_ID_4 0x04
#define SERVO_ID_5 0x05
#define SERVO_ID_6 0x06
#define SPEED 20

String pc_data = "";
boolean pc_data_complete = false;

void setup() {
  Serial.begin(57600);

  DynamixelMX12.begin(850000, 2);
  DynamixelMX12.setAngleLimit(SERVO_ID_5, 1024, 3072); // wristh
  DynamixelMX12.setAngleLimit(SERVO_ID_6, 1024, 3072); // spoon

  Dynamixel.begin(59000, 2);
  Dynamixel.setMode(SERVO_ID_1, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_2, SERVO, 1024, 3072);  // 1592, 3413);
  Dynamixel.setMode(SERVO_ID_3, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_4, SERVO, 682, 2048);

  stopAll();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

int x, y;
Watch watch;
bool flag_front = false, flag_back = false, back_gesture_active = false;
Watch front_watch, back_watch;

void loop() {
  int new_x = analogRead(A0);
  int new_y = analogRead(A1);

  if (abs(new_x - x) > 5 || abs(new_y - y) > 5) {
    x = new_x; y = new_y;
    if (watch.after(100)) {
      watch.reset();
      Serial.println("{\"x\": " + String(x) + ", \"y\": " + String(y) + "}");
    }
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

  // gesture
  if (y > 850) {
    if (front_watch.after(2000)) {
      flag_front = true;
      Serial.println("{\"key\": \"FLAG_FRONT\", \"value\": \"TRUE\" }");
    }
  } else if (y < 100) {
    if (back_watch.after(2000)) {
      flag_back = true;
      Serial.println("{\"key\": \"FLAG_BACK\", \"value\": \"TRUE\" }");
    }
  } else {
    flag_front = false;
    flag_back = false;
    front_watch.reset();
    back_watch.reset();
  }

  // spoon free movement
  if (y >= 100 && y <=850) {
    execute("angle_D", String(map(x, 100, 900, 1536, 2560)));
    execute("angle_E", String(map(y, 100, 900, 1536, 2560)));
  }

  // front gesture
  if (flag_front) {
    execute("angle_A", "2341");
    execute("angle_B", "2409");
    execute("angle_C", "3190");
    flag_front = false;
    front_watch.reset();
    back_gesture_active = false;
  }

  // back gesture
  if (flag_back) {
    execute("angle_A", "1518");
    execute("angle_B", "2038");
    execute("angle_C", "3051");

    flag_back = false;
    back_watch.reset();
    back_gesture_active = true;
  }

//  if (back_gesture_active) {
//    unsigned int p = readPosition(SERVO_ID_1);
//    execute("angle_E", String(map(map(p, 4095 - 2341, 4095 - 1518, 530, 730), 100, 900, 1024, 3072)));
//  }

  check_serial();
}

unsigned int angle;
void execute(String key, String value) {
  if (key == "angle_A") {
    angle = abs(value.toInt());
    position_mx(SERVO_ID_1, abs(4095 - angle), SPEED);
  } else if (key == "angle_B") {
    angle = abs(value.toInt());
    position_mx(SERVO_ID_3, abs(4095 - angle), SPEED);
    position_mx(SERVO_ID_2, angle, SPEED);
  } else if (key == "angle_C") {
    angle = abs(value.toInt());
    position_mx(SERVO_ID_4, abs(4095 - angle), SPEED);
  } else if (key == "angle_D") {
    angle = abs(value.toInt());
    position_mx12(SERVO_ID_5, angle, SPEED);
  } else if (key == "angle_E") {
    angle = abs(value.toInt());
    position_mx12(SERVO_ID_6, angle, SPEED);
  }
}

// TODO check excess size of pc_data
void check_serial() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar != '\n') {
      pc_data += inChar;
    } else {
      pc_data_complete = true;
      break;
    }
  }
}

void position_mx12(unsigned int id, unsigned int position, unsigned int speed) {
  DynamixelMX12.begin(850000, 2);
  DynamixelMX12.moveSpeed(id, position, speed);
}

void position_mx(unsigned int id, unsigned int position, unsigned int speed) {
  Dynamixel.begin(59000, 2);
  Dynamixel.servo(id, position, speed);
  Serial.println("{\"mx64\": \".\"}");
}

unsigned int readPosition(unsigned int id) {
  Dynamixel.begin(59000, 2);
  return Dynamixel.readPosition(id);
  Serial.println("{\"mx64\": \".\"}");
}

void stopAll() {
  position_mx(SERVO_ID_1, 2048, SPEED);
  position_mx(SERVO_ID_2, 2048, SPEED);
  position_mx(SERVO_ID_3, 4095-2048, SPEED);
  position_mx(SERVO_ID_4, 2048, SPEED);
  position_mx12(SERVO_ID_5, 2048, SPEED);
  position_mx12(SERVO_ID_6, 2048, SPEED);
}
