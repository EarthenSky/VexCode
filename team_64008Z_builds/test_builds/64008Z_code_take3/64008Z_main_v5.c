#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, in2,    miniGoalPot,    sensorPotentiometer)
#pragma config(Sensor, in3,    largeGoalPot,   sensorPotentiometer)
#pragma config(Sensor, in5,    lightLeft,      sensorReflection)
#pragma config(Sensor, in6,    lightRight,     sensorReflection)
#pragma config(Sensor, in7,    lightMid,       sensorReflection)
#pragma config(Sensor, dgtl1,  encArm,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  handsDown,      sensorTouch)
#pragma config(Sensor, dgtl6,  handsUp,        sensorTouch)
#pragma config(Sensor, dgtl8,  urfOut,         sensorSONAR_inch)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           goalHand2Motor, tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           backBackLeftDrive, tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port3,           backRightDrive, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port4,           miniGoalTwo,   tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           coneL,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           rightDrive,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           leftDrive,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           goalHand2,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           goalHand,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          coneR,         tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

const int bot_auton = 0;  //same as top
const int top_auton = 1;  //same as bot
const int skills_auton = 2;

/*Auto Type*/
const int autonType = bot_auton;
//
//     ^
//     |
//     |
//
//     ^
//     |
//     |
//
// Set this for game type

/*Compo Init*/
#pragma platform(VEX);

#pragma competitionControl(Competition);
#pragma autonomousDuration(9000);
#pragma userControlDuration(9000);

#include "Vex_Competition_Includes.c" //backcode, no modify pls.

/*Global Constants*/
//#include "64008Z_auto_v2.c" //autonomous code.
const int down = 1;
const int up = 2;
const int mid = 3;
const int completed = 4;
const int stopAutoCorrect = 5;

const int dir_forwards = 1;
const int dir_backwards = -1;
const int dir_left = 1;
const int dir_right = -1;

const int pos_bot = 0;
const int pos_mid = 1;
const int pos_top = 2;

const float mod_degrees = 10;  //multiply this with degrees to get
const float mod_wheel_circumference = 4 * 3.14159265358979; //in inches.
const int task_time_limit = 2500;  //2.5s

//const float armKp = 1.5;
//const int degToMove = -100;

bool inTeleop = false;

//can be negitave too
int capMinMax (int val, int min, int max) {
	if(val < min && val > 0) {
		val = min;
	} else if(val > -min && val < 0) {
		val = -min;
	}

	if(val > max) {
		val = max;
	} else if(val < -max) {
		val = -max;
	}

	return val;
}

///initialize the gyroscope
void gyroInit() {
	//writeDebugStreamLine("Gyro Reset.");
	SensorType[gyro] = sensorNone;
	wait1Msec(1000);

	//writeDebugStreamLine("Gyro Assign..");
	SensorType[gyro] = sensorGyro;
	wait1Msec(1500);

	//writeDebugStreamLine("Gyro Setup...");
	SensorScale[gyro] = 102;  //?
	SensorValue[gyro] = 0;
	SensorFullCount[gyro] = 3600;
}

//TODO: TEST THIS.
///direction = up(const) for up move and down(const) for down move.
int armParam = 0;
task autoMoveGoalArms() {
	clearTimer(T4);

	if(armParam == down) {
			motor[goalHand] = 120;
			motor[goalHand2] = -120;
    waitUntil(SensorValue[handsDown] == 1 || time1(T4) >= task_time_limit);
	} else if(armParam == mid) {
		while(armParam != stopAutoCorrect) {
			motor[goalHand] = capMinMax(0.2 * (1100 - SensorValue[largeGoalPot]) + 5, 2, 80);
			wait1Msec(20);
		}
	} else if(armParam == up) {
			motor[goalHand] = -120;
			motor[goalHand2] = 120;
    waitUntil(SensorValue[handsUp] == 1 || time1(T4) >= task_time_limit);
	}

  armParam = completed;

	motor[goalHand] = 0;
}

//sensorPotentiometer:
const int pot_up = 3000;
const int pot_down = 1800;
const int pot_no_ground = 2400;

