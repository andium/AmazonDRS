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

#ifndef __AmazonTokens_H
#define __AmazonTokens_H


//API Definitions---------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

//OAuth2 Creds------------------------------------------------------------------------------------------
//These can be found in your DRS app's LWA details screen https://developer.amazon.com/lwa/sp/overview.html
//------------------------------------------------------------------------------------------------------

#define client_id "amzn1.application-oa2-client.02d6a3b6f7ad45589b344e71ab040cff"

#define client_secret "c0daf124d15f7717de644109b7ef77769c68ed0155cf33156b926cd8c8b78541"

//------------------------------------------------------------------------------------------------------

//Redirect URI------------------------------------------------------------------------------------------

#define redirect_uri "https%3A%2F%2Fwww.getpostman.com%2Foauth2%2Fcallback" //must be URI encoded and https
                     //^^you could use this one from Postman if you'd like if you have an app or site replace 
					 //this wth your sites uri encoded callback
//------------------------------------------------------------------------------------------------------

//Refresh Token-----------------------------------------------------------------------------------------
//Used for all subsequent calls to requestNewAccessTokens() new access_token is valid for 1hr
//Refresh tokens are always valid and won't change as long as you keep your app authroized under your Amazon account
//Log in and go to 'Your Account >> Manage Login With Amazon' to see where to deauthorize and invalidate this token

static String refresh_token = "Atzr|IwEBIIKDkzgMa5g7O0R1mLZZ6YZo-Z1Ae5Ffj6wmV_rPbmAI0oixxTff8g3PN6MrOIEQpB9o_6BlDjiR6gkfRS_82Wp_0GWWvTrtYehcFJ8iKjuqf8n1ff9OrdHQU1_5nW_5Hf5loeIQ88TGazK2wy18UmOXOz1lbl9FDvWVQZl4t_6ggzDfMTVupvNr_ZtDrPm72auzoJIkGmGIM4S1RUw-Ru8X4q-UlzzYYfvRSJnPLAUTM3tTOEkv5e33SzExAaHl0tF22drRqvR1dEVlR_Zzw_IZ-1hzhIAvHrJs22K5dSEmkJKN7g8sD1tUKkn-ErMJhJrMehmvjwanF_O141Z5bVokmOLwiQKq_AYKGz8mJ0WPjBNQ3TbZ1g-3Xp-LqeMcKViNImcl1-_K89K5rupwAh-OmIPP5VhBArvhdeqEnreNU7LVLNoHvvyFB5ypwKQsHLe8";


//------------------------------------------------------------------------------------------------------

//Authorization Code------------------------------------------------------------------------------------
//Upon Authorizing DRS via LWA this is passed to requestBearerAndRefreshTokens() to grab first pair of tokens
//copy the string following code= from the LWA response ex:https://localhost/?code=ANdNAVhyhqirUelHGEHA&scope=dash:replenish

static String authorization_grant_code = "ANEcEEKnDNSZsbKveGOR";

//------------------------------------------------------------------------------------------------------

//Access Token------------------------------------------------------------------------------------------
//must be passed in all API calls in order to authenticate, requestNewAccessTokens() prior to making an api call
//to automaticall udpate this
static String access_token = "";

//------------------------------------------------------------------------------------------------------


#endif /*__AmazonTokens_H */
