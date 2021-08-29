#pragma once
#include "afx.h"
class CTestResult :
	public CObject
{
	DECLARE_SERIAL(CTestResult)

public:
	CTestResult(void);
	virtual ~CTestResult(void);

	CTestResult(int ok_cnt, int failed_cnt, int all_ok_cnt, int all_failed_cnt);

	CTestResult (const CTestResult& result);
	CTestResult& operator=(const CTestResult& result);

	int m_ok_count;			// ���һ�β���OK����
	int m_failed_count;

	int m_total_failed_count;		// �ܲ���OK������
	int m_total_ok_count;

	virtual void Serialize(CArchive& ar);
};

