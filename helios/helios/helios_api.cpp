//
//  helios_api.cpp
//
//  Created by Colin on 2020-03-08.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#include "stdafx.h"
#include "SumsAddin.h"

#pragma warning(disable:4190)
using namespace ATL;

STDAPI_(CComVariant) SFunc( char** func_name, long& code_type, long& param_count,
							CComVariant& param1, CComVariant& param2, CComVariant& param3,
							CComVariant& param4, CComVariant& param5, CComVariant& param6,
							CComVariant& param7, CComVariant& param8, CComVariant& param9 )
{
	CComVariant* ParamArray[9] = { &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9 };
	CComVariant RetVal = CSumsAddin::GetAddin()->GetDoc()->SingleCalc(*func_name, (CCalcEngine::CodeType)code_type, param_count, ParamArray);
	return RetVal;
}