//
//  SafeArray.cpp
//
//  Created by Colin on 2020-03-06.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#include "stdafx.h"
#include "SafeArray.h"
#include "lava_utils_api.h"

using namespace Helios;
using namespace ATL;

OleSafeArray::OleSafeArray(const SAFEARRAY& saSrc, VARTYPE vtSrc)
{
	SafeArrayInit();
	vt = (VARTYPE)(vtSrc | VT_ARRAY);
	CheckError(::SafeArrayCopy((LPSAFEARRAY)&saSrc, &parray));
	m_dwDims = GetDim();
	m_dwElementSize = GetElemSize();
}

OleSafeArray::OleSafeArray(const SAFEARRAY* pSrc, VARTYPE vtSrc)
{
	SafeArrayInit();
	vt = (VARTYPE)(vtSrc | VT_ARRAY);
	CheckError(::SafeArrayCopy((LPSAFEARRAY)pSrc, &parray));
	m_dwDims = GetDim();
	m_dwElementSize = GetElemSize();
}

OleSafeArray::OleSafeArray(const OleSafeArray& saSrc)
{
	SafeArrayInit();
	*this = saSrc;
	m_dwDims = GetDim();
	m_dwElementSize = GetElemSize();
}

OleSafeArray::OleSafeArray(const VARIANT& varSrc)
{
	SafeArrayInit();
	*this = varSrc;
	m_dwDims = GetDim();
	m_dwElementSize = GetElemSize();
}

OleSafeArray::OleSafeArray(const CComVariant& varSrc)
{
	SafeArrayInit();
	*this = varSrc;
	m_dwDims = GetDim();
	m_dwElementSize = GetElemSize();
}

OleSafeArray::OleSafeArray(const VARIANT* pSrc)
{
	SafeArrayInit();
	*this = pSrc;
	m_dwDims = GetDim();
	m_dwElementSize = GetElemSize();
}

// Operations
void OleSafeArray::Attach(VARIANT& varSrc)
{
	ATLASSERT(varSrc.vt & VT_ARRAY);

	if ( !(varSrc.vt & VT_ARRAY) )
		throw std::runtime_error("invalid argument: type error.");

	// Free up previous safe array if necessary
	Clear();

	// give control of data to OleSafeArray 
	::memcpy_s(this, sizeof(VARIANT), &varSrc, sizeof(varSrc));
	varSrc.vt = VT_EMPTY;
}

VARIANT OleSafeArray::Detach()
{
	VARIANT varResult = *this;
	vt = VT_EMPTY;
	return varResult;
}

//void OleSafeArray::GetByteArray(CByteArray& bytes)
//{
//	LPVOID pSrc;
//	LPVOID pDest;
//	HRESULT hResult;
//	ULONG nDim;
//	LONG iLowerBound;
//	LONG iUpperBound;
//	LONG nElements;
//	ULONG nBytes;
//
//	ASSERT(V_ISARRAY(this));
//
//	hResult = ::SafeArrayAccessData(V_ARRAY(this), &pSrc);
//	AfxCheckError(hResult);
//
//	nDim = ::SafeArrayGetDim(V_ARRAY(this));
//	ASSERT((nDim == 0) || (nDim == 1));
//
//	if (nDim == 1)
//	{
//		::SafeArrayGetLBound(V_ARRAY(this), 1, &iLowerBound);
//		::SafeArrayGetUBound(V_ARRAY(this), 1, &iUpperBound);
//		nElements = (iUpperBound - iLowerBound) + 1;
//		nBytes = nElements * ::SafeArrayGetElemsize(V_ARRAY(this));
//		bytes.SetSize(nBytes);
//		pDest = bytes.GetData();
//		Checked::memcpy_s(pDest, nBytes, pSrc, nBytes);
//	}
//	else
//	{
//		bytes.SetSize(0);
//	}
//
//	::SafeArrayUnaccessData(V_ARRAY(this));
//}

// Assignment operators
OleSafeArray& OleSafeArray::operator=(const OleSafeArray& saSrc)
{
	ATLASSERT(saSrc.vt & VT_ARRAY);

	if ( !(saSrc.vt & VT_ARRAY) )
		throw std::runtime_error("invalid argument: type error.");

	CheckError(::VariantCopy(this, (LPVARIANT)&saSrc));
	return *this;
}