//not super accurate (doesn't need to be.)
int miniArmParam = 0;
task autoMoveMiniGoalArms() {
	clearTimer(T3);

	if(miniArmParam == up) {
		while(miniArmParam != stopAutoCorrect) {
			motor[goalHand2Motor] = -0.3 * (pot_up - SensorValue[miniGoalPot]);
			motor[miniGoalTwo] = 0.3 * (pot_up - SensorValue[miniGoalPot]);
			wait1Msec(20);
		}
	}
	else if(miniArmParam == down) {
		while(miniArmParam != stopAutoCorrect) {
			motor[goalHand2Motor] = -0.3 * (pot_down - SensorValue[miniGoalPot]);
			motor[miniGoalTwo] = 0.3 * (pot_down - SensorValue[miniGoalPot]);
			wait1Msec(20);
		}
	}
	else if (miniArmParam == mid) {
		while(miniArmParam != stopAutoCorrect) {
			motor[goalHand2Motor] = -0.3 * (pot_no_ground - SensorValue[miniGoalPot]);
			motor[miniGoalTwo] = 0.3 * (pot_no_ground - SensorValue[miniGoalPot]);
			wait1Msec(20);
		}
	}

  miniArmParam = completed;

	motor[goalHand2Motor] = 0;
}

//sets the speed of the left and right sides of the drive.  Empty is not moving.
void setLeftRightMoveSpeed(int leftSpeed=0, int rightSpeed=0) {
  //calculate deadzone.
  if(abs(leftSpeed) < 8) { leftSpeed = 0; }
  if(abs(rightSpeed) < 8) { rightSpeed = 0; }

	//left side.
	motor[leftDrive] = leftSpeed;
  motor[backBackLeftDrive] = leftSpeed;

	//right side.
	motor[rightDrive] = rightSpeed * 0.98;
	motor[backRightDrive] = rightSpeed * 0.98;

	//writeDebugStreamLine("L - R is %d - %d", leftSpeed, rightSpeed); //DEBUG: this
}

void resetEncoders() {
	nMotorEncoder[backBackLeftDrive] = 0;
	nMotorEncoder[backRightDrive] = 0;
}

float speedMod=0.23;

float inches;
float currentInchValue;

int negitaveMod=dir_forwards;
float gyroInitVal=0;

int maxTimeout=250;
int maxPower=102;
int minPower=17;

float drivingComplete = false;

task moveStraightGyro() {
	float disKp = speedMod; //distance kp.  //was 0.22
	//if(inches <= 24) { disKp = 0.4; }
	float gyroKp = 1.8;  //was 2.9

	float error = 0;
	float gyroError = 0;

	int sideMod = 0;

  float speed = 0;

	int ticks = 0;

	//init encoders
	resetEncoders();

	drivingComplete = false;

	bool exitLoop = false;
	while(exitLoop == false && ticks < maxTimeout) {
		float encAvgLR = (abs(nMotorEncoder[backBackLeftDrive]) + abs(nMotorEncoder[backRightDrive])) / 2;

		error = (abs(inches / mod_wheel_circumference * (380)) - encAvgLR);  //how close to completed.  // 380 is one rotation? ... /2? ...
    speed = error * disKp;

    gyroError = gyroInitVal - SensorValue[gyro];
    sideMod = gyroError * gyroKp;

    writeDebugStreamLine("speed is %d, error is %d, sideMod is %d, enc is %d, gyroerr is %d", speed, error, sideMod, nMotorEncoder[backRightDrive], gyroError); //DEBUG: this

    //keep speed between min and max power.
		speed = capMinMax(speed, minPower, maxPower);

		setLeftRightMoveSpeed((speed - sideMod * (speed/127)) * negitaveMod, (speed + sideMod * (speed/127)) * negitaveMod);  //move forwards

		writeDebugStreamLine("speed is %d, error is %d, sideMod is %d", speed, error, sideMod); //DEBUG: this

		// check for completion
		if (abs(error) > 8) 	// if robot is within 8 degree wheel rotations and timer flag is off
			clearTimer(T1);			// start a timer

		if (time1(T1) >= 30)	// if the timer is over 90ms and timer flag is true (3 ticks)
			exitLoop = true;	// set boolean to complete while loop

		currentInchValue = encAvgLR / (380/2) * mod_wheel_circumference;  //convert to inches?

		ticks++;
		wait1Msec(20);  //loop speed.  //50hz
	}

	setLeftRightMoveSpeed(); //turn off motors.

  //reset again, just in case.
	resetEncoders();

	currentInchValue = 0;
	drivingComplete = true;
	return;  //this?
}

