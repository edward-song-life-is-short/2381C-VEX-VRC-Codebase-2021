#include "main.h"
#include "globals.hpp"
#include "robot/intakes.cpp"
#include "posTracking.cpp"

void initialize()
{

    int time = pros::millis();
    int iter = 0;
    pros::lcd::initialize();

    master.clear();

    inertial.reset();

    while (inertial.is_calibrating())
    {
        printf("IMU calibrating... %d\n", iter);
        iter += 10;
        pros::delay(10);
    }

    printf("IMU is done calibrating (took %d ms)\n", iter - time);

    // pros::Task intake_task(intake_tasks_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT,"Intake Task");
    // pros::Task vector2_task(printVector_tasks_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT,"Vector Task");
}

void intake_tasks_fn(void *param)
{
    while (true)
    {
        Intakes run(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1), master.get_digital(pros::E_CONTROLLER_DIGITAL_L2));
        rightIntake.move_voltage(run.rightSpeed());
        leftIntake.move_voltage(run.leftSpeed());
    }
}

double currentx = 0, currenty = 0;

void printVector_tasks_fn(void *param)
{

    while (true)
    {

        leftFront.move(-1 * master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) + 0.8 * master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X));
        leftBack.move(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) + -0.8 * master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X));
        rightBack.move(-1 * master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) + -0.8 * master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X));
        rightFront.move(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y) + 0.8 * master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X));
    }
    pros::delay(10);
}

void opcontrol()
{
    double newAngle = 0, oldAngle = 0, angDiff = 0;
    double newEnc = 0, oldEnc = 0, enDiff = 0, newEnc1 = 0, oldEnc1 = 0, enDiff1 = 0;
    double angOrientation = 0;
    double oldX = 0, oldY = 0; // these values must be changed to reflect our coordinate system

    while (true)
    {
        // oldAngle = inertial.get_heading();
        oldEnc = (verticalEncoder.get_value() / 360) * pi * 3.25;
        oldEnc1 = (horizontalEncoder.get_value() / 360) * pi * 2.75;

        pros::delay(10);

        newAngle = inertial.get_heading();
        newEnc = (verticalEncoder.get_value() / 360) * pi * 3.25;
        newEnc1 = (horizontalEncoder.get_value() / 360) * pi * 2.75;

        angDiff = newAngle - oldAngle;
        enDiff = newEnc - oldEnc;
        enDiff1 = newEnc1 - oldEnc1;

        positionTracking findPos(oldAngle, newAngle, angDiff, enDiff, enDiff1, oldX, oldY); // the last two variables are actually useless to pass to the function as of right now
        // but since we might need to do something with them later they will stay there.

        oldX += findPos.returnX();
        oldY += findPos.returnY();
        oldAngle = findPos.returnOrientation() * 180 / pi;

        master.print(0, 0, "V: %f", oldX);
        pros::delay(50);

        master.print(1, 0, "H: %f", oldY);
        pros::delay(50);

        master.print(0, 0, "I: %f", oldAngle);
        pros::delay(50);


        
    }
}