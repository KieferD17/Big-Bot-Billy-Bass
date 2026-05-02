# Big-Bot-Billy-Bass
Solo Project by: Kiefer Dick

<img width="4032" height="3024" alt="showcase" src="https://github.com/user-attachments/assets/6a72cc62-f7fb-4d47-99f8-487fe3a229a4" />

## Description
This project involves taking apart the classic Big Mouth Billy Bass toy from the 1990's and integrating the Particle Photon and a custom AI chatbot.
The user is able to communicate with the Billy AI through a web interface that is connected to the Particle cloud. When the user inputs a prompt into
the webpage, Billy comes alive and talks back with his own unique response to every question asked! 

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
 - Also has the original button and motion sensor still functional
     - Motion sensor activates the original songs and animations
     - Button acts as another way of triggering the songs and animations for the toy
 - Custom audio amplifier with volume control, so you can lower the volume and make Billy more or less annoying as you please

## Project Files
 - final_product.cpp - cummulative code that is flashed to the Photon
 - billyMovement.h - custom header file that declares Billy class as well as movement functions
 - billyMovement.cpp - defines all Billy class functions
 - index.html - contains source code for the HTML project website
 - style.css - CSS code to customize the Billy interface
 - server.py - Custom Python Server that runs locally to hold the OpenAI model used for Billy

## Hardware Components
 - Particle Photon 2
 - Adafruit VS1053 MP3 Decoder Module
 - 16 GB Micro SD card
 - 2 Polulu DRV8835 Dual Motor Drivers
 - LM386N-1 Operational Amplifier IC

<img width="3000" height="3631" alt="circuit_diagram" src="https://github.com/user-attachments/assets/7aefbd39-dd3a-4a3b-aeff-20426e955252" /> 

<img width="1640" height="1901" alt="image" src="https://github.com/user-attachments/assets/3cbff0b3-7d62-4235-879a-4af6f7372506" />

