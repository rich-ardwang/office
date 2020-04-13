//
//  CMatrixData.cpp
//
//  Created by Colin on 2020-03-06.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#include "stdafx.h"
#include "CMatrixData.h"

using namespace ATL;
using namespace Helios;

CMatrixData::CMatrixData(long start_row, long start_col, long rows, long cols)
	: m_start_row(start_row), m_start_col(start_col), m_bold_font(false)
{

}

CMatrixData::CMatrixData(POSITION coords, SIZE size)
	: m_start_row(coords.y), m_start_col(coords.x)
{

}

CMatrixData::CMatrixData(Helios::OleSafeArray& src)
{

}

CMatrixData::~CMatrixData()
{

}

void CMatrixData::GetPos(__lv_out long& start_row, __lv_out long& start_col)
{
	start_row = m_start_row;
	start_col = m_start_col;
}

POSITION CMatrixData::GetPos()
{
	POSITION pos = {0};
	GetPos(pos.y, pos.x);
	return pos;
}

void CMatrixData::GetSize(__lv_out long& rows, __lv_out long& cols)
{
	long row_upper;
	m_data_block.GetUBound(1, &row_upper);
	rows = row_upper - m_start_row + 1;

	long col_upper;
	m_data_block.GetUBound(2, &col_upper);
	cols = col_upper - m_start_col + 1;
}

SIZE CMatrixData::GetSize()
{
	SIZE size = { 0 };
	GetSize(size.cy, size.cx);
	return size;
}

void CMatrixData::ReSize(long rows, long cols)
{
	// Keep the coordinate, only change size.
	SAFEARRAYBOUND bounds[] = { { rows, m_start_row }, { cols, m_start_col } };
	m_data_block.Destroy();
	m_data_block.Create(VT_VARIANT, 2, bounds);
}

void CMatrixData::ReSize(SIZE size)
{
	ReSize(size.cy, size.cx);
}

void CMatrixData::SetFont(std::wstring font_name, long font_size, bool is_bold)
{
	if ( m_font_name != font_name )
		m_font_name = font_name;

	m_font_size = font_size;
	m_bold_font = is_bold;
}

void CMatrixData::GetFont(std::wstring& font_name, long& font_size, bool& is_bold)
{
	font_name = m_font_name;
	font_size = m_font_size;
	is_bold = m_bold_font;
}

void CMatrixData::SetElem(long row, long col, ATL::CComVariant& val)
{
	long pos[] = { m_start_row + row, m_start_col + col };
	m_data_block.PutElement(pos, &val);
}

CComVariant CMatrixData::GetElem(long row, long col)
{
	CComVariant val;
	long pos[] = { m_start_row + row, m_start_col + col };
	m_data_block.GetElement(pos, &val);
	return val;
}

void CMatrixData::Transpose()
{
	long row_upper;
	m_data_block.GetUBound(1, &row_upper);

	long col_upper;
	m_data_block.GetUBound(2, &col_upper);

	//
	// 1. Create two-dimension safe array with VARIANT type, whose size is
	//    rows * cols, indices starting from m_start_row and m_start_col.
	// 2. Set value to every element of this new safe array.
	//
	OleSafeArray des_array;
	SAFEARRAYBOUND bounds[] = { { col_upper - m_start_col + 1, m_start_col }, { row_upper - m_start_row + 1, m_start_row } };
	des_array.Create(VT_VARIANT, 2, bounds);

	for ( long row = m_start_row; row <= row_upper; row++ )
	{
		for ( long col = m_start_col; col <= col_upper; col++ )
		{
			CComVariant val;
			long src_pos[] = { row, col };
			m_data_block.GetElement(src_pos, &val);

			long des_pos[] = { col, row };
			des_array.PutElement(des_pos, &val);
		}
	}

	//
	// 1. Swap start col and row
	// 2. Update data block by using new one.
	// 3. Free resource.
	//
	std::swap(m_start_row, m_start_col);
	m_data_block = des_array;
	des_array.Destroy();
}