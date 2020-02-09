/**@main.ino */

// IMPORTS
#include <Servo.h>

// PINOUT

// ACTUATORS
#define ACT_1 2
#define ACT_2 3
#define ACT_3 4
#define ACT_4 5
#define ACT_5 6
#define ACT_6 7

// THRUSTERS
#define THRUST_1 10
#define THRUST_2 9
#define THRUST_3 9
#define THRUST_4 9
#define THRUST_5 9
#define THRUST_6 9
#define THRUST_7 9
#define THRUST_8 9

// PARAMETERS
#define BUFFER_SIZE 40
#define BAUD_RATE 115200
#define ACTUATOR_QUANTITY 6
#define THRUSTER_QUANTITY 8
#define SENSOR_QUANTITY 3
#define IDENTITY "AVALONROV"

int actuatorList[ACTUATOR_QUANTITY] = {ACT_1, ACT_2, ACT_3, ACT_4, ACT_5, ACT_6};
int thrusterList[THRUSTER_QUANTITY] = {THRUST_1, THRUST_2, THRUST_3, THRUST_4, THRUST_5, THRUST_6, THRUST_7, THRUST_8};
Servo thrusters[THRUSTER_QUANTITY];

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial) {}
  Serial.flush();

  // SETUP ACTUATORS PINS
  for (int i = 0; i < ACTUATOR_QUANTITY; i++) {
    pinMode(actuatorList[i], OUTPUT);
    digitalWrite(actuatorList[i], LOW);
  }

  // SETUP THRUSTER PINS
  for (int i = 0; i < THRUSTER_QUANTITY; i++) {
    thrusters[i].attach(thrusterList[i]);
    thrusters[i].writeMicroseconds(1000);
  }

  // INITIATE RANDOM NUMBER GENERATOR
  randomSeed(1234);
}

int charArrayToInt(const char *data, size_t start, size_t finish) {
  int val = 0;
  while (start < finish) {
    val = val * 10 + data[start++] - '0';
  }
  return val;
}

void armThrusters() {
  for (int i = 0; i < THRUSTER_QUANTITY; i++) {
    thrusters[i].writeMicroseconds(1500);
  }
}

void setThrusters(int *thrusterSpeeds) {
  /*
     PURPOSE
     Sets the PWM for each thruster pin.

     INPUTS
     int *thrusterSpeeds = Name of the array that stores the processed thruster speeds (0-1023).

     RETURNS
     NONE
  */
  //Serial.println("@Setting Thrusters");
  for (int i = 0; i < THRUSTER_QUANTITY; i++) {
    thrusters[i].writeMicroseconds(map(thrusterSpeeds[i],1,999,1100,1900));
  }
}

void setActuators(bool * actuatorStates) {
  /*
     PURPOSE
     Sets the actuator pins HIGH or LOW.

     INPUTS
     bool *actuatorStates = Name of the array that stores the processed actuator states (true/false).

     RETURNS
     NONE
  */

  //Serial.println("@Setting Actuators");

  // LOOP THROUGH EACH ACTUATOR AND SET OUTPUT STATE
  for (int i = 0; i < ACTUATOR_QUANTITY; i++) {
    digitalWrite(actuatorList[i], actuatorStates[i]);
  }
}

void setCameras() {
  //Serial.println("Setting Cameras");
}

void getSensors() {
  for (int i = 0 ; i < SENSOR_QUANTITY ; i++) {
    Serial.print(random(0, 100));
    if (i != SENSOR_QUANTITY - 1)
      Serial.print(",");
  }
  Serial.println();
  //Serial.println("@Getting Sensors");
}

bool getSerialCommand(int bufferSize, char *receivedData) {
  /*
    PURPOSE
    Receives data over serial and stores it in a char array via a pointer.
    Keeps trying to receive data until a newline character is received.

    INPUTS
    int bufferSize = Maximum number of characters to be stored in the received data array.
    char *receivedData = Name of the array to stores the received data in.

    RETURNS
    readStatus = True if a command has been recieved, False otherwise.
  */
  bool readStatus;
  char character;
  int index = 0;

  if (Serial.available() > 0) {
    // KEEP READING UNTIL COMMAND IS FULLY RECEIVED
    while (true) {
      // READ SINGLE CHARACTER
      character = Serial.read();

      // IF COMMAND IS COMPLETE
      if (character == '\n') {
        // ADD NULL TERMINATION WHEN DATA IS FULLY RECEIVED
        receivedData[index] = '\0';
        readStatus = true;
        // STOP RECEIVING
        break;
      }
      // IF VALID CHARACTER IS RECEIVED
      else if (character != -1) {
        if (index < (bufferSize - 1)) {
          // ADD CHARACTER TO CHAR BUFFER
          receivedData[index] = character;
          index++;
        }
      }
    }
  }
  else {
    readStatus = false;
  }
  return (readStatus);
}

void processCommand(char *receivedData) {
  // REMOVE BLANK SPACES FROM ARRAY

  // CHECK IF THE COMMAND IS VALID
  if (receivedData[0] == '?') {

    // ROV COMMAND
    if (receivedData[1] == 'R') {
      //Serial.println("ROV COMMAND");
      switch (receivedData[2]) {

        // SET THRUSTERS
        case 'T':
          // EXTRACT THRUSTER SPEEDS
          int thrusterSpeeds[THRUSTER_QUANTITY];
          for (int i = 0; i < THRUSTER_QUANTITY; i++) {
            // CONVERT CHAR ARRAY TO INTEGERS
            int thrustSpeed = charArrayToInt(receivedData, 3 + (3 * i), 6 + (3 * i));
            thrusterSpeeds[i] = thrustSpeed;
          }
          setThrusters(thrusterSpeeds);
          break;

        // SET ACTUATORS
        case 'A':
          // EXTRACT ACTUATOR STATES
          bool actuatorStates[ACTUATOR_QUANTITY];
          for (int i = 0; i < ACTUATOR_QUANTITY; i++) {
            if (receivedData[3 + i] == '1')
              actuatorStates[i] = true;
            else
              actuatorStates[i] = false;
          }
          setActuators(actuatorStates);
          break;

        // SET CAMERAS
        case 'C':
          setCameras();
          break;

        // GET SENSORS
        case 'S':
          getSensors();
          break;

        case 'X':
          armThrusters();
          break;

        // INVALID COMMAND
        default:
          //Serial.println("INVALID COMMAND");
          break;
      }
    }

    // MINI-ROV COMMAND
    else if (receivedData[1] == 'M') {
      //Serial.println("MINI ROV COMMAND");q
    }

    // IDENTITY REQUEST
    else if (receivedData[1] == 'I') {
      Serial.println(IDENTITY);
    }

    else {
      //Serial.println("INVALID COMMAND");
    }
  }
  else {
    //Serial.println("INVALID COMMAND");
  }
}

void loop() {
  // DEFINE ARRAY TO STORE RECEIVED DATA IN
  char receivedData[BUFFER_SIZE];

  // UPDATE ARRAY WITH RECEIVED DATA
  bool readStatus = getSerialCommand(BUFFER_SIZE, receivedData);

  // COMMAND SUCCESSFULLY RECEIVED
  if (readStatus)
  {
    //Serial.print("Command Received: ");
    //Serial.println(receivedData);
    processCommand(receivedData);
  }
}
