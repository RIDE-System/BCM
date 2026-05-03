#include "BCM.h"


#define SLIP_LED_PIN 1



//Constructor
BCM::BCM() {
    imuAcc = 0.0f;
    wheelVel = 0.0f;
    prev_imuAcc = 0.0f;
    prev_wheelVel = 0.0f;
    servoStep = 10.f;
    servoPos = 0.0f;
    trueVel = 0.0f;
    lastUpdateTime = millis();
    enableServo = true;

    //Initialize slip indicator pin
    pinMode(SLIP_LED_PIN, OUTPUT);

    //Setup brake release servo
    serv.attach(15);
    serv.write(0); 

    
}

void BCM::enableIntervention(){
    //Enable the servo
    enableServo = true;

}

void BCM::disableIntervention(){
    //Disable the servo
    enableServo = false;

    //Clear any pending interventions
    if(servoPos > 0){
        servoPos = 0;
        serv.write(servoPos);
    }   
}

bool BCM::isLimiting(){
    
    //Return the status of the servo state
    if(enableServo && servoPos > 0){
        return true;
    }

    return false;
}

// Process
void BCM::process(){

    // Update IMU velocity estimate by integrating acceleration
    wheelAcc = (wheelVel - prev_wheelVel) / diff(); // simple numerical differentiation

    // Update true velocity
    if(isSlipping()){

        //Integrate IMU acceleration to estimate true velocity during slip
        trueVel += imuAcc * diff();
        //trueVel  = max(trueVel, 0.0f);   // velocity cannot go negative

        //Turn on slip indicator
        digitalWrite(SLIP_LED_PIN, HIGH);

        //Engage the brake release servo to mitigate slip
        servoPos += servoStep;
        if(servoPos > 180) servoPos = 180;

        serv.write(servoPos); // Adjust this value based on your servo's range and

    } else {

        // Normal rolling — trust the wheel speed, re-anchor IMU velocity
        servoPos -= servoStep;
        if(servoPos < 0) servoPos = 0;

        serv.write(servoPos); 
       
        //Turn off slip indicator
        digitalWrite(SLIP_LED_PIN, LOW);
    }
    
}  

//Update last update time
float BCM::diff(){
    unsigned long currentTime = millis();
    float dt = (currentTime - lastUpdateTime) / 1000.0f; // convert ms to seconds
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

void BCM::update(){

    // Simulate sensor updates (replace with actual sensor reads in real implementation)
    imuAcc = random(-5, 5) / 10.0f; // Simulated IMU acceleration
    wheelVel = random(0, 100) / 10.0f; // Simulated wheel velocity

    //Update time since last update
    lastUpdateTime = millis();

    //Update last velocity
    prev_wheelVel = wheelVel;
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
    txData.imuAcc = imuAcc;
    txData.wheelVel = wheelVel;
    txData.wheelAcc = wheelAcc;

    Serial.write((uint8_t*)&txData, sizeof(txData));
}
