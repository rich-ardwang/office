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
	void OnCalculate(__lv_in IDispatch* Sh);
	void OnSheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target);
	void OnAfterCalculate();
	void OnStopCalculate();

	void OnDataArrived();
	void OnManualSend();

private:
	std::shared_ptr<ScContribFormula>	m_ScContribHandler;
};

