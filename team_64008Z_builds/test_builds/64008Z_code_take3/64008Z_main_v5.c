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
#pragma config(Motor,  port1,           pushGoalHand,  tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           frontLeftDrive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           frontRightDrive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           goalHands,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           coneArms,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           backBackLeftDrive, tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port7,           backLeftDrive, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           backBackRightDrive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           backRightDrive, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port10,          claw,          tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

const int bot_auton = 0;
const int top_auton = 1;
const int skills_auton = 2;

/*Auto Type*/
const int autonType = skills_auton;

/*Compo Init*/
#pragma platform(VEX);

#pragma competitionControl(Competition);
#pragma autonomousDuration(9000);
#pragma userControlDuration(9000);

#include "Vex_Competition_Includes.c" //backcode no modify pls.

/*Global Constants*/
//#include "64008Z_auto_v2.c" //autonomous code.
const int down = 1;
const int up = 2;
const int mid = 3;
const int completed = 4;
const int stopAutoCorrect = 5;

const int dir_forwards = 1;
const int dir_backwards = -1;
const int dir_left = -1;
const int dir_right = 1;

const int pos_bot = 0;
const int pos_mid = 1;
const int pos_top = 2;

const float mod_degrees = (278/360) * 10;  //multiply this with degrees to get
const float mod_wheel_circumference = 4 * 3.14159265358979; //in inches.
const int task_time_limit = 2500;  //2.5s

//const float armKp = 1.5;
//const int degToMove = -100;

bool inTeleop = false;

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
	wait1Msec(2000);

	//writeDebugStreamLine("Gyro Setup...");
	//SensorScale[gyro] = 278;  //?
	SensorValue[gyro] = 0;
	SensorFullCount[gyro] = 3600;  //not working? gyro is auto set to 3600. dunno dont care.
}

//TODO: TEST THIS.
///direction = up(const) for up move and down(const) for down move.
int armParam = 0;
task autoMoveGoalArms() {
	clearTimer(T4);

	if(armParam == up) {
		motor[goalHands] = 127;
    waitUntil(SensorValue[handsUp] == 1 || time1(T4) >= task_time_limit);
	} else if(armParam == mid) {
		while(armParam != stopAutoCorrect) {
			motor[goalHands] = capMinMax(0.2 * (1100 - SensorValue[largeGoalPot]) + 5, 2, 80);
			wait1Msec(20);
		}
	} else if(armParam == down) {
		motor[goalHands] = -127;
    waitUntil(SensorValue[handsDown] == 1 || time1(T4) >= task_time_limit);
	}

  armParam = completed;

	motor[goalHands] = 0;
}

//TODO: TEST THIS.
//sensorPotentiometer:
const int pot_up = 3800;
const int pot_down = 1675;
const int pot_no_ground = 2150;

//not super accurate (doesn't need to be.)
int miniArmParam = 0;
task autoMoveMiniGoalArms() {
	clearTimer(T3);

	if(miniArmParam == up) {
		while(miniArmParam != stopAutoCorrect) {
			motor[pushGoalHand] = capMinMax(-0.2 * (pot_up - SensorValue[miniGoalPot]), 2, 127);
			wait1Msec(20);
		}
	}
	else if(miniArmParam == down) {
		while(miniArmParam != stopAutoCorrect) {
			motor[pushGoalHand] = capMinMax(-0.2 * (pot_down - SensorValue[miniGoalPot]), 2, 127);
			wait1Msec(20);
		}
	}
	else if (miniArmParam == mid) {
		while(miniArmParam != stopAutoCorrect) {
			motor[pushGoalHand] = capMinMax(-0.2 * (pot_no_ground - SensorValue[miniGoalPot]), 2, 127);
			wait1Msec(20);
		}
	}

  miniArmParam = completed;

	motor[pushGoalHand] = 0;
}

