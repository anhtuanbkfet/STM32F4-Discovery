#pragma once
#include <stdio.h>
#include <windows.h>
#include <thread>

#define READ_TIMEOUT 10      // milliseconds


static void delay(unsigned long ms)
{
	Sleep(ms);
}

typedef enum __SERIAL_BAUDRATE
{
	B4800 = 4800,
	B9600 = 9600,
	B19200 = 19200,
	B38400 = 38400,
	B57600 = 57600,
	B115200 = 115200,

} SERIAL_BAUDRATE;

typedef enum __SERIAL_RESULT
{
	Serial_result_ok = 0,
	Serial_result_error = -1,

} SERIAL_RESULT;

class ISerialSTM32Delegate
{
public:
	virtual void OnSTM32LogReceived(char* strLog){};
};

//-----------------------------------------------------------------------------
class SerialCOM {
	
public:
	SerialCOM();
	SerialCOM(long PortNo, long BaudRate);
	~SerialCOM();

	SERIAL_RESULT	Open(void);//return 0 if success
	void			Close();
	char			ReadChar(bool& success);//return read char if success
	bool			WriteChar(char ch);////return success flag
	bool			Write(char *data);//write null terminated string and return success flag
	bool			SetRTS(bool value);//return success flag
	bool			SetDTR(bool value);//return success flag
	bool			GetCTS(bool& success);
	bool			GetDSR(bool& success);
	bool			GetRI(bool& success);
	bool			GetCD(bool& success);
	bool			IsOpened();
	void			SetPortNumber(long PortNo);
	long			GetPortNumber();
	void			SetBaudRate(long baudrate);
	long			GetBaudRate();
	void			Listen();
	void			SetOwnerDelegate(ISerialSTM32Delegate* owner){ m_ownerDelegate = owner; }

protected:
	static void		_thListen(void* param);
	
private:
	HANDLE			m_hComm; //handle
	OVERLAPPED		m_osReader;
	OVERLAPPED		m_osWrite;
	long			m_portnum;
	long			m_baudRate;
	BOOL			m_fWaitingOnRead;
	char			m_rxchar;
	COMMTIMEOUTS	m_timeouts;
	//CRITICAL_SECTION	m_sec;

	char			m_readDataBuffer[512];
	ISerialSTM32Delegate* m_ownerDelegate;
};