OleSafeArray& OleSafeArray::operator=(const VARIANT& varSrc)
{
	ATLASSERT(varSrc.vt & VT_ARRAY);

	if ( !(varSrc.vt & VT_ARRAY) )
		throw std::runtime_error("invalid argument: type error.");

	CheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
	return *this;
}

OleSafeArray& OleSafeArray::operator=(const VARIANT* pSrc)
{
	ATLASSERT(pSrc->vt & VT_ARRAY);

	if (!(pSrc->vt & VT_ARRAY))
		throw std::runtime_error("invalid argument: type error.");

	CheckError(::VariantCopy(this, (LPVARIANT)pSrc));
	return *this;
}

OleSafeArray& OleSafeArray::operator=(const CComVariant& varSrc)
{
	ATLASSERT(varSrc.vt & VT_ARRAY);

	if (!(varSrc.vt & VT_ARRAY))
		throw std::runtime_error("invalid argument: type error.");

	CheckError(::VariantCopy(this, (LPVARIANT)&varSrc));
	return *this;
}

// Comparison operators
BOOL OleSafeArray::operator==(const SAFEARRAY& saSrc) const
{
	return CompareSafeArrays(parray, (LPSAFEARRAY)&saSrc);
}

BOOL OleSafeArray::operator==(const SAFEARRAY* pSrc) const
{
	return CompareSafeArrays(parray, (LPSAFEARRAY)pSrc);
}

BOOL OleSafeArray::operator==(const OleSafeArray& saSrc) const
{
	if (vt != saSrc.vt)
		return FALSE;

	return CompareSafeArrays(parray, saSrc.parray);
}

BOOL OleSafeArray::operator==(const VARIANT& varSrc) const
{
	if (vt != varSrc.vt)
		return FALSE;

	return CompareSafeArrays(parray, varSrc.parray);
}

BOOL OleSafeArray::operator==(const VARIANT* pSrc) const
{
	if (vt != pSrc->vt)
		return FALSE;

	return CompareSafeArrays(parray, pSrc->parray);
}

BOOL OleSafeArray::operator==(const CComVariant& varSrc) const
{
	if (vt != varSrc.vt)
		return FALSE;

	return CompareSafeArrays(parray, varSrc.parray);
}

void OleSafeArray::CreateOneDim(VARTYPE vtSrc, DWORD dwElements, const void* pvSrcData, long nLBound)
{
	if (!(dwElements > 0))
		throw std::runtime_error("invalid argument: dwElements.");

	// Setup the bounds and create the array
	SAFEARRAYBOUND rgsabound;
	rgsabound.cElements = dwElements;
	rgsabound.lLbound = nLBound;
	Create(vtSrc, 1, &rgsabound);

	// Copy over the data if neccessary
	if (pvSrcData != NULL)
	{
		void* pvDestData;
		AccessData(&pvDestData);

		unsigned __int64 tmp_64 = static_cast<unsigned __int64>(GetElemSize())* static_cast<unsigned __int64>(dwElements);
		if ( tmp_64 > INT_MAX ) // no overflow ENSURE  
			throw std::runtime_error("element count too many");

		::memcpy_s(pvDestData, static_cast<size_t>(tmp_64), pvSrcData, static_cast<size_t>(tmp_64));
		UnaccessData();
	}
}

DWORD OleSafeArray::GetOneDimSize()
{
	if ( GetDim() != 1 )
		throw std::runtime_error("size not one-dimension.");

	long nUBound, nLBound;
	GetUBound(1, &nUBound);
	GetLBound(1, &nLBound);

	return nUBound + 1 - nLBound;
}

void OleSafeArray::ResizeOneDim(DWORD dwElements)
{
	ATLASSERT(GetDim() == 1);
	if ( !(GetDim() == 1) )
		throw std::runtime_error("size not one-dimension.");

	SAFEARRAYBOUND rgsabound;
	rgsabound.cElements = dwElements;
	rgsabound.lLbound = 0;

	Redim(&rgsabound);
}

