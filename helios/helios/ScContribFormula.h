#pragma once
#include <string>
#include "lava_container.h"

typedef uint32_t	hash_val_type;

struct ScContribData
{
    std::string asset_class;
    std::pair<uint32_t, std::string> mode;
    std::vector<std::string> fids;
    std::vector<double>		 vals;
	hash_val_type	hash_val;
};

class ScContribFormula
{
	struct CELL_POS
	{
		long col;
		long row;
		bool operator == (const CELL_POS& rhs) const
		{
			return col == rhs.col && row == rhs.row;
		}
		//bool operator != (const CELL_POS& rhs) const
		//{
		//	return !(this == rhs);
		//}
		bool operator < (const CELL_POS& rhs) const
		{
			return (col < rhs.col) ? true : (col == rhs.col) ? (row < rhs.row) : false;
		}
	};

public:
	ScContribFormula();
	~ScContribFormula();

	void OnCalculate();
	void OnAfterCalculate();
	void OnStopCalculate();

	ATL::CComVariant Calc(long& param_count, ATL::CComVariant** params);

private:
	ATL::CComVariant CacheFormula(std::string& asset_class, std::string& mode, std::string& format, 
								std::vector<std::string>& fids, std::vector<double>& vals, int count);

	static std::string standard_mode(std::string str);
	static std::string standard_format(std::string str);

private:
	lava::utils::vector_lv<ScContribData>			m_data_to_send;
	lava::utils::map_lv<hash_val_type, bool>		m_hash_data;
	lava::utils::map_lv<CELL_POS, hash_val_type>	m_pos_hash;
	ATL::CComPtr<Range>								m_single_cell;
	bool	m_prev_sent;
};

