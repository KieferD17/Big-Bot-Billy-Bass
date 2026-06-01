#include "billyMovement.h"
#include "Arduino.h"
#include "Adafruit_VS1053.h"
#include "WebServer.h"

//default constructor
Billy::Billy() {
  mouthPin1 = 17; 
  mouthPin2 = 16; 
  bodyPin1 = 14;  
  bodyPin2 = 13; 
  tailPin1 = 27;
  tailPin2 = 32;
}

//overloaded constructor
Billy::Billy(int m1, int m2, int b1, int b2, int t1, int t2) {
  mouthPin1 = m1; 
  mouthPin2 = m2; 
  bodyPin1 = b1;  
  bodyPin2 = b2; 
  tailPin1 = t1;
  tailPin2 = t2;
}

void Billy::billyDelay(int ms) {
  unsigned long startTime = millis();

  while (millis() - startTime < ms) {
    if (Billy_VS1053.playingMusic) {
      Billy_VS1053.feedBuffer();
    }

    server.handleClient();

    delay(1); 
  }
}

void Billy::openMouth() {
  digitalWrite(mouthPin1, HIGH);
  digitalWrite(mouthPin2, LOW);
}

void Billy::closeMouth() {
  digitalWrite(mouthPin1, LOW);
  digitalWrite(mouthPin2, LOW);
}

void Billy::raiseBody() {
  digitalWrite(bodyPin1, HIGH);
  digitalWrite(bodyPin2, LOW);
}

void Billy::lowerBody() {
  digitalWrite(bodyPin1, HIGH);
  digitalWrite(bodyPin2, HIGH);
}

void Billy::saveBody() {
  digitalWrite(bodyPin1, LOW);
  digitalWrite(bodyPin2, LOW);
}

void Billy::raiseTail() {
  digitalWrite(tailPin1, HIGH);
  digitalWrite(tailPin2, LOW);
}

void Billy::lowerTail() {
  digitalWrite(tailPin1, LOW);
  digitalWrite(tailPin2, LOW);
}

void Billy::tailWag() {
  raiseTail();
  billyDelay(250);
  lowerTail();
  billyDelay(250);
}

void Billy::takeMeToTheRiver() {
  int count = 0;

  while (count < 26) {
    tailWag();
    count++;
  }
  count = 0;

  raiseBody();

  openMouth();
  billyDelay(300);
  closeMouth();
  billyDelay(300);
  openMouth();
  billyDelay(150);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(200);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(200);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(450);
  closeMouth();
  billyDelay(150);
  openMouth();
  billyDelay(450);
  closeMouth();

  lowerBody(); 

  while (count < 3) { 
    tailWag();
    count++;
  }
  count = 0;

  raiseBody(); 

  openMouth();
  billyDelay(300);
  closeMouth();
  billyDelay(250);
  openMouth();
  billyDelay(200);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(250);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(150);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(450);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(450);
  closeMouth();

  lowerBody(); 

  while (count < 3) {
    tailWag();
    count++;
  }
  count = 0;

  raiseBody();  

  openMouth();
  billyDelay(250);
  closeMouth();
  billyDelay(250);
  openMouth();
  billyDelay(100);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(250);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(150);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(400);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(500);
  closeMouth();

  lowerBody();

  while (count < 4) {
    tailWag();
    count++;
  }
  count = 0;

  raiseBody();

  openMouth();
  billyDelay(250); 
  closeMouth();
  billyDelay(250);
  openMouth();
  billyDelay(250);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(150);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(150);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(500);
  closeMouth();
  billyDelay(50);
  openMouth(); 
  billyDelay(450);
  closeMouth();

  lowerBody(); 

  while (count < 8) { //wags 8 times
    tailWag();
    count++;
  }
  count = 0; //reset count

  delay(1000); //ensures body is all the way lowered
  saveBody(); //disables the breaks
}

void Billy::dontWorryBeHappy() {
  int count = 0;

  while (count < 12) { //wags tail 13 times
    tailWag();
    count++;
  }
  count = 0; //reset count for next loop

  raiseBody();

  openMouth();
  billyDelay(400);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(200);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(200);

  closeMouth();
  billyDelay(2450); 

  openMouth();
  billyDelay(300);
  closeMouth();
  billyDelay(150);
  openMouth();
  billyDelay(200);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(250);
  closeMouth();

  lowerBody();

  while (count < 15) { 
    tailWag();
    count++;
  }
  count = 0;

  raiseBody(); 

  openMouth();
  billyDelay(400);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(250);
  closeMouth();
  billyDelay(50);
  openMouth();
  billyDelay(300);
  closeMouth();
  billyDelay(2450); 

  openMouth();
  billyDelay(250);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(150);
  closeMouth();
  billyDelay(150);
  openMouth();
  billyDelay(250);
  closeMouth();

  lowerBody();

  while(count < 5) { 
    tailWag();
    count++;
  }
  count = 0;

  raiseBody();

  openMouth();
  billyDelay(300);
  closeMouth();
  billyDelay(150);
  openMouth();
  billyDelay(350);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(250);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(200);
  closeMouth();
  billyDelay(100);
  openMouth();
  billyDelay(300);
  closeMouth();

  lowerBody();

  delay(1000); //ensures body is all the way lowered
  saveBody(); //disables the breaks
}

void Billy::rickRoll() {
  int count = 0;

  while (count < 32) {
    tailWag();
    count++;
  }
  count = 0;

  delay(1000); //ensures body is all the way lowered
  saveBody(); //disables the breaks
}