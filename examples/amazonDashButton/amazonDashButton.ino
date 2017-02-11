/*******************************************************************
 Check out the included Arduino sketches and the getting started
 guide here!
 https://github.com/andium/AmazonDRS

 This is an Arduino implementation of an Amazon Dash Replenishment
 device. It currently supports the critical API endpoints necessary
 for registering a device and submitting replenishment requests. This
 library is tightly coupled to the WiFi101 library, which means it will
 work great with the Arduino MKR1000, Adafruit Feather MO w/ the ATWINC1500,
 Arduino WiFi101 shiled or anywhere the WiFi library is supported. Json
 parsing is provided via ArduinoJson, thanks bblanchon!
 https://github.com/bblanchon/ArduinoJson

 Written by Brian Carbonette Copyright © 2016 Andium
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Andres Sabas @ Electronic Cats support more boards @ 2017

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 *******************************************************************/
 
#ifdef ESP8266 
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#else
#include <WiFi101.h>
#endif

#include "AmazonDRS.h"

AmazonDRS DRS = AmazonDRS();

//WiFi creds ----------------------------------------------------------------------------------
char ssid[] = ""; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)
//------------------------------------------------------------------------------------------------------

#define slotNumber 1 //This will vary for multi slot devices - dash buttons typically only serve one product/slot

const int dashButton = 14;     //DIO number of the pushbutton pin
static long buttonHigh = 12;    //millis of last button push for switch debouncing
static String slotStatus = "true"; //boolean which depicts if slot is available for replenishment
static String slotId = "11111111-2222-3333-4444-555555555555";     //unique slot id ex: 0a5038b7-7609-4b81-b87e-3e291f386324
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);

  pinMode(dashButton, INPUT);


   WiFiClientSecure client;

   //Start up DRS
   DRS.begin(&client);

  //connect to WiFi
  Serial.println("Initializing DRS... connecting to WiFi");
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    Serial.println(".");
    delay(3000);
    status = WiFi.status();
  }

  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());

  //initialize slots
  DRS.retrieveSubscriptionInfo();  //check slot statuses

  slotStatus = DRS.getSlotStatus(slotNumber);
  slotId = DRS.getSlotId(slotNumber);
}

void loop() {

  //Check for button push on the arduino dash button
  //if the slot status is true proceed to request replenishment for the associated slot

  if (buttonPushed())
    {

       //Check if slot is available, if so replenish

        if(slotStatus == "true")   //if the product in slot are available
        {
            //we have a match! replenish the products associated with that slot!
            DRS.requestReplenishmentForSlot(slotId);
        }
        else
        {
          Serial.print("Sorry, slot ");
          Serial.print(slotId);
          Serial.println(" is not available at this time");
        }

    }

}

bool buttonPushed(void)
{
  int buttonState = digitalRead(dashButton);

  if(buttonState && ((millis() - buttonHigh) > 5000))
  {
    buttonHigh = millis();
    Serial.println("Button pressed!!");
    return true;
  }
  else
  {
    return false;
  }
}
