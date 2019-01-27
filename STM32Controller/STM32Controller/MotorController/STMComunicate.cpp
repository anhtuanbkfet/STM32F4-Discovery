#include "stdafx.h"
#include "STMComunicate.h"


STM32Communicate::STM32Communicate()
{
	m_serialCOM = NULL;
}

STM32Communicate::~STM32Communicate()
{

}

BOOL STM32Communicate::UARTSend(char* strMsg)
{
	if (m_serialCOM && m_serialCOM->IsOpened())
	{
		bool ret = m_serialCOM->Write(strMsg);
		m_serialCOM->WriteChar(0x13);
		Sleep(10);
		return ret;
	}
	return false;
}

BOOL STM32Communicate::UARTSend(long nMsg)
{
	char strMsg[64];
	sprintf(strMsg, "%f", nMsg);
	return UARTSend(strMsg);
}

bool STM32Communicate::SendData(MOTOR motor, MotorSetting* param)
{
	if (!param)
		return false;
	char strMsg[256];

	sprintf(strMsg, 
		"%s %s %f %f %f %f %d %s ",
		POST_TAG, GET_MOTOR_NAME(motor),
		param->currentPIDValue.kP, param->currentPIDValue.kI, param->currentPIDValue.kD, param->refAngle, (int)param->maxSpeed,
		POST_TAG_END
		);

	return UARTSend(strMsg);
}

/*
@
@ Send Request to get response data from STM32F4
@ Just send <GET></GET> message
*/
bool STM32Communicate::GetData()
{
	char strMsg[64];

	sprintf(strMsg,
		"%s %s ",
		GET_TAG, GET_TAG_END
		);
	return UARTSend(strMsg);
}