#pragma once

#include "afxdialogex.h"
#include "Resource.h"
#include "afxwin.h"

class ConstraintsDlg : public CDialogEx
{
public:
	ConstraintsDlg(CWnd* pParent = NULL);// standard constructor
	~ConstraintsDlg();

	// Dialog Data
	enum { IDD = IDD_CONSTRAINTS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CButton m_btnOK;
	CButton m_btnCancel;
	CEdit m_editTriConstraintEps; //Triangular Constraint Epsilon
	CEdit m_editRootConstraintEps;//Root Constraint Epsilon
	afx_msg void OnEnChangeEditTriEpsilon();
	afx_msg void OnBnClickedRadiobfs();
	afx_msg void OnBnClickedRadiodfs();
	
	afx_msg void OnEnChangeEditRootEpsilon();
	CEdit m_editLeastRichness;
	afx_msg void OnEnChangeLeastRichness();
};

