#include "main.h"
#include "posTracking.cpp"
#include "globals.hpp"
#include <array>
#include "pid.hpp"

double currentx = 0, currenty = 0;

//initation of array pointer values
PID* anglerPIDController = new PID(
    &anglerPIDParams[0],
    &anglerPIDParams[1],
    &anglerPIDParams[2]);

PID* drivebasePIDController = new PID(
    &drivebasePIDParams[0],
    &drivebasePIDParams[1],
    &drivebasePIDParams[2]);

PID* turningPIDController = new PID(
    &turningPID[0],
    &turningPID[1],
    &turningPID[2]);

//driving PID taking in 5 arguments, distance you need to drive, target destination for inertial, direction, target x and y destination
void motionPID(double distance, double constInertial, DIRECTION direction) {
    int cofLB = 1, cofLF = 1, cofRB = 1, cofRF = 1;

    switch (direction) {
        case FORWARD:
            cofRB = -1;
            cofLF = -1;
            break;

        case REVERSE:
            cofRF = -1;
            cofLB = -1;
            break;
    }

    //converts inches to ticks
    //double encDistance = distance *(1.0/vertToInch);
    double encDistance = distance;

    double diff = encDistance;
    double origin = 0;
    double inverseDis, inverseEnc;

    while(true) {
        
        if(verticalEncoder.get_value() > distance) {
            inverseDis = -encDistance;
            inverseEnc = -verticalEncoder.get_value();
        }
        else {
            inverseDis = encDistance;
            inverseEnc = verticalEncoder.get_value();
        }
        
        //voltage output for pid
        
        double voltage = drivebasePIDController->update(inverseDis, inverseEnc);

        //sets voltage to 0 when distance is reached (exit condition)

        if(abs(verticalEncoder.get_value()) > abs(encDistance) - 20 && abs(verticalEncoder.get_value()) < abs(encDistance) + 20) {
            pros::lcd::set_text(5, "pid stoppped");
            voltage = 0;
            return;
        }

        pros::lcd::set_text(6, "voltage: " + std::to_string(voltage));
        master.print(2, 0, "VC: %f", abs(encDistance) - abs(verticalEncoder.get_value()));
       
        //gets motors to move certain speed
        leftFront.move_voltage(voltage * cofLF);
        leftBack.move_voltage(voltage * cofLB);
        rightFront.move_voltage(voltage * cofRF);
        rightBack.move_voltage(voltage * cofRB);
        
        pros::delay(10);

    }
}

//pid function made for turning: it takes in 2 arguments, the angle you want to turn to and the direction in which you are turning
void turnPID(double constInertial, DIRECTION direction) {
    //coefficients that direct the direction of rotation for the motors
    int cofLB = 1, cofLF = 1, cofRB = 1, cofRF = 1;
    // verticalEncoder.reset(); // ?
    double original = inertial.get_rotation();
    //switch case changes motor rotation depending on direction you want to turn
    switch(direction) {
        case RIGHT:
            cofRB = -1;
            cofLB = -1;
            break;

        case LEFT:
            cofRF = -1;
            cofLF = -1;
            break;
    }

    //while loop initiating the PID function
    while(true) {
        //calls the PID class passing two arguments, 
        double voltage, negative;
        // debug: print out the voltage
        printf("Voltage: %lf\n", voltage);
        printf("Inertial: %lf\n", inertial.get_rotation());

        //gets motors to move
        leftFront.move_voltage(voltage * cofLF);
        leftBack.move_voltage(voltage * cofLB);
        rightFront.move_voltage(voltage * cofRF);
        rightBack.move_voltage(voltage * cofRB);
        
        //checks the inertial sensor reading at an error of +- 2 degrees and exits function once destination reached

        if (abs(inertial.get_rotation()) >= abs(constInertial) - 0.5 && abs(inertial.get_rotation()) <= abs(constInertial) + 0.5) {
            printf("TURNING STOPPED\n");
            pros::lcd::print(4, "TURNING STOPPED to %lf", constInertial); // prints Y coord on brain
            voltage = 0;
            return;
        }
        else {
            
            if(constInertial < original) {
                negative = -inertial.get_rotation();
            }
            else {
                negative = inertial.get_rotation();
            }
            
            voltage = turningPIDController->update(abs(constInertial), negative);   
        }

        pros::delay(10);
    }

}                                  

