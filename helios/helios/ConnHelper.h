//
//  ConnHelper.h
//
//  Created by Colin on 2020-03-11.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#pragma once

//
// NOTE: class T derives from TSinker and CConnHelper
//
template< class T >
class CConnHelper
{
public:
	template<class TSinker> 
	HRESULT PlugInSinkerToSvr(IUnknown* pUnkSvr)
	{
		if ( pUnkSvr )
		{
			T* pThis = static_cast<T*>(this);
			return pThis->TSinker::DispEventAdvise(pUnkSvr);
		}
		return E_INVALIDARG;
	}

	template<class TSinker> 
	HRESULT PlugOutSinkerFromSvr(IUnknown* pUnkSvr)
	{
		if ( pUnkSvr )
		{			
			T* pThis = static_cast<T*>(this);
			return pThis->TSinker::DispEventUnadvise(pUnkSvr);
		}
		return E_INVALIDARG;
	}
};