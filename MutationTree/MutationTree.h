
// MutationTree.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "CSVFile.h"
#include "VAF_CSVTree_struct.h"
#include "TreeFactory.h"
#include "TreeDiagram.h"
#include "Constraints.h"




// CMutationTreeApp:
// See MutationTree.cpp for the implementation of this class
//

class CMutationTreeApp : public CWinApp
{
public:
	CMutationTreeApp();
	CString curDir;
	CSVFile csvFile;
	std::vector<VAF>  VAFVector;
	TreeFactory  treeFactory;
	TreeDiagram treeDiagram;
	Constraints constraint;
	bool stopComputation;
	

	

// Overrides
public:
	virtual BOOL InitInstance();


	

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMutationTreeApp theApp;