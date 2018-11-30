#include "robot-config.h"

vex::competition Competition;

#define WINDING 0
#define FIRING 1
#define READY 2

int sgn(int a)
{
    if (a > 0)
    {
        return 1;
    }
    if (a < 0)
    {
        return -1;
    }
    return 0;
}

void turn_time(int angle)
{
    mtr_front_right.spin(vex::directionType::rev, 100 * sgn(angle), vex::velocityUnits::pct);
    mtr_front_left.spin(vex::directionType::fwd, 100 * sgn(angle), vex::velocityUnits::pct);
    mtr_back_right.spin(vex::directionType::rev, 100 * sgn(angle), vex::velocityUnits::pct);
    mtr_back_left.spin(vex::directionType::fwd, 100 * sgn(angle), vex::velocityUnits::pct);
    vex::task::sleep(abs(angle) * 6); //vex::task::sleep(abs(angle) * 6); turns 90 deg
    mtr_front_right.stop(vex::brakeType::brake);
    mtr_front_left.stop(vex::brakeType::brake);
    mtr_back_right.stop(vex::brakeType::brake);
    mtr_back_left.stop(vex::brakeType::brake);
}

void drive_time(int distance)
{
    mtr_front_right.spin(vex::directionType::fwd, 100 * sgn(distance), vex::velocityUnits::pct);
    mtr_front_left.spin(vex::directionType::fwd, 100 * sgn(distance), vex::velocityUnits::pct);
    mtr_back_right.spin(vex::directionType::fwd, 100 * sgn(distance), vex::velocityUnits::pct);
    mtr_back_left.spin(vex::directionType::fwd, 100 * sgn(distance), vex::velocityUnits::pct);
    vex::task::sleep(abs(distance) * 16.8); //vex::task::sleep(abs(angle) * 16.8); drives 30cm
    mtr_front_right.stop(vex::brakeType::brake);
    mtr_front_left.stop(vex::brakeType::brake);
    mtr_back_right.stop(vex::brakeType::brake);
    mtr_back_left.stop(vex::brakeType::brake);
}

void flip_claw(int speed)
{
    static bool flipped = false;
    int current_angle;
    if (flipped)
    {
        current_angle = 180;
    }
    else
    {
        current_angle = 0;
    }
    
    if (flipped && (fabs(mtr_claw_rotate.rotation(vex::rotationUnits::deg) - current_angle) < 2.0f))
    {
        mtr_claw_rotate.startRotateTo(0, vex::rotationUnits::deg, speed, vex::velocityUnits::pct);
        flipped = false;
    }
    else
    {
        mtr_claw_rotate.startRotateTo(180, vex::rotationUnits::deg, speed, vex::velocityUnits::pct);
        flipped = true;
    }
    vex::task::sleep(250);
}

void pre_auton()
{   
    mtr_launcher.setStopping(vex::brakeType::hold);
    mtr_claw_rotate.setStopping(vex::brakeType::hold);
    
    mtr_arm_left.resetRotation();
    mtr_arm_right.resetRotation();
}

void arm_set_height(int height)
{
}

void autonomous()
{
    drive_time(20);
    vex::task::sleep(2000);
    turn_time(-90);
    vex::task::sleep(2000);
    drive_time(50);
    vex::task::sleep(2000);
    turn_time(-90);
    vex::task::sleep(2000);
    drive_time(-160);
    //drive_time(70);
    //turn_time(45);
}

