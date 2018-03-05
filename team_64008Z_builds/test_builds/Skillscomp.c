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
#pragma config(Motor,  port1,           claw,          tmotorVex393_HBridge, openLoop, reversed)
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

//newnew version

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

const float mod_wheel_circumference = 4 * 3.14159265358979; //in inches.
const int task_time_limit = 2500;  //2.5s

// This also handles negitave values (keeps polarity)
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
	SensorScale[gyro] = 102;
	SensorValue[gyro] = 0;

	/*SENSOR MUST NEVER FULL COUNT OR ROBOT DIES*/
	SensorFullCount[gyro] = 1800000;
}

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
			motor[goalHand2] = capMinMax(-0.2 * (1100 - SensorValue[largeGoalPot]) + 5, 2, 80);
			wait1Msec(20);
		}
		} else if(armParam == up) {
		motor[goalHand] = -120;
		motor[goalHand2] = 120;
		waitUntil(SensorValue[handsUp] == 1 || time1(T4) >= task_time_limit);
	}

	armParam = completed;

	motor[goalHand] = 0;
	motor[goalHand2] = 0;
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

/*Drive *TASK* Variables*/
float speedMod=0.23;
float inches;
float currentInchValue;
int negitaveMod=dir_forwards;
float gyroInitVal=0;
int maxTimeout=350;
int maxPower=102;
int minPower=17;
float drivingComplete = false;
float drivingMoveBack = false;

// The drive task works by slowing down as it gets closer to it's end point
//This is the drive *TASK* (not function)
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
	drivingMoveBack = false;

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
		if (abs(error) > 10) 	// if robot is within 10 degree wheel rotations and timer flag is off
			clearTimer(T3);			// start a timer

		if (time1(T3) >= 30)	// if the timer is over 90ms and timer flag is true (3 ticks)
			exitLoop = true;	// set boolean to complete while loop

		currentInchValue = encAvgLR / (380/2) * mod_wheel_circumference;  //convert to inches?

		// checks if robot has started moving back
		if (error <= 0)
			drivingMoveBack = true;

		ticks++;
		wait1Msec(20);  //loop speed.  //50hz
	}

	setLeftRightMoveSpeed(); //turn off motors.

	//reset again, just in case.
	resetEncoders();

	currentInchValue = 0;
	wait1Msec(20);
	drivingComplete = true;
	return;  //this?
}

