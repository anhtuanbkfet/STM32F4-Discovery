/*
@
@ SMOTOR Common define
@ Development by anhtuan.bkfet.k55@gmail.com
@
*/
#pragma once
#define GET_MOTOR_NAME GetMotorName

static char* gl_MotorName[] = { "UNKNOWN_MOTOR", "YAW_MOTOR", "PITCH_MOTOR", "ROLL_MOTOR" };

typedef enum _MOTOR
{
	YAW_MOTOR = 1,
	PITCH_MOTOR,
	ROLL_MOTOR
} MOTOR;

typedef struct _PID_Parameter
{
	double kP = 0;
	double kI = 0;
	double kD = 0;
} PID_Parameter;

typedef struct _MotorSetting
{
	PID_Parameter	currentPIDValue;
	PID_Parameter	minPIDValue;
	PID_Parameter	maxPIDValue;

	double			refAngle = 0.0;
	double			maxSpeed = 0.0;
} MotorSetting;

static char* GetMotorName(MOTOR motor)
{
	return gl_MotorName[motor];
}


