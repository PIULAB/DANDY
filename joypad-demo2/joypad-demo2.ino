#include <Dynamixel_Serial.h> // MX all but 12
#include <DynamixelSerial1.h> // MX-12

class Watch {
    long t0;
  public:
    Watch ();
    boolean after (long ms) {return millis() - t0 >= ms;}
    bool reset () {t0 = millis(); return true;}
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

#define still_size 100
#define lane_size 100

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

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

enum States {INIT, DISH_FREE, DISH_STILL, DISH_TO_EAT, EAT_FREE};

int x, y, prev_x, prev_y;
Watch joy_on_dish_watch,
      joy_on_eat_watch,
      joy_still_watch,
      dish_to_eat_watch;
bool p_joy_on_dish,
     p_joy_on_eat,
     p_joy_still,
     joy_on_dish,
     joy_on_eat,
     joy_still,
     joy_in_lane;

States state = INIT;
String s;

void loop() {
  x = analogRead(A0);
  y = analogRead(A1);

//  if (abs(x - prev_x) > 5 || abs( y - prev_y) > 5) {
//    prev_x = x;
//    prev_y = y;
//    once_print(String(x) + "," + String(y));
//  }

  s = state == INIT
    ?"INIT" :state == DISH_FREE
      ?"DISH_FREE" :state == DISH_STILL
        ?"DISH_STILL" :state == DISH_TO_EAT
          ?"DISH_TO_EAT" :state == EAT_FREE
            ?"EAT_FREE" :"";
  once_print(s);

  switch (state) {
    case INIT: {
      execute("angle_A", "3072");
      execute("angle_B", "2409");
      execute("angle_C", "3190");

      if (joy_on_dish && joy_on_dish_watch.after(2000)) {
        state = DISH_FREE;
      }
      if (joy_on_eat && joy_on_eat_watch.after(2000)) {
        dish_to_eat_watch.reset();
        state = DISH_TO_EAT;
      }
      break;
    }
    case DISH_FREE: {
      execute("angle_A", "2341");
      execute("angle_B", "2409");
      execute("angle_C", "3190");
      execute("angle_D", String(map(x, 100, 900, 2560, 1536)));
      execute("angle_E", String(map(y, 100, 900, 2560, 1536)));
      if (joy_still && joy_still_watch.after(2000)) {
        state = DISH_STILL;
      }
      break;
    }
    case DISH_STILL: {
      if (!joy_in_lane) {
        state = DISH_FREE;
      }
      if (joy_on_eat && joy_on_eat_watch.after(2000)) {
        state = DISH_TO_EAT;
        dish_to_eat_watch.reset();
      }
      break;
    }
    case DISH_TO_EAT: {  // state active during the journey from dish to eat
      execute("angle_A", "1518");
      execute("angle_B", "2038");
      execute("angle_C", "3051");

      unsigned int p = readPosition(SERVO_ID_1);
      execute("angle_E", String(map(map(p, 4095 - 2341, 4095 - 1518, 530, 730), 100, 900, 1024, 3072)));

      if (dish_to_eat_watch.after(5000)) {
        state = EAT_FREE;
      }
      break;
    }
    case EAT_FREE: {
      // free spoon
      execute("angle_D", String(map(x, 100, 900, 2560, 1536)));
      execute("angle_E", String(map(y, 500, 1300, 2560, 1536))); // remapped to make spoon perpendicualr to gravity

      if (joy_on_dish && joy_on_dish_watch.after(2000)) {
        state = DISH_FREE;
      }
      break;
    }
  }

  p_joy_on_dish = joy_on_dish;
  p_joy_on_eat = joy_on_eat;
  p_joy_still = joy_still;

  joy_on_dish = y > 850;
  joy_on_eat = y < 100;
  joy_still = y > 512 - still_size && y < 512 + still_size &&  x > 512 - still_size && x < 512 + still_size;
  joy_in_lane = x > 512 - lane_size && x < 512 + lane_size && y < 512 + lane_size;

  p_joy_on_dish != joy_on_dish && joy_on_dish_watch.reset();
  p_joy_on_eat != joy_on_eat && joy_on_eat_watch.reset();
  p_joy_still != joy_still && joy_still_watch.reset();

//  if (eat_gesture_active) {
//    unsigned int p = readPosition(SERVO_ID_1);
//    execute("angle_E", String(map(map(p, 4095 - 2341, 4095 - 1518, 530, 730), 100, 900, 1024, 3072)));
//  }
}

String once_previous = "";
void once_print(String s) {
  if (s == once_previous)
    return;
  once_previous = s;
  Serial.println(s);
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

void position_mx12(unsigned int id, unsigned int position, unsigned int speed) {
  speed = speed / 2;
  if (speed == 0)
    speed = 1;
  DynamixelMX12.begin(850000, 2);
  DynamixelMX12.moveSpeed(id, position, speed);
}

void position_mx(unsigned int id, unsigned int position, unsigned int speed) {
  if (speed == 0)
    speed = 1;
  Dynamixel.begin(59000, 2);
  Dynamixel.servo(id, position, speed);
  Serial.println("{\"mx64\": \".\"}");
}

unsigned int readPosition(unsigned int id) {
  Dynamixel.begin(59000, 2);
  return Dynamixel.readPosition(id);
  Serial.println("{\"mx64\": \".\"}");
}
