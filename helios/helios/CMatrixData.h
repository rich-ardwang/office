//
//  CMatrixData.h
//
//  Created by Colin on 2020-03-06.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#pragma once
#include <string>
#include "lava_base.h"
#include "SafeArray.h"

#ifndef POSITION
#define POSITION POINT
#else
#undef POSITION
#define POSITION POINT
#endif

//
// NOTE: in this class, we use POSITION as type of coord, so POSITION::x indicates start_col,
// POSITION::y indicates start_row. we treat size in the same way, so SIZE::cx indicates cols,
// SIZE::cy indicates rows.
//
class CMatrixData
{
public:
	CMatrixData(long start_row, long start_col, long rows, long cols);
	CMatrixData(POSITION coords, SIZE size);
	CMatrixData(Helios::OleSafeArray& src);
	~CMatrixData();

	void GetPos(__lv_out long& start_row, __lv_out long& start_col);
	POSITION GetPos();

	void GetSize(__lv_out long& rows, __lv_out long& cols);
	SIZE GetSize();

	void ReSize(long rows, long cols);
	void ReSize(SIZE size);

	void SetFont(std::wstring font_name, long font_size, bool is_bold);
	void GetFont(std::wstring& font_name, long& font_size, bool& is_bold);

	void SetElem(long row, long col, ATL::CComVariant& val);
	ATL::CComVariant GetElem(long row, long col);

	void Transpose();

	inline Helios::OleSafeArray GetData()
	{
		return m_data_block;
	}

	inline int GetAlignment()
	{
		return m_alignment;
	}

	inline void SetAlignment(int alignment)
	{
		m_alignment = alignment;
	}

	inline void SetFormat(std::string& format)
	{
		m_data_format = format;
	}

	inline std::string GetFormat()
	{
		return m_data_format;
	}

private:
	Helios::OleSafeArray	m_data_block;
	long					m_start_row;
	long					m_start_col;
	std::wstring			m_font_name;
	long					m_font_size;
	bool					m_bold_font;
	int						m_alignment;
	std::string				m_data_format;
};

