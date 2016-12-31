/*******************************************************************
 Check out the included Arduino sketches and the getting started 
 guide here! 
 https://github.com/andium/AmazonDRS
 
 This is an Arduino implementation of an Amazon Dash Replenishment
 device. It currently supports the critical API endpoints necessary
 for registering a device and submitting replenishment requests. This 
 library is tightly coupled to the WiFi101 library, which means it will 
 work great with the Arduino MKR1000, Adafruit Feather MO w/ the ATWINC1500, 
 Arduino WiFi101 shiled or anywhere the WiFi101 library is supported. Json 
 parsing is provided via ArduinoJson, thanks bblanchon!
 https://github.com/bblanchon/ArduinoJson
 
 Written by Brian Carbonette Copyright © 2016 Andium

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 *******************************************************************/

#include "AmazonDRS.h"
#include "AnduinoNFC.h"

AmazonDRS DRS = AmazonDRS();
AnduinoNFC NFC = AnduinoNFC();


//WiFi creds ----------------------------------------------------------------------------------
char ssid[] = ""; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)
//------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
   }

  //Setup NFC
  NFC.begin();

  //Start up DRS
  DRS.begin(ssid,pass);
  
  //initialize slots
  DRS.retrieveSubscriptionInfo();  //check slot statuses


}

void loop() {

  //scan nfc tag containing slotId
  //if scanned id matches a valid slot and the slot is available
  //request replenishment for the supplied slot
  
   if (NFC.packetReady())
    {
      
        NfcTag tag = NFC.read();  //attempt to read the RFID tag 
        tag.print();              //and print the results to the terminal
        NdefMessage message = tag.getNdefMessage();
        NdefRecord record = message.getRecord(0); //grab the bits that contain the DRS Slot ID
        
        int payloadLength = record.getPayloadLength();
        byte payloadBytes[payloadLength];
        record.getPayload(payloadBytes);
        String payloadString = "";  //store the RFID msg bits in a String for comparison
       
        for(int i=3; i<payloadLength; i++)
        {
          payloadString += (char)payloadBytes[i]; //load up the cmp string with payload less the encoding 
        }
        
        String slotId0 = "0a5028b7-7609-4b91-b97e-3e291f386471";  //eventually this comes from slotStatus getter
        String slotid3 = "9e144300-69e2-4e8e-8608-225377b10150";   //card #3
        if(slotId0 == payloadString)    //eventually if(slotId[i] has a match and slotStatus[i] is available 
        {
            //we have a match! replenish the products associated with that slot!            
            DRS.requestReplenishmentForSlot(payloadString);
        }
        else
        {
          Serial.print("Sorry, slot ");
          Serial.print(payloadString);
          Serial.println(" is not available at this time");
        }
        

    }
  

}