//init a drive task and start it
void startMoveTask(float inches_in, const int negitaveMod_in=dir_forwards, float gyroInitVal_in=SensorValue[gyro], float speedMod_in=0.24, int maxTimeout_in=275, int maxPower_in=102, int minPower_in=17) {
	inches = inches_in;
	negitaveMod = negitaveMod_in;
	gyroInitVal = gyroInitVal_in;
	maxTimeout = maxTimeout_in;
	maxPower = maxPower_in;
	minPower = minPower_in;
	speedMod = speedMod_in;

	startTask(moveStraightGyro);
}

/*Gyro Turn*/
//run PD loop to turn to target deg.
void rotateTo (int turnDirection, int targetDegrees, int maxPower=115, int minPower=25, int timeOut=2500) {
	// initialize PD loop variables
	float kp = 0.10; // TODO: tune this. still smaller?
	int error = targetDegrees;
	int drivePower = 0;

	clearTimer(T2);

	// finish check variables
	bool atTarget = false;

	// initialize gyro data variables
	//int targetReading = SensorValue[gyro];
	int targetReading = 0;  //no relative rotation pls, thanks.

	// get gyroscope target reading
	if (turnDirection >= 1)
		targetReading += abs(targetDegrees);
	else if (turnDirection <= 0)
		targetReading -= abs(targetDegrees);

	// change kp if target is under 20 degree threshold
	//if (targetDegrees < 200) { kp = 0.2; }  //ok?

	// run motors until target is within 1 degree certainty
	while (!atTarget && (time1[T2] < timeOut)) {
		error = targetReading - SensorValue[gyro]; 	// calculate error
		drivePower = error * kp;	// calculate PD loop output  //speed

		writeDebugStreamLine("error -> %d, gyro -> %d", error, SensorValue[gyro]);  //DEBUG: this

		//keep speed between min and max power.
		drivePower = capMinMax(drivePower, minPower, maxPower);

		/*
		//<Enc Straightening Start
			float kp = 2.4;  //Tune?

			float errorVal = 0;

  		float leftMod = 0;
			float rightMod = 0;
  		//set offset value, if 0 both sides are moving at same speed.  use abs value for both motors, pretending both sides going forwards.
  		errorVal = (abs(nMotorEncoder[backBackLeftDrive]) - abs(nMotorEncoder[backRightDrive]));

  		//update both modifiers.
  		rightMod = -(errorVal * kp);
  		leftMod = rightMod;

  		writeDebugStreamLine("mod = error[%d] * kp[%d] = +[%d]", errorVal, kp, leftMod);  //DEBUG: this
		//Enc Straightening End/>
  	*/

		//send power to motors and add mods (scaled with drive power).
		setLeftRightMoveSpeed(-(drivePower), (drivePower));

		// check for finish
		if (abs(error) > 10) 	// if robot is within 1 degree of target and timer flag is off
			clearTimer(T1);			// start a timer

		if (time1(T1) > 100)	// if the timer is over 100ms and timer flag is true
			atTarget = true;	// set boolean to complete while loop

		wait1Msec(20);  //let motors update.
	}

	setLeftRightMoveSpeed();  //reset motors.

	// reset kp  wtf is this?  TODO: delete?
	kp = 0.22;

	resetEncoders();
  return; //I think I need this.
}

