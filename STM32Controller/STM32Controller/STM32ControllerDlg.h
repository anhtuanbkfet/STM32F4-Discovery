
// STM32ControllerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "SettingXML/SettingXML.h"
#include "SerialCOM/SerialCOM.h"
#include "MotorController\STMComunicate.h"
#include "afxcmn.h"
#include "CMenuEdit.h"

// CSTM32ControllerDlg dialog
class CSTM32ControllerDlg : public CDialogEx, ISerialSTM32Delegate
{
// Construction
public:
	CSTM32ControllerDlg(CWnd* pParent = NULL);	// standard constructor

	SerialCOM* GetSerialCOM(){ return &m_serialCOM; }

// Dialog Data
	enum { IDD = IDD_STM32CONTROLLER_DIALOG };

// Implementation
protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnSTM32LogReceived(char* strLog);

private:
	void ReloadAllContents();
	void UpdateCurrentMotorSetting();
	void GetAllSettingValue();
	void PrintLog(char* strLog);
	char* GetTagName(char* strLog);
	void PrintResponseValue(char* strLog);

private:
	//Setting XML:
	SettingXML			m_settingXML;
	ControllerSetting*	m_controllerSetting;
	MOTOR				m_currentMotor;
	MotorSetting*		m_currentMotorSetting;

	//SerialCOM:
	SerialCOM			m_serialCOM;
	char				m_character;
	bool				m_comPortIsOpened;
	CWnd*				m_currentEditTextFocus;

	//STM Communicate:
	STM32Communicate	m_stmCommunicate;
	BOOL				m_waitResponseFlag;
public:
	afx_msg void OnEnChangeEditMinPidYaw();
	afx_msg void OnBnClickedRadioMotorYaw();
	afx_msg void OnBnClickedRadioMotorPitch();
	afx_msg void OnBnClickedRadioMotorRoll();
	afx_msg void OnBnClickedButtonSendCommand();
	afx_msg void OnNMCustomdrawSliderPidYaw(NMHDR *pNMHDR, LRESULT *pResult);

	CMenuEdit m_editLog;
	CEdit m_txtYaw;
	CEdit m_txtPitch;
	CEdit m_txtRoll;

	CEdit txt_refAngleYaw;
	CEdit txt_refAnglePitch;
	CEdit txt_refAngleRoll;

	CEdit txt_maxSpeedYaw;
	CEdit txt_maxSpeedPitch;
	CEdit txt_maxSpeedRoll;

	CSliderCtrl sld_controlYaw;
	CSliderCtrl sld_controlPitch;
	CSliderCtrl sld_controlRoll;

	CEdit txt_minKP;
	CEdit txt_minKI;
	CEdit txt_minKD;

	CEdit txt_maxKP;
	CEdit txt_maxKI;
	CEdit txt_maxKD;
	CStatic txt_currentKP;
	CStatic txt_currentKI;
	CStatic txt_currentKD;
	
	afx_msg void OnNMCustomdrawSliderPidPitch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderPidRoll(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox cbb_comPort;
	CButton btn_OpenCOMPort;
	CButton btn_CloseCOMPort;
	CButton btn_sendCommand;
	CEdit txt_baudRate;
	afx_msg void OnBnClickedButtonOpenPort();
	afx_msg void OnBnClickedButtonClosePort();
	CEdit txt_command;
	afx_msg void OnEnSetfocusEditCommand();
	afx_msg void OnEnKillfocusEditCommand();
};
