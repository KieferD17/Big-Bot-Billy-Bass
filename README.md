# Big-Bot-Billy-Bass
Solo Project by: Kiefer Dick - 
[Project GitHub](https://github.com/KieferD17/Big-Bot-Billy-Bass/tree/main) 

<img width="3024" height="4032" alt="final_product_display" src="https://github.com/user-attachments/assets/cba617c3-e3f9-48d1-8c5d-9b089f02202b" />

## Description
This is an extension to the Big Bot Billy Bass to incorporate a second input device of a microhpone. This way, you are now able to actually have a conversation with Billy! Same as before this project involved taking apart the classic Big Mouth Billy Bass toy from the 1990's, but instead of the Particle Photon 2, I switched to the much faster ESP32. The user can now either type a message into the website or ask Billy verbally by pressing the button on the toy and it will respond with its own Billy AI.  

## Features
 - Has unique Billy AI voice response to each question entered into the website
 - Custom web interface
     - Can toggle the sensors from being able to trigger the songs and animations
     - Can enter any question for Billy to answer
     - Can control which song Billy will play
     - Replay Billy's response in case you did not hear the first time
     - Reset Billy's AI memory for RAM purposes
 - Can play the original songs from the toy
     - Don't Worry, Be Happy by Bobby McFarin
     - Take me to the River by Al Green
 - Custom audio amplifier with volume control
     - User can change the volume to make Billy more or less annoying
     - The on/off switch on the back of Billy turns off the speaker
 - Microphone input which allows user to communicate with the Billy AI
     - Press and hold the button and speak to Billy
     - Release button when done talking, then wait for the response
     - Can request the original Billy Bass songs to be played by asking "Play Take me to the River" or "Play Don't Worry Be Happy" 

## Project Files
 - main.cpp - cummulative code that is flashed to the ESP32
 - billyMovement.h - custom header file that declares Billy class as well as movement functions
 - billyMovement.cpp - defines all Billy class functions
 - index.html - contains source code for the HTML project website
 - style.css - CSS code to customize the Billy interface
 - server.py - Custom Python Server that runs locally to hold the OpenAI model used for Billy

## Hardware Components
 - Espressif ESP WROOM32 Dev Module
 - Adafruit VS1053 MP3 Decoder Module
 - 16 GB Micro SD card
 - 2 Polulu DRV8835 Dual Motor Drivers
 - LM386N-1 Operational Amplifier IC
 - ICS-43434 I2S MEMS Digital Microphone

<img width="1257" height="778" alt="ESP_circuit_diagram" src="https://github.com/user-attachments/assets/4412d7d3-c573-4ed8-a95f-307935ad3d53" />
