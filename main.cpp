#include "robot-config.h"

vex::competition Competition;

#define STOPPED 0
#define MOVING  1
#define SLOWING 2
#define AUTO_MOVING 3
#define RAISING 0
#define LOWERING 1
#define WINDING 0
#define FIRING 1
#define READY 2

int arm_state = STOPPED;
int arm_direction = STOPPED;
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

void arm_raise_pressed()
{
    arm_state = MOVING;
    mtr_arm_left.spin(vex::directionType::fwd, 70, vex::velocityUnits::pct);
    mtr_arm_right.spin(vex::directionType::fwd, 70, vex::velocityUnits::pct);
}
void arm_raise_released()
{
    arm_state = SLOWING;
}
void arm_lower_pressed()
{
    arm_state = MOVING;
    mtr_arm_left.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
    mtr_arm_right.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);
}
void arm_lower_released()
{
    arm_state = SLOWING;
}

void pre_auton()
{
    //Set Button Callbacks
    Controller1.ButtonR1.pressed(arm_raise_pressed);
    Controller1.ButtonR1.released(arm_raise_released);
    Controller1.ButtonR2.pressed(arm_lower_pressed);
    Controller1.ButtonR2.released(arm_lower_released);
    
    mtr_launcher.setStopping(vex::brakeType::hold);
    mtr_claw_rotate.setStopping(vex::brakeType::hold);
}

void arm_set_height(int height)
{
    switch (height)
    {
        case 0:
            mtr_arm_left.startRotateTo(0, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            mtr_arm_right.startRotateTo(0, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            break;
        case 1:
            mtr_arm_left.startRotateTo(100, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            mtr_arm_right.startRotateTo(100, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            break;
        case 2:
            mtr_arm_left.startRotateTo(500, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            mtr_arm_right.startRotateTo(500, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            break;
        case 3:
            mtr_arm_left.startRotateTo(900, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            mtr_arm_right.startRotateTo(900, vex::rotationUnits::deg, 100, vex::velocityUnits::pct);
            break;
    }
    arm_state = AUTO_MOVING;
    if (mtr_arm_left.direction() == vex::directionType::fwd)
    {
        arm_direction = RAISING;
    }
    else
    {
        arm_direction = LOWERING;
    }
}

void autonomous()
{
    
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
        if (arm_state == SLOWING)
        {
            if (fabs(mtr_arm_left.velocity(vex::velocityUnits::pct)) > 5)
            {
                if (arm_direction == RAISING)
                {
                    mtr_arm_left.spin(vex::directionType::fwd, fabs(mtr_arm_left.velocity(vex::velocityUnits::pct)) - 1, vex::velocityUnits::pct);
                    mtr_arm_right.spin(vex::directionType::fwd, fabs(mtr_arm_right.velocity(vex::velocityUnits::pct)) - 1, vex::velocityUnits::pct);
                }
                if (arm_direction == LOWERING)
                {
                    mtr_arm_left.spin(vex::directionType::rev, fabs(mtr_arm_left.velocity(vex::velocityUnits::pct)) - 1, vex::velocityUnits::pct);
                    mtr_arm_right.spin(vex::directionType::rev, fabs(mtr_arm_right.velocity(vex::velocityUnits::pct)) - 1, vex::velocityUnits::pct);
                }
            }
            else
            {
                arm_state = STOPPED;
                mtr_arm_left.stop(vex::brakeType::hold);
                mtr_arm_right.stop(vex::brakeType::hold);
            }
        }
        if (arm_state == AUTO_MOVING)
        {
            if (mtr_arm_left.isDone())
            {
                arm_state = SLOWING;
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