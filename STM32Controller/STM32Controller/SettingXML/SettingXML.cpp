#include "stdafx.h"
#include "SettingXML.h"
#include "../MotorController/MotorCommon.h"

#include <iostream>
#include "string.h"
using namespace std;

SettingXML::SettingXML()
{
	
}

SettingXML::~SettingXML()
{

}

bool SettingXML::CheckSettingFile(const char* fileName)
{
	TiXmlDocument doc(fileName);
	if (!doc.LoadFile())
		return false;
	return true;
}

void SetParameter(TiXmlElement* root, const char* parameterName, PID_Parameter parameterValue)
{
	TiXmlElement* pParameter = new TiXmlElement(parameterName);
	root->LinkEndChild(pParameter);

	pParameter->SetDoubleAttribute("KP", parameterValue.kP);
	pParameter->SetDoubleAttribute("KI", parameterValue.kI);
	pParameter->SetDoubleAttribute("KD", parameterValue.kD);
}

bool SettingXML::SetMotorSetting(TiXmlElement* root, const char* motor, MotorSetting value)
{
	TiXmlElement* motorElement = new TiXmlElement(motor);

	root->LinkEndChild(motorElement);
	//Current:
	SetParameter(motorElement, "CURRENT_PID", value.currentPIDValue);
	//Min:	
	SetParameter(motorElement, "MIN_PID", value.minPIDValue);
	//Max:
	SetParameter(motorElement, "MAX_PID", value.maxPIDValue);

	TiXmlElement* motionElement = new TiXmlElement("MOTION");
	//Common motion:
	motorElement->LinkEndChild(motionElement);
	motionElement->SetDoubleAttribute("REF_ANGLE", value.refAngle);
	motionElement->SetDoubleAttribute("MAX_SPEED", value.maxSpeed);
	
	return true;
}

bool SettingXML::SaveSettingFile()
{
	TiXmlDocument doc;
	TiXmlElement* msg;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	TiXmlElement * root = new TiXmlElement("STM_Controller");
	doc.LinkEndChild(root);

	////////////////////////////////////////////////////////////
	//Motor(s) Setting save:
	TiXmlComment * comment = new TiXmlComment();
	comment->SetValue(" Settings for STM_Controller ");
	root->LinkEndChild(comment);

	TiXmlElement * motorSetting = new TiXmlElement("Motor-Setting");
	root->LinkEndChild(motorSetting);

	SetMotorSetting(motorSetting, "Yaw-Motor", m_settingValue.yawMotor);
	SetMotorSetting(motorSetting, "Pitch-Motor", m_settingValue.pitchMotor);
	SetMotorSetting(motorSetting, "Roll-Motor", m_settingValue.rollMotor);
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	comment = new TiXmlComment();
	comment->SetValue(" Common Settings for App ");
	root->LinkEndChild(comment);

	TiXmlElement * AppSetting = new TiXmlElement("App-Setting");
	root->LinkEndChild(AppSetting);

	/*-------------------Last COM port-----------------------*/
	TiXmlElement * comPort;
	comPort = new TiXmlElement("COM-Port");
	AppSetting->LinkEndChild(comPort);
	comPort->SetAttribute("PORT", m_settingValue.comPort.lastCOMPort);
	comPort->SetAttribute("BAUD_RATE", m_settingValue.comPort.baudRate);

	/*------------------Windows setting-----------------------*/
	TiXmlElement * window;
	window = new TiXmlElement("Window");
	AppSetting->LinkEndChild(window);
	window->SetAttribute("name", m_settingValue.window.windowName);
	window->SetAttribute("w", m_settingValue.window.width);
	window->SetAttribute("h", m_settingValue.window.height);

	////////////////////////////////////////////////////////////
	return doc.SaveFile(m_settingFilePath);
}

