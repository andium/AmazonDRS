# amazonDRS

# Getting Started
This repo is intended to serve as an Arduino library aimed at getting started with Amazon's [Dash Replenishment Service](https://developer.amazon.com/dash-replenishment-service)! Amazon currently has some great [DRS documentation](https://developer.amazon.com/public/solutions/devices/dash-replenishment-service/) that I'll reference in this guide although currently offers nothing directed towards Arduino and the DIY hacker community. This guide is meant to supplement Amazon's documentation with a focus on implementing the API on [WiFi101](https://www.arduino.cc/en/Reference/WiFi101) or [ESP8266](https://github.com/esp8266/Arduino) enabled Arduinos! This includes the [ArduinoMKR1000](https://www.arduino.cc/en/Main/ArduinoMKR1000), an Arduino sporting the [WiFi101 shield](https://store-usa.arduino.cc/products/asx00001), an [Adafruit MO WiFi Feather](https://www.adafruit.com/product/3010), Adafruit Feather Huzzah ESP8266, or really any Arduino with sufficient space and TLS capable WiFi.

By the end of this guide you should have a functioning Dash Button similar to Amazon's [AWS IoT Button](https://aws.amazon.com/iotbutton/), with one small difference! Although the first example [amazonDashButton](https://github.com/andium/AmazonDRS/tree/master/examples/amazonDashButton) allows you to initiate purchases on amazon using a pushbutton, you now have the tools necessary to leverage the power of Arduino to initate these frictionless purchases with actuations beyond the motion of a simple button push!

This guide is not meant to be a complete implementation of the Dash API and is by no means the most effecient or cost effective way to incorporate frictionless purchasing into your product. That being said, if you're looking to prototype and hack together your own dash button or frictionless purchase proof of concept, you've come to the right place!

