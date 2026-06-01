#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <Adafruit_VS1053.h>
#include <billyMovement.h>

//VSPI Pins
#define CLK  18
#define MISO 19
#define MOSI 23

//I2S info
#define I2S_PORT I2S_NUM_0
#define BUFFER_LEN 256
#define RECORD_TIME 5 //seconds
#define SAMPLE_RATE 16000 //Hz

//ICS 43434 Pins
#define I2S_WS  22 //LRCLK
#define I2S_SD  36 //DOUT
#define I2S_SCK 33 //BCLK

//VS1053 Pins
#define VS1053_RST  -1 
#define VS1053_CS   21
#define VS1053_DCS  26
#define VS1053_DREQ 34
#define VS1053_SDCS 25

const char* ssid = "Argylian Booster ";
const char* pswd = "44470036";

const char* host = "192.168.0.100";
const int port = 5000;

//Define your target static IP settings
IPAddress local_IP(192, 168, 0, 105);   // <-- The web address you will type into your browser
IPAddress gateway(192, 168, 0, 1);     // <-- Your home router's IP address
IPAddress subnet(255, 255, 255, 0);   // <-- Standard subnet mask

WebServer server(80);
WiFiClient client;

uint8_t buffer[2048];
File file;

int32_t samples[BUFFER_LEN];
uint32_t totalBytes = 0;

File audioFile;

Billy Bass; //custom Billy object

//create VS1053 object
Adafruit_VS1053_FilePlayer Billy_VS1053 = Adafruit_VS1053_FilePlayer(
  VS1053_RST,
  VS1053_CS,
  VS1053_DCS,
  VS1053_DREQ,
  VS1053_SDCS
);

//Button Input
int billyButton = 35; //analog input
int billyButtonData;
bool buttonPressed;
bool recording = false;

bool happy = false;
bool river = false;

