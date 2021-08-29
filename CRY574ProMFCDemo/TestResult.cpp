#include "stdafx.h"
#include "TestResult.h"


IMPLEMENT_SERIAL(CTestResult, CObject, 1);

CTestResult::CTestResult(void)
{
	m_ok_count = 0;
	m_failed_count = 0;
}

CTestResult::CTestResult(int ok_cnt, int failed_cnt)
{
	m_ok_count = ok_cnt;
	m_failed_count = failed_cnt;
}


CTestResult::~CTestResult(void)
{
}



CTestResult::CTestResult(const CTestResult& result)
{
	m_ok_count = result.m_ok_count;
	m_failed_count = result.m_failed_count;
}


CTestResult& CTestResult::operator=(const CTestResult& result)
{
	if (&result != this)
	{
		m_ok_count = result.m_ok_count;
		m_failed_count = result.m_failed_count;
	}

	return *this;
}

void CTestResult::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_ok_count;
		ar << m_failed_count;
	}
	else
	{	// loading code
		ar >> m_ok_count;
		ar >> m_failed_count;
	}

}
