#ifndef BILLY_MOVEMENT
#define BILLY_MOVEMENT

#include "Arduino.h"
#include "Adafruit_VS1053.h"
#include "WebServer.h"

extern Adafruit_VS1053_FilePlayer Billy_VS1053;
extern WebServer server;

class Billy {
    public: 
        Billy();
        Billy(int m1, int m2, int b1, int b2, int t1, int t2);

        void closeMouth();
        void openMouth();
        void raiseBody();
        void lowerBody();
        void saveBody();
        void raiseTail();
        void lowerTail();

        void tailWag();
        void takeMeToTheRiver();
        void dontWorryBeHappy();
        void rickRoll();
    private:
        int mouthPin1;
        int mouthPin2;
        int bodyPin1;
        int bodyPin2;
        int tailPin1;
        int tailPin2;

        void billyDelay(int ms);
};

#endif