//sets the speed of the left and right sides of the drive.  Empty is not moving.
void setLeftRightMoveSpeed(int leftSpeed=0, int rightSpeed=0) {
  //calculate deadzone.
  if(abs(leftSpeed) < 6) { leftSpeed = 0; }
  if(abs(rightSpeed) < 6) { rightSpeed = 0; }

	//left side.
	motor[frontLeftDrive] = leftSpeed;
  motor[backLeftDrive] = leftSpeed;
  motor[backBackLeftDrive] = leftSpeed;

	//right side.
	motor[frontRightDrive] = rightSpeed;
	motor[backRightDrive] = rightSpeed;
	motor[backBackRightDrive] = rightSpeed;

	writeDebugStreamLine("L - R is %d - %d", leftSpeed, rightSpeed); //DEBUG: this
}

void resetEncoders() {
	nMotorEncoder[backBackLeftDrive] = 0;
	nMotorEncoder[backRightDrive] = 0;
}


float inches;
float currentInchValue;

const int negitaveMod;
float gyroInitVal=0;

int maxTimeout=250;
int maxPower=102;
int minPower=17;

float drivingComplete = false;

//init a drive task and start it
void startMoveTask(float inches_in, const int negitaveMod_in=dir_forwards, float gyroInitVal_in=SensorValue[gyro], int maxTimeout_in=250, int maxPower_in=102, int minPower_in=17) {
	inches = inches_in;
	negitaveMod = negitaveMod_in;
	gyroInitVal = gyroInitVal_in;
	maxTimeout = maxTimeout_in;
	maxPower = maxPower_in;
	minPower = minPower_in;

	startTask(moveStraightGyro);
}

task moveStraightGyro() {
	float disKp = 0.24; //distance kp.  //was 0.22
	if(inches <= 12) { disKp = 0.3; }
	float gyroKp = 2.9;

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
		float encAvgLR = (abs(nMotorEncoder[backBackLeftDrive]) + abs(nMotorEncoder[backLeftDrive])) / 2;

		error = (abs(inches / mod_wheel_circumference * (380/2)) - encAvgLR);  //how close to completed.  // 380 is one rotation? ... /2? ...
    speed = error * disKp;

    gyroError = gyroInitVal - SensorValue[gyro];
    sideMod = gyroError * gyroKp;

    writeDebugStreamLine("speed is %d, error is %d, sideMod is %d, enc is %d, gyroerr is %d", speed, error, sideMod, nMotorEncoder[backLeftDrive], gyroError); //DEBUG: this

    //keep speed between min and max power.
		speed = capMinMax(speed, minPower, maxPower);

		setLeftRightMoveSpeed((speed - sideMod * (speed/127)) * negitaveMod, (speed + sideMod * (speed/127)) * negitaveMod);  //move forwards

		writeDebugStreamLine("speed is %d, error is %d, sideMod is %d", speed, error, sideMod); //DEBUG: this

		// check for completion
		if (abs(error) > 5) 	// if robot is within 5 degree wheel rotations and timer flag is off
			//exitLoop = true;	// set boolean to complete while loop
			clearTimer(T1);			// start a timer

		if (time1(T1) >= 90)	// if the timer is over 90ms and timer flag is true (3 ticks)
			exitLoop = true;	// set boolean to complete while loop

		currentInchValue = encAvgLR / (380/2) * mod_wheel_circumference;  //convert to inches?

		ticks++;
		wait1Msec(20);  //loop speed.  //60hz
	}

	setLeftRightMoveSpeed(); //turn off motors.

  //reset again, just in case.
	resetEncoders();

	currentInchValue = 0;
	drivingComplete = true;
	return;  //this?
}

