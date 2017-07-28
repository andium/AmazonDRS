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

#ifndef __AmazonDRS_H
#define __AmazonDRS_H

#ifdef ESP8266 
#include <Client.h>
#else
#include <WiFi101.h>
#endif 

#include "Arduino.h"
#include <ArduinoJson.h>
#include "AmazonTokens.h"

#define DEBUG_DASH 

class AmazonDRS
{ 

	public:

		#ifdef ESP8266 
		Client * m_client;
		void begin(Client * client);
		#else
	        WiFiSSLClient * m_client;
	    	void begin(WiFiSSLClient * client);
		#endif 

		//Slot Subscription Status
		String slotsSubscriptionStatus = "";

		//Slot Id Array and Statuses---
	  	String slotId[10] = {};
		String slotStatus[10] = {};
	  	int numSlots = 0;

		AmazonDRS();

   		void requestNewAccessTokens();
		String requestReplenishmentForSlot(String slotId);
		void requestBearerAndRefreshTokens();

		void retrieveSubscriptionInfo();

		String getRefreshToken();
		String getSlotStatus(int slotNumber);
	  	String getSlotId(int slotNumber);

	  	void setAuthCode(String authCode);


	private:
		//json parsing vars---------
		String _responseBody = "";
		bool _jsonBody = false;
		//----------------------------
		String assembleRefreshTokenBody(String refreshToken, String clientId, String clientSecret, String redirectUri);
		String assembleAuthCodeBody(String authCode, String clientId, String clientSecret, String redirectUri);
		void parseSlotIds(String subscriptionStatus);
	  	void processSubscriptionInfoResponse();
		void processRefreshTokenResponse();


};






#endif /*__AmazonDRS_H */
