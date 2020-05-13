
#pragma once
#include "ConditionFormula.h"

class CondChangedFormula : public ConditionFormula
{
public:
	CondChangedFormula() : m_prev_sent(true) {}

	ATL::CComVariant CacheFormula(std::string& asset_class, std::string& mode, std::string& format,
		std::vector<std::string>& fids, std::vector<double>& vals, int count);

	bool SendData(__lv_in IDispatch* Sh);
	void SheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target);
	void ManualSend();

private:
	bool CachePosHashValue(CELL_POS& pos, hash_value_type& new_hash_value); // return true: need collect it

private:
	lava::utils::vector_lv<ScContribData>	m_data_to_send;
	ATL::CComPtr<Range>						m_single_cell;
	bool									m_prev_sent;
};

