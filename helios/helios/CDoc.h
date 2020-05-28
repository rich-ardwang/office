
#pragma once
#include <string>
#include "lava_base.h"
#include "CCalcEngine.h"
#include "cdh_client.h"
#include "lava_utils_api.h"

using namespace ATL;

typedef struct acc
{
    CString   m_user;
    CString   m_password;
    int       m_storePwd;
    int       m_autologin;

    acc() : m_user(_T("")),
            m_password(_T("")),
            m_storePwd(0),
            m_autologin(0) {}
} accoutInfo;

typedef struct conf
{
    std::string     m_ip;
    int             m_port;
    int             m_timeout;
    int             m_retryTimes;
    int             m_retrySpan;

    conf() : m_ip("127.0.0.1"),
             m_port(20480),
             m_timeout(5),
             m_retryTimes(5),
             m_retrySpan(3) {}
} confInfo;

typedef struct tagGlobalInfo
{
    typedef struct tagChangedSendInfo
    {
        std::string     m_formatPrecision;
        tagChangedSendInfo() : m_formatPrecision("10.2") {}
    } ChangedSendInfo;

    typedef struct tagFreqSendInfo
    {
        int     m_defaultSendTimeSpan;
        bool    m_sendDataRightNow;
        tagFreqSendInfo() : m_defaultSendTimeSpan(60), m_sendDataRightNow(false) {}
    } FreqSendInfo;

    ChangedSendInfo     m_changedSendInfo;
    FreqSendInfo        m_FreqSendInfo;
} GlobalInfo;

class CDoc
{
	friend class AddinMsgWnd;
public:
	CDoc();
	~CDoc();

	void Initialzie();
    void SetLoginResult(bool is_success);
	inline bool GetLoginResult()
	{
		return m_is_login;
	}

	ATL::CComVariant SingleCalc(char* func_name, CCalcEngine::CodeType code_type, long& param_count, ATL::CComVariant** params);

	inline void OnCalculate(__lv_in IDispatch* Sh)
	{
		m_CalcEngine->OnCalculate(Sh);
	}
    inline void OnSheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target)
    {
        m_CalcEngine->OnSheetChange(Sh, Target);
    }
	inline void CDoc::OnAfterCalculate()
	{
		m_CalcEngine->OnAfterCalculate();
	}

	BOOL OpenWorkBook(BSTR file_name, _Workbook**);
	CComPtr<_Worksheet> AddNewSheet();
	CComPtr<_Worksheet> AddNewSheet(__lv_in CString name);
    void OnManualSend();

public:
    int GetUserAccount(accoutInfo &acInfo);
    int WriteUserAccount(const accoutInfo &acInfo);
    int GetGlobalConfigInfo(GlobalInfo &gbInfo);
    int WriteGlobalConfigInfo(const GlobalInfo &gbInfo);
    bool LoginCDH(const accoutInfo &acInfo);
    CDHClient* GetCDHClient();

private:
	CString RegularSheetName(CString org_name);
	void OnDataArrived();
    void loadConfig();

private:
	bool            m_is_login;
    confInfo        m_confInfo;
    GlobalInfo      m_globalInfo;
    CDHClient*      m_cliProxy;
	std::shared_ptr<CCalcEngine>	m_CalcEngine;
};

