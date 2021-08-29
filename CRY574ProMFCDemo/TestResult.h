#pragma once
#include "afx.h"
class CTestResult :
	public CObject
{
	DECLARE_SERIAL(CTestResult)

public:
	CTestResult(void);
	virtual ~CTestResult(void);

	CTestResult(int ok_cnt, int faled_cnt);

	CTestResult (const CTestResult& result);
	CTestResult& operator=(const CTestResult& result);


	int m_ok_count;
	int m_failed_count;
	virtual void Serialize(CArchive& ar);
};

