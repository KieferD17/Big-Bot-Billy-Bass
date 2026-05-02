#include "billyMovement.h"
#include "Particle.h"

//default constructor
Billy::Billy() {
  mouthPin1 = D1; 
  mouthPin2 = D0; 
  bodyPin1 = D3;  
  bodyPin2 = D2; 
  tailPin1 = D4;
  tailPin2 = D5;
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
  delay(250);
  lowerTail();
  delay(250);
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
  delay(300);
  closeMouth();
  delay(300);
  openMouth();
  delay(150);
  closeMouth();
  delay(100);
  openMouth();
  delay(200);
  closeMouth();
  delay(50);
  openMouth();
  delay(200);
  closeMouth();
  delay(50);
  openMouth();
  delay(450);
  closeMouth();
  delay(150);
  openMouth();
  delay(450);
  closeMouth();

  lowerBody(); 

  while (count < 3) { 
    tailWag();
    count++;
  }
  count = 0;

  raiseBody(); 

  openMouth();
  delay(300);
  closeMouth();
  delay(250);
  openMouth();
  delay(200);
  closeMouth();
  delay(50);
  openMouth();
  delay(250);
  closeMouth();
  delay(50);
  openMouth();
  delay(150);
  closeMouth();
  delay(50);
  openMouth();
  delay(450);
  closeMouth();
  delay(50);
  openMouth();
  delay(450);
  closeMouth();

  lowerBody(); 

  while (count < 3) {
    tailWag();
    count++;
  }
  count = 0;

  raiseBody();  

  openMouth();
  delay(250);
  closeMouth();
  delay(250);
  openMouth();
  delay(100);
  closeMouth();
  delay(50);
  openMouth();
  delay(250);
  closeMouth();
  delay(50);
  openMouth();
  delay(150);
  closeMouth();
  delay(50);
  openMouth();
  delay(400);
  closeMouth();
  delay(50);
  openMouth();
  delay(500);
  closeMouth();

  lowerBody();

  while (count < 4) {
    tailWag();
    count++;
  }
  count = 0;

  raiseBody();

  openMouth();
  delay(250); 
  closeMouth();
  delay(250);
  openMouth();
  delay(250);
  closeMouth();
  delay(50);
  openMouth();
  delay(150);
  closeMouth();
  delay(100);
  openMouth();
  delay(150);
  closeMouth();
  delay(50);
  openMouth();
  delay(500);
  closeMouth();
  delay(50);
  openMouth(); 
  delay(450);
  closeMouth();

  lowerBody(); 

  while (count < 8) { //wags 8 times
    tailWag();
    count++;
  }
  count = 0; //reset count
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
  delay(400);
  closeMouth();
  delay(100);
  openMouth();
  delay(200);
  closeMouth();
  delay(50);
  openMouth();
  delay(200);

  closeMouth();
  delay(2450); 

  openMouth();
  delay(300);
  closeMouth();
  delay(150);
  openMouth();
  delay(200);
  closeMouth();
  delay(100);
  openMouth();
  delay(250);
  closeMouth();

  lowerBody();

  while (count < 15) { 
    tailWag();
    count++;
  }
  count = 0;

  raiseBody(); 

  openMouth();
  delay(400);
  closeMouth();
  delay(100);
  openMouth();
  delay(250);
  closeMouth();
  delay(50);
  openMouth();
  delay(300);
  closeMouth();
  delay(2450); 

  openMouth();
  delay(250);
  closeMouth();
  delay(100);
  openMouth();
  delay(150);
  closeMouth();
  delay(150);
  openMouth();
  delay(250);
  closeMouth();

  lowerBody();

  while(count < 5) { 
    tailWag();
    count++;
  }
  count = 0;

  raiseBody();

  openMouth();
  delay(300);
  closeMouth();
  delay(150);
  openMouth();
  delay(350);
  closeMouth();
  delay(100);
  openMouth();
  delay(250);
  closeMouth();
  delay(100);
  openMouth();
  delay(200);
  closeMouth();
  delay(100);
  openMouth();
  delay(300);
  closeMouth();

  lowerBody();
}

void Billy::rickRoll() {
  int count = 0;

  while (count < 32) {
    tailWag();
    count++;
  }
  count = 0;
}