void SettingXML::LoadSetting()
{
	TiXmlDocument doc(m_settingFilePath);
	if (!doc.LoadFile()) return;

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	// block: app name
	pElem = hDoc.FirstChildElement().Element();
	// should always have a valid root but handle gracefully if it does
	if (!pElem)
		return;
	const char *pRoot = pElem->Value();
	// save this for later
	hRoot = TiXmlHandle(pElem);

	////////////////////////////////////////////////////////////////
	// block: Motor-Setting

	MotorSetting* settingValue[3] = { &m_settingValue.yawMotor, &m_settingValue.pitchMotor, &m_settingValue.rollMotor };
	

	TiXmlElement* pMotorSettingNode = hRoot.FirstChild("Motor-Setting").FirstChild().Element();
	int index = 0;
	for (pMotorSettingNode; pMotorSettingNode; pMotorSettingNode = pMotorSettingNode->NextSiblingElement())
	{
		//Now we in Yaw/Pitch/Roll-Motor... nodes.
		MotorSetting* motorSetting = settingValue[index];
		//Get CurrentPID:
		TiXmlElement* currentPID = pMotorSettingNode->FirstChildElement();
		const char* name = currentPID->Value();
		if (currentPID)
		{
			currentPID->QueryDoubleAttribute("KP", &(motorSetting->currentPIDValue.kP));
			currentPID->QueryDoubleAttribute("KI", &(motorSetting->currentPIDValue.kI));
			currentPID->QueryDoubleAttribute("KD", &(motorSetting->currentPIDValue.kD));
		}

		//Get MinPID:
		TiXmlElement* minPID = currentPID->NextSiblingElement();
		const char* nameminPID = minPID->Value();
		if (minPID)
		{
			minPID->QueryDoubleAttribute("KP", &(motorSetting->minPIDValue.kP));
			minPID->QueryDoubleAttribute("KI", &(motorSetting->minPIDValue.kI));
			minPID->QueryDoubleAttribute("KD", &(motorSetting->minPIDValue.kD));
		}

		//Get MaxPID:
		TiXmlElement* maxPID = minPID->NextSiblingElement();
		const char* namemaxPID = maxPID->Value();
		if (maxPID)
		{
			maxPID->QueryDoubleAttribute("KP", &(motorSetting->maxPIDValue.kP));
			maxPID->QueryDoubleAttribute("KI", &(motorSetting->maxPIDValue.kI));
			maxPID->QueryDoubleAttribute("KD", &(motorSetting->maxPIDValue.kD));
		}

		//Get Motion:
		TiXmlElement* motionParameter = maxPID->NextSiblingElement();
		const char* namemotionParameter = motionParameter->Value();
		if (motionParameter)
		{
			motionParameter->QueryDoubleAttribute("REF_ANGLE", &(motorSetting->refAngle));
			motionParameter->QueryDoubleAttribute("MAX_SPEED", &(motorSetting->maxSpeed));
		}

		index++;
	}

	// block: App-Setting
	//find COM-Port node.
	TiXmlElement* comPortNode = hRoot.FirstChild("App-Setting").FirstChild().Element();
	if (comPortNode)
	{
		comPortNode->QueryIntAttribute("PORT", &m_settingValue.comPort.lastCOMPort);
		comPortNode->QueryUnsignedAttribute("BAUD_RATE", &m_settingValue.comPort.baudRate);
	}

	//Now we in Window node.
	TiXmlElement* windowsNode = comPortNode->NextSiblingElement();
	if (windowsNode)
	{
		m_settingValue.window.windowName = windowsNode->Attribute("name");
		windowsNode->QueryIntAttribute("w", &m_settingValue.window.width);
		windowsNode->QueryIntAttribute("h", &m_settingValue.window.height);
	}
}

bool SettingXML::Init()
{
	//Get current folder and save setting file on here.
	TCHAR c_wPath[_MAX_PATH];
	char file[_MAX_PATH];
	GetModuleFileName(NULL, c_wPath, sizeof(c_wPath));
	wcstombs(file, c_wPath, wcslen(c_wPath) + 1);
	string strWFile(file);
	string filePath = strWFile.substr(0, strWFile.find_last_of("\\"));
	filePath.append("\\setting.xml");
	const char* settingFilePath = filePath.c_str();
	strcpy(m_settingFilePath, settingFilePath);

	//Make Setting file:
	if (!CheckSettingFile(m_settingFilePath))
		SaveSettingFile();
	//Load Setting file:
	LoadSetting();

	return true;
}