///the real autonomous command.
void runAutoSkills() {
	miniArmParam = down; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm DOWN
	wait1Msec(400);  //TODO: check this

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(600);  //TODO: check this
	drivingComplete = false; startMoveTask(97, dir_forwards, 0);  //start 102.3in FWD
		waitUntil(currentInchValue >= 48);  //wait for first GOAL picked up

		armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
		waitUntil(currentInchValue >= 130);  //wait for first GOAL picked up

		miniArmParam = up; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm UP

	waitUntil(drivingComplete == true);  //wait for driving position reached

	rotateTo(dir_left, -686);  //rotate to correct position

	drivingComplete = false; startMoveTask(20, dir_forwards, SensorValue[gyro]);  //18.9in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	rotateTo(dir_right, 150);  //rotate to initial forwards position

	drivingComplete = false; startMoveTask(28, dir_forwards, SensorValue[gyro], 10, 127);  //24in FWD
	wait1Msec(1500);  //Wait 1500 seconds until robot is STRAIGHT with the BAR.

	miniArmParam = down; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm DOWN
	wait1Msec(1000);  //Wait 1000 seconds until GOAL has FALLEN.

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	drivingComplete = false; startMoveTask(30, dir_backwards, SensorValue[gyro], 1, 125);  //18.9in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	/*30 POINTS*/
	armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
	miniArmParam = up; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm UP
	wait1Msec(100);

	rotateTo(dir_left, -900);  //rotate to correct position

	drivingComplete = false; startMoveTask(17.8, dir_forwards, SensorValue[gyro]);  //17.8in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	rotateTo(dir_left, -1800);  //rotate to correct position

	miniArmParam = down; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm DOWN
	wait1Msec(100);  //TODO: check this

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	drivingComplete = false; startMoveTask(0, dir_forwards, SensorValue[gyro]);  //move 0in FWD
		waitUntil(currentInchValue >= 0);  //wait for first GOAL picked up 40-x

		armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
		waitUntil(currentInchValue >= 0);  //wait for second GOAL picked up 70-x

		miniArmParam = up; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm UP

	waitUntil(drivingComplete == true);  //wait for driving position reached

	rotateTo(dir_left, (-68.6 + -180) * 10);  //rostate to correct position
	drivingComplete = false; startMoveTask(18.9, dir_forwards, SensorValue[gyro]);  //18.9in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	rotateTo(dir_right, -1800);  //rotate to initial forwards position

	drivingComplete = false; startMoveTask(24, dir_forwards, SensorValue[gyro]);  //24in FWD
	wait1Msec(1500);  //Wait 1500 seconds until robot is STRAIGHT with the BAR.

	miniArmParam = down; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm DOWN
	wait1Msec(1000);  //Wait 1000 seconds until GOAL has FALLEN.

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	drivingComplete = false; startMoveTask(-24, dir_backwards, SensorValue[gyro]);  //18.9in Back
	waitUntil(drivingComplete == true);  //wait for driving position reached

	/*60 POINTS*/
	//dun ^^

  /*Ore wo dare da to omotte yagaru?!*/
}

//TODO: test this
void runAutoCompBottom() {
	//screw you cone arm (�?�^�?�)
	motor[coneL] = 80;  //check negitave or pos
	motor[coneR] = 80;
	wait1Msec(1500);  //TODO: check this
	motor[coneL] = 0;
	motor[coneR] = 0;

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(100);  //TODO: check this

	drivingComplete = false; startMoveTask(57.8, dir_forwards, 0);  //move 57.8in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
	wait1Msec(100);  //TODO: check this

	rotateTo(dir_left, -180 * mod_degrees);  //rotate to correct position

	drivingComplete = false; startMoveTask(57.8, dir_forwards, -180 * mod_degrees);  //move 57.8in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	drivingComplete = false; startMoveTask(-66.3, dir_backwards, -180 * mod_degrees);  //move 66.3in BACK
	waitUntil(drivingComplete == true);  //wait for driving position reached

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(100);  //TODO: check this

	/*Moves away from goal (doesnt need to be precise)*/
	setLeftRightMoveSpeed(-120, -120);
	wait1Msec(2500);
	setLeftRightMoveSpeed(0, 0);
	wait1Msec(500);

	setLeftRightMoveSpeed(-70, -70);
	wait1Msec(1000);
	setLeftRightMoveSpeed(0, 0);
	wait1Msec(500);

  /*Ore wo dare da to omotte yagaru?!*/
}

