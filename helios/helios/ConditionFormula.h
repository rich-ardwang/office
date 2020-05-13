
#pragma once
#include <string>
#include "lava_container.h"

#ifndef CDH_FUNCTION_CODE
#define CDH_FUNCTION_CODE 1005
#endif

typedef uint32_t hash_value_type;
struct ScContribData
{
    std::string asset_class;
    std::pair<uint32_t, std::string> mode;
    std::vector<std::string> fids;
    std::vector<double>	vals;
    hash_value_type	hash_val;
};

class ConditionFormula
{
public:
	bool SendData() {};

protected:
	typedef std::wstring prompt_type;
	struct CELL_POS
	{
		MSExcel::_Worksheet* sheet_ptr;
		long col;
		long row;
		bool operator == (const CELL_POS& rhs) const
		{
			return sheet_ptr == rhs.sheet_ptr && col == rhs.col && row == rhs.row;
		}
		//bool operator != (const CELL_POS& rhs) const
		//{
		//	return !(this == rhs);
		//}
		bool operator < (const CELL_POS& rhs) const
		{
			if (sheet_ptr < rhs.sheet_ptr)
				return true;
			else if (sheet_ptr == rhs.sheet_ptr)
				return (col < rhs.col) ? true : (col == rhs.col) ? (row < rhs.row) : false;
			else
				return false;
		}
	};

protected:
	// 
	// if same formula exists in different cells, then they will have same hash value.
	//		1. same position(that is same col and row), but different sheet/book.
	//		2. same sheet/book, but different position(that is same col and row).
	//		3. neither position nor sheet/book.
	// in this situation, we should show same prompt for these different cells.
	//
	lava::utils::map_lv<hash_value_type, prompt_type>	m_hash_data;
	lava::utils::map_lv<CELL_POS, hash_value_type>		m_pos_to_hash;
};