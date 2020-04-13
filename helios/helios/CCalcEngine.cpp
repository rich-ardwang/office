#include "stdafx.h"
#include "CCalcEngine.h"
#include "SumsAddin.h"
#include "SafeArray.h"

using namespace Helios;
using namespace ATL;

CCalcEngine::CCalcEngine()
{
	m_ScContribHandler = std::make_shared<ScContribFormula>();
}

CCalcEngine::~CCalcEngine()
{
	m_ScContribHandler = nullptr;
}

CComVariant CCalcEngine::SingleCalc(char* func_name, CodeType code_type, long& param_count, CComVariant** params)
{
	if ( _stricmp(func_name, "ScContrib") == 0 )
		return m_ScContribHandler->Calc(param_count, params);
	else if (0)
		return CComVariant("Null");
	else
		return CComVariant("Null");
}

void CCalcEngine::OnCalculate()
{
	m_ScContribHandler->OnCalculate();
}

void CCalcEngine::OnAfterCalculate()
{
	m_ScContribHandler->OnAfterCalculate();
}

void CCalcEngine::OnStopCalculate()
{
	m_ScContribHandler->OnStopCalculate();
}