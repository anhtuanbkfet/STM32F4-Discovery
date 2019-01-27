/*
@ Setting save class - Using TinyXMl to save and read current setting.
@ Author: anhtuan.bkfet.k55@gmail.com
*/
#pragma once
#include "tinyxml.h"
#include "../MotorController/MotorCommon.h"

typedef struct _COMPortSetting
{
	int				lastCOMPort = 0;
	unsigned int	baudRate = 9600;
} COMPortSetting;

typedef struct _WindowSetting
{
	const char*		windowName = "STM32 Controller";
	int				width = 600;
	int				height = 400;
} WindowSetting;

///////////////////////////////////////////////

typedef struct _ControllerSetting
{
	//All motor setting:
	MotorSetting	yawMotor;
	MotorSetting	pitchMotor;
	MotorSetting	rollMotor;

	COMPortSetting  comPort;
	WindowSetting	window;
} ControllerSetting;

class SettingXML
{
public:
	SettingXML();
	~SettingXML();
	bool Init();
	ControllerSetting* GetControllSetting(){ return &m_settingValue; }
	bool SaveSettingFile();
	void LoadSetting();

private:

	bool CheckSettingFile(const char* fileName);
	bool SetMotorSetting(TiXmlElement* root, const char* motor, MotorSetting value);

private:
	char					m_settingFilePath[_MAX_PATH];
	ControllerSetting		m_settingValue;
};
