#define motorPinRightDir  0  // D2
#define motorPinRightSpeed 5 // D1

void setup() {
  // put your setup code here, to run once:
  pinMode(motorPinRightDir, OUTPUT);
  pinMode(motorPinRightSpeed, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) { // Check if data is available to read
    char command = Serial.read(); // Read the incoming byte

    if (command == 'f') { // Forward motion command
      moveForward();
    } else if (command == 'b') { // Backward motion command
      moveBackward();
    }
  }
}

void moveForward() {
  int speed = 1024;
  int dir = 0;

  digitalWrite(motorPinRightDir, dir);
  analogWrite(motorPinRightSpeed, speed);
}

void moveBackward() {
  int speed = 1024;
  int dir = 1;

  digitalWrite(motorPinRightDir, dir);
  analogWrite(motorPinRightSpeed, speed);
}
