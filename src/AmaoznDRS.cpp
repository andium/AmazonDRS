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



AmazonDRS::AmazonDRS()
{


}


void AmazonDRS::begin(char ssid[], char pass[])
{
  //connect to WiFi
  Serial.println("Initializing DRS... connecting to WiFi");
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    Serial.println(".");
    delay(3000);
  }

  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());

}

void AmazonDRS::requestNewAccessTokens()
{
    char server[] = "api.amazon.com";    //api endpoint uri
    //assemble the body
    String refreshTokenBody = assembleRefreshTokenBody(refresh_token, client_id, client_secret, redirect_uri);
    //----------------------------------------------------------------
    //Request for new Access Token using Refresh Token
    //----------------------------------------------------------------
    
    if (client.connect(server, 443)) 
    {
      client.println("POST /auth/o2/token HTTP/1.1");
      client.println("Host: api.amazon.com");
      client.println("Cache-Control: no-cache");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(refreshTokenBody.length());
      client.println();
      client.println(refreshTokenBody);
      //----------------------------------------------------------------
      client.println("Connection: close");
      client.println();
    } 

    delay(1000);
    processRefreshTokenResponse();

}

void AmazonDRS::requestReplenishmentForSlot(String slotId)
{   
    Serial.print("Requesting Replenishment for slotId ");
    Serial.println(slotId);
    char server[] = "dash-replenishment-service-na.amazon.com";    //api endpoint uri
    // Assemble POST request:
    //----------------------------------------------------------------
    //Request replenishment for supplied slotId
    //----------------------------------------------------------------
    if (client.connect(server, 443)) 
    {
      client.print("POST /replenish/");
      client.print(slotId);
      client.println(" HTTP/1.1");
      client.println("Host: dash-replenishment-service-na.amazon.com");
      client.print("Authorization: Bearer ");
      client.println(access_token);
      client.println("x-amzn-accept-type: com.amazon.dash.replenishment.DrsReplenishResult@1.0");
      client.println("x-amzn-type-version: com.amazon.dash.replenishment.DrsReplenishInput@1.0");
      client.println("Cache-Control: no-cache");
      //----------------------------------------------------------------
      client.println("Connection: close");
      client.println();
    }
    delay(1000);
    
     while (client.available()) {
    char c = client.read();
    Serial.write(c);
    }

    client.stop();

}

void AmazonDRS::retrieveSubscriptionInfo()
{
    requestNewAccessTokens();

    #ifdef DEBUG_DASH
    Serial.println("Updating subscription info for all slots!");
    #endif

    char server[] = "dash-replenishment-service-na.amazon.com";    //api endpoint uri
    // Assemble GET request:
    //----------------------------------------------------------------
    //Request to gather subscription info {"slot_id":true/false}
    //----------------------------------------------------------------
    if (client.connect(server, 443)) 
    {
      client.println("GET /subscriptionInfo HTTP/1.1");
      client.println("Host: dash-replenishment-service-na.amazon.com");
      client.print("Authorization: Bearer ");
      client.println(access_token);
      client.println("x-amzn-accept-type: com.amazon.dash.replenishment.DrsSubscriptionInfoResult@1.0");
      client.println("x-amzn-type-version: com.amazon.dash.replenishment.DrsSubscriptionInfoInput@1.0");
      client.println("Cache-Control: no-cache");
      //----------------------------------------------------------------
      client.println("Connection: close");
      client.println();
    }
    delay(1000);
    processSubscriptionInfoResponse();

  
}

void AmazonDRS::requestBearerAndRefreshTokens()
{   
    char server[] = "api.amazon.com";    //api endpoint uri
    //assemble the body                     
    String authCodeBody = assembleAuthCodeBody(authorization_grant_code, client_id, client_secret, redirect_uri);
    // Assemble POST request:
    //----------------------------------------------------------------
    //Request for Bearer and Refresh Tokens
    //----------------------------------------------------------------
    if (client.connect(server, 443)) 
    {
      client.println("POST /auth/o2/token HTTP/1.1");
      client.println("Host: api.amazon.com");
      client.println("Cache-Control: no-cache");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(authCodeBody.length());
      client.println();
      client.println(authCodeBody);
      //----------------------------------------------------------------
      client.println("Connection: close");
      client.println();
    }

    delay(1000);
    processRefreshTokenResponse();
    
}


