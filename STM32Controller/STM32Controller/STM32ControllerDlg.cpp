
// STM32ControllerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "STM32Controller.h"
#include "STM32ControllerDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <queue>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSTM32ControllerDlg dialog



CSTM32ControllerDlg::CSTM32ControllerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSTM32ControllerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_controllerSetting = NULL;
	m_currentMotor = YAW_MOTOR;
	m_currentMotorSetting = NULL;
	m_comPortIsOpened = FALSE;
	m_currentEditTextFocus = NULL;
}

void CSTM32ControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	DDX_Control(pDX, IDC_EDIT_YAW, m_txtYaw);
	DDX_Control(pDX, IDC_EDIT_PITCH, m_txtPitch);
	DDX_Control(pDX, IDC_EDIT_ROLL, m_txtRoll);

	DDX_Control(pDX, IDC_EDIT1_REF_YAW, txt_refAngleYaw);
	DDX_Control(pDX, IDC_EDIT1_REF_PITCH, txt_refAnglePitch);
	DDX_Control(pDX, IDC_EDIT1_REF_ROLL, txt_refAngleRoll);

	DDX_Control(pDX, IDC_EDIT_MAX_SPEED_YAW, txt_maxSpeedYaw);
	DDX_Control(pDX, IDC_EDIT_MAX_SPEED_PITCH, txt_maxSpeedPitch);
	DDX_Control(pDX, IDC_EDIT_MAX_SPEED_ROLL, txt_maxSpeedRoll);

	DDX_Control(pDX, IDC_SLIDER_PID_YAW, sld_controlYaw);
	DDX_Control(pDX, IDC_SLIDER_PID_PITCH, sld_controlPitch);
	DDX_Control(pDX, IDC_SLIDER_PID_ROLL, sld_controlRoll);

	DDX_Control(pDX, IDC_EDIT_MIN_PID_YAW, txt_minKP);
	DDX_Control(pDX, IDC_EDIT_MIN_PID_PITCH, txt_minKI);
	DDX_Control(pDX, IDC_EDIT_MIN_PID_ROLL, txt_minKD);

	DDX_Control(pDX, IDC_EDIT_MAX_PID_YAW, txt_maxKP);
	DDX_Control(pDX, IDC_EDIT_MAX_PID_PITCH, txt_maxKI);
	DDX_Control(pDX, IDC_EDIT_MAX_PID_ROLL, txt_maxKD);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PID_KP, txt_currentKP);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PID_KI, txt_currentKI);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PID_KD, txt_currentKD);
	DDX_Control(pDX, IDC_COMBO_COM_PORT, cbb_comPort);
	DDX_Control(pDX, IDC_BUTTON_OPEN_PORT, btn_OpenCOMPort);
	DDX_Control(pDX, IDC_BUTTON_CLOSE_PORT, btn_CloseCOMPort);
	DDX_Control(pDX, IDC_BUTTON_SEND_COMMAND, btn_sendCommand);
	DDX_Control(pDX, IDC_EDIT_BAUDRATE, txt_baudRate);
	DDX_Control(pDX, IDC_EDIT_COMMAND, txt_command);
}

BEGIN_MESSAGE_MAP(CSTM32ControllerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_MIN_PID_YAW, &CSTM32ControllerDlg::OnEnChangeEditMinPidYaw)
	ON_BN_CLICKED(IDC_RADIO_MOTOR_YAW, &CSTM32ControllerDlg::OnBnClickedRadioMotorYaw)
	ON_BN_CLICKED(IDC_RADIO_MOTOR_PITCH, &CSTM32ControllerDlg::OnBnClickedRadioMotorPitch)
	ON_BN_CLICKED(IDC_RADIO_MOTOR_ROLL, &CSTM32ControllerDlg::OnBnClickedRadioMotorRoll)
	ON_BN_CLICKED(IDC_BUTTON_SEND_COMMAND, &CSTM32ControllerDlg::OnBnClickedButtonSendCommand)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PID_YAW, &CSTM32ControllerDlg::OnNMCustomdrawSliderPidYaw)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PID_PITCH, &CSTM32ControllerDlg::OnNMCustomdrawSliderPidPitch)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PID_ROLL, &CSTM32ControllerDlg::OnNMCustomdrawSliderPidRoll)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_PORT, &CSTM32ControllerDlg::OnBnClickedButtonOpenPort)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_PORT, &CSTM32ControllerDlg::OnBnClickedButtonClosePort)
	ON_EN_SETFOCUS(IDC_EDIT_COMMAND, &CSTM32ControllerDlg::OnEnSetfocusEditCommand)
	ON_EN_KILLFOCUS(IDC_EDIT_COMMAND, &CSTM32ControllerDlg::OnEnKillfocusEditCommand)
