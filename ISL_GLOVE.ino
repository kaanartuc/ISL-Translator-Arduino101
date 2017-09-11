#include <BlynkSimpleCurieBLE.h>
#include "CurieIMU.h"
#include <CurieBLE.h>
//#include "CurieTimerOne.h"
const int ledPin = 13;
const int FlexFirstPin = A3;
const int FlexSecondPin = A4;
int value1, value2, letter;
char auth[] = "f6d646f1bb144313bb7502a56f677d20";

BLEPeripheral blePeripheral;
BLEService imuService("917649A0-D98E-11E5-9EEC-0002A5D5C51B");
BLECharacteristic imuAccCharacteristic("917649A1-D98E-11E5-9EEC-0002A5D5C51B", BLERead | BLENotify, 12 );

BLEDescriptor imuAccDescriptor("2902", "block");
int lastOrientation = - 1;

WidgetLCD lcdDown(V2);
WidgetLCD lcdUp(V3);

const int FLEX_PIN = A3; // Pin connected to voltage divider output
const int FLEX_PIN2 = A4;
// Measure the voltage at 5V and the actual resistance of your
// 47k resistor, and enter them below:
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 100000.0; // Measured resistance of 3.3k resistor
// Upload the code, then try to adjust these values to more
// accurately calculate bend degree.
const float STRAIGHT_RESISTANCE = 37300.0; // resistance when straight
const float BEND_RESISTANCE = 90000.0; // resistance at 90 deg
int Bend, Bend2, angle, angle2, t, l, a;
int orientation = 22;

const float VCC2 = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV2 = 100000.0; // Measured resistance of 3.3k resistor
// Upload the code, then try to adjust these values to more
// accurately calculate bend degree.
const float STRAIGHT_RESISTANCE2 = 37300.0; // resistance when straight
const float BEND_RESISTANCE2 = 90000.0; // resistance at 90 deg




//const int oneSecInUsec = 100000;

void setup() {
  //Timer1.initialize(100000);
  //Timer1.attachInterrupt(MainLoop);
  //  CurieTimerOne.start(oneSecInUsec, &LoopInter);
  Serial.begin(9600);
  Serial.println("International Sign Language Translator Glove Started");
  pinMode(13, OUTPUT);

  pinMode(FLEX_PIN, INPUT);
  //  pinMode(FLEX_PIN2, INPUT);

  blePeripheral.setLocalName("ISLGlove");
  blePeripheral.setDeviceName("ISLGlove");
  blePeripheral.setAppearance(384);
  Blynk.begin(blePeripheral, auth);
  blePeripheral.begin();

  CurieIMU.begin();
  CurieIMU.setAccelerometerRange(16);
  CurieIMU.setGyroRange(250);
  blePeripheral.setAdvertisedServiceUuid(imuService.uuid());  // add the service UUID
  blePeripheral.addAttribute(imuService);
  blePeripheral.addAttribute(imuAccCharacteristic);
  blePeripheral.addAttribute(imuAccDescriptor);
  delay(10);

  Serial.println("Bluetooth device active, waiting for connections...");
  IMUCallibration();
  delay(10);
  Serial.println("Callibrating the Flex");
  //  FlexCallibration();
  Serial.println("Done");
}

void loop() {
  Blynk.run();
  blePeripheral.poll();
  printLetter();
 
}

void IMUCallibration() {
  Serial.println("Internal sensor offsets BEFORE calibration...");
  Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
  Serial.print("\t"); // -76
  Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
  Serial.print("\t"); // -235
  Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));
  Serial.print("\t"); // 168
  Serial.print("Starting Acceleration calibration and enabling offset compensation...");
  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
  Serial.println(" Done");
  Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
  Serial.print("\t"); // -76
  Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
  Serial.print("\t"); // -2359
  Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));
  Serial.print("\t"); // 1688
}

