
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define USE_SERIAL Serial
WiFiMulti WiFiMulti; 
// Insert Firebase project API Key
#define API_KEY "API_KEY_FIREBASE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "DB_URL" 
#define USER_EMAIL "EMAIL_ID"
#define USER_PASSWORD "EMAIL_PASSWORD"
//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
//ultrasonic
const int trigPin = 5;
const int echoPin = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

void setup(){
  USE_SERIAL.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP("WIFI_NAME", "WIFI_PASSWORD");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        USE_SERIAL.printf("trying to connect to wifi.....");
        USE_SERIAL.flush();
        delay(100);
    }

    String ip = WiFi.localIP().toString();
    USE_SERIAL.printf("[SETUP] WiFi Connected %s\n", ip.c_str());


  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  /* Sign up */
  // if (Firebase.signUp(&config, &auth, "gokulmedfic@gmail.com", "gokul@28")){
  //   Serial.println("ok");
  //   signupOK = true;
  // }
  // else{
  //   Serial.printf("%s\n", config.signer.signupError.message.c_str());
  // }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  
  if ((millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
  Serial.println("in");

    sendDataPrevMillis = millis();
    
    digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (Feet): ");
  Serial.println(distanceInch/12);
  
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "board1/inputs/WaterDist", distanceInch/12)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } 
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    Serial.println("out");
  }
}
