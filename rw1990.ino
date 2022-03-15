// Based:
// https://danman.eu/blog/cloning-ibutton-using-rw1990-and-avr/
// http://elimelecsarduinoprojects.blogspot.com/2013/06/read-dallas-ibutton-arduino.html
// TODO: danger to atmega328! Use OpenCollector logic!
// Used 4.8kΩ Pull-up and 5 Vcc for arduino/pull-up

#include <OneWire.h>
#define PIN 2

OneWire ibutton (PIN); // I button connected on PIN 2.

byte addr[8]; //array to store the Ibutton ID.

void setup(){
 Serial.begin(9600); 
}

void loop(){
  if (!ibutton.search (addr)){//read attached ibutton and asign value to buffer
    ibutton.reset_search();
    delay(1000);
    return;
 }
  Serial.print(" >< ");
  for (byte x = 0; x<8; x++){  
    Serial.print(addr[x],HEX); //print the buffer content in LSB. For MSB: for (int x = 8; x>0; x--) 
     Serial.print(" ");
  }
 
  //compute crc//
  byte crc;
  crc = ibutton.crc8(addr, 7);
  Serial.print("CRC: ");
  Serial.println(crc,HEX);
  
  //When pressed 'w' in the serial monitor
  if ( Serial.read() == 'w' ){
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0x33);
    Serial.print("  ID before write:");
    for (byte x=0; x<8; x++){
      Serial.print(' ');
      Serial.print(ibutton.read(), HEX);
    }
    Serial.print('\n');
    Serial.print("Writing iButton ID:\n");
    
    //Generate random number to use as ID
    byte id1 = random(0x00, 0xFF);
    byte id2 = random(0x00, 0xFF);
    byte id3 = random(0x00, 0xFF);
    byte id4 = random(0x00, 0xFF);
    byte id5 = random(0x00, 0xFF);
    byte id6 = random(0x00, 0xFF);
    //            __________________________
    //ID Format: |Type|ID|ID|ID|ID|ID|ID|CRC|
    //            ¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬
    
    byte newID[8] = {0x01, id6, id5, id4, id3, id2, id1, 0x00};
    
    //Calculate the new CRC-8/MAXIM based in the new id Values 
    
    byte ncrc = ibutton.crc8(newID, 7);
    
    newID[7] = ncrc;
    
    Serial.print("New ID :");
    for (byte x=0; x<8; x++){
      Serial.print(' ');
      Serial.print(newID[x], HEX);
    }
    Serial.print('\n');
    
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0xD5);
    for (byte x = 0; x<8; x++){
      writeByte(newID[x]);
      Serial.print('*');
    }
    Serial.print('\n');
    ibutton.reset();
  }
} 

int writeByte(byte data){
  int data_bit;
  for(data_bit=0; data_bit<8; data_bit++){
    if (data & 1){
      digitalWrite(PIN, LOW); pinMode(PIN, OUTPUT);
      delayMicroseconds(60);
      pinMode(PIN, INPUT); digitalWrite(PIN, HIGH);
      delay(10);
    } else {
      digitalWrite(PIN, LOW); pinMode(PIN, OUTPUT);
      pinMode(PIN, INPUT); digitalWrite(PIN, HIGH);
      delay(10);
    }
    data = data >> 1;
  }
  return 0;
}
