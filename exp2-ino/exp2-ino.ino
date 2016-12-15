#include <Dynamixel_Serial.h> // MX all but 12
typedef unsigned int uint;

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

enum MotorStates {STILL, MOVING, IN_RAMP_A, IN_RAMP_B};
String MotorStates_to_string(MotorStates s) {
  return s == STILL ?"STILL" :s == IN_RAMP_A ?"IN_RAMP_A" :s == IN_RAMP_B ?"IN_RAMP_B" :"???";
}

class Motor {
  protected:
    uint id;
    uint id2;
    MotorStates state = STILL;
    uint target;
    Watch watch;
  public:
    Motor(uint);
    Motor(uint, uint);
    uint read_position() {
      Dynamixel.begin(59000, 2);
      Serial.print("*");
      return Dynamixel.readPosition(id);
    }
    void set_position(uint _target, uint speed) {
      target = _target;
      Dynamixel.begin(59000, 2);
      Dynamixel.servo(id, target, speed);
      Serial.print(".");
    }
    void update() {
      if (watch.after(1000)) {
        Serial.print(MotorStates_to_string(state));
        watch.reset();
      }
      if (abs(read_position() - target) < 10)
        state = STILL;
    }
};

Motor::Motor (uint motor_id) {
  id = motor_id;
}

Motor::Motor (uint motor_id, uint motor_id2) {
  id = motor_id;
  id2 = motor_id2;
}

class OpposedMotor: public Motor {
  public:
    OpposedMotor(uint arg_id1, uint arg_id2) : Motor(arg_id1, arg_id2) {id = arg_id1; id2 = arg_id2;}
    void set_position(uint position, uint speed) {
      Dynamixel.begin(59000, 2);
      Dynamixel.servo(id, position, speed);
      Dynamixel.servo(id2, 4095 - position, speed);
      Serial.print(".");
    }
};



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

enum STATES {INIT, TO_BASE, ACTION, STOP};

STATES state = INIT;
uint p;
uint target = 2000;
uint start;
long t0 = millis();
Watch watch;
Motor shoulder_yaw(SERVO_ID_1);
OpposedMotor shoulder_roll(SERVO_ID_2, SERVO_ID_3);
Motor elbow(SERVO_ID_4);

void loop() {
  switch (state) {
    case INIT: {
      watch.reset();
      state = TO_BASE;
      break;
    }
    case TO_BASE: {
      start = elbow.read_position();
      if (abs(start - 1000) < 20 && watch.after(2500))
        state = ACTION;
      break;
    }
    case ACTION: {
      p = elbow.read_position();
      uint value = smooth(start, target, p, SPEED);
      elbow.set_position(target, value);
      if (p > 1200)
        state = STOP;
      break;
    }
    case STOP: {
      break;
    }
  }

  elbow.update();
}

uint smooth(uint start, uint target, uint p, uint topSpeed) {
  float slice = abs(target - start) * 0.05;
  float x = abs(p - start) + 1;
  uint value = (uint) (topSpeed / slice * x);
  if (value == 0)
    value = 1;

  if (x > slice)
    return topSpeed;
  else
    return value;
}

void stopAll() {
  shoulder_yaw.set_position(2048, SPEED);
  shoulder_roll.set_position(2048, SPEED);
  elbow.set_position(1000, SPEED);
}
