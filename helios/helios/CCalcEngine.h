#pragma once
#include <string>
#include <memory>
#include "lava_container.h"
#include "ScContribFormula.h"

class CCalcEngine
{
public:
	enum CodeType { CT_BOND = 1, CT_STOCK };
	CCalcEngine();
	~CCalcEngine();

	ATL::CComVariant SingleCalc(char* func_name, CodeType code_type, long& param_count, ATL::CComVariant** params);
	void OnCalculate();
	void OnAfterCalculate();
	void OnStopCalculate();

private:
	std::shared_ptr<ScContribFormula>	m_ScContribHandler;
};

