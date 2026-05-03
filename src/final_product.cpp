#include "Particle.h"
#include "SPI.h" //For SPI communication protocal
#include "billyMovement.h" //custom header file for Billy's functions and animations!
#include "Adafruit_VS1053.h" //Adafruit_VS1053.h
#include "SdFat.h" //SdFat.h

//variables for Billy sensors ON the Billy Bass toy!
int billySensor = A1;
int billyButton = A0;

int billyButtonData;
int currSensorData;
int prevSensorData;

int threshold = 15; //threshold for sensor 
unsigned long int cooldown = 40000; //cooldown timer to prevent back to back triggers 
unsigned long int lastTrigger = 0; //time since last trigger
unsigned long int timeToToggle;

//variables for activating Billy's motion
int count = 0;
bool rolling = false;
bool happy = false;
bool river = false;
bool sensorToggle = false;
bool startResponse = false;

// VS1053 pins
#define VS1053_RESET  D10 //Reset pin
#define VS1053_CS     D13 // Chip Select
#define VS1053_DCS    D6  // Data Control Select pin 
#define VS1053_DREQ   D7  // Data Request pin
#define CARDCS        D14 // SD card chip select

SdFat SD; //creates SD object for SdFat library use

//create VS1053 object
Adafruit_VS1053_FilePlayer Billy_VS1053 = Adafruit_VS1053_FilePlayer(
  VS1053_RESET,
  VS1053_CS,
  VS1053_DCS,
  VS1053_DREQ,
  CARDCS
);

Billy Bass; //creates custom Billy Bass object

TCPClient client; //object to be able to get/stream data from website!
String host = "host_IP_address"; //IP address of the computer hosting the server to stream data from
int port = 0; //the port number of the server to stream data from

uint8_t buffer[256]; //creates buffer of 256 bytes of data to be able to stream data to the VS1053 cleanly

//Particle function declaration
int playResponse(String command); //to play the reponse gotten from the website via TCPClient
int playTakeMeToTheRiver(String command); //plays audio and animation for Take me to the River
int playDontWorryBeHappy(String command); //plays audio and animation for Don't Worry, Be Happy
int playRickRoll(String command); //plays audio and animation for the Super Secret Rick Roll feature
int toggleSensor(String command); //ability to toggle the sensor readings from the website!

void streamResponse() { 
  //Connecting to the local server
  Serial.println("Conecting to server...");
  if (!client.connect(host, port)) { //attempts to connect to the server on computer's IP address and port 
    Serial.println("Connection failed!"); 
    return;
  }
  Serial.println("Connected to server!");

  //sending the request to the server
  client.println("GET /static/response.mp3 HTTP/1.1"); //sending GET request for this specific path  
  client.println("Host: " + host + ":" + String(port)); //tells the host server 
  client.println("Connection: close"); 
  client.println(); //tells the server we are done sending the request
  Serial.println("Request sent"); //debugging print statement

  //skip the header from the recieved message from server!
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break; //this means the headers are done
  }
  Serial.println("Headers skipped. Starting audio...");

  static unsigned long int prevMillis = 0;
  
  //Streaming audio
  while (client.connected()) { //keeps going as long as the server is connected
    if (client.available()) { //if bytes are avalible to stream
      int length = client.read(buffer, sizeof(buffer)); //pulls bytes from stream and stores it in buffer[]

      if (length <= 0) continue; //continue if there are bytes in the stream

      int i = 0;
      while (i < length) { //while the index is not larger than the length of the buffer size

        if (Billy_VS1053.readyForData()) { //manually streaming the bytes to the VS1053
          int chunkSize = min(32, length - i); //compares the max amount I want to send (32) to whatever might be in the buffer

          Billy_VS1053.playData(buffer + i, chunkSize); //plays data from the entire buffer

          i += chunkSize; //moves the index to the next chunk of data to stream

          unsigned long int currmillis = millis();
          if (currmillis - prevMillis >= 200) { //non-blocking code for "talking"
            static bool isOpen = false;

            if (isOpen) {
              Bass.closeMouth();
            } else {
              Bass.openMouth();
            }

            isOpen = !isOpen;
            prevMillis = currmillis;
          }
        }
      } //while there is still data left, and if the VS1053 is ready for said data, then send it from the buffer
    }
  }

  Bass.closeMouth(); //ensures the mouth is closed after response is over
  client.stop(); //after done streaming the data, close the connection and reset the client
  Serial.println("Stream complete"); 
}

