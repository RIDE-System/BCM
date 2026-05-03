#ifndef BCM_H
#define BCM_H
#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include <Servo.h>

#define BUF_SIZE 20

class BCM {
    public:
        
        //Constructor
        BCM();

        // =========== Sensor Inputs ===========
        

        // Get true velocity estimate
        float getTrueVel() { return trueVel; }


        // =====================================

        //Update sensor states
        void update();

        // Process
        void process();

        //Transmit status msg to debugger
        void txStatus();

        // Slippage
        bool isSlipping();

        //Servo command functions
        void enableIntervention();
        void disableIntervention();
        bool isLimiting();


    private:

        // IMU Acceleration
        float imuAcc;

        //Servo 
        Servo serv;  
        bool enableServo;

       

        //Servo incremental tracker
        float servoPos;
        float servoStep = 10.0f; // degrees per increment

        // Wheel Speed
        float wheelVel, wheelAcc;

        //Last update
        unsigned long lastUpdateTime;

        //Get time since last update
        float diff();

        // Previous IMU acc estimate and wheel velocity 
        float prev_imuAcc, prev_wheelVel;

        // True speed 
        float trueVel;      

        //Return plausible max deceleration based on friction limits
        float getDecelLimit();
        


        
    
    };

#endif