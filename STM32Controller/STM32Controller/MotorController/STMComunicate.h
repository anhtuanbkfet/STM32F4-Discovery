/*
@
@ STM32F4 UART COMMUNICATE
@ Development by anhtuan.bkfet.k55@gmail.com
@
*/
#pragma once
#include "MotorCommon.h"
#include "../SerialCOM/SerialCOM.h"

//All defines:
#define POST_TAG				"<POST>"
#define GET_TAG					"<GET>"
#define RESPONSE_TAG			"<RESPONSE>"

#define POST_TAG_END			"</POST>"
#define GET_TAG_END				"</GET>"
#define RESPONSE_TAG_END		"</RESPONSE>"

class STM32Communicate
{
public:
	STM32Communicate();
	~STM32Communicate();

	void SetSerialCOM(SerialCOM* comObj){ m_serialCOM = comObj; }
	bool SendData(MOTOR motor, MotorSetting* param);
	bool GetData();
private:
	BOOL	UARTSend(char* strMsg);
	BOOL	UARTSend(long nMsg);

private:
	SerialCOM*			m_serialCOM;
};


