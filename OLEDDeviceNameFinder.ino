#include <Wire.h>

void setup()
{
  Serial.begin (115200);
   while (!Serial)
    {
    }

  Serial.println ();
  byte c = 0;
  pinMode(13,OUTPUT); 
  digitalWrite(13,HIGH);
  Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Address: ");
      Serial.print (i, DEC);
      Serial.print (" 0x");
      Serial.print (i, HEX);
      Serial.println ();
      Serial.print (i, BIN);
      Serial.println ("");
      c++;
      delay (1); 
      } 
  } 
  Serial.println ("Scan Succesful");
  Serial.print (c, DEC);
  Serial.println (" device ");
} 

void loop() {}