END_MESSAGE_MAP()


// CSTM32ControllerDlg message handlers

BOOL CSTM32ControllerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	sld_controlYaw.SetRangeMax(10000);
	sld_controlPitch.SetRangeMax(10000);
	sld_controlRoll.SetRangeMax(10000);

	//Default choose YAW motor:
	CButton* pButton = (CButton*)this->GetDlgItem(IDC_RADIO_MOTOR_YAW);
	pButton->SetFocus();
	pButton->SetCheck(true);

	//Init XML setting:
	m_settingXML.Init();

	//GetCurrentSetting from XML:
	m_controllerSetting = m_settingXML.GetControllSetting();
	UpdateCurrentMotorSetting();
	ReloadAllContents();

	//Load all COMport available:
	for (int i = 0; i < 10; i++)
	{
		CString strCOMPort;
		strCOMPort.Format(L"COM%d", i);
		cbb_comPort.AddString(strCOMPort);
	}
	//Select last COM port used:
	cbb_comPort.SetCurSel(m_controllerSetting->comPort.lastCOMPort);
	CString strBaudRate;
	strBaudRate.Format(L"%d", m_controllerSetting->comPort.baudRate);
	txt_baudRate.SetWindowTextW(strBaudRate);

	btn_OpenCOMPort.EnableWindow(TRUE);
	btn_CloseCOMPort.EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSTM32ControllerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSTM32ControllerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSTM32ControllerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSTM32ControllerDlg::OnEnChangeEditMinPidYaw()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CSTM32ControllerDlg::UpdateCurrentMotorSetting()
{
	if (!m_controllerSetting)
		return;

	switch (m_currentMotor)
	{
	case YAW_MOTOR:
		m_currentMotorSetting = &m_controllerSetting->yawMotor;
		break;
	case PITCH_MOTOR:
		m_currentMotorSetting = &m_controllerSetting->pitchMotor;
		break;
	case ROLL_MOTOR:
		m_currentMotorSetting = &m_controllerSetting->rollMotor;
		break;
	}
}