//goes faster + has an instant stop at the end
task moveStraightGyro2() {
	float disKp = 0.5; //distance kp.  //was 0.24
	if(inches <= 12) { disKp = 0.3; }
	float gyroKp = 2.9;

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
		float encAvgLR = (abs(nMotorEncoder[backBackLeftDrive]) + abs(nMotorEncoder[backLeftDrive])) / 2;

		error = (abs(inches / mod_wheel_circumference * (380/2)) - encAvgLR);  //how close to completed.  // 380 is one rotation
    speed = error * disKp;

    gyroError = gyroInitVal - SensorValue[gyro];
    sideMod = gyroError * gyroKp;

    writeDebugStreamLine("speed is %d, error is %d, sideMod is %d, enc is %d, gyroerr is %d", speed, error, sideMod, nMotorEncoder[backLeftDrive], gyroError); //DEBUG: this

    //keep speed between min and max power.
		speed = capMinMax(speed, minPower, maxPower);

		setLeftRightMoveSpeed((speed - sideMod * (speed/127)) * negitaveMod, (speed + sideMod * (speed/127)) * negitaveMod);  //move forwards

		writeDebugStreamLine("speed is %d, error is %d, sideMod is %d", speed, error, sideMod); //DEBUG: this

		if (abs(error) > 5) 	// if robot is within 5 degree wheel rotations do auto stop thing
			setLeftRightMoveSpeed(-(speed - sideMod * (speed/127)) * negitaveMod, -(speed + sideMod * (speed/127)) * negitaveMod);  //move backwards
			wait1Msec(200);  //loop speed.  //60hz
			exitLoop = true;	// set boolean to complete while loop
		}

		currentInchValue = encAvgLR / (380/2) * mod_wheel_circumference;  //convert to inches?

		ticks++;
		wait1Msec(20);  //loop speed.  //60hz
	}

	setLeftRightMoveSpeed(); //turn off motors.

  //reset again, just in case.
	resetEncoders();

	currentInchValue = 0;
	drivingComplete = true;

	return;  //this?
}

