
//Set up Ethernet and OSC 
//adapted from https://trippylighting.com/teensy-arduino-ect/touchosc-and-arduino-oscuino/)
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <OSCBundle.h>

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 

// NOTE: Alternatively, you can assign a fixed IP to configure your
//       Ethernet shield.
//       byte ip[] = { 192, 168, 0, 154 };

int serverPort  = 8000; //TouchOSC (incoming port)
int destPort = 9000;    //TouchOSC (outgoing port)
int ledPin =  4;       
int runState = LOW;

//Create UDP message object
EthernetUDP Udp;


//Set up Stepper Shield and Accel Library 
//adapted from Adafruit_MotorShield / Accel_MultiStepper sample
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMStop(0x60); // Default address, no jumpers

//Adafruit_StepperMotor *myStepper1 = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *myStepper2 = AFMStop.getStepper(200, 2);

// you can change these to SINGLE, DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {  
  myStepper2->onestep(BACKWARD, DOUBLE);
}

// Now we'll wrap the steppers in an AccelStepper object
AccelStepper stepper2(forwardstep2, backwardstep2);

void setup()
{  
  AFMStop.begin(); // Start the top shield
   
  stepper2.setMaxSpeed(500.0);
  stepper2.setAcceleration(50.0);
  stepper2.moveTo(5000);

  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); //9600 for a "normal" Arduino board (Uno for example). 115200 for a Teensy ++2 
  Serial.println("OSC test");

  // start the Ethernet connection:
  // NOTE: Alternatively, you can assign a fixed IP to configure your
  //       Ethernet shield.
  //       Ethernet.begin(mac, ip);   
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  // print your local IP address:
  Serial.print("Arduino IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    if(thisByte < 3) Serial.print("."); 
    else Serial.println("");
  }

  Udp.begin(serverPort);

}

void loop()
{
    // Change direction at the limits
    if (stepper2.distanceToGo() == 0)
	  stepper2.moveTo(-stepper2.currentPosition());

//    stepper2.run();

  if(runState==LOW) {
    //Serial.println("Motor On");
    stepper2.run();
  }
  else {
    //Serial.println("Motor Off");
    stepper2.disableOutputs();
    myStepper2->release();
  }
  
    //process received messages
    OSCMsgReceive();
  
}