//has a rotation different.
void runAutoCompTop() {
	runAutoCompBottom();	//there is just one auto.

  /*Ore wo dare da to omotte yagaru?!*/
}

void pre_auton() {
	gyroInit();
}

//0 is stop rotating, positive -> left, negitave -> right.
void setRotSpeed(int speed) {
	setLeftRightMoveSpeed(speed, -speed);
}

void auto(int dir) {
  motor[coneL] = 100;  //check negitave or pos
	motor[coneR] = 100;
	wait1Msec(1500);  //TODO: check this
	motor[coneL] = 0;
	motor[coneR] = 0;

	/*Drop Goal*/
	motor[goalHand] = 127;
	motor[goalHand2] = -127;
	wait1Msec(1250);
	motor[goalHand] = 0;
	motor[goalHand2] = 0;

	/*Move Forwards*/
	drivingComplete = false; startMoveTask(59, dir_forwards, 0);  //move 57.8in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	/*Pick Up Goal*/
	motor[goalHand] = -127;
	motor[goalHand2] = 127;
	wait1Msec(1500);
	motor[goalHand] = 0;
	motor[goalHand2] = -0;

	/*pull out*/
	setLeftRightMoveSpeed(-60, -60);
	wait1Msec(2200);
	setLeftRightMoveSpeed(0, 0);

	//rotateTo(dir_left, 210 * 10);

	/*Rotate 180 Degrees*/
	setRotSpeed(-90);
	bool exit = false;
	while(exit == false) {
		if(abs(SensorValue[gyro]) >= 180 * 8 * 1.30) { setRotSpeed(0); exit = true; }  //180 degrees.
	}

	/*Move To place goal*/
	setLeftRightMoveSpeed(100, 100);
	wait1Msec(1200);
	setLeftRightMoveSpeed(0, 0);
	//`````````````````````````````````````````
	/*Drop Goal*/
	motor[goalHand] = 127;
	motor[goalHand2] = -127;
	wait1Msec(1250);
	motor[goalHand] = 0;
	motor[goalHand2] = 0;

	/*Move To place goal*/
	setLeftRightMoveSpeed(-80, -80);
	wait1Msec(150);

	motor[goalHand] = 127;
	motor[goalHand2] = -127;

	setLeftRightMoveSpeed(0, 0);
	wait1Msec(150);

	setLeftRightMoveSpeed(-127, -127);
	wait1Msec(300);

	motor[goalHand] = 0;
	motor[goalHand2] = 0;

	setLeftRightMoveSpeed(0, 0);
	wait1Msec(150);

	setLeftRightMoveSpeed(-80, -80);
	wait1Msec(1500);

	setLeftRightMoveSpeed(0, 0);
	wait1Msec(150);

	setLeftRightMoveSpeed(-127, -127);
	wait1Msec(1700);

	setLeftRightMoveSpeed(0, 0);
	wait1Msec(150);
}