// This calls the drive *TASK* (not function)
void startMoveTask(float inches_in, const int negitaveMod_in=dir_forwards, float gyroInitVal_in=SensorValue[gyro], float speedMod_in=0.28, int maxTimeout_in=275, int maxPower_in=94, int minPower_in=17) {
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
//run PD loop to turn to target degree.
void _rotateTo (int turnDirection, int targetDegrees, int maxPower=90, int minPower=22, int timeOut=2500) {
	// initialize PD loop variables
	float kp = 0.095; // TODO: tune this. still smaller?  //used to be 0.100
	int error = targetDegrees;
	int drivePower = 0;

	clearTimer(T2);

	// finish check variables
	bool atTarget = false;

	// initialize gyro data variables
	//int targetReading = SensorValue[gyro];
	int targetReading = 0;  //no relative rotation pls, thanks.

	// get gyroscope target reading
	// (turnDirection >= 1)
		//targetReading += abs(targetDegrees);
	//else if (turnDirection <= 0)
	targetReading -= targetDegrees;

	// run motors until target is within 1 degree certainty
	while (!atTarget && (time1[T2] < timeOut)) {
		error = targetReading - SensorValue[gyro]; 	// calculate error
		drivePower = error * kp;	// calculate PD loop output  //speed

		writeDebugStreamLine("error -> %d, gyro -> %d", error, SensorValue[gyro]);  //DEBUG: this

		//keep speed between min and max power.
		drivePower = capMinMax(drivePower, minPower, maxPower);

		//send power to motors and add mods.
		setLeftRightMoveSpeed(-drivePower, drivePower);

		// check for finish
		if (abs(error) > 10) 	// if robot is within 1 degree of target and timer flag is off
			clearTimer(T1);			// start a timer

		if (time1(T1) > 30)	// if the timer is over 30ms and timer flag is true
			atTarget = true;	// set boolean to complete while loop

		wait1Msec(16);  //let motors update. (used to be 20ms)
	}

	setLeftRightMoveSpeed();  //reset motors.

	// reset kp  wtf is this?  TODO: delete?
	//kp = 0.22;

	resetEncoders();
	return; //I think I need this...
}

// This gets called by the game_switch, not the comp_switch.
void pre_auton() {
	gyroInit();
}

// The code run while driving is one tab up, datte easier to see.
// **THIS IS THE MAIN CODE**
void first_goal() {
	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(300);

	drivingComplete = false; startMoveTask(34, dir_forwards, 0, 0.28, 100);  //FWD
		waitUntil(drivingComplete == true);  //wait for driving position reached

	armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
	wait1Msec(500);

	_rotateTo(dir_right, 1800 * 1.00);  //rotate to correct position

	drivingComplete = false; startMoveTask(30 - 3, dir_forwards, SensorValue[gyro]);  //FWD  //FIRST SCORE
		waitUntil(drivingComplete == true);  //wait for driving position reached

	//drop goal ###########################################
	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(700);

	drivingComplete = false; startMoveTask(5, dir_backwards, SensorValue[gyro]);  //BWD
	//waitUntil(drivingComplete == true);  //wait for driving position reached

		wait1Msec(200);  //TODO: TUNE THIS

		armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
		wait1Msec(200);

		waitUntil(drivingComplete == true);  //make sure driving is stopped

	////////////////////////**10 POINTS**////////////////////////
}

void second_goal() {
	_rotateTo(dir_right, 2700 * 1.00);  //rotate to correct position

	drivingComplete = false; startMoveTask(12, dir_backwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	_rotateTo(dir_right, (3600 + 450) * 1.00);  //rotate to correct position

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(300);

	drivingComplete = false; startMoveTask(40, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
	wait1Msec(500);

	_rotateTo(dir_right, (3600 + 1800 + 450) * 1.00);  //rotate to correct position

	drivingComplete = false; startMoveTask(40 - 3, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	_rotateTo(dir_right, (3600 + 1800 + 900) * 1.00);  //rotate to correct position

	drivingComplete = false; startMoveTask(24, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	_rotateTo(dir_right, (3600 + 3600 + 1800) * 1.00);  //rotate to correct position

	//start placing 30 POINTS

	drivingComplete = false; startMoveTask(40, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	////Let go of goal ###########################################
	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(800);

	armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(300);

	drivingComplete = false; startMoveTask(35, dir_backwards, SensorValue[gyro]);  //BWD
		waitUntil(drivingComplete == true);  //wait for driving position reached
}

// Gets the twenty point goal
void other_twenty() {
	_rotateTo(dir_right, (3600 + 3600 + 7200)   * 1.00);  //rotate to correct position

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(800);

	drivingComplete = false; startMoveTask(96, dir_forwards, SensorValue[gyro]);  //TODO: TUNING

		waitUntil(currentInchValue >= 90);
		armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN

		waitUntil(drivingComplete == true);  //wait for driving position reached

	_rotateTo(dir_right, (3600 + 3600 + 7200 + 900) * 1.00);  //rotate to correct position

	drivingComplete = false; startMoveTask(12, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	_rotateTo(dir_right, (3600 + 3600 + 7200 + 3600) * 1.00, 80);  //rotate to correct positionv

	drivingComplete = false; startMoveTask(40, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	////Let go of goal ###########################################
	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(800);

	armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(800);

	drivingComplete = false; startMoveTask(35, dir_backwards, SensorValue[gyro]);  //BWD
		waitUntil(drivingComplete == true);  //wait for driving position reached

	drivingComplete = false; startMoveTask(50, dir_backwards, SensorValue[gyro]);  //BWD
	waitUntil(drivingComplete == true);  //make sure driving is stopped
}

void newCompAuto() {

	first_goal();

	////////////////////////**10 POINTS**////////////////////////

	second_goal();

	////////////////////////**30 POINTS**////////////////////////

	_rotateTo(dir_right, (3600 + 3600 + 2700) * 1.00);  //rotate to correct position

	drivingComplete = false; startMoveTask(16, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	_rotateTo(dir_right, (3600 + 3600 + 3600) * 1.00);  //rotate to correct position

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(650);

	drivingComplete = false; startMoveTask(29, dir_forwards, SensorValue[gyro]);  //TODO: TUNING
		waitUntil(drivingComplete == true);  //wait for driving position reached

	armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
	wait1Msec(500);

	_rotateTo(dir_right, (3600 + 3600 + 5400) * 1.00);  //rotate to correct position

	drivingComplete = false; startMoveTask(37, dir_forwards, SensorValue[gyro]);  //FWD  //TODO: test this thing
		waitUntil(drivingComplete == true);  //wait for driving position reached

	//Let go of goal ###########################################
	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	wait1Msec(800);

	drivingComplete = false; startMoveTask(4, dir_backwards, SensorValue[gyro]);  //BWD
		//waitUntil(drivingComplete == true);  //wait for driving position reached

		wait1Msec(200);  //TODO: TUNE THIS

		armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
		wait1Msec(400);

		waitUntil(drivingComplete == true);  //make sure driving is stopped

	////////////////////////**40 POINTS**////////////////////////

	other_twenty();

	////////////////////////**60 POINTS**////////////////////////

	/*Ore wo dare da to omotte yagaru?!*/
}

//choose type of auto to run
task autonomous	{
	gyroInit();  //TODO: REMOVE THIS IN COMP. (this allready gets called in pre_auton)
	newCompAuto();
}

task usercontrol {
	writeDebugStreamLine("***************Start User*************");
	writeDebugStreamLine("***************Start User*************");

	writeDebugStreamLine("Done");
	while(true) {
		/*Large Goal Arm*/
		if(vexRT[Btn8D] == 1 && SensorValue[handsDown] == 0)	{
			motor[goalHand] = 125;
			motor[goalHand2] = -125;
			} else if(vexRT[Btn8R] == 1 && SensorValue[handsUp] == 0) {
			motor[goalHand] = -125;
			motor[goalHand2] = 125;
			} else {
			motor[goalHand] = 0;
			motor[goalHand2] = 0;
		}

		//0.8 is rotation speed
		setLeftRightMoveSpeed(vexRT[Ch3] + (vexRT[Ch4] * 0.8) - 2, vexRT[Ch3] - (vexRT[Ch4] * 0.8) - 2);

		//this is goalHand deadzone & movement code
		if(abs(vexRT[Ch2]) > 6) {
			motor[goalHand] = vexRT[Ch2];
			motor[goalHand2] = -vexRT[Ch2];
		}

		//writeDebugStreamLine("gyro is %d", SensorValue[gyro]);  //DEBUG: this

		//let everything update
		wait1Msec(16);
	}
}

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
