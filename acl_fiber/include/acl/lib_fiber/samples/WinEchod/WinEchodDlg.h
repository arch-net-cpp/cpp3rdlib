
// WinEchodDlg.h : ͷ�ļ�
//

#pragma once

// CWinEchodDlg �Ի���
class CWinEchodDlg : public CDialogEx
{
// ����
public:
	CWinEchodDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CWinEchodDlg();

// �Ի�������
	enum { IDD = IDD_WINECHOD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenDos();
private:
	FILE* m_dosFp;
	UINT m_listenPort;
	CString m_listenIP;
	acl::string m_listenAddr;
	acl::server_socket m_listen;
	acl::fiber* m_fiberListen;
public:
	afx_msg void OnBnClickedListen();
	afx_msg void OnBnClickedCreateTimer();
	afx_msg void OnBnClickedConnect();
private:
	UINT m_cocurrent;
	UINT m_count;
	void Uni2Str(const CString& in, acl::string& out);
	void InitFiber();

public:
	void OnFiberConnectExit(void);
	afx_msg void OnBnClickedOk();
};