//20 maybe?
void auto2(int dir) {
  motor[coneL] = 70;  //check negitave or pos
	motor[coneR] = 70;
	wait1Msec(1350);  //TODO: check this
	motor[coneL] = 0;
	motor[coneR] = 0;

	/*Drop Goal*/
	motor[goalHand] = 127;
	motor[goalHand2] = -127;
	wait1Msec(1000);
	motor[goalHand] = 10;
	motor[goalHand2] = -10;

	/*Move Forwards*/
	//setLeftRightMoveSpeed(82, 80);
	//wait1Msec(2000);
	//setLeftRightMoveSpeed(0, 0);

	/*Move Forwards*/
	drivingComplete = false; startMoveTask(57.8, dir_forwards, 0);  //move 57.8in FWD
	waitUntil(drivingComplete == true);  //wait for driving position reached

	/*Pick Up Goal*/
	motor[goalHand] = -127;
	motor[goalHand2] = 127;
	wait1Msec(1500);
	motor[goalHand] = 0;
	motor[goalHand2] = -0;

	/*pull out*/
	setLeftRightMoveSpeed(-60, -60);
	wait1Msec(1750);
	setLeftRightMoveSpeed(0, 0);

	/*Rotate */
	setRotSpeed(-100);
	bool exit = false;
	while(exit == false) {
		if(abs(SensorValue[gyro]) >= 180 * 8 * 0.75) { setRotSpeed(0); exit = true; }  //180 degrees.
	}

	/*Move To place goal*/
	setLeftRightMoveSpeed(60, 60);
	wait1Msec(600); //50
	setLeftRightMoveSpeed(0, 0);

	/*Rotate */
	setRotSpeed(-100);
	bool exit2 = false;
	while(exit2 == false) {
		if(abs(SensorValue[gyro]) >= 180 * 8 * 1.3) { setRotSpeed(0); exit2 = true; }  //180 degrees.
	}

	/*Move To place goal*/
	setLeftRightMoveSpeed(110, 110);
	wait1Msec(1300);  //600
	setLeftRightMoveSpeed(0, 0);

	/*Drop Goal*/
	motor[goalHand] = 127;
	motor[goalHand2] = -127;
	wait1Msec(1250);
	motor[goalHand] = 0;
	motor[goalHand2] = 0;

	/*Move To place goal*/
	setLeftRightMoveSpeed(-90, -90);
	wait1Msec(150);

	motor[goalHand] = 127;
	motor[goalHand2] = -127;

	setLeftRightMoveSpeed(-30, -30);
	wait1Msec(150);

	setLeftRightMoveSpeed(-127, -127);
	wait1Msec(300);

	motor[goalHand] = 0;
	motor[goalHand2] = 0;

	setLeftRightMoveSpeed(-40, -40);
	wait1Msec(150);

	setLeftRightMoveSpeed(-110, -110);
	wait1Msec(1500);

	setLeftRightMoveSpeed(-10, -10);
	wait1Msec(150);

	setLeftRightMoveSpeed(-127, -127);
	wait1Msec(1700);

	setLeftRightMoveSpeed(-10, -10);
	wait1Msec(150);
}

//choose type of auto to run
task autonomous	{
	gyroInit();  //TODO: REMOVE THIS. (gets called in the function)

	//drivingComplete = false; startMoveTask(60, dir_forwards, SensorValue[gyro]);  //18.9in FWD
	//waitUntil(drivingComplete == true);  //wait for driving position reached

	//if(autonType == 0)
		//runAutoCompBottom();
	//else if(autonType == 1)
		//runAutoCompTop();
	//else if(autonType == 2)

	runAutoSkills();

	/* 1 is left -1 is right */
	//auto(1);
	//auto2(1)
}

float armError = 0;
int armSpeed = 50;

bool isGoalArmMovingDown = false;
bool goalArmToStack = false;
bool is8Up = true;

bool inDropPos = false;

int goalArmPos = 0;

bool isHoldingClaw = false;
bool clawClosed = false;

int miniGoalHoldVal = 1800;