//ayan's odometry system in opcontrol
void vector_tasks_fn(void *param) {
    double lastpos = 0, currentpos = 0; // variables to hold vertical tracking wheel encoder position, in intervals of 10 ms
    double lastposH = 0, currentposH = 0; // horizontal counterparts of above variables
    double newAngle = 0, lastAngle = 0; // angles taken by inertial sensor (IMU), in intervals of 10 ms
    double globalX = 0, globalY = 0; // global X and Y coordinates of the robot
    
    while (true) // control loop 
    {
        currentpos = -verticalEncoder.get_value() * vertToInch; // reverses vertical encoder, finds position and converts to inches
        currentposH = -(horizontalEncoder.get_value() * horiToInch); // same function as above, horizontal counterpart
        newAngle = (inertial.get_rotation()) * imuToRad; // gets inertial angle, converts to radians. Use of rotation as opposed to heading is to account for vector math with negative angles.
        positionTracking robotPos(newAngle, lastAngle, currentposH, lastposH, currentpos, lastpos); // creates a Position tracking class, where math is done. 
        
        if (!isnan(robotPos.returnX()) || !isnan(robotPos.returnY())) // to avoid turning global coordinates into null values when inertial calibrates, conditional statement
        {
            globalX += robotPos.returnX(); // adds the horizontal vector passed by the position tracking class to the global X coordinate
            globalY += robotPos.returnY(); // same function as above, vertical counterpart
        }
        
        lastposH = currentposH; // sets the last values for the function as the current values, to continue the loop
        lastpos = currentpos;  // ""
        lastAngle = newAngle; // ""
        
        pros::lcd::print(0, "X: %f", globalX); // prints X coord on brain
        pros::lcd::print(1, "Y: %f", globalY); // prints Y coord on brain 
        master.print(1,0, "I: %f", lastAngle); // prints angle on controller
        
        pros::delay(50);

        master.print(0, 0, "Iner: %f", inertial.get_rotation());        
        pros::delay(10);      // runs loop every 10ms
    }
}

void autonomous() {
    // calibrate imu
    inertial.reset();
    while (inertial.is_calibrating())
    {
        pros::delay(10);
    }

    pros::Task position_task(vector_tasks_fn, (void*)"PROS", TASK_PRIORITY_MAX, TASK_STACK_DEPTH_DEFAULT,"Print X and Y Task");
    // 846.21458975586392379449251804839‬ ticks = 24 inches
    motionPID(846, 20, FORWARD);
    //pros::delay(500);
    //turnPID(270, LEFT);
    // pros::delay(500);
    // turnPID(90, RIGHT);
    
    // pros::delay(500);
    // turnPID(-45, LEFT);

    pros::delay(500);
    motionPID(0, 20, REVERSE);
    
    // motionPID(846,20, REVERSE);
    pros::delay(10);
}

//combines turning and driving all into one function
void combineAlgorithm(double targetX, double targetY, DIRECTION direction) {
    //calls the class getting it to calculate desired angle and driving distance
    motion moveTo(currentx, currenty, targetX, targetY);

    //angle returned by class
    double angle = moveTo.returnAngle();
    
    //distance returned by class
    double distance = moveTo.returnDistance();

    //acounts for margin of error, there is no point turning such a small degree
    if(!(angle < 0.5)) {
        //calls the turning function
        turnPID(inertial.get_heading(), direction); 
    }
    
    //calls the driving PID to drive a certain distance
    distance = distance *(1.0/vertToInch);
    motionPID(distance, inertial.get_heading(), FORWARD);
}
