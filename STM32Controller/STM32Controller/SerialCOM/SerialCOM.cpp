#include "stdafx.h"
#include "SerialCOM.h"
//-----------------------------------------------------------------------------
SerialCOM::SerialCOM()
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_portnum = 7;
	m_baudRate = B9600;
	m_ownerDelegate = NULL;
}

SerialCOM::SerialCOM(long PortNo, long BaudRate)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_portnum = PortNo;
	m_baudRate = BaudRate;
	m_ownerDelegate = NULL;
	//InitializeCriticalSection(&m_sec);
}

SerialCOM::~SerialCOM()
{
	Close();
	//DeleteCriticalSection(&m_sec);
}

void SerialCOM::SetPortNumber(long PortNo) {
	if (PortNo >= 0)
		m_portnum = PortNo;
}

long SerialCOM::GetPortNumber() {
	return m_portnum;
}

void SerialCOM::SetBaudRate(long baudrate) {
	if (baudrate < B9600) m_baudRate = B4800;
	else if (baudrate < B19200) m_baudRate = B9600;
	else if (baudrate < B38400) m_baudRate = B19200;
	else if (baudrate < B57600) m_baudRate = B38400;
	else if (baudrate < B115200) m_baudRate = B57600;
	else m_baudRate = B115200;
}

long SerialCOM::GetBaudRate() {
	return m_baudRate;
}

SERIAL_RESULT SerialCOM::Open()
{

	if (!IsOpened())
	{
#ifdef UNICODE
		wchar_t wtext[32];
		swprintf(wtext, L"\\\\.\\COM%d", m_portnum);
#else
		char wtext[32];
		sprintf(wtext, "\\\\.\\COM%d", m_portnum);
#endif
		m_hComm = CreateFile(wtext,
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			0);
		if (m_hComm == INVALID_HANDLE_VALUE) 
			return Serial_result_error;

		if (PurgeComm(m_hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == 0) 
			return Serial_result_error; //purge

		//get initial state
		DCB dcbOri;
		bool fSuccess;
		fSuccess = GetCommState(m_hComm, &dcbOri);
		if (!fSuccess) 
			return Serial_result_error;

		DCB dcb1 = dcbOri;
		dcb1.BaudRate = m_baudRate;
		dcb1.Parity = NOPARITY;
		dcb1.ByteSize = 8;
		dcb1.StopBits = ONESTOPBIT;
		dcb1.fOutxCtsFlow = false;
		dcb1.fOutxDsrFlow = false;
		dcb1.fOutX = false;
		dcb1.fDtrControl = DTR_CONTROL_DISABLE;
		dcb1.fRtsControl = RTS_CONTROL_DISABLE;
		fSuccess = SetCommState(m_hComm, &dcb1);
		delay(60);
		if (!fSuccess) 
			return Serial_result_error;

		fSuccess = GetCommState(m_hComm, &dcb1);
		if (!fSuccess)
			return Serial_result_error;

		m_osReader = { 0 };// Create the overlapped event. Must be closed before exiting to avoid a handle leak.
		m_osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		if (m_osReader.hEvent == NULL) 
			return Serial_result_error; // Error creating overlapped event; abort.
		m_fWaitingOnRead = FALSE;

		m_osWrite = { 0 };
		m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_osWrite.hEvent == NULL) { return Serial_result_error; }

		if (!GetCommTimeouts(m_hComm, &m_timeouts)) 
			return Serial_result_error;  // Error getting time-outs.
		COMMTIMEOUTS timeouts;
		timeouts.ReadIntervalTimeout = 20;
		timeouts.ReadTotalTimeoutMultiplier = 15;
		timeouts.ReadTotalTimeoutConstant = 100;
		timeouts.WriteTotalTimeoutMultiplier = 15;
		timeouts.WriteTotalTimeoutConstant = 100;
		if (!SetCommTimeouts(m_hComm, &timeouts)) 
			return Serial_result_error; // Error setting time-outs.
	}
	return Serial_result_ok;
}