task usercontrol {
	writeDebugStreamLine("***************Start************");
	writeDebugStreamLine("***************Start************");
	writeDebugStreamLine("***************Start************");
	writeDebugStreamLine("***************Start************");

	//miniArmParam = down; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm DOWN
	//wait1Msec(5000);
	//at();
	//runAutoSkills();

	//rotateTo(dir_left, -68.6 * mod_degrees);  //rotate to correct position

	writeDebugStreamLine("Done");
	inTeleop = true;
	while(true) {
		/*Large Goal Arm*/

		if(vexRT[Btn8D] == 1 && SensorValue[handsDown] == 0)	{
			motor[goalHand] = 110;
			motor[goalHand2] = -110;
			goalArmToStack = false;
		} else if(vexRT[Btn8R] == 1 && SensorValue[handsUp] == 0) {
			motor[goalHand] = -127;
			motor[goalHand2] = 127;
			goalArmToStack = false;
		} else {
			motor[goalHand] = 0;
			motor[goalHand2] = 0;
		}

		/*Move Goal Arm Correctly*/
		if(vexRT[Btn8U] == 1) {
			if(is8Up == true) {
				goalArmToStack = true;
			}
			is8Up = false;
		} else {
			is8Up = true;
		}

		if (goalArmToStack == true) {
			// 1100 is the pot value, 0.2 is the p value, 5 is the pressure mod.
			motor[goalHand] = -1 * capMinMax(0.2 * (1100 - SensorValue[largeGoalPot]) + 5, 2, 80);
			motor[goalHand2] = capMinMax(0.2 * (1100 - SensorValue[largeGoalPot]) + 5, 2, 80);
		}

		/*Cone Arm*/
		if(vexRT[Btn5U] == 1)	{
			motor[coneL] = 90;
			motor[coneR] = 90;
		} else if (vexRT[Btn5D] == 1) {
			motor[coneL] = -90;
			motor[coneR] = -90;
		} else {
			motor[coneL] = 0;
			motor[coneR] = 0;
		}

		/*Claws (negitave is closed)*/
		if(vexRT[Btn6U] == 1)	{
			//isHoldingClaw = false;
			//motor[claw] = 127;
		} else if (vexRT[Btn6D] == 1) {
			//isHoldingClaw = true;
			//motor[claw] = -110;
		} else {
			//clawClosed = false;
			//keeps pressure on the cone when picked up.
			if(isHoldingClaw == true) {
				//motor[claw] = -30;  //TODO: change value down or up if pressure is wrong.
			} else {
				//motor[claw] = 0;
			}
		}

		/*Mini Goal*/  //check polarity
		if(vexRT[Btn7R] == 1)	{  //up
			motor[goalHand2Motor] = -127;
			motor[miniGoalTwo] = -127;
			miniGoalHoldVal = SensorValue[miniGoalPot];
		} else if(vexRT[Btn7D] == 1) {  //down
			motor[goalHand2Motor] = 127;
			motor[miniGoalTwo] = 127;
			miniGoalHoldVal = SensorValue[miniGoalPot];
		} else if(vexRT[Btn7U] == 1) {  //pickup goal pos
			miniGoalHoldVal = 1800;
			motor[goalHand2Motor] = (miniGoalHoldVal - SensorValue[miniGoalPot]) * -0.3;
			motor[miniGoalTwo] = (miniGoalHoldVal - SensorValue[miniGoalPot]) * -0.3;
		} else {  //stay at pos
			motor[goalHand2Motor] = (miniGoalHoldVal - SensorValue[miniGoalPot]) * -0.3;
			motor[miniGoalTwo] = (miniGoalHoldVal - SensorValue[miniGoalPot]) * -0.3;
		}

		/*Tank Drive*/
		//setLeftRightMoveSpeed(vexRT[Ch3], vexRT[Ch2]);


		//left side.
		//motor[leftDrive] = vexRT[Ch3] + vexRT[Ch4] - 4;
  	//motor[backBackLeftDrive] = vexRT[Ch3] + vexRT[Ch4] - 4;

		//right side.
		//motor[rightDrive] = vexRT[Ch3] - vexRT[Ch4] - 4;
		//motor[backRightDrive] = vexRT[Ch3] - vexRT[Ch4] - 4;

		setLeftRightMoveSpeed(vexRT[Ch3] + (vexRT[Ch4] * 0.8) - 2, vexRT[Ch3] - (vexRT[Ch4] * 0.8) - 2);

		if(abs(vexRT[Ch2]) > 6) {
			motor[goalHand] = vexRT[Ch2];
			motor[goalHand2] = -vexRT[Ch2];
		}

		//TODO: button for stack cone

		writeDebugStreamLine("gyro is %d", SensorValue[gyro]);  //DEBUG: this

		//let everything update
		wait1Msec(16);  //TODO: remove?
	}
}
