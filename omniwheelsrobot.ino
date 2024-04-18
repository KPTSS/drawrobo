#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define FRONT_LEFT_PIN 3
#define REAR_LEFT_PIN 22
#define FRONT_RIGHT_PIN 5
#define REAR_RIGHT_PIN 6

#define servoPin 2

#define IRFORWARD 26
#define IRLEFT 28
#define IRRIGHT 30
#define IRREAR 32

#define ROWS 50
#define COLS 70
int matrix[ROWS][COLS] = {};

Servo frontLeftMotor;
Servo frontRightMotor;
Servo rearLeftMotor;
Servo rearRightMotor;

Servo pen;
int servopos = 0;

/*Radio*/
RF24 radio(8, 9); // CE, CSN
const byte address[6] = "00001";
char receivedData[32] = "";
int buttonPushCounter;
int  xAxis, yAxis;

void setup() {
  // Serial1.begin(115200); // Initialize Serial1 with baud rate 115200
  // Serial1.setTimeout(1);

  // // Read the matrix data from Python and store it in the matrix array
  // for (int i = 0; i < ROWS; i++) {
  //   for (int j = 0; j < COLS; j++) {
  //     while (!Serial1.available()); // Wait until data is available
  //     matrix[i][j] = Serial1.parseInt(); // Read the integer value from Python
  //     Serial.print(2234);

  //     digitalWrite(LED_BUILTIN, HIGH);
  //     delay(2); // Delay for 1 second
      
  //     // Turn off the built-in LED
  //     digitalWrite(LED_BUILTIN, LOW);
  //   }
  // }

  // put your setup code here, to run once:

  pinMode(FRONT_LEFT_PIN, OUTPUT);
  pinMode(REAR_LEFT_PIN, OUTPUT);
  pinMode(FRONT_RIGHT_PIN, OUTPUT);
  pinMode(REAR_RIGHT_PIN, OUTPUT);  
  pinMode(servoPin, OUTPUT);

  frontLeftMotor.attach(FRONT_LEFT_PIN);
  rearLeftMotor.attach(REAR_LEFT_PIN);
  frontRightMotor.attach(FRONT_RIGHT_PIN);
  rearRightMotor.attach(REAR_RIGHT_PIN);

  pen.attach(servoPin);
  
  pinMode(IRFORWARD, INPUT);
  pinMode(IRLEFT, INPUT);
  pinMode(IRRIGHT, INPUT);
  pinMode(IRREAR, INPUT);

  Serial.begin(9600);
  radio.begin();
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  else{
    Serial.print("connected");
  }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    // Read the received data into the variable
    radio.read(&buttonPushCounter, sizeof(buttonPushCounter));
    Serial.println(buttonPushCounter);
    if (buttonPushCounter == 0) {
      Serial.println("Button State 0 received");
      delay(20);
      // Process accordingly
    } else if (buttonPushCounter == 1) {
      // If the NRF240L01 module received data
      if (digitalRead(IRFORWARD) == 0 || digitalRead(IRLEFT) == 0 || digitalRead(IRRIGHT) == 0 || digitalRead(IRREAR) == 0){
        frontLeftMotorControl(0);
        rearLeftMotorControl(0);
        frontRightMotorControl(0);
        rearRightMotorControl(0);
      }
      else {
        xyvalues();
      }

    } else if (buttonPushCounter == 2) {
      Serial.println("Button State 2 received");
      // Process accordingly
      drawing();
      delay(20);
    }
  } else {
    // If no data is available, you can print a message or take other actions
    Serial.println("No data available from radio module.");
    // You could also add a delay here to avoid continuous printing
    delay(20);
  }
}

void xyvalues(){
  radio.read(&receivedData, sizeof(receivedData)); // Read the data and put it into character array
  xAxis = atoi(&receivedData[0]); // Convert the data from the character array (received X value) into integer
  Serial.print("x: ");
  Serial.print(xAxis);
  Serial.print("\t");
  Serial.print("\n");
  delay(5);
  radio.read(&receivedData, sizeof(receivedData));
  yAxis = atoi(&receivedData[0]);
  Serial.print("y: ");
  Serial.print(yAxis);
  Serial.print("\t");
  Serial.print("\n");
  calculateAndSetMotorSpeeds(xAxis, yAxis);
  delay(5);
}

void calculateAndSetMotorSpeeds(int xAxis, int yAxis) {
  // Calculate motor speeds based on joystick input
  int motorSpeedX = map(xAxis, 0, 1023, -100, 100); // Map joystick X axis to motor speed
  int motorSpeedY = map(yAxis, 0, 1023, -100, 100); // Map joystick Y axis to motor speed
  
  // Calculate individual motor speeds for omni-directional movement
  int frontLeftSpeed = motorSpeedY + motorSpeedX;
  int rearLeftSpeed = motorSpeedY - motorSpeedX;
  int frontRightSpeed = motorSpeedY - motorSpeedX;
  int rearRightSpeed = motorSpeedY + motorSpeedX;

  // Set motor speeds
  frontLeftMotorControl(frontLeftSpeed);
  rearLeftMotorControl(rearLeftSpeed);
  frontRightMotorControl(frontRightSpeed);
  rearRightMotorControl(rearRightSpeed);

  // Print motor speeds for debugging
  Serial.print("Front Left Motor: ");
  Serial.print(frontLeftSpeed);
  Serial.print("\t");
  Serial.print("Rear Left Motor: ");
  Serial.print(rearLeftSpeed);
  Serial.print("\t");
  Serial.print("Front Right Motor: ");
  Serial.print(frontRightSpeed);
  Serial.print("\t");
  Serial.print("Rear Right Motor: ");
  Serial.print(rearRightSpeed);
  Serial.println();
}

