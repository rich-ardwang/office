
#pragma once
#include "CondChangedFormula.h"
#include "CondFreqFormula.h"

class ScContribFormula
{
	enum UPLOAD_MODE { um_none = 0, um_changed = 1, um_frequency = 2 };
	struct CONDITION
	{
		UPLOAD_MODE		m_upload_mode;
		std::string		m_ft;
		uint32_t		m_fq;
	};

public:
	ScContribFormula() {};
	~ScContribFormula() {};

	void OnCalculate(__lv_in IDispatch* Sh);
	void OnSheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target);
	void OnAfterCalculate();
	void OnStopCalculate();
	void OnManualSend();

	ATL::CComVariant Calc(long& param_count, ATL::CComVariant** params);

private:
	std::string check_mode(std::string str);
	bool parse_condition(std::string str, CONDITION& ret);

private:
	CondChangedFormula	m_cond_changed_formula;
	CondFreqFormula m_cond_freq_formula;
};