void SerialCOM::Close()
{
	if (IsOpened())
	{
		SetCommTimeouts(m_hComm, &m_timeouts);
		CloseHandle(m_osReader.hEvent);
		CloseHandle(m_osWrite.hEvent);
		CloseHandle(m_hComm);//close comm port
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool SerialCOM::IsOpened()
{
	if (m_hComm == INVALID_HANDLE_VALUE) return false;
	else return true;
}

bool SerialCOM::Write(char *data)
{
	if (!IsOpened()) {
		return false;
	}
	BOOL fRes;
	DWORD dwWritten;
	long n = strlen(data);
	if (n < 0) n = 0;
	else if (n > 1024) n = 1024;

	// Issue write.
	if (!WriteFile(m_hComm, data, n, &dwWritten, &m_osWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) { fRes = FALSE; }// WriteFile failed, but it isn't delayed. Report error and abort.
		else {// Write is pending.
			if (!GetOverlappedResult(m_hComm, &m_osWrite, &dwWritten, TRUE)) fRes = FALSE;
			else fRes = TRUE;// Write operation completed successfully.
		}
	}
	else fRes = TRUE;// WriteFile completed immediately.
	return fRes;
}

bool SerialCOM::WriteChar(char ch)
{
	char s[2];
	s[0] = ch;
	s[1] = 0;//null terminated
	return Write(s);
}

char SerialCOM::ReadChar(bool& success)
{
	success = false;
	if (!IsOpened()) { return 0; }

	DWORD dwRead;
	DWORD length = 1;
	BYTE* data = (BYTE*)(&m_rxchar);
	//the creation of the overlapped read operation
	if (!m_fWaitingOnRead) {
		// Issue read operation.
		if (!ReadFile(m_hComm, data, length, &dwRead, &m_osReader)) {
			if (GetLastError() != ERROR_IO_PENDING) { /*Error*/ }
			else { m_fWaitingOnRead = TRUE; /*Waiting*/ }
		}
		else { if (dwRead == length) success = true; }//success
	}


	//detection of the completion of an overlapped read operation
	DWORD dwRes;
	if (m_fWaitingOnRead) {
		dwRes = WaitForSingleObject(m_osReader.hEvent, READ_TIMEOUT);
		switch (dwRes)
		{
			// Read completed.
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(m_hComm, &m_osReader, &dwRead, FALSE)) {/*Error*/ }
			else {
				if (dwRead == length) success = true;
				m_fWaitingOnRead = FALSE;// Reset flag so that another opertion can be issued.
			}// Read completed successfully.
			break;

		case WAIT_TIMEOUT:
			// Operation isn't complete yet.
			break;

		default:
			// Error in the WaitForSingleObject;
			break;
		}
	}
	return m_rxchar;
}

bool SerialCOM::SetRTS(bool value)
{
	bool r = false;
	if (IsOpened()) {
		if (value) {
			if (EscapeCommFunction(m_hComm, SETRTS)) r = true;
		}
		else {
			if (EscapeCommFunction(m_hComm, CLRRTS)) r = true;
		}
	}
	return r;
}

bool SerialCOM::SetDTR(bool value)
{
	bool r = false;
	if (IsOpened()) {
		if (value) {
			if (EscapeCommFunction(m_hComm, SETDTR)) r = true;
		}
		else {
			if (EscapeCommFunction(m_hComm, CLRDTR)) r = true;
		}
	}
	return r;
}

bool SerialCOM::GetCTS(bool& success)
{
	success = false;
	bool r = false;
	if (IsOpened()) {
		DWORD dwModemStatus;
		if (GetCommModemStatus(m_hComm, &dwModemStatus)){
			r = MS_CTS_ON & dwModemStatus;
			success = true;
		}
	}
	return r;
}

bool SerialCOM::GetDSR(bool& success)
{
	success = false;
	bool r = false;
	if (IsOpened()) {
		DWORD dwModemStatus;
		if (GetCommModemStatus(m_hComm, &dwModemStatus)) {
			r = MS_DSR_ON & dwModemStatus;
			success = true;
		}
	}
	return r;
}

bool SerialCOM::GetRI(bool& success)
{
	success = false;
	bool r = false;
	if (IsOpened()) {
		DWORD dwModemStatus;
		if (GetCommModemStatus(m_hComm, &dwModemStatus)) {
			r = MS_RING_ON & dwModemStatus;
			success = true;
		}
	}
	return r;
}

bool SerialCOM::GetCD(bool& success)
{
	success = false;
	bool r = false;
	if (IsOpened()) {
		DWORD dwModemStatus;
		if (GetCommModemStatus(m_hComm, &dwModemStatus)) {
			r = MS_RLSD_ON & dwModemStatus;
			success = true;
		}
	}
	return r;
}

void SerialCOM::Listen()
{
	std::thread GetLogDataTh = std::thread(_thListen, this);
	GetLogDataTh.detach();
}

void SerialCOM::_thListen(void* param)
{
	SerialCOM *parent = (SerialCOM*)param;
	char *pLog = parent->m_readDataBuffer;
	bool successFlag;

	while (parent->IsOpened())
	{
		*pLog = parent->ReadChar(successFlag); //read a char

		if (successFlag && *pLog)
		{
			if (*pLog == '\n')		//If received enter character, print log:
			{
				*pLog++ = '\r';
				*pLog++ = '\n';
				*pLog = '\0';
				pLog = parent->m_readDataBuffer;	//Reset pointer;
				if (parent->m_ownerDelegate)
				{
					parent->m_ownerDelegate->OnSTM32LogReceived(pLog);
				}
				Sleep(10);
			}
			else
				pLog++;
		}
		else
			Sleep(5);
	}
}

