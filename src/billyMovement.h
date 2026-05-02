#ifndef BILLY_MOVEMENT
#define BILLY_MOVEMENT

#include "Particle.h"

class Billy {
    public: 
        Billy();
        Billy(int m1, int m2, int b1, int b2, int t1, int t2);

        void closeMouth();
        void openMouth();
        void raiseBody();
        void lowerBody();
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
};

#endif