void OleSafeArray::Create(VARTYPE vtSrc, DWORD dwDims, DWORD* rgElements)
{
	ATLASSERT(rgElements != NULL);

	if ( rgElements == NULL )
		throw std::runtime_error("invalid parameter: rgElements.");

	// Allocate and fill proxy array of bounds (with lower bound of zero)
	SAFEARRAYBOUND* rgsaBounds = new SAFEARRAYBOUND[dwDims];
	for (DWORD dwIndex = 0; dwIndex < dwDims; dwIndex++)
	{
		// Assume lower bound is 0 and fill in element count
		rgsaBounds[dwIndex].lLbound = 0;
		rgsaBounds[dwIndex].cElements = rgElements[dwIndex];
	}

	try
	{
		Create(vtSrc, dwDims, rgsaBounds);
	}
	catch(std::exception& e)
	{
		// Must free up memory
		delete[] rgsaBounds;
		log_error(helios_module, "%s", e.what());
	}
	delete[] rgsaBounds;
}

void OleSafeArray::Create(VARTYPE vtSrc, DWORD dwDims, SAFEARRAYBOUND* rgsabound)
{
	ATLASSERT(dwDims > 0);
	ATLASSERT(rgsabound != NULL);

	// Validate the VARTYPE for SafeArrayCreate call
	ATLASSERT(!(vtSrc & VT_ARRAY));
	ATLASSERT(!(vtSrc & VT_BYREF));
	ATLASSERT(!(vtSrc & VT_VECTOR));
	ATLASSERT(vtSrc != VT_EMPTY);
	ATLASSERT(vtSrc != VT_NULL);

	if (dwDims == 0 || rgsabound == NULL || (vtSrc & VT_ARRAY) || (vtSrc & VT_BYREF)
		|| (vtSrc & VT_VECTOR) || vtSrc == VT_EMPTY || vtSrc == VT_NULL )
	{
		throw std::runtime_error("invalid parameter.");
	}

	// Free up old safe array if necessary
	Clear();

	parray = ::SafeArrayCreate(vtSrc, dwDims, rgsabound);
	if ( parray == NULL )
		throw std::runtime_error("error to call SafeArrayCreate.");

	vt = unsigned short(vtSrc | VT_ARRAY);
	m_dwDims = dwDims;
	m_dwElementSize = GetElemSize();
}

void OleSafeArray::AccessData(void** ppvData)
{
	CheckError(::SafeArrayAccessData(parray, ppvData));
}

void OleSafeArray::UnaccessData()
{
	CheckError(::SafeArrayUnaccessData(parray));
}

void OleSafeArray::AllocData()
{
	CheckError(::SafeArrayAllocData(parray));
}

void OleSafeArray::AllocDescriptor(DWORD dwDims)
{
	CheckError(::SafeArrayAllocDescriptor(dwDims, &parray));
}

void OleSafeArray::Copy(LPSAFEARRAY* ppsa)
{
	CheckError(::SafeArrayCopy(parray, ppsa));
}

void OleSafeArray::GetLBound(DWORD dwDim, long* pLbound)
{
	CheckError(::SafeArrayGetLBound(parray, dwDim, pLbound));
}

void OleSafeArray::GetUBound(DWORD dwDim, long* pUbound)
{
	CheckError(::SafeArrayGetUBound(parray, dwDim, pUbound));
}

void OleSafeArray::GetElement(long* rgIndices, void* pvData)
{
	CheckError(::SafeArrayGetElement(parray, rgIndices, pvData));
}

void OleSafeArray::PtrOfIndex(long* rgIndices, void** ppvData)
{
	CheckError(::SafeArrayPtrOfIndex(parray, rgIndices, ppvData));
}

void OleSafeArray::PutElement(long* rgIndices, void* pvData)
{
	CheckError(::SafeArrayPutElement(parray, rgIndices, pvData));
}

void OleSafeArray::Redim(SAFEARRAYBOUND* psaboundNew)
{
	CheckError(::SafeArrayRedim(parray, psaboundNew));
}

void OleSafeArray::Lock()
{
	CheckError(::SafeArrayLock(parray));
}

void OleSafeArray::Unlock()
{
	CheckError(::SafeArrayUnlock(parray));
}

