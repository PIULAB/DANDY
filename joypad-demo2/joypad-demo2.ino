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

enum States {INIT, FREE_SPOON, STILL_SPOON, EAT_GESTURE, DISH_GESTURE};  // TODO implement toString
String States_to_string(States s) {
  return s == FREE_SPOON ?"FREE_SPOON" :s == STILL_SPOON ?"STILL_SPOON" :s == EAT_GESTURE ?"EAT_GESTURE" :"???";
}

int x, y;
bool flag_dish = false, flag_eat = false, eat_gesture_active = false;
Watch dish_watch, eat_watch;
States state = INIT;

void loop() {
  x = analogRead(A0);
  y = analogRead(A1);

  switch (state) {
    case INIT: {
      execute("angle_A", "3072");
      execute("angle_B", "2409");
      execute("angle_C", "3190");

      if (joystick_on_dish && dish_watch.after(2000)) {
        state = DISH;
      }
      if (joystick_on_eat && eat_watch.after(2000)) {
        state = EAT;
      }
      break;
    }
    case DISH: {
      if (spoon_still && spoon_watch.after(1000)) {
        state = STILL_SPOON;
      }
      break;
    }
    case STILL_SPOON: {
      if (!move_outside_lane && joystick_on_eat && eat_watch.after(2000)) {
        state = EAT_GESTURE;
      }
      break;
    }
    case EAT: {
      break;
    }

  }

  // gesture
  if (y > 850) {
    if (dish_watch.after(2000)) {
      flag_dish = true;
      Serial.println("{\"key\": \"FLAG_DISH\", \"value\": \"TRUE\" }");
    }
  } else if (y < 100) {
    if (eat_watch.after(2000)) {
      flag_eat = true;
      Serial.println("{\"key\": \"FLAG_EAT\", \"value\": \"TRUE\" }");
    }
  } else {
    flag_dish = false;
    flag_eat = false;
    dish_watch.reset();
    eat_watch.reset();
  }

  // spoon free movement
  if (y >= 100 && y <=850) {
    execute("angle_D", String(map(x, 100, 900, 1536, 2560)));
    execute("angle_E", String(map(y, 100, 900, 1536, 2560)));
  }

  // dish gesture
  if (flag_dish) {
    execute("angle_A", "2341");
    execute("angle_B", "2409");
    execute("angle_C", "3190");
    flag_dish = false;
    dish_watch.reset();
    eat_gesture_active = false;
  }

  // eat gesture
  if (flag_eat) {
    execute("angle_A", "1518");
    execute("angle_B", "2038");
    execute("angle_C", "3051");

    flag_eat = false;
    eat_watch.reset();
    eat_gesture_active = true;
  }

//  if (eat_gesture_active) {
//    unsigned int p = readPosition(SERVO_ID_1);
//    execute("angle_E", String(map(map(p, 4095 - 2341, 4095 - 1518, 530, 730), 100, 900, 1024, 3072)));
//  }
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