void setupI2S() {
  //configure the ICS mic settings
  i2s_config_t config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

void startRecording() {
  Serial.println("Starting to record...");

  //Opens file on the SD card to write into
  audioFile = SD.open("/recording.wav", FILE_WRITE);
  if (!audioFile) {
    Serial.println("Could not create file");
    return;
  } //failing print statement

  totalBytes = 0; //resets the amount of bytes everytime a new recording starts

  // Reserve space for WAV header
  for (int i = 0; i < 44; i++) {
    audioFile.write((byte)0);
  }
}

void recordChunk() {
  int32_t samples[512]; //amount of samples per chunk
  size_t bytesRead; //variable for the amount of bytes read

  i2s_read( //tells the microphone to read and record audio signal
    I2S_NUM_0,
    samples,
    sizeof(samples),
    &bytesRead,
    portMAX_DELAY
  );

  // Convert 32-bit ICS format to 16-bit WAV format
  for (int i = 0; i < bytesRead / 4; i++) {
    int16_t sample16 = samples[i] >> 14;
    audioFile.write((uint8_t*)&sample16, 2);
    totalBytes += 2;
  }
}

void writeWavHeader(File file, uint32_t dataSize) { //writes WAV header for the file
  uint32_t fileSize = dataSize + 36;

  file.write((const uint8_t*)"RIFF", 4);
  file.write((byte*)&fileSize, 4);
  file.write((const uint8_t*)"WAVE", 4);
  file.write((const uint8_t*)"fmt ", 4);

  uint32_t subChunk1Size = 16;
  uint16_t audioFormat = 1;
  uint16_t numChannels = 1;
  uint32_t sampleRate = SAMPLE_RATE;
  uint16_t bitsPerSample = 16;
  uint32_t byteRate = sampleRate * numChannels * bitsPerSample / 8;
  uint16_t blockAlign = numChannels * bitsPerSample / 8;

  file.write((byte*)&subChunk1Size, 4);
  file.write((byte*)&audioFormat, 2);
  file.write((byte*)&numChannels, 2);
  file.write((byte*)&sampleRate, 4);
  file.write((byte*)&byteRate, 4);
  file.write((byte*)&blockAlign, 2);
  file.write((byte*)&bitsPerSample, 2);

  file.write((const uint8_t*)"data", 4);
  file.write((byte*)&dataSize, 4);
}

void stopRecording() {
  Serial.println("Recording stopped.");

  audioFile.seek(0); //goes back to start where space was reserved for header
  writeWavHeader(audioFile, totalBytes); //writes the WAV header

  audioFile.close(); //closes the file
}

void uploadAudio() {
  //opens the recording file
  File file = SD.open("/recording.wav");
  if (!file) {
    Serial.println("Unable to open file.");
    return;
  } //failing print statement

  size_t fileSize = file.size(); 
  Serial.printf("File size: %u\n", fileSize);           //records the file size 
  Serial.printf("Free heap: %u\n", ESP.getFreeHeap());  //records the amount of free heap

  //malloc means memory allocation
  uint8_t* buffer = (uint8_t*)malloc(fileSize); //dynamically allocates memory of the size of the file
  if (!buffer) {
    Serial.println("malloc failed!");
    file.close();
    return;
  } //if it fails to write to memory, print this

  size_t bytesRead = file.read(buffer, fileSize); //records the amount of bytes written to memory
  file.close(); //close the file

  if (bytesRead != fileSize) {
    Serial.println("Read failed!");
    free(buffer);
    return;
  } //debugging check for file writing accuracy

  HTTPClient http; //new http client

  http.begin("http://" + String(host) + ":" + String(port) + "/upload"); //initialize HTTP connection from ESP32 to server
  http.addHeader("Content-Type", "audio/wav"); //next line of HTTP request
  http.addHeader("Connection", "close"); //tells it that we are done sending a request

  http.setTimeout(60000); //stops trying to connect after 60 seconds

  Serial.println("Uploading from RAM...");
  int responseCode = http.POST(buffer, fileSize); //send all data inside buffer to the http client (the server)
  Serial.printf("Response: %d\n", responseCode); //debugging print statement

  if (responseCode > 0) {
    String response = http.getString();
    Serial.println(response);
  } //prints response code for debugging

  free(buffer); //deallocates all memory from buffer
  http.end(); //ends http stream and client

  Serial.println("Done uploading!");
}

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
  client.println("Host: " + String(host) + ":" + String(port)); //tells the host server (my computer IP)
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
  while (client.connected() || client.available()) { //keeps going as long as the server is connected
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
          if (currmillis - prevMillis >= 200) { //non-blocking code for "talking" animation
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

void handleResponse() {
  Serial.println("Starting response...");
  server.send(200, "text/plain", "Billy is speaking!"); //sends reply to the server saying it received the request

  Bass.raiseBody(); //initiates the talking animation
  streamResponse(); //stream the response while moving the mouth
  Bass.lowerBody(); //ends by lowering the body

  delay(1000); //ensures body is all the way lowered
  Bass.saveBody(); //saves the motors
}

void handleRiver() {
  Serial.println("Playing Take me to the River");
  server.send(200, "text/plain", "Take me to the River"); //sends reply to the server saying it received the request

  river = true; //sends to loop function
}

void handleHappy() {
  Serial.println("Playing Don't Worry Be Happy");
  server.send(200, "text/plain", "Don't Worry, Be Happy"); //sends reply to the server saying it received the request

  happy = true; //sends to loop function
}

void handleStop() {
  Serial.println("Stopping playback...");
  server.send(200, "text/plain", "Shut up Billy"); //sends reply to the server saying it received the request

  //stops and resets everything
  Billy_VS1053.stopPlaying();
  Bass.closeMouth();
  Bass.lowerBody();
  Bass.lowerTail();
}

void setup() {
  Serial.begin(115200); //initializes serial monitor for 115200 baud rate
  delay(1000); //setup delay
  SPI.begin(CLK, MISO, MOSI); //initializes SPI communication protocol

  //Motor Pins
  pinMode(13, OUTPUT); //bodyPin2
  pinMode(14, OUTPUT); //bodyPin1
  pinMode(16, OUTPUT); //mouthPin2
  pinMode(17, OUTPUT); //mouthPin1
  pinMode(27, OUTPUT); //tailPin1
  pinMode(32, OUTPUT); //tailPin2

  //sets up a static IP address for the ESP32 to run on
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Static IP config failed");
  }

  WiFi.begin(ssid, pswd); //starts the WiFi setup 

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); //waiting message
  }
  Serial.println("\nWiFi is connected!");
  Serial.println(WiFi.localIP()); //prints the ESP32's IP address

  WiFi.setSleep(false); //makes it so the WiFi connection does not timeout on the ESP32

  // Initialize VS1053
  if (!Billy_VS1053.begin()) {
    Serial.println("VS1053 not found!");
    while (1);
  }
  Serial.println("VS1053 initialized.");

  //Use to set initial volume before amp (lower = louder)
  Billy_VS1053.setVolume(20, 20);

  //SD card initialization
  if (!SD.begin(VS1053_SDCS)) {
    Serial.println("SD card failure.");
    while (1); //quits program
  }
  Serial.println("SD card initialized!");

  //ICS 43434 Initialization
  Serial.println("Starting ICS-43434 initialization...");
  setupI2S(); //calls ICS setup function

  //WiFi functions (similar to Particle functions!)
  server.on("/play/response", handleResponse);
  server.on("/play/Happy", handleHappy);
  server.on("/play/River", handleRiver);
  server.on("/play/stop", handleStop);

  server.begin(); //Starts the server we set up on the ESP32
  Serial.println("Server has started on the ESP32");

  Serial.println("\nBilly is ready!"); //Shows that Billy is ready and can be talked to!
}

void loop() {
  server.handleClient(); //keeps the Webserver up and running

  if (Billy_VS1053.playingMusic) {
    Billy_VS1053.feedBuffer(); //keeps the audio playback running
  }

  //checks to see if the button on the toy is being pressed
  billyButtonData = analogRead(billyButton);
  if (billyButtonData <= 350) {
    buttonPressed = true;
  }
  else {
    buttonPressed = false;
  }

  //if the button is pressed, then start recording
  if (buttonPressed && !recording) {
    Bass.raiseBody(); //shows he is listening

    startRecording(); //turn on mic to capture audio
    recording = true;
  }

  //keep recording while button is pressed
  if (buttonPressed && recording) {
    recordChunk(); //record one chunk at a time for data stream
  }

  //if button is released then stop recording
  if (!buttonPressed && recording) {
    Bass.lowerBody(); //shows he is done listening

    stopRecording(); //stops recording audio
    uploadAudio(); //uploads the audio to server
    recording = false;
  }

  //from happyHandler, if happy is now true, triggers song and animation
  if (happy == true) {
    happy = false;

    if (Billy_VS1053.startPlayingFile("/track001.mp3")) { //plays song
      Bass.dontWorryBeHappy(); //play animation
    }
    else {
      Serial.println("Unable to open file.");
    }
  }

  //from riverHandler, if happy is now true, triggers song and animation
  if (river == true) {
    river = false;

    if (Billy_VS1053.startPlayingFile("/track002.mp3")) { //plays audio
      Bass.takeMeToTheRiver(); //play animation
    }
    else {
      Serial.println("Unable to open file.");
    }
  }

  delay(10); //small delay to prevent throttling
}
