vex::brain Brain;

vex::controller Controller1;

vex::motor mtr_front_left  = vex::motor(vex::PORT9);
vex::motor mtr_front_right = vex::motor(vex::PORT7, true);
vex::motor mtr_back_left   = vex::motor(vex::PORT3);
vex::motor mtr_back_right  = vex::motor(vex::PORT1, true);
vex::motor mtr_launcher    = vex::motor(vex::PORT4);
vex::motor mtr_arm_left    = vex::motor(vex::PORT2, vex::gearSetting::ratio36_1);
vex::motor mtr_arm_right   = vex::motor(vex::PORT5, vex::gearSetting::ratio36_1, true);

vex::motor mtr_claw_rotate = vex::motor(vex::PORT8);

vex::gyro gyroscope = vex::gyro(Brain.ThreeWirePort.A);