void OleSafeArray::Destroy()
{
	CheckError(::SafeArrayDestroy(parray));
	// The underlying SafeArray object was destroyed, so we need to detach the object to void operating on it anymore.
	// We don't care the destroyed object, so we can just simply call Detach after ::SafeArrayDestroy to set vt to VT_EMPTY.
	Detach();
}

void OleSafeArray::DestroyData()
{
	CheckError(::SafeArrayDestroyData(parray));
}

void OleSafeArray::DestroyDescriptor()
{
	CheckError(::SafeArrayDestroyDescriptor(parray));
}

BOOL OleSafeArray::CompareSafeArrays(SAFEARRAY* parray1, SAFEARRAY* parray2)
{
	BOOL bCompare = FALSE;

	// If one is NULL they must both be NULL to compare
	if (parray1 == NULL || parray2 == NULL)
	{
		return parray1 == parray2;
	}

	// Dimension must match and if 0, then arrays compare
	DWORD dwDim1 = ::SafeArrayGetDim(parray1);
	DWORD dwDim2 = ::SafeArrayGetDim(parray2);
	if (dwDim1 != dwDim2)
		return FALSE;
	else if (dwDim1 == 0)
		return TRUE;

	// Element size must match
	DWORD dwSize1 = ::SafeArrayGetElemsize(parray1);
	DWORD dwSize2 = ::SafeArrayGetElemsize(parray2);
	if (dwSize1 != dwSize2)
		return FALSE;

	long* pLBound1 = NULL;
	long* pLBound2 = NULL;
	long* pUBound1 = NULL;
	long* pUBound2 = NULL;

	void* pData1 = NULL;
	void* pData2 = NULL;

	try
	{
		// Bounds must match
		pLBound1 = new long[dwDim1];
		pLBound2 = new long[dwDim2];
		pUBound1 = new long[dwDim1];
		pUBound2 = new long[dwDim2];

		size_t nTotalElements = 1;

		// Get and compare bounds
		for (DWORD dwIndex = 0; dwIndex < dwDim1; dwIndex++)
		{
			CheckError(::SafeArrayGetLBound(parray1, dwIndex + 1, &pLBound1[dwIndex]));
			CheckError(::SafeArrayGetLBound(parray2, dwIndex + 1, &pLBound2[dwIndex]));
			CheckError(::SafeArrayGetUBound(parray1, dwIndex + 1, &pUBound1[dwIndex]));
			CheckError(::SafeArrayGetUBound(parray2, dwIndex + 1, &pUBound2[dwIndex]));

			// Check the magnitude of each bound
			if (pUBound1[dwIndex] - pLBound1[dwIndex] != pUBound2[dwIndex] - pLBound2[dwIndex])
			{
				delete[] pLBound1;
				delete[] pLBound2;
				delete[] pUBound1;
				delete[] pUBound2;

				return FALSE;
			}

			// Increment the element count
			nTotalElements *= pUBound1[dwIndex] - pLBound1[dwIndex] + 1;
		}

		// Access the data
		CheckError(::SafeArrayAccessData(parray1, &pData1));
		CheckError(::SafeArrayAccessData(parray2, &pData2));

		// Calculate the number of bytes of data and compare
		size_t nSize = nTotalElements * dwSize1;
		int nOffset = memcmp(pData1, pData2, nSize);
		bCompare = nOffset == 0;

		// Release the array locks
		CheckError(::SafeArrayUnaccessData(parray1));
		CheckError(::SafeArrayUnaccessData(parray2));
	}
	catch (std::exception & e)
	{
		log_error(helios_module, "%s", e.what());
		// Clean up bounds arrays
		delete[] pLBound1;
		delete[] pLBound2;
		delete[] pUBound1;
		delete[] pUBound2;

		// Release the array locks
		if (pData1 != NULL)
			::SafeArrayUnaccessData(parray1);
		if (pData2 != NULL)
			::SafeArrayUnaccessData(parray2);
	}

	// Clean up bounds arrays
	delete[] pLBound1;
	delete[] pLBound2;
	delete[] pUBound1;
	delete[] pUBound2;

	return bCompare;
}