String AmazonDRS::assembleRefreshTokenBody(String refreshToken, String clientId, String clientSecret, String redirectUri)
{
   String tempTokenBody = "";
   
   tempTokenBody += "grant_type=refresh_token";
   tempTokenBody += "&"; 
   tempTokenBody += "refresh_token=";
   tempTokenBody += refreshToken;
   tempTokenBody += "&";
   tempTokenBody += "client_id=";
   tempTokenBody += clientId;
   tempTokenBody += "&";
   tempTokenBody += "client_secret=";   
   tempTokenBody += clientSecret;
   tempTokenBody += "&";
   tempTokenBody += "redirect_uri=";
   tempTokenBody += redirectUri;   

   return tempTokenBody;
}

String AmazonDRS::assembleAuthCodeBody(String authCode, String clientId, String clientSecret, String redirectUri)
{
  String tempTokenBody = "";
   
   tempTokenBody += "grant_type=authorization_code";
   tempTokenBody += "&"; 
   tempTokenBody += "code=";
   tempTokenBody += authCode;
   tempTokenBody += "&";
   tempTokenBody += "client_id=";
   tempTokenBody += clientId;
   tempTokenBody += "&";
   tempTokenBody += "client_secret=";   
   tempTokenBody += clientSecret;
   tempTokenBody += "&";
   tempTokenBody += "redirect_uri=";
   tempTokenBody += redirectUri;   

   return tempTokenBody;
}

void AmazonDRS::processSubscriptionInfoResponse()
{
  while (client.available()) 
     {
      char c = client.read();

      #ifdef DEBUG_DASH
      Serial.write(c);
      #endif
       
        if(c == '{') //catch the beginning of the json
        {
          _jsonBody = true;
        }
      
        if(_jsonBody)
        {
          _responseBody += c;
        }
     }

     delay(1000);
      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(_responseBody);
       if (!root.success()) 
       {
        Serial.println("parse subscriptionStatus failed");
        return;
       }
       String temp = root["slotsSubscriptionStatus"];
       slotsSubscriptionStatus = temp;
       //Serial.println(slotsSubscriptionStatus);
       //clear response body for next process
       _responseBody = "";
       _jsonBody = false;

       parseSlotIds(slotsSubscriptionStatus);
       client.stop();

}

void AmazonDRS::parseSlotIds(String subscriptionStatus)
{
    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject&  root = jsonBuffer.parseObject(subscriptionStatus);
    
    if (!root.success()) 
    {
      Serial.println("parseObject() failed");
      return;
    }   

  int j=1; //So slot# correlates with App slot#'s
  //iterate through all slots and store their status
  for(JsonObject::iterator it=root.begin(); it!=root.end(); ++it)
  {
    slotId[j] = it->key;
    slotStatus[j] = it->value.asString();
    j++; //number of slots

  }

  numSlots = j-1;

  Serial.print(numSlots);
  Serial.println(" Slot(s) statuses have been updated!");
}

void AmazonDRS::processRefreshTokenResponse()
{
     while (client.available()) 
     {
      char c = client.read();

      #ifdef DEBUG_DASH
      Serial.write(c);
      #endif
       
        if(c == '{')
        {
          _jsonBody = true;
        }
      
        if(_jsonBody)
        {
          _responseBody += c;
        }
     }
      
      
      //Parse the json resposne constaining access and refresh tokens
      StaticJsonBuffer<1100> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(_responseBody);
       if (!root.success()) 
       {
        Serial.println("refresh token response parse  failed");
        return;
       }
       String newAccessToken = root["access_token"];
       String newRefreshToken = root["refresh_token"]; //refresh token does not change unless LWA is deauthorized
       access_token = newAccessToken;
       refresh_token = newRefreshToken;  
       //clear response body for next process
       _responseBody = "";
       _jsonBody = false;

       #ifdef DEBUG_DASH
       Serial.println(access_token);
       Serial.println(refresh_token);
       Serial.println("Tokens updated!");
       #endif

       client.stop();
}

void AmazonDRS::setAuthCode(String authCode)
{
  authorization_grant_code = authCode;
}

String AmazonDRS::getRefreshToken()
{
  return refresh_token;
}

//Slot getters so Arduino app can make decisions based on slot availability
String AmazonDRS::getSlotStatus(int slotNumber)
{
  return slotStatus[slotNumber];
}

String AmazonDRS::getSlotId(int slotNumber)
{
  return slotId[slotNumber];
}