void setup() {
  Serial.begin(9600); //intitializes Serial Monitor
  waitUntil(WiFi.ready); //waits to run code until the Photon has connected to WiFi

  //Motor Pins
  pinMode(D0, OUTPUT); //mouthPin2
  pinMode(D1, OUTPUT); //mouthPin1
  pinMode(D2, OUTPUT); //bodyPin2
  pinMode(D3, OUTPUT); //bodyPin1
  pinMode(D4, OUTPUT); //tailPin1
  pinMode(D5, OUTPUT); //tailPin2

  // Initialize VS1053
  if (!Billy_VS1053.begin()) {
    Serial.println("VS1053 not found!");
    while (1);
  }
  Serial.println("VS1053 initialized.");

  //Use to set initial volume before amp (lower = louder)
  Billy_VS1053.setVolume(20, 20);

  //initialize the SD card
  if (!SD.begin(CARDCS, SD_SCK_MHZ(4))) { //slows SPI clock down to 4MHz (instead of default 50MHz)so SD can be initialized
    Serial.println("SD failed.");
    while (1);
  }
  Serial.println("SD card initialized.");

  //VS1053 interrupt pin (DREQ). Allows Photon to play music from VS1053 in the background cleanly!
  //when DREQ goes HIGH, this allows DREQ to interrupt current process to stream data, then returns to what it was doing previously
  Billy_VS1053.useInterrupt(VS1053_FILEPLAYER_PIN_INT); 

  //Initializes all Particle Functions used
  Particle.function("cF_Billy_Audio", playResponse);
  Particle.function("cF_To_The_River", playTakeMeToTheRiver);
  Particle.function("cF_Be_Happy", playDontWorryBeHappy);
  Particle.function("cF_Rick_Roll", playRickRoll);
  Particle.function("cF_Toggle_Sensor", toggleSensor);

  prevSensorData = analogRead(billySensor); //stops the trigger on startup
  timeToToggle = millis() + 100; //non-blocking delay initialization
}

void loop() {
  unsigned long int currTime = millis(); //establishes current time
  billyButtonData = analogRead(billyButton); //reads data from the button

  //MOTION SENSOR TRIGGER
  if (sensorToggle) { //can be disabled from Particle Function
    if (currTime >= timeToToggle) { //reads and prints sensor data every 100ms
      currSensorData = analogRead(billySensor); //reads data from toy's sensor
      
      //Sensor data print statements
      Serial.print(currSensorData);
      Serial.print(", ");
      Serial.println(billyButtonData);

      //if the sensor triggers OR the button triggers AND it is past the cooldown!
      if ((((currSensorData - prevSensorData) >= threshold) || (billyButtonData <= 350)) && ((millis() - lastTrigger) >= cooldown)) { //if it senses motion
        lastTrigger = millis(); //updates lastTrigger

        //Sensor data print statements
        Serial.print(currSensorData);
        Serial.print(", ");
        Serial.println(billyButtonData);

        if (count % 2 == 1) {
          Billy_VS1053.startPlayingFile("track002.mp3"); //non-blocking play feature to play file in the background
          Bass.takeMeToTheRiver(); //blocking code that stops everything to do this 
        }
        else if (count % 2 == 0) {
          Billy_VS1053.startPlayingFile("track001.mp3"); //plays from SD card
          Bass.dontWorryBeHappy();
        }

        count++;
      }
      prevSensorData = currSensorData; //updates previous data
      timeToToggle += 100; //sets new time to toggle 100ms in the future
    }
  }

  //for activating the Rick Roll Particle Function from website
  if (rolling) { 
    delay(2500); //waits for video to load :)
    rolling = false;

    Billy_VS1053.startPlayingFile("track003.mp3");
    Bass.rickRoll();
  }

  //activating the Take me to the River Particle Function from website
  if (river) {
    river = false;

    Billy_VS1053.startPlayingFile("track002.mp3");
    Bass.takeMeToTheRiver();
  }

  //activating the Don't Worry, Be Happy Particle Function from website
  if (happy) {
    happy = false;

    Billy_VS1053.startPlayingFile("track001.mp3");
    Bass.dontWorryBeHappy();
  }

  //activating the response animation and audio from Particle Function
  if (startResponse) {
    startResponse = false;

    Bass.raiseBody();
    streamResponse();
    Bass.lowerBody();
  }
}

// Particle Function Definitions //
int playResponse(String command) {
  if (command == "HIGH") {
    startResponse = true; //enables variable to start the response

    return 0;
  }
  else return -1; //error return statement
}

int playTakeMeToTheRiver(String command) {
  if (command == "HIGH") {
    river = true;

    return 0;
  }
  else return -1;
}

int playDontWorryBeHappy(String command) {
  if (command == "HIGH") {
    happy = true;

    return 0;
  }
  else return -1;
}

int playRickRoll(String command) {
  if (command == "ROLL") {
    rolling = true;

    return 0;
  }
  else return -1;
}

int toggleSensor(String command) {
  if (command == "HIGH") {
    sensorToggle = true; //sensorToggle = HIGH;

    return 0;
  }
  else if (command == "LOW") {
    sensorToggle = false; //sensorToggle = LOW;

    return 0;
  }
  else return -1; //error return statement
}
