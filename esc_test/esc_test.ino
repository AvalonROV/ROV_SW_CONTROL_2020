/* 
########################################
###### ESC BREAKOUT BOARD PROGRAM ######
########################################
*/

// PINOUT
#define REC_OUT             0                   // RS485 
#define DRIVE_IN            1 
#define DEBUG_LED           LED_BUILTIN_TX
#define REC_OUT_ENABLE      12
#define DRIVE_OUT_ENABLE    6
#define MOTOR_ENABLE        4
#define DISABLED_LED        A2
#define ENABLED_LED         A3
#define SPD_CNTRL_IN        A4
#define DIR_SRC             A5

// VARIABLES
bool enableStatus = false;
bool dirSource;
bool speedSrc;
float speedInput;

unsigned long currentMillis, previousMillis = 0;

void setup(){   
  // SET PIN MODE
  pinMode(REC_OUT,OUTPUT);
  pinMode(DRIVE_IN,INPUT);
  pinMode(DEBUG_LED,OUTPUT);
  pinMode(REC_OUT,OUTPUT);
  pinMode(DRIVE_OUT_ENABLE,OUTPUT);
  pinMode(MOTOR_ENABLE,OUTPUT);
  pinMode(DISABLED_LED,OUTPUT);
  pinMode(ENABLED_LED,OUTPUT);
  pinMode(SPD_CNTRL_IN,INPUT);
  pinMode(DIR_SRC,INPUT);

  // SET DEFAULT STATES
  digitalWrite(DEBUG_LED,LOW);
  digitalWrite(DISABLED_LED,HIGH);
  digitalWrite(ENABLED_LED,LOW);

  // INITIATE SERIAL
  Serial.begin(115200); 
}

void printStatus(){
  Serial.println();
  Serial.print("Motor Status: ");
  if(enableStatus == false) Serial.println("Disabled");
  else if(enableStatus == true) Serial.println("Enabled");
  Serial.print("Motor Speed: ");
  Serial.print(map(speedInput,0,1023,0,100));
  Serial.println("%");
  Serial.print("Direction Source: ");
  if(dirSource == false) Serial.println("Manual Switch");
  else if(dirSource == true) Serial.println("Software");
}

void readInputs(){
  // READ ALL DIGITAL AND ANALOG INPUTS
  dirSource = digitalRead(DIR_SRC);
  speedInput = analogRead(SPD_CNTRL_IN);
  if(digitalRead(MOTOR_ENABLE))
  {
    enableStatus = !enableStatus;
    digitalWrite(ENABLED_LED,enableStatus);
    digitalWrite(DISABLED_LED,!enableStatus);
  }
  // DEBOUNCE PUSH BUTTON
  while(digitalRead(MOTOR_ENABLE) == true){}
}

void loop()
{
  currentMillis = millis();
  readInputs();

  // PRINT STATUS TO SERIAL TERMINAL 
  if(currentMillis - previousMillis >= 500) 
  {
    previousMillis = currentMillis;
    printStatus();
  }
}
