#pragma once
#include "afxdialogex.h"
#include "Resource.h"
#include "afxshelltreectrl.h"
#include "afxwin.h"
#include "afxshelllistctrl.h"


class OpenCsvDlg : public CDialogEx
{
public:
	
	OpenCsvDlg(CWnd* pParent = NULL);	// standard constructor
	~OpenCsvDlg();

	// Dialog Data
	enum { IDD = IDD_OPENCSV_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	
public:
	CMFCShellTreeCtrl m_treePath;
	CButton m_btnOK;
	CButton m_btnCancel;
	CMFCShellListCtrl m_fileList;
	CComboBox m_comboxStatus;
	afx_msg void OnBnClickedOk();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	void filterFileDisplay();
	afx_msg void OnTvnSelchangedMfcshelltree1(NMHDR *pNMHDR, LRESULT *pResult);
};
