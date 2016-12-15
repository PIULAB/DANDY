#include <Dynamixel_Serial.h> // MX all but 12

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
#define SPEED 50

String pc_data = "";
boolean pc_data_complete = false;

void setup() {
  Serial.begin(115200);

  Dynamixel.begin(57600, 2);
  Dynamixel.setMode(SERVO_ID_1, SERVO, 1024, 3072);
  Dynamixel.setMode(SERVO_ID_2, SERVO, 1592, 3413);
  Dynamixel.setMode(SERVO_ID_3, SERVO, 1592, 3413);
  Dynamixel.setMode(SERVO_ID_4, SERVO, 682, 2048);

  stopAll();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

int x, y;
Watch watch;

void loop() {
  int new_x = analogRead(A0);
  int new_y = analogRead(A1);

  if (abs(new_x - x) > 5 || abs(new_y - y) > 5) {
    x = new_x; y = new_y;
    if (watch.after(500)) {
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
      // Serial.println("{\"key\": \"" + key + "\", \"value\": \"" + value + "\" }");
      execute(key, value);
    }
    // clear the string:
    pc_data = "";
    pc_data_complete = false;
  }

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

void position_mx(unsigned int id, unsigned int position, unsigned int speed) {
//  Dynamixel.begin(59000, 2);
  Dynamixel.servo(id, position, speed);
  Serial.println("{\"mx64\": \".\"}");
}

void stopAll() {
  position_mx(SERVO_ID_1, 2048, SPEED);
  position_mx(SERVO_ID_2, 2048, SPEED);
  position_mx(SERVO_ID_3, 4095-2048, SPEED);
  position_mx(SERVO_ID_4, 2048, SPEED);
}