void getIMUdata() {

 // int orientation = - 1;   // the board's orientation
  String orientationString; // string for printing description of orientation

  int x = CurieIMU.readAccelerometer(X_AXIS);
  int y = CurieIMU.readAccelerometer(Y_AXIS);
  int z = CurieIMU.readAccelerometer(Z_AXIS);

  int absX = abs(x);
  int absY = abs(y);
  int absZ = abs(z);
  if ( (absZ > absX) && (absZ > absY)) {
    // base orientation on Z
    if (z > 0) {
      orientationString = "up";
      orientation = 0;
    
    } else {
      orientationString = "down";
      orientation = 1;
    }
  } else if ( (absY > absX) && (absY > absZ)) {
    // base orientation on Y
    if (y > 0) {
      orientationString = "digital pins up";
      orientation = 2;
    } else {
      orientationString = "analog pins up";
      orientation = 3;
     }
  } else {
    // base orientation on X
    if (x < 0) {
      orientationString = "connector up";
      orientation = 4;
    } else {
      orientationString = "connector down";
      orientation = 5;
    }
  }

  // if the orientation has changed, print out a description:
  if (orientation != lastOrientation) {
    Serial.println(orientationString);
    lcdDown.print(0, 0, orientationString);
    lastOrientation = orientation;
  }
  Serial.println("X: " + String(absX) + "---Y: " + String(absY) + "---Z: " + String(absZ));
  
}

void FlexCallibration() {
  getFlexSensorData();
  int i;
  int a = t;
  for (i = 0; i < 500; i++) {
    a = a + a;
    l = l + l;
  }
  a = a / 500;
  l = l / 500;
}

void getFlexSensorData() {

  // Read the ADC, and calculate voltage and resistance from it
  int flexADC = analogRead(FLEX_PIN);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  // Serial.println("Resistance: " + String(flexR) + " ohms");

  // Use the calculated resistance to estimate the sensor's
  // bend angle:
  float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE,
                    0, 90.0);
  Bend = int(angle);
  Serial.println("Bend: " + String(Bend) + " degrees");


  // Read the ADC, and calculate voltage and resistance from it
  int flexADC2 = analogRead(FLEX_PIN2);
  float flexV2 = flexADC2 * VCC2 / 1023.0;
  float flexR2 = R_DIV2 * (VCC2 / flexV2 - 1.0);
  //  Serial.println("Resistance: " + String(flexR2) + " ohms");

  // Use the calculated resistance to estimate the sensor's
  // bend angle:
  float angle2 = map(flexR2, STRAIGHT_RESISTANCE2, BEND_RESISTANCE2,
                     0, 90.0);
  Bend2 = int(angle2);
  Serial.println("Bend2: " + String(Bend2) + " degrees"); 
  Serial.println();

}

void printLetter() {

  getIMUdata();
  delay(20);
  getFlexSensorData();
  // FlexCallibration();
  delay(1000);
  lcdDown.clear();
  /* lcdDown.print(0, 0, orientation);
    lcdDown.print(7, 0, orientation);
    lcdDown.print(0, 1, orientation);
  */ lcdDown.print(0, 1, Bend);
  lcdDown.print(7, 1, Bend2);
  Serial.println(orientation);

  if ( lastOrientation == 2 && Bend2>150 && Bend<70 && Bend > 25 && Bend2<230 )
    letter = 0;
  else if ( orientation == 2 && Bend > 70&& Bend < 150 && Bend2 >-10 && Bend2 < 30)
    letter = 1;
  else if (orientation == 2 && Bend > 40&& Bend < 100 && Bend2 >100 && Bend2 < 150 )
    letter = 2;
  else if (orientation == 2 && Bend > -10&& Bend < 50 && Bend2 >-10 && Bend2 < 40)
    letter = 3;
  else if (orientation == 1 && Bend > 10&& Bend < 70 && Bend2 >-20 && Bend2 < 30)
    letter = 4;
 /* else if (orientation = 0 )
    letter = 5;
  else if (orientation = 0 )
    letter = 6;
  else if (orientation = 0 )
    letter = 7;*/
  else
    letter = 8;

  switch (letter) {
    case 0:
      lcdUp.clear();
      lcdUp.print(0, 0, "A");
      Serial.println("A");
      break;
    case 1:
      lcdUp.clear();
      lcdUp.print(0, 0, "B");
      Serial.println("B");
      break;
    case 2:
      lcdUp.clear();
      lcdUp.print(0, 0, "C");
      Serial.println("C");
      break;
    case 3:
      lcdUp.clear();
      lcdUp.print(0, 0, "Hello");
      Serial.println("Hello");
      break;
    case 4:
      lcdUp.clear();
      lcdUp.print(0, 0, "Welcome");
      Serial.println("Welcome");
      break;
    case 5:
      lcdUp.clear();
      lcdUp.print(0, 0, "F");
      break;
    case 6:
      lcdUp.clear();
      lcdUp.print(0, 0, "G");
      break;
    case 7:
      lcdUp.clear();
      lcdUp.print(0, 0, "H");
      break;
    case 8:
      lcdUp.clear();
      lcdUp.print(0, 0, "");
      Serial.println("");
      break;
  }
}
