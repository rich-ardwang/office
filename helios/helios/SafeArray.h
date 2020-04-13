//
//  SafeArray.h
//
//  Created by Colin on 2020-03-06.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#pragma once
#include <stdexcept>

namespace Helios
{
	class OleSafeArray : public tagVARIANT
	{
		// Constructors
	public:
		OleSafeArray()
		{
			SafeArrayInit();
		}
		OleSafeArray(const SAFEARRAY& saSrc, VARTYPE vtSrc);
		OleSafeArray(const SAFEARRAY* pSrc, VARTYPE vtSrc);
		OleSafeArray(const OleSafeArray& saSrc);
		OleSafeArray(const VARIANT& varSrc);
		OleSafeArray(const VARIANT* pSrc);
		OleSafeArray(const ATL::CComVariant& varSrc);
		~OleSafeArray()
		{
			Clear();
		}

		// Operations
	public:
		inline void Clear()
		{
			ATLASSERT(::VariantClear(this) == NOERROR);
		}
		void Attach(VARIANT& varSrc);
		VARIANT Detach();

		OleSafeArray& operator=(const OleSafeArray& saSrc);
		OleSafeArray& operator=(const VARIANT& varSrc);
		OleSafeArray& operator=(const VARIANT* pSrc);
		OleSafeArray& operator=(const ATL::CComVariant& varSrc);

		BOOL operator==(const SAFEARRAY& saSrc) const;
		BOOL operator==(const SAFEARRAY* pSrc) const;
		BOOL operator==(const OleSafeArray& saSrc) const;
		BOOL operator==(const VARIANT& varSrc) const;
		BOOL operator==(const VARIANT* pSrc) const;
		BOOL operator==(const ATL::CComVariant& varSrc) const;

		inline operator VARIANT* ()
		{
			return this;
		}
		inline operator const VARIANT* () const
		{
			return this;
		}

		// One dim array helpers
		void CreateOneDim(VARTYPE vtSrc, DWORD dwElements, const void* pvSrcData = NULL, long nLBound = 0);
		DWORD GetOneDimSize();
		void ResizeOneDim(DWORD dwElements);

		// Multi dim array helpers
		void Create(VARTYPE vtSrc, DWORD dwDims, DWORD* rgElements);

		// SafeArray wrapper classes
		void Create(VARTYPE vtSrc, DWORD dwDims, SAFEARRAYBOUND* rgsabounds);
		void AccessData(void** ppvData);
		void UnaccessData();
		void AllocData();
		void AllocDescriptor(DWORD dwDims);
		void Copy(LPSAFEARRAY* ppsa);
		void GetLBound(DWORD dwDim, long* pLBound);
		void GetUBound(DWORD dwDim, long* pUBound);
		void GetElement(long* rgIndices, void* pvData);
		void PtrOfIndex(long* rgIndices, void** ppvData);
		void PutElement(long* rgIndices, void* pvData);
		void Redim(SAFEARRAYBOUND* psaboundNew);
		void Lock();
		void Unlock();
		inline DWORD GetDim()
		{
			return ::SafeArrayGetDim(parray);
		}

		inline DWORD GetElemSize()
		{
			return ::SafeArrayGetElemsize(parray);
		}

		void Destroy();
		void DestroyData();
		void DestroyDescriptor();

	private:
		inline void SafeArrayInit()
		{
			memset(this, 0, sizeof(OleSafeArray));
			vt = VT_EMPTY;
		}

		inline static void CheckError(SCODE sc)
		{
			if (FAILED(sc))
			{
				if (sc == E_OUTOFMEMORY)
					throw std::runtime_error("memory exception.");
				else
					throw std::runtime_error("ole exception.");
			}
		}

		static BOOL CompareSafeArrays(SAFEARRAY* parray1, SAFEARRAY* parray2);

	public:
		// Cache info to make element access (operator []) faster
		DWORD m_dwElementSize;
		DWORD m_dwDims;
	};
}