void drawing(){
      for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
          if(i % 2 == 0){
            frontLeftMotorControl(20);
            rearLeftMotorControl(20);
            frontRightMotorControl(20);
            rearRightMotorControl(20);
            Serial.print("FORWARD \n");
          }
          else {
            frontLeftMotorControl(-20);
            rearLeftMotorControl(-20);
            frontRightMotorControl(-20);
            rearRightMotorControl(-20);
            Serial.print("BACKWARDS \n");
          }

          // if(matrix[i][j] == 0 && j < COLS - 1 && matrix[i][j + 1] == 0){
          //   pen.write(5);
          //   delay(5);
          // }
          // else if (j > 0 && matrix[i][j - 1] == 0 && matrix[i][j] >= 1) {
          //   pen.write(20);
          //   delay(5);
          // }
          // else {
          //   delay(100);
          // }

          //Print the value of the matrix element
          if(matrix[i][j] == 0){
            for (servopos = 10; servopos <= 27; servopos += 1) {
              pen.write(servopos);
              delay(3); 
            }
            for (servopos = 27; servopos >= 10; servopos -= 1) {
              pen.write(servopos);
              delay(3);
            }
          }
          else {
            delay(50);
          }
          Serial.print(i);
          Serial.print(" ");
          Serial.print(j);
          Serial.print("\n");
          Serial.print(" ");
          Serial.print(matrix[i][j]);
        }

      // Move to the next line after printing each row
      Serial.println();
      
        /* right */
      frontLeftMotorControl(20);
      rearLeftMotorControl(-20);
      frontRightMotorControl(-20);
      rearRightMotorControl(20);
      delay(500);
  }
}

/* frunction for the control of main wheel DO NOT TOUCH */
void frontLeftMotorControl(int value) {
  frontLeftMotor.writeMicroseconds(map(value, -100, 100, 1000, 2000));
}

void rearLeftMotorControl(int value) {
  rearLeftMotor.writeMicroseconds(map(value, -100, 100, 1000, 2000));
}

void frontRightMotorControl(int value) {
  frontRightMotor.writeMicroseconds(map(value, -100, 100, 1000, 2000));
}

void rearRightMotorControl(int value) {
  rearRightMotor.writeMicroseconds(map(value, -100, 100, 1000, 2000));
}

/* Getting jjoystick values from another module */
  // if (radio.available()) {   // If the NRF240L01 module received data
  //   radio.read(&receivedData, sizeof(receivedData)); // Read the data and put it into character array
  //   xAxis = atoi(&receivedData[0]); // Convert the data from the character array (received X value) into integer
  //   Serial.print("x: ");
  //   Serial.print(xAxis);
  //   Serial.print("\t");
  //   Serial.print("\n");
  //   delay(20);
  //   radio.read(&receivedData, sizeof(receivedData));
  //   yAxis = atoi(&receivedData[0]);
  //   Serial.print("y: ");
  //   Serial.print(yAxis);
  //   Serial.print("\t");
  //   Serial.print("\n");
  //   delay(20);
  // }


  /* ================= movement ===================== */

  /* forward */
  // frontLeftMotorControl(17);
  // rearLeftMotorControl(17);
  // frontRightMotorControl(17);
  // rearRightMotorControl(17);
  // delay(2000);
  /* idling */
  // frontLeftMotorControl(0);
  // rearLeftMotorControl(0);
  // frontRightMotorControl(0);
  // rearRightMotorControl(0);
  // delay(1000);

  /* back */
  // frontLeftMotorControl(-50);
  // rearLeftMotorControl(-50);
  // frontRightMotorControl(-50);
  // rearRightMotorControl(-50);
  // delay(1000);
  /* idling */
  // frontLeftMotorControl(0);
  // rearLeftMotorControl(0);
  // frontRightMotorControl(0);
  // rearRightMotorControl(0);
  // delay(1000);

  /* left */
  // frontLeftMotorControl(-17);
  // rearLeftMotorControl(17);
  // frontRightMotorControl(17);
  // rearRightMotorControl(-17);
  // delay(500);
  // /* idling */
  // frontLeftMotorControl(0);
  // rearLeftMotorControl(0);
  // frontRightMotorControl(0);
  // rearRightMotorControl(0);
  // delay(1000);

  /* right */
  // frontLeftMotorControl(50);
  // rearLeftMotorControl(-50);
  // frontRightMotorControl(-50);
  // rearRightMotorControl(50);
  // delay(1000);

  /* upper left */
  // frontLeftMotorControl(0);
  // rearLeftMotorControl(50);
  // frontRightMotorControl(50);
  // rearRightMotorControl(0);
  // delay(1000);

  /* upper right */
  // frontLeftMotorControl(50);
  // rearLeftMotorControl(0);
  // frontRightMotorControl(0);
  // rearRightMotorControl(50);
  // delay(1000);

  /* ================= movement ===================== */

  // /* algorithm for controlling the pen (just up and down for about 20 degrees) */
  //   for (servopos = 0; servopos <= 20; servopos += 1) {
  //     pen.write(servopos);
  //     delay(10); 
  //   }
  //   for (servopos = 20; servopos >= 0; servopos -= 1) {
  //     pen.write(servopos);
  //     delay(10); 
  //   }