void CSTM32ControllerDlg::ReloadAllContents()
{
	if (!m_controllerSetting || !m_currentMotorSetting)
		return;

	m_settingXML.LoadSetting();
	CString strValue;

	//MinPID value:
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->minPIDValue.kP);
	txt_minKP.SetWindowTextW(strValue);
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->minPIDValue.kI);
	txt_minKI.SetWindowTextW(strValue);
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->minPIDValue.kD);
	txt_minKD.SetWindowTextW(strValue);

	//MaxPID value:
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->maxPIDValue.kP);
	txt_maxKP.SetWindowTextW(strValue);
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->maxPIDValue.kI);
	txt_maxKI.SetWindowTextW(strValue);
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->maxPIDValue.kD);
	txt_maxKD.SetWindowTextW(strValue);

	//CurrentPID value:
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->currentPIDValue.kP);
	txt_currentKP.SetWindowTextW(strValue);
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->currentPIDValue.kI);
	txt_currentKI.SetWindowTextW(strValue);
	strValue.Format(L"%.04f", (float)m_currentMotorSetting->currentPIDValue.kD);
	txt_currentKD.SetWindowTextW(strValue);

	//Calculator and update SliderBar:
	int yawIndex, pitchIndex, rollIndex;
	yawIndex = 10000 * (m_currentMotorSetting->currentPIDValue.kP - m_currentMotorSetting->minPIDValue.kP) / (m_currentMotorSetting->maxPIDValue.kP - m_currentMotorSetting->minPIDValue.kP);
	pitchIndex = 10000 * (m_currentMotorSetting->currentPIDValue.kI - m_currentMotorSetting->minPIDValue.kI) / (m_currentMotorSetting->maxPIDValue.kI - m_currentMotorSetting->minPIDValue.kI);
	rollIndex = 10000 * (m_currentMotorSetting->currentPIDValue.kD - m_currentMotorSetting->minPIDValue.kD) / (m_currentMotorSetting->maxPIDValue.kD - m_currentMotorSetting->minPIDValue.kD);
	
	sld_controlYaw.SetPos(yawIndex);
	sld_controlPitch.SetPos(pitchIndex);
	sld_controlRoll.SetPos(rollIndex);

	//Render refAngle, max speed:
	strValue.Format(L"%.02f", (float)m_controllerSetting->yawMotor.refAngle);
	txt_refAngleYaw.SetWindowTextW(strValue);
	strValue.Format(L"%d", (int)m_controllerSetting->yawMotor.maxSpeed);
	txt_maxSpeedYaw.SetWindowTextW(strValue);

	strValue.Format(L"%.02f", (float)m_controllerSetting->pitchMotor.refAngle);
	txt_refAnglePitch.SetWindowTextW(strValue);
	strValue.Format(L"%d", (int)m_controllerSetting->pitchMotor.maxSpeed);
	txt_maxSpeedPitch.SetWindowTextW(strValue);

	strValue.Format(L"%.02f", (float)m_controllerSetting->rollMotor.refAngle);
	txt_refAngleRoll.SetWindowTextW(strValue);
	strValue.Format(L"%d", (int)m_controllerSetting->rollMotor.maxSpeed);
	txt_maxSpeedRoll.SetWindowTextW(strValue);
}

void CSTM32ControllerDlg::OnBnClickedRadioMotorYaw()
{
	m_currentMotor = YAW_MOTOR;
	UpdateCurrentMotorSetting();
	ReloadAllContents();
}


void CSTM32ControllerDlg::OnBnClickedRadioMotorPitch()
{
	m_currentMotor = PITCH_MOTOR;
	UpdateCurrentMotorSetting();
	ReloadAllContents();
}


void CSTM32ControllerDlg::OnBnClickedRadioMotorRoll()
{
	m_currentMotor = ROLL_MOTOR;
	UpdateCurrentMotorSetting();
	ReloadAllContents();
}


void CSTM32ControllerDlg::OnBnClickedButtonSendCommand()
{
	bool ret;
	//Get all value need to save:
	GetAllSettingValue();
	//Save to xml:
	m_settingXML.SaveSettingFile();

	//Get command line:
	CString strCommand;
	txt_command.GetWindowTextW(strCommand);
	if (strCommand.GetLength())
	{
		CT2A ascii(strCommand);
		ret = m_serialCOM.Write(ascii);
		m_serialCOM.WriteChar(0x13);	
	}
	else
	{
		ret = m_stmCommunicate.SendData(m_currentMotor, m_currentMotorSetting);
	}
	if (!ret)
		PrintLog("Send command line error!\r\n");
}


void CSTM32ControllerDlg::OnNMCustomdrawSliderPidYaw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	if (m_currentMotorSetting)
	{
		int kpPos = sld_controlYaw.GetPos();
		CString strMinKP, strMaxKP;

		//Get current value:
		txt_minKP.GetWindowTextW(strMinKP);
		txt_maxKP.GetWindowTextW(strMaxKP);

		//Update Setting:
		m_currentMotorSetting->minPIDValue.kP = _tstof(strMinKP);
		m_currentMotorSetting->maxPIDValue.kP = _tstof(strMaxKP);
		m_currentMotorSetting->currentPIDValue.kP = m_currentMotorSetting->minPIDValue.kP +
			(m_currentMotorSetting->maxPIDValue.kP - m_currentMotorSetting->minPIDValue.kP) * kpPos / 10000.0;
		CString strValue;
		//CurrentPID value:
		strValue.Format(L"%.04f", (float)m_currentMotorSetting->currentPIDValue.kP);
		txt_currentKP.SetWindowTextW(strValue);
	}
	*pResult = 0;
}


