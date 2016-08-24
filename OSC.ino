
void OSCMsgReceive(){
  OSCMessage msgIN;
  int size;
  if((size = Udp.parsePacket())>0){
    while(size--)
      msgIN.fill(Udp.read());
    if(!msgIN.hasError()){
      msgIN.route("/OnOff/toggle1",toggleOnOff);
      msgIN.route("/Fader/Value",funcValue);
    }
  }
}

void toggleOnOff(OSCMessage &msg, int addrOffset){
  runState = (boolean) msg.getFloat(0);
  OSCMessage msgOUT("/OnOff/toggle1");

  digitalWrite(ledPin, runState);

  msgOUT.add(runState);

  Serial.print("runState = ");
  Serial.println(runState);

  runState = !runState;     // toggle the state from HIGH to LOW to HIGH to LOW ...

  //send osc message back to controll object in TouchOSC
  //Local feedback is turned off in the TouchOSC interface.
  //The button is turned on in TouchOSC interface whe the conrol receives this message.
  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp); // send the bytes
  Udp.endPacket(); // mark the end of the OSC Packet
  msgOUT.empty(); // free space occupied by message
}


int faderValue = 0;

void funcValue(OSCMessage &msg, int addrOffset ){

  OSCMessage msgOUT("/Fader/Value");

  faderValue = msg.getFloat(0);

  stepper2.setMaxSpeed(max(1, faderValue*5));

  Serial.print("faderValue = ");
  Serial.println(faderValue);

  msgOUT.add(faderValue);

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp); // send the bytes
  Udp.endPacket(); // mark the end of the OSC Packet
  msgOUT.empty(); // free space occupied by message
}