/*Gyro Turn*/
//run PD loop to turn to target deg.
void rotateTo (int turnDirection, int targetDegrees, int maxPower=110, int minPower=22, int timeOut=2500) {
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
		targetReading += targetDegrees;
	else if (turnDirection <= 0)
		targetReading -= targetDegrees;

	// change kp if target is under 20 degree threshold
	if (targetDegrees < 200) { kp = 0.2; }  //ok?

	// run motors until target is within 1 degree certainty
	while (!atTarget && (time1[T2] < timeOut)) {
		error = targetReading - SensorValue[gyro]; 	// calculate error
		drivePower = error * kp;	// calculate PD loop output  //speed

		writeDebugStreamLine("error -> %d, gyro -> %d", error, SensorValue[gyro]);  //DEBUG: this

		//keep speed between min and max power.
		drivePower = capMinMax(drivePower, minPower, maxPower);

		//<Enc Stuff Start
			float kp = 2.5;  //Tune?

			float errorVal = 0;

  		float leftMod = 0;
			float rightMod = 0;
  		//set offset value, if 0 both sides are moving at same speed.  use abs value for both motors, pretending both sides going forwards.
  		errorVal = (abs(nMotorEncoder[backLeftDrive]) - abs(nMotorEncoder[backRightDrive]));

  		//update both modifiers.
  		rightMod = (errorVal * kp);
  		leftMod = -rightMod;

  		writeDebugStreamLine("mod = error[%d] * kp[%d] = +[%d]", errorVal, kp, leftMod);  //DEBUG: this
		//Enc Stuff End/>

		//send power to motors and add mods (scaled with drive power).
		setLeftRightMoveSpeed(-(drivePower + (leftMod * (drivePower/127))), (drivePower + (rightMod * (drivePower/127))));

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
	//screw you cone arm (‾^‾)
	motor[coneArms] = -80;
	wait1Msec(150);  //TODO: check this
	motor[coneArms] = 0;

	miniArmParam = pot_down; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm DOWN
	wait1Msec(100);  //TODO: check this

	armParam = down; startTask(autoMoveGoalArms);  //MAIN GOAL arm DOWN
	drivingComplete = false; startMoveTask(102.3, dir_forwards, 0);  //102.3in FWD
	waitUntil(currentInchValue >= 40)  //wait for first GOAL picked up

	armParam = up; startTask(autoMoveGoalArms);  //MAIN GOAL arm UP
	waitUntil(currentInchValue >= 70)  //wait for second GOAL picked up

	miniArmParam = pot_up; startTask(autoMoveMiniGoalArms);  //MINI GOAL arm DOWN
	waitUntil(drivingComplete == true)  //wait for driving position reached

	rotateTo(dir_left, -68.6 * mod_degrees)  //rotate to correct position

	drivingComplete = false; startMoveTask(18.9, dir_forwards, -68.6 * mod_degrees);  //102.3in FWD


  /*Ore wo dare da to omotte yagaru?!*/
}

void runAutoCompBottom() {
  /*Ore wo dare da to omotte yagaru?!*/
}

//has a rotation different.
void runAutoCompTop() {
  /*Ore wo dare da to omotte yagaru?!*/
}

void pre_auton() {
	gyroInit();
}

task autonomous	{
	//if(autonType == 0)
		//runAutoCompBottom();
	//if(autonType == 1)
		//runAutoCompTop();
	//if(autonType == 2)
		//runAutoSkills();

	moveStraightGyro(72, dir_forwards);
	wait1Msec(1000);

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

int miniGoalHoldVal = 1675;

task usercontrol {
	writeDebugStreamLine("***************Start************");
	writeDebugStreamLine("***************Start************");
	writeDebugStreamLine("***************Start************");
	writeDebugStreamLine("***************Start************");

	writeDebugStreamLine("Done");
	inTeleop = true;
	while(true) {
		/*Large Goal Arm*/
		if(vexRT[Btn8R] == 1 && SensorValue[handsDown] == 0)	{
			motor[goalHands] = -127;
			goalArmToStack = false;
		} else if(vexRT[Btn8D] == 1 && SensorValue[handsUp] == 0) {
			motor[goalHands] = 80;
			goalArmToStack = false;
		} else {
			motor[goalHands] = 0;
		}

		/*Move Goal Arm Correctly*/
		if(vexRT[Btn8U] == 1) {
			if(is8Up == true) {
				goalArmToStack = true;
			}
			is8Up = false
		} else {
			is8Up = true;
		}

		if (goalArmToStack == true) {
			// 1100 is the pot value, 0.2 is the p value, 5 is the pressure mod.
			motor[goalHands] = capMinMax(0.2 * (1100 - SensorValue[largeGoalPot]) + 5, 2, 80);
		}

		/*Cone Arm*/
		if(vexRT[Btn5U] == 1)	{
			motor[coneArms] = 90;
		} else if (vexRT[Btn5D] == 1) {
			motor[coneArms] = -90;
		} else {
			motor[coneArms] = 0;
		}

		/*Claws (negitave is closed)*/
		if(vexRT[Btn6U] == 1)	{
			isHoldingClaw = false;
			motor[claw] = 127;
		} else if (vexRT[Btn6D] == 1) {
			isHoldingClaw = true;
			motor[claw] = -110;
		} else {
			clawClosed = false;
			//keeps pressure on the cone when picked up.
			if(isHoldingClaw == true) {
				motor[claw] = -30;  //TODO: change value down or up if pressure is wrong.
			} else {
				motor[claw] = 0;
			}
		}

		/*Mini Goal*/
		if(vexRT[Btn7L] == 1)	{
			motor[pushGoalHand] = 127;
			miniGoalHoldVal = SensorValue[miniGoalPot];
		} else if(vexRT[Btn7D] == 1) {
			motor[pushGoalHand] = -127;
			miniGoalHoldVal = SensorValue[miniGoalPot];
		} else if(vexRT[Btn7U] == 1) {
			miniGoalHoldVal = 1675;
		} else {
			motor[pushGoalHand] = (miniGoalHoldVal - SensorValue[miniGoalPot]) * -0.2;
		}

		/*Tank Drive*/
		//setLeftRightMoveSpeed(vexRT[Ch3], vexRT[Ch2]);

		//left side.
		motor[frontLeftDrive] = vexRT[Ch3] + vexRT[Ch4];
  	motor[backLeftDrive] = vexRT[Ch3] + vexRT[Ch4];
  	motor[backBackLeftDrive] = vexRT[Ch3] + vexRT[Ch4];

		//right side.
		motor[frontRightDrive] = vexRT[Ch3] - vexRT[Ch4];
		motor[backRightDrive] = vexRT[Ch3] - vexRT[Ch4];
		motor[backBackRightDrive] = vexRT[Ch3] - vexRT[Ch4];

		//TODO: button for stack cone

		//writeDebugStreamLine("gyro is %d", SensorValue[gyro]);  //DEBUG: this

		//let everything update
		wait1Msec(20);  //TODO: remove?
	}
}