void CSTM32ControllerDlg::OnNMCustomdrawSliderPidPitch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	if (m_currentMotorSetting)
	{
		int kiPos = sld_controlPitch.GetPos();
		CString strMinKI, strMaxKI;

		//Get current value:
		txt_minKI.GetWindowTextW(strMinKI);
		txt_maxKI.GetWindowTextW(strMaxKI);

		//Update Setting:
		m_currentMotorSetting->minPIDValue.kI = _tstof(strMinKI);
		m_currentMotorSetting->maxPIDValue.kI = _tstof(strMaxKI);
		m_currentMotorSetting->currentPIDValue.kI = m_currentMotorSetting->minPIDValue.kI +
			(m_currentMotorSetting->maxPIDValue.kI - m_currentMotorSetting->minPIDValue.kI) * kiPos / 10000.0;

		CString strValue;
		//CurrentPID value:
		strValue.Format(L"%.04f", (float)m_currentMotorSetting->currentPIDValue.kI);
		txt_currentKI.SetWindowTextW(strValue);
	}
	*pResult = 0;
}


void CSTM32ControllerDlg::OnNMCustomdrawSliderPidRoll(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	if (m_currentMotorSetting)
	{
		int kdPos = sld_controlRoll.GetPos();
		CString strMinKD, strMaxKD;

		//Get current value:
		txt_minKD.GetWindowTextW(strMinKD);
		txt_maxKD.GetWindowTextW(strMaxKD);

		//Update Setting:
		m_currentMotorSetting->minPIDValue.kD = _tstof(strMinKD);
		m_currentMotorSetting->maxPIDValue.kD = _tstof(strMaxKD);
		m_currentMotorSetting->currentPIDValue.kD = m_currentMotorSetting->minPIDValue.kD +
			(m_currentMotorSetting->maxPIDValue.kD - m_currentMotorSetting->minPIDValue.kD) * kdPos / 10000.0;

		CString strValue;
		//CurrentPID value:
		strValue.Format(L"%.04f", (float)m_currentMotorSetting->currentPIDValue.kD);
		txt_currentKD.SetWindowTextW(strValue);
	}
	*pResult = 0;
}

void CSTM32ControllerDlg::GetAllSettingValue()
{
	CString strValue;

	txt_refAngleYaw.GetWindowTextW(strValue);
	m_controllerSetting->yawMotor.refAngle = _tstof(strValue);
	txt_maxSpeedYaw.GetWindowTextW(strValue);
	m_controllerSetting->yawMotor.maxSpeed = _tstof(strValue);

	txt_refAnglePitch.GetWindowTextW(strValue);
	m_controllerSetting->pitchMotor.refAngle = _tstof(strValue);
	txt_maxSpeedPitch.GetWindowTextW(strValue);
	m_controllerSetting->pitchMotor.maxSpeed = _tstof(strValue);

	txt_refAngleRoll.GetWindowTextW(strValue);
	m_controllerSetting->rollMotor.refAngle = _tstof(strValue);
	txt_maxSpeedRoll.GetWindowTextW(strValue);
	m_controllerSetting->rollMotor.maxSpeed = _tstof(strValue);

}

void CSTM32ControllerDlg::OnBnClickedButtonOpenPort()
{
	//Open COM port:
	int comPortNo = cbb_comPort.GetCurSel();
	CString strBaudRateValue;
	txt_baudRate.GetWindowTextW(strBaudRateValue);

	m_serialCOM.SetBaudRate(_tstoi(strBaudRateValue));
	m_serialCOM.SetPortNumber(comPortNo);
	m_serialCOM.SetOwnerDelegate(this);

	char log[100];
	sprintf(log, "Opening serial port COM %ld... ", m_serialCOM.GetPortNumber());
	PrintLog(log);

	if (m_serialCOM.Open() == Serial_result_ok)
	{
		m_comPortIsOpened = TRUE;
		PrintLog(" SUCCESS!\r\n");
		//Start listen:
		m_serialCOM.Listen();

		m_controllerSetting->comPort.lastCOMPort = comPortNo;
		m_controllerSetting->comPort.baudRate = _tstoi(strBaudRateValue);
		m_settingXML.SaveSettingFile();	

		m_stmCommunicate.SetSerialCOM(&m_serialCOM);

		//Disable this button:
		btn_OpenCOMPort.EnableWindow(FALSE);
		btn_CloseCOMPort.EnableWindow(TRUE);
	}
	else
	{
		PrintLog(" FAILED!\r\n");
		btn_OpenCOMPort.EnableWindow(TRUE);
		btn_CloseCOMPort.EnableWindow(FALSE);
	}
}

