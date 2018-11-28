vex::brain Brain;

vex::controller Controller1;

vex::motor mtr_front_left  = vex::motor(vex::PORT20);
vex::motor mtr_front_right = vex::motor(vex::PORT19, true);
vex::motor mtr_back_left   = vex::motor(vex::PORT17);
vex::motor mtr_back_right  = vex::motor(vex::PORT18, true);
vex::motor mtr_launcher    = vex::motor(vex::PORT10);
vex::motor mtr_arm_left    = vex::motor(vex::PORT7, vex::gearSetting::ratio36_1);
vex::motor mtr_arm_right   = vex::motor(vex::PORT8, vex::gearSetting::ratio36_1, true);

vex::motor mtr_claw_rotate = vex::motor(vex::PORT13);

vex::gyro gyroscope = vex::gyro(Brain.ThreeWirePort.A);