String pc_data = "";
boolean pc_data_complete = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
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

//  if (pc_data_complete) {
//    Serial.println(pc_data);
//    // clear the string:
//    pc_data = "";
//    pc_data_complete = false;
//  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    pc_data += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      pc_data_complete = true;
    }
  }
}
