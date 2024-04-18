/*Arduino JOYSTICK CONTROLLED CAR (TRANSMITTER)
          
YOU HAVE TO INSTALL THE RF24 LIBRARY BEFORE UPLOADING THE CODE
   https://github.com/tmrh20/RF24/      
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(8,9); // CE, CSN
const byte address[6] = "00001";
char xyData[32] = "";
String xAxis, yAxis;

const int buttonPin = 2;  // the pin that the pushbutton is attached to
int buttonPushCounter = 0;  // counter for the number of button presses
int buttonState = 0;        // current state of the button
int lastButtonState = 0;    // previous state of the button

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  radio.begin();
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  // print example's introductory prompt
  Serial.println(F("RF24/examples/GettingStarted"));
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  
  /*GPT*/
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == LOW) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);

      if(buttonPushCounter == 3) {
        // Reset the counter if it reaches 3
        buttonPushCounter = 0;
      }

    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
  if (buttonPushCounter == 0) {
    // Send button state 0
    radio.write(&buttonPushCounter, sizeof(buttonPushCounter));
    Serial.println("Button State 0 sent");
    delay(20);
  } 
  else if (buttonPushCounter == 1) {
    radio.write(&buttonPushCounter, sizeof(buttonPushCounter));
    Serial.println("Button State 1 sent");
    delay(10);
    xAxis = analogRead(A1); // Read Joysticks X-axis
    yAxis = analogRead(A0); // Read Joysticks Y-axis
    // X value
    xAxis.toCharArray(xyData, 5); // Put the String (X Value) into a character array
    radio.write(&xyData, sizeof(xyData)); // Send the array data (X value) to the other NRF24L01 modile
    Serial.print("x: ");
    Serial.print(xyData);
    Serial.print("\t");
    Serial.print("\n");
    delay(5);
    // Y value
    yAxis.toCharArray(xyData, 5);
    radio.write(&xyData, sizeof(xyData));
    Serial.print("y: ");
    Serial.print(xyData);
    Serial.print("\t");
    Serial.print("\n");
    delay(5);
  } 
  else if (buttonPushCounter == 2) {
    // Send button state 2
    radio.write(&buttonPushCounter, sizeof(buttonPushCounter));
    Serial.println("Button State 2 sent");
    delay(20);
  }
}