void usercontrol()
{
    while (true)
    {
        //Driving
        if (abs(Controller1.Axis1.value()) > 30.0f)
        {
            mtr_front_left.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()) / 2, vex::velocityUnits::pct); //(Axis3+Axis4)/2
            mtr_back_left.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()) / 2, vex::velocityUnits::pct); //(Axis3+Axis4)/2
            mtr_front_right.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()) / 2, vex::velocityUnits::pct);//(Axis3-Axis4)/2
            mtr_back_right.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()) / 2, vex::velocityUnits::pct);//(Axis3-Axis4)/2
        }
        else
        {
            mtr_front_left.spin(vex::directionType::fwd, Controller1.Axis3.value(), vex::velocityUnits::pct);
            mtr_back_left.spin(vex::directionType::fwd, Controller1.Axis3.value(), vex::velocityUnits::pct);
            mtr_front_right.spin(vex::directionType::fwd, Controller1.Axis3.value(), vex::velocityUnits::pct);
            mtr_back_right.spin(vex::directionType::fwd, Controller1.Axis3.value(), vex::velocityUnits::pct);
        }

        //Flipping
        if (Controller1.ButtonA.pressing())
        {
            flip_claw(100);
        }

        //Lifting
        if (Controller1.ButtonR1.pressing())
        {
            mtr_arm_left.spin(vex::directionType::fwd, 70, vex::velocityUnits::pct);
            mtr_arm_right.spin(vex::directionType::fwd, 70, vex::velocityUnits::pct);
        }
        else
        {
            if (Controller1.ButtonR2.pressing())
            {
                mtr_arm_left.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
                mtr_arm_right.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
            }
            else
            {
                mtr_arm_left.stop(vex::brakeType::hold);
                mtr_arm_right.stop(vex::brakeType::hold);
            }
        }
        
        
        //Launcher
        static int launcher_status = WINDING;
        static int reached_resistance = 0;
        switch (launcher_status)
        {
            case WINDING:
                Brain.Screen.clearScreen();
                if (reached_resistance == 0) Brain.Screen.printAt(64, 64,"%f - 0", mtr_launcher.power(vex::powerUnits::watt));
                if (reached_resistance == 1) Brain.Screen.printAt(64, 64,"%f - 1", mtr_launcher.power(vex::powerUnits::watt));
                if (reached_resistance == 2) Brain.Screen.printAt(64, 64,"%f - 2", mtr_launcher.power(vex::powerUnits::watt));
                vex::task::sleep(100);
                if (reached_resistance == 1)
                {
                    reached_resistance = 2;
                    mtr_launcher.resetRotation();
                    //mtr_launcher.startRotateFor(2800, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
                    mtr_launcher.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
                    
                }
                else
                {
                    if (reached_resistance == 0)
                    {
                        mtr_launcher.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
                        //Brain.Screen.clearSc reen();
                        //Brain.Screen.printAt(64, 64,"%f - %f", vex::timer::system(), mtr_launcher.power(vex::powerUnits::watt));
                        //vex::task::sleep(100);
                        if (mtr_launcher.power(vex::powerUnits::watt) > 3.5f)
                        {
                            Controller1.Screen.print("RES");
                            reached_resistance = 1;
                        }
                    }
                    if (reached_resistance == 2)
                    {
                        if (mtr_launcher.rotation(vex::rotationUnits::deg) > 2350)
                        {
                            mtr_launcher.stop(vex::brakeType::hold);
                            launcher_status = READY;
                            Controller1.Screen.print("READY");
                        }
                    }
                }
                break;
            case FIRING:
                //mtr_launcher.startRotateFor(200, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
                mtr_launcher.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
                if (mtr_launcher.rotation(vex::rotationUnits::deg) > 300)
                {
                    mtr_launcher.stop(vex::brakeType::hold);
                    reached_resistance = 0;
                    launcher_status = WINDING;
                    Controller1.Screen.print("WINDING");
                }
                break;
            case READY:
                if (Controller1.ButtonL1.pressing())
                {
                    launcher_status = FIRING;
                    mtr_launcher.resetRotation();
                    Controller1.Screen.print("FIRING");
                }
        }
        /*if (Controller1.ButtonL1.pressing())
        {
            mtr_launcher.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);
        }
        if (Controller1.ButtonL2.pressing())
        {
            mtr_launcher.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);
        }
        if (!Controller1.ButtonL1.pressing() && !Controller1.ButtonL2.pressing())
        {
            mtr_launcher.stop(vex::brakeType::hold);
        }*/

        //Preset Heights
        if (Controller1.ButtonUp.pressing())
        {
            arm_set_height(3);
        }
        if (Controller1.ButtonLeft.pressing())
        {
            arm_set_height(2);
        }
        if (Controller1.ButtonDown.pressing())
        {
            arm_set_height(1);
        }
        
        if (Controller1.ButtonX.pressing())
        {
            arm_set_height(3);
            flip_claw(50);
        }
        if (Controller1.ButtonY.pressing())
        {
            arm_set_height(2);
            flip_claw(50);
        }
        if (Controller1.ButtonB.pressing())
        {
            arm_set_height(1);
            flip_claw(50);
        }
    }
}

int main()
{
    pre_auton();

    Competition.autonomous(autonomous);
    Competition.drivercontrol(usercontrol);
    
    while(true)
    {
        vex::task::sleep(100);
    }
}