Before we dive right in it couldn't hurt to run a quick test that will save you some headache later. If you're using the WiFi101 library and the ATWINC1500 you may need to load the SSL certificates for Amazon's DRS API endpoints. You can check this by running the example [WiFiSSLClient](https://github.com/arduino-libraries/WiFi101/blob/master/examples/WiFiSSLClient/WiFiSSLClient.ino) sketch and attempt to connect (just try a "GET /") to https://api.amazon.com or https://dash-replenishment-service-na.amazon.com. If you can't connect you'll have to run the firmware updater and manually load these certificiates before moving forward. Here's a [great guide](https://www.arduino.cc/en/Tutorial/FirmwareUpdater) on Arduino's site on how to do this, just scroll down to 'Certificate Loading'.

# Initial Setup

## SNS Simple Notification Service
To start you're going to need some amazon accounts! It probably goes without saying that you'll need an amazon account to authorize your arduino to make purchaes on your behalf, but you'll also need to sign up for a free tier of AWS.

* [Create a developer account with amazon web services](https://aws.amazon.com/free/?sc_channel=PS&sc_campaign=acquisition_US&sc_publisher=google&sc_medium=cloud_computing_b&sc_content=aws_account_bmm_control_q32016&sc_detail=%2Baws%20%2Baccount&sc_category=cloud_computing&sc_segment=102882724242&sc_matchtype=b&sc_country=US&s_kwcid=AL!4422!3!102882724242!b!!g!!%2Baws%20%2Baccount&ef_id=WGw8xgAABK9kcEZO:20170104000734:s). With your AWS account activated you'll be creating an SNS [Simple Notifications Service](https://aws.amazon.com/documentation/sns/) to handle the messaging for your device. This typically includes e-mails that are sent on behalf of your device when products or replenished, or if you were to authorize/deauthrize the device.

* Amazon has some great step-by-step instructions with screenshots on setting up the SNS Topic for Dash Replenishment! You can find those [here](https://developer.amazon.com/public/solutions/devices/dash-replenishment-service/docs/dash-create-an-sns-topic). Follow along and once you create the SNS topic you can pretty much forget about it. You won't touch it again during the dev process.

##LWA Login With Amazon
Next you're going to need to create a [login with amazon](https://developer.amazon.com/login-with-amazon) security profile.

* [Creating the security profile](https://developer.amazon.com/lwa/sp/create-security-profile.html) will allow you to authorize your Arduino to make purchases by doing just that, 'Logging in with Amazon!' Typically device manufacturers would inlcude this step as a flow in their product registration website or application. Since we're going for a bare bones proof of concept we're going to keep this as simple as possible. Fill out the required fields...

 * **Security Profile Name:**	Pick any name, this is the name you'll see when the login with amazon screen pops up
 * **Security Profile Description:**	anything goes
 * **Consent Privacy Notice URL:**  http://www.andium.com/thisdoesntexistyet (any url doesn't have to be valid yet)
 * **Consent Logo Image:** choose an img/not totally necessary, this appears as the thumbnail represeting your product in 'Your Account >> Manage Login With Amazon' where you can deauthorize your device and "un-tie" it from your amazon account.
 
* When creating your security profile don't forget to add your redirect_uri to the "Allowed Return URL" section (ex: https://www.getpostman.com/oauth2/callback). This field can be found under 'Web Settings', this will make more sense in the 'consent request' section.

* Click save and you're ready to move on! Your LWA security profile is created, time to save some important info for later.

* Click show Client ID and Client Secret. Save these details or keep the tab open, you're going to hard code them into your [AmazonTokens.h](https://github.com/andium/AmazonDRS/blob/master/src/AmazonTokens.h) file. More on this later when we get to setting up the Arduino. They should look something like this...
```
Client ID: amzn1.application-oa2-client.c0a2100c1af289b1bf4011c71f6029b3
Client Secret: ec48483c54c34a23a71aa8ccb2742902f7d3d00c2dd78fc5ac404eef0111e485
```

##Create a Dash Replenishment Device
Now it's time to create what amazon refers to as your Dash Replenishment Device. This [device creation wizard](https://developer.amazon.com/dash-replenishment/create_device.html) makes this process really simple. Essentially you're creating the infrastructure that will maintin what products you'd like to make available to your device.
* First step is to create the Device Name, Model ID, and upload a transparent png img (<1MB)
* Next you'll create "Slots" for your device
 * It seems Slots are intended to represent discrete product types although you don't need to maintain this convention. For example a coffee maker may utilize Slot 1 for replacement coffee flavors, and Slot 2 for various coffee filters. When the user authorizes the device for replenishment they will choose one product from each slot to be eligible for replenishment.
  * In each slot you can add any number of ASINs. ASINs are unique product identifiers used by Amazon to identify a product. You can find the ASIN within the product details of an item on Amazon. For example these batteries [https://www.amazon.com/gp/product/B00MNV8E0C/ref=s9_dcacsd_bhz_bw_c_x_1_w](https://www.amazon.com/gp/product/B00MNV8E0C/ref=s9_dcacsd_bhz_bw_c_x_1_w) have an ASIN of B00MNV8EOC. Another thing to keep in mind is that not all products available on Amazon are eligible for dash replenishment. Currently only items that are "Shipped and Sold by Amazon" are eligible for DRS.

* Upon completing these steps you have completed creating your device! Take note of your devices Model ID as well as the associated Slot IDs. We'll need the Model ID when we create our login with amazon link.

* It's also worth noting that you can create multiple devices for testing and utilize the same LWA profile for authorizing the device.

##Login With Amazon [Consent Request](https://developer.amazon.com/public/solutions/devices/dash-replenishment-service/docs/dash-lwa-web-api)/Authorization Code Grant
Phewf! At this point we have created everything we need from Amazon to get started! Now it's time to start hacking this together! Before we can start running some code on the Arduino we need to authorize the device to make purchases on behalf of our amazon account. Don't worry there is a [test flag](https://developer.amazon.com/public/solutions/devices/dash-replenishment-service/docs/dash-test-device-purchases) that we're going to set so we won't actually be ordering anything. (Even though we'll still get notifications that a purchase was made).

* It's time to construct the url that will bring us to the consent request screens! Here are the details we'll need...

 * **client_id:** amzn1.application-oa2-client.c0a2100c1af289b1bf4011c71f6029b3 (from the LWA Security Profile)
 * **scope:** dash:replenish. (DRS always uses this scope)
 * **scope_data:** ```{"dash:replenish":{"device_model":"Arduino_dash","serial":"arduino123", "is_test_device":"true"}}```
  * **device_model:** Arduino_dash (Model ID from the "Create Dash Replenishment Step")
  * **serial:** arduino123 (Make this up! It's supposed to uniquely identify your hardware.)
 * **response_type:** code (Code, because we're asking for an authorization_code which we'll use to gain tokens for API access)
 * **redirect_uri:** https://www.getpostman.com/oauth2/callback (This would be the uri of your customer facing app, but we're skipping the whole app part so we're just going to use this from [Postman](https://www.getpostman.com/))

 Before it's encoded, here's what an example request url to https://www.amazon.com/ap/oa should look like...
```
https://www.amazon.com/ap/oa?client_id=amzn1.application-oa2-client.c0a2100c1af289b1bf4011c71f6029b3&scope=
dash:replenish&scope_data={"dash:replenish":{"device_model":"Arduino_dash","serial":"arduino123", "is_test_device":"true"}}&response_type=code&redirect_uri=https://www.getpostman.com/oauth2/callback
```
Now with URI encoded fields
```
https://www.amazon.com/ap/oa?client_id=amzn1.application-oa2-client.c0a2100c1af289b1bf4011c71f6029b3&scope=dash%3Areplenish&scope_data=%7B%22dash%3Areplenish%22%3A%7B%22device_model%22%3A%22Arduino_dash%22%2C%22serial%22%3A%22arduino123%22%2C%20%22is_test_device%22%3A%22true%22%7D%7Dtest&response_type=code&redirect_uri=https%3A%2F%2Fwww.getpostman.com%2Foauth2%2Fcallback
```
You'll notice that not all field values change when they are URI encoded, only the scope and redirect_uri actually change. When composing your request url you can use this handy [encoder/decoder](http://meyerweb.com/eric/tools/dencoder/) to quickly encode your values.

If you've composed everything correctly paste the encoded url into the browser and it should bring you to an Amazon login page where you'll log in and authorize your device for replenishment! Notice your LWA Security Profile name displayed on the top. Follow the prompts and select which product you'd like to assign to each slot. On the last screen you confirm your shipping and billing information and click 'complete setup'! Keep an eye on the url bar at the top of the browser, you should get a response back that looks simillar to this...

```
https://app.getpostman.com/oauth2/callback?code=ANdNAVhyhqirUelHGEHA&scope=dash%3Areplenish
```
Now you've got your authorization_code! Jot down the string following code= "ANdNAVhyhqirUelHGEHA". Time to move on to the Arduino and use this authorization code to gain access and refresh tokens to access the Dash API.

## authCodeGrant
At this point you should have all the details you need to get started working with the AmazonDRS Arduino library! Be sure to clone the repo and [import the library to your Arduino IDE](https://www.arduino.cc/en/Guide/Libraries). The AmazonDRS library also has a couple dependencies. You will need to make sure you have the [Wifi101 library](https://github.com/arduino-libraries/WiFi101) installed as well as JSON parsing library [ArduinoJson](https://github.com/bblanchon/ArduinoJson).

Once you've installed the necessary libraries navigate to Arduino>>Libraries where the AmazonDRS library is stored and open the [AmazonTokens.h](https://github.com/andium/AmazonDRS/blob/master/src/AmazonTokens.h) file in your editor. Time to fill in some details...

```
#define client_id "amzn1.application-oa2-client.c0a2100c1af289b1bf4011c71f6029b3"

#define client_secret "ec48483c54c34a23a71aa8ccb2742902f7d3d00c2dd78fc5ac404eef0111e485"

#define redirect_uri "https%3A%2F%2Fwww.getpostman.com%2Foauth2%2Fcallback"
```

You can leave authorization_code, refresh_token and access_token fields blank for now. In the Arduino IDE navigate to File>>Examples>>AmazonDRS>>authCodeGrant to launch the sketch.  Before running the sketch be sure to edit
```
#define auth_code "ANdNAVhyhqirUelHGEHA" //ex: ANdNAVhyhqirUelHGEHA
```
with your authorization code and enter your WiFi SSID and password. Open up the serial terminal and watch for the output. If everything goes well your refresh_token will be printed to the screen. It should look something like this...
```
  refresh token = "Atzr|IwEBILdfysz66E9sRHsUobHgfh1X_h-esnBfcCdYjdcCfhGRkZqXujzXSN_3a8yqj5btX1B6NgbrmEX6wax_wmJ7Sgaaa39GbR-6hjDt_tHpKsFXPGwnIhy-14CWuE4oeYDWG4pCvQ4JEMKk2DiAsuwlUtoOVwaEOif1gWErh5rswCJ8mRhaXQ7SJhZB0CWYHm_ZA_PY8xTTVTUcZFqP7iz8kBw5QGDePyOb8NvJvSuBYYkwRQTj-qrytfdcHwMWOJc5QdoyPFpmchSfsMUpMqjDmwVPBfFzb0xZWYKxdUerSeKV1VVlS4Bwl2j-4gHnjHGohUsxau4Bn9SfG9McP-7RqD9Vmk3g--rsfACQ-uVLCJSJ29sBEMNkA5sxh9E9fpTwEw166WY-xBfYa_XB9aAU3n6Fn2yFM0I7ZpPIY1fy0gkdYkFOFD0uAOdoTFDxGXwbBWE";
```
Save your refresh_token in a safe place! For as long as your app is authorized to make purchases on belahf of your account this token will be valid. You'll use it to exchange for an access_token each time your app accesses the API. Be sure to edit the AmazonTokens.h file and add your refresh_token.

This example sketch also checks the status of your slots by accessing the [/subscriptionInfo endpoint](https://developer.amazon.com/public/solutions/devices/dash-replenishment-service/docs/dash-getsubscriptioninfo-endpoint). You should have received a response like this...
```
{"slot1":true,"slot2":false}
```
slot1 is the slot ID which corresponds to the slots you created in your dash replenishment device.

You've got everything you need now to initiate purchases from your Arduino!

##amazonDashButton
This example sketch assumes you've made it through all of the initial getting started steps and have successfully ran the authCodeGrant example sketch and have copied over your refresh_token to the AmazonTokens.h file in your Arduino>>libraries>>AmazonDRS>>src directory. Before you run this sketch you'll need to wire up a pushbutton to a DIO pin and set dashButton = to whichever pin you connect the pushbutton to. Don't forget to edit the ssid and password for the WiFi connection!

Unsure how to wire up a pushbutton? Check out [this example](https://www.arduino.cc/en/Tutorial/Button) first!
```
#define slotNumber 1 //This will vary for multi slot devices - dash buttons typically only serve one product/slot
```
slotNumber correlates to the slots you created when configuring your dash replenishment device.

Run the sketch and check for the output in the terminal, if all goes well you'll also receive an e-mail from the Amazon SNS service you created notifying you a purchase was made when you push the button!

Your custom Arduino Dash button is complete! Curious on how else you might be able to initiate purchases? Check out the [amazonDashNFC](https://github.com/andium/AmazonDRS/tree/master/examples/amazonDashNfc) example sketch to see how you can utilize NFC to initate purchases. Or check out some ideas on [hackster.io](https://www.hackster.io/contests/DRS/ideas) submissions on the Amazon DRS Developer Challenge.
