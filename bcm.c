#include "bcm.h"


#define SLIP_LED_PIN 1

//Constructor
BCM::BCM() {
    imuAcc = 0.0f;
    wheelVel = 0.0f;
    prev_imuAcc = 0.0f;
    prev_wheelVel = 0.0f;
    trueVel = 0.0f;
    lastUpdateTime = millis();

    //Initialize slip indicator pin
    pinMode(SLIP_LED_PIN, OUTPUT);

    //Setup brake release servo
    Servo myservo;  
    myservo.attach(15);
    myservo.write(0); 

    
}

// Process
void BCM::process(){

    // Update IMU velocity estimate by integrating acceleration
    wheelAcc = (wheelVel - prev_wheelVel) / dt;

    // Update true velocity
    if(isSlipping()){

        //Integrate IMU acceleration to estimate true velocity during slip
        trueVel += imuAcc * dt;
        //trueVel  = max(trueVel, 0.0f);   // velocity cannot go negative

        //Turn on slip indicator
        digitalWrite(SLIP_LED_PIN, HIGH);

        //Engage the brake release servo to mitigate slip
        myservo.write(180); // Adjust this value based on your servo's range and

    } else {

        // Normal rolling — trust the wheel speed, re-anchor IMU velocity
       m yservo.write(180); 
       
        //Turn off slip indicator
        digitalWrite(SLIP_LED_PIN, LOW);
    }
    
}  

//Update last update time
float BCM::diff(){
    unsigned long currentTime = millis();
    dt = (currentTime - lastUpdateTime) / 1000.0f; // convert ms to seconds
    return dt;
}

//Return plausible max deceleration based on friction limits
float BCM::getDecelLimit(){
    return 2.f; // m/s^2
}

bool BCM::isSlipping(){

    //Check if wheel acceleration exceeds that of plausible friction limits
    if(wheelAcc < -getDecelLimit()){
        return true;
    }

    return false;
}


//Transmit status msg to debugger
void BCM::txStatus(){

    //Status structure to hold data for transmission
    struct TxStruct {

    float imuAcc;
    float wheelVel;
    float wheelAcc;
 
    } txData;

    // Populate and send TxData over serial
    txData.imuAcc = bcm.imuAcc;
    txData.wheelVel = bcm.wheelVel;
    txData.wheelAcc = bcm.wheelAcc;

    Serial.write((uint8_t*)&txData, sizeof(txData));
}