void CSTM32ControllerDlg::PrintLog(char* strLog)
{
	CString currentLog;
	CString newLog(strLog);

	m_editLog.GetWindowTextW(currentLog);
	m_editLog.SetSel(currentLog.GetLength(), 0xffff);  //select position after last char in editbox
	m_editLog.ReplaceSel(newLog); // replace selection with new text
}

void CSTM32ControllerDlg::OnBnClickedButtonClosePort()
{
	if (m_comPortIsOpened)
	{
		m_serialCOM.Close();
		char log[100];
		sprintf(log, "Serial port COM %ld closed!\r\n", m_serialCOM.GetPortNumber());
		PrintLog(log);
		m_comPortIsOpened = FALSE;	//To exit GetLogData thread;

		btn_OpenCOMPort.EnableWindow(TRUE);
		btn_CloseCOMPort.EnableWindow(FALSE);
	}
	
}

BOOL CSTM32ControllerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (m_currentEditTextFocus == &txt_command)
			{
				//Send commandLine:
				OnBnClickedButtonSendCommand();
				return TRUE;
			}
		}
		if(pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}



void CSTM32ControllerDlg::OnEnSetfocusEditCommand()
{
	m_currentEditTextFocus = &txt_command;
}


void CSTM32ControllerDlg::OnEnKillfocusEditCommand()
{
	m_currentEditTextFocus = NULL;
}

void CSTM32ControllerDlg::OnSTM32LogReceived(char* strLog)
{
	//PrintLog(strLog);
	char* tagName = GetTagName(strLog);
	if (strcmp(tagName, "<RESPONSE>") == 0)
	{
		PrintResponseValue(strLog);
		m_waitResponseFlag = false;
	}
	else
	{
		PrintLog(strLog);
	}
}

#define MAX_TAG_LEN 30
char* CSTM32ControllerDlg::GetTagName(char* strLog)
{
	char strTag[MAX_TAG_LEN];
	int i = 0;
	for (int i = 0; i < MAX_TAG_LEN; i++)
	{
		strTag[i] = 0;
		if (strLog[i] && strLog[i] != ' ')
		{
			strTag[i] = strLog[i];
		}
	}
	return strTag;
}

void CSTM32ControllerDlg::PrintResponseValue(char* strLog)
{
	std::deque<char*> responseValue;
	char* pChar = strLog;
	char* strSegment = new char[MAX_TAG_LEN];
	memset(strSegment, 0, MAX_TAG_LEN);
	int i = 0;
	while (*pChar++)
	{
		if (*pChar == ' ' || *pChar == '\r\n')
		{
			responseValue.push_back(strSegment);
			strSegment = new char[MAX_TAG_LEN];
			memset(strSegment, 0, MAX_TAG_LEN);
			i = 0;
		}
		else
		{
			strSegment[i] = *pChar;
			i++;
		}
	}

	//Drop first tag data:
	//<TAG>-yaw-pitch-roll-PWM1-2-3<>
	CString strValue;
	float nValue;
	
	nValue = _ttof(CString(responseValue.at(1)));
	strValue.Format(L"%.02f", nValue);
	m_txtYaw.SetWindowTextW(strValue);

	nValue = _ttof(CString(responseValue.at(2)));
	strValue.Format(L"%.02f", nValue);
	m_txtPitch.SetWindowTextW(strValue);

	nValue = _ttof(CString(responseValue.at(3)));
	strValue.Format(L"%.02f", nValue);
	m_txtRoll.SetWindowTextW(strValue);

	//Clear deque:
	while (responseValue.size())
	{
		strSegment = responseValue.front();
		responseValue.pop_front();
		delete[] strSegment;
	}

}
