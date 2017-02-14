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

 Written by Brian Carbonette Copyright © 2017 Andium

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Andres Sabas @ Electronic Cats added ESP8266 support @ 2017

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

//Extract the code from the response during the LWA process, example response below
//https://app.getpostman.com/oauth2/callback?code=ANQEgiAOjkQWjKvhNWIN&scope=dash%3Areplenish
//------------------------------------------------^^^^^^^^^^^^^^^^^^^^-----------------------
#define auth_code "ANqbDAujrtMgtplRkYgx" //ex: ANQEgiAOjkQWjKvhNWIN
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
   }

   #ifdef ESP8266 
   WiFiClientSecure client;
   #else
   WiFiSSLClient client;
   #endif

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

  DRS.setAuthCode(auth_code);
  DRS.requestBearerAndRefreshTokens();   //only call this on initial setup - once user has authorized
                                         //LWA for the device just use the refresh_token
  delay(3000);                           //wait a moment for the authorization code grant to complete the exchange

  DRS.retrieveSubscriptionInfo();        //test if the tokens are valid by checking slot statuses

  Serial.println("Copy everything below...");
  Serial.println(DRS.getRefreshToken()); //Print the refresh_token to the screen

  /*copy this to AmazonTokens.h and set refresh_token equal to what's printed to the terminal
  Should look like this (~460 characters)...
  refresh token = "Atzr|IwEBILdfysz66E9sRHsUobHgfh1X_h-esnBfcCdYjdcCfhGRkZqXujzXSN_3a8yqj5btX1B6NgbrmEX6wax_wmJ7Sgaaa39GbR-6hjDt_tHpKsFXPGwnIhy-14CWuE4oeYDWG4pCvQ4JEMKk2DiAsuwlUtoOVwaEOif1gWErh5rswCJ8mRhaXQ7SJhZB0CWYHm_ZA_PY8xTTVTUcZFqP7iz8kBw5QGDePyOb8NvJvSuBYYkwRQTj-qrytfdcHwMWOJc5QdoyPFpmchSfsMUpMqjDmwVPBfFzb0xZWYKxdUerSeKV1VVlS4Bwl2j-4gHnjHGohUsxau4Bn9SfG9McP-7RqD9Vmk3g--rsfACQ-uVLCJSJ29sBEMNkA5sxh9E9fpTwEw166WY-xBfYa_XB9aAU3n6Fn2yFM0I7ZpPIY1fy0gkdYkFOFD0uAOdoTFDxGXwbBWE";

  From now on you can use the refresh_token to authorize API calls for as long as you keep your app
  authrozied for LWA in "Your Account > Manage Login with Amazon"*/

}

void loop() {

}
