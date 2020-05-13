
#include "stdafx.h"
#include <fstream>
#include "CDoc.h"
#include "SumsAddin.h"
#include "lava_crt.h"

using namespace ATL;

CDoc::CDoc() : m_is_login(false), m_cliProxy(nullptr)
{
	m_CalcEngine = std::make_shared<CCalcEngine>();
}

CDoc::~CDoc()
{
	m_CalcEngine->OnStopCalculate();
	m_CalcEngine = nullptr;
}

void CDoc::Initialzie()
{
	std::wstring xla_file = CSumsAddin::GetAddin()->GetModulePath();
	xla_file += L"sumscope.xla";

	CComPtr<_Workbook> spWorkbook;
	BOOL ret = OpenWorkBook(CComBSTR(xla_file.c_str()), &spWorkbook);
	if (ret == FALSE)
		log_error(helios_module, "open sumscope.xla file failed.");

    //init cdh client proxy
    m_cliProxy = CDHClient::getInstance();
    if (nullptr == m_cliProxy)
        log_error(helios_module, "CDH client init failed!");

    loadConfig();
    m_cliProxy->setAutoReconnect(m_confInfo.m_retryTimes, m_confInfo.m_retrySpan);
    m_cliProxy->setTimeout(m_confInfo.m_timeout);

    //auto login CDH
    if (!m_is_login)
    {
        accoutInfo ac;
        int ret = GetUserAccount(ac);
        if (0 == ret)
        {
            if (TRUE == ac.m_autologin)
            {
                bool res = LoginCDH(ac);
                if (res)
                    m_is_login = true;
                else
                    log_error(helios_module, "auto login CDH failed!");
            }
        }
    }
}

CComVariant CDoc::SingleCalc(char* func_name, CCalcEngine::CodeType code_type, long& param_count, CComVariant** params)
{
	return m_CalcEngine->SingleCalc(func_name, code_type, param_count, params);
}

BOOL CDoc::OpenWorkBook(BSTR file_name, _Workbook** workbook)
{
	CComPtr<Workbooks> spWorkbooks;
	CSumsAddin::GetAddin()->GetApp()->get_Application()->get_Workbooks(&spWorkbooks);
	HRESULT hr = spWorkbooks->raw_Open(file_name, CComVariant(0), CComVariant(VARIANT_FALSE), CComVariant(1),
		CComVariant(""), CComVariant(""), CComVariant(VARIANT_TRUE), CComVariant(MSExcel::xlWindows), CComVariant(6),
		CComVariant(VARIANT_FALSE), CComVariant(VARIANT_FALSE), CComVariant(0), CComVariant(VARIANT_FALSE), CComVariant(VARIANT_FALSE),
		CComVariant(MSExcel::xlNormalLoad), 0, workbook);

	return SUCCEEDED(hr);
}

CComPtr<_Worksheet> CDoc::AddNewSheet()
{
	CComPtr<_Workbook> spWorkbook;
	CComPtr<_Application> spApp = CSumsAddin::GetAddin()->GetApp()->get_Application();
	spApp->get_ActiveWorkbook(&spWorkbook);
	if ( nullptr == spWorkbook )
	{
		CComPtr<Workbooks> spWorkbooks;
		spApp->get_Workbooks(&spWorkbooks);
		spWorkbooks->raw_Add(vtMissing, 0, &spWorkbook);
	}

	if ( spWorkbook )
	{
		CComPtr<Sheets> spSheets;
		if ( SUCCEEDED(spWorkbook->get_Sheets(&spSheets)) )
		{
			CComPtr<IDispatch> spAfter = spSheets->GetItem(CComVariant(spSheets->GetCount()));
			CComPtr<IDispatch> spNew = spSheets->Add(vtMissing, CComVariant(spAfter), CComVariant(1), CComVariant(xlWorksheet));
			if ( spNew )
			{
				CComQIPtr<_Worksheet> spWorksheet( spNew );
				return spWorksheet;
			}
			log_error(helios_module, "Add new sheet failed.");
		}
		else
			log_error(helios_module, "Get sheets failed.");
	}
	return nullptr;
}

CComPtr<_Worksheet> CDoc::AddNewSheet(__lv_in CString name)
{
	name = RegularSheetName(name);
	CComPtr<_Worksheet> spWorksheet = AddNewSheet();
	if ( spWorksheet )
	{
		CComPtr<IDispatch> spDisp;
		spWorksheet->get_Parent(&spDisp);
		CComQIPtr<_Workbook> spWorkbook(spDisp);

		CComPtr<Sheets> spSheets;
		spWorkbook->get_Sheets(&spSheets);

		for ( int i = 0; ;i++ )
		{
			CString sheet_name;
			if (0 == i)
				sheet_name = name;
			else
				sheet_name.Format(_T("%s%d"), name.Left(30), i); // name too length lead to crash.

			try
			{
				spDisp = nullptr;
				spSheets->get_Item(CComVariant(sheet_name), &spDisp);
				if (spDisp)
					continue;
			}
			catch (...)
			{
				log_error(helios_module, "exception occurs when add named sheet.");
			}
			spWorksheet->put_Name(CComBSTR(sheet_name));
			break;
		}
		return spWorksheet;
	}
	return nullptr;
}

CString CDoc::RegularSheetName(CString org_name)
{
	CString name;
	org_name.Replace(_T("*"), _T("_"));

	CString token(_T(":\\/?[]"));
	int cur_pos = 0;
	CString res = org_name.Tokenize(token, cur_pos);
	while (res != _T(""))
	{
		name += res;
		res = org_name.Tokenize(token, cur_pos);
	}
	return (name.GetLength() > 30) ? name.Left(30) : name;
}

void CDoc::OnDataArrived()
{
    m_CalcEngine->OnDataArrived();
}

void CDoc::OnManualSend()
{
    m_CalcEngine->OnManualSend();
}

void CDoc::loadConfig()
{
    // Config init
    lava::utils::i_lava_config* pConf = create_lava_config();
    if (pConf)
    {
        bool initFlg = pConf->init();
        if (!initFlg)
        {
            log_error(helios_module, "initial config failed!");
            return;
        }
        else
            log_info(helios_module, "initial config complete.");
    }
    else
    {
        log_error(helios_module, "lava config create failed!");
        return;
    }

    char *conf_buf = nullptr; int conf_len = 0;
    pConf->value("gateway.addr", conf_buf, conf_len);
    if ((conf_buf) && (conf_len > 0))
        m_confInfo.m_ip = conf_buf;

    std::string sPort;
    pConf->value("gateway.port", conf_buf, conf_len);
    if ((conf_buf) && (conf_len > 0))
        sPort = conf_buf;

    if (!sPort.empty())
        m_confInfo.m_port = std::stoi(sPort);

    std::string sTimeout;
    pConf->value("gateway.timeout", conf_buf, conf_len);
    if ((conf_buf) && (conf_len > 0))
        sTimeout = conf_buf;

    if (!sTimeout.empty())
        m_confInfo.m_timeout = std::stoi(sTimeout);

    std::string sRtyTimes;
    pConf->value("gateway.reconnect.retryTimes", conf_buf, conf_len);
    if ((conf_buf) && (conf_len > 0))
        sRtyTimes = conf_buf;

    if (!sRtyTimes.empty())
        m_confInfo.m_retryTimes = std::stoi(sRtyTimes);

    std::string sRtyTimeSpan;
    pConf->value("gateway.reconnect.timeSpan", conf_buf, conf_len);
    if ((conf_buf) && (conf_len > 0))
        sRtyTimeSpan = conf_buf;

    if (!sRtyTimeSpan.empty())
        m_confInfo.m_retrySpan = std::stoi(sRtyTimeSpan);

    if (conf_buf)
    {
        delete[] conf_buf;
        conf_buf = nullptr;
    }
    release_lava_config(pConf);
}

int CDoc::GetUserAccount(accoutInfo &acInfo)
{
    int ret = -1;
    std::wstring filePath = CSumsAddin::GetAddin()->GetModulePath() + L"DT";
    std::ifstream in;
    in.open(filePath);
    if (!in.is_open())
    {
        log_error(helios_module, "[GetUserAccount]open DT file failed!");
        return ret;
    }

    //read from file
    std::string b64User;
    std::string b64Pwd;
    in >> acInfo.m_autologin;
    in >> b64User;
    in >> b64Pwd;
    in.close();

    //parse the info
    uint32_t lenUser = 0;
    uint32_t lenPwd = 0;
    uint8_t *cUser = nullptr;
    uint8_t *cPwd = nullptr;
    cUser = base64_decode((uint8_t *)b64User.c_str(), lenUser);
    cPwd = base64_decode((uint8_t *)b64Pwd.c_str(), lenPwd);
    if (cUser && lenUser > 0)
    {
        std::string user((char *)cUser, lenUser);
        acInfo.m_user = CA2T(user.c_str());
        if (cPwd && lenPwd > 0)
        {
            std::string pwdMd5((char *)cPwd, lenPwd);
            acInfo.m_password = CA2T(pwdMd5.c_str());
            acInfo.m_storePwd = TRUE;
            ret = 0;
        }
        else
        {
            ret = -3;
            log_info(helios_module, "[GetUserAccount]emtpy user password.");
        }
    }
    else
    {
        ret = -2;
        log_info(helios_module, "[GetUserAccount]emtpy user name.");
    }

    //release base64 result
    if (cUser) { free_base64_result(cUser); }
    if (cPwd) { free_base64_result(cPwd); }

    return ret;
}

int CDoc::WriteUserAccount(const accoutInfo &acInfo)
{
    int ret = -1;
    std::wstring filePath = CSumsAddin::GetAddin()->GetModulePath() + L"DT";
    std::ofstream out(filePath);
    if (out)
    {
        ret = 0;
        out << acInfo.m_autologin;
        std::string u = CT2A(acInfo.m_user.GetString());
        std::string usr = lava::utils::trim(u);
        uint8_t *cypUser = base64_encode((uint8_t *)usr.c_str(), usr.length());
        if (cypUser && usr.length() > 0)
        {
            out << " " << cypUser;
            if (acInfo.m_storePwd)
            {
                std::string p = CT2A(acInfo.m_password.GetString());
                std::string pd = lava::utils::trim(p);
                uint8_t *cypPwd = base64_encode((uint8_t *)pd.c_str(), pd.length());
                if (cypPwd && pd.length() > 0)
                    out << " " << cypPwd << std::endl;
                else
                {
                    ret = -3;
                    log_error(helios_module, "[WriteUserAccount]store user password failed!");
                    out << std::endl;
                }
                if (cypPwd) { free_base64_result(cypPwd); }
            }
            else
                out << std::endl;
        }
        else
        {
            ret = -2;
            log_error(helios_module, "[WriteUserAccount]store user name failed!");
            out << std::endl;
        }
        out.close();
        if (cypUser) { free_base64_result(cypUser); }
    }
    else
        log_error(helios_module, "[WriteUserAccount]open DT file failed!");

    return ret;
}

int CDoc::GetGlobalConfigInfo(GlobalInfo& gbInfo)
{
    int ret = -1;
    std::wstring filePath = CSumsAddin::GetAddin()->GetModulePath() + L"GlobalConfig";
    std::ifstream in;
    in.open(filePath);
    if (!in.is_open())
    {
        log_error(helios_module, "[GetGlobalConfigInfo]open GlobalConfig file failed!");
        return ret;
    }

    //read from file
    in >> gbInfo.m_changedSendInfo.m_formatPrecision;
    in >> gbInfo.m_FreqSendInfo.m_defaultSendTimeSpan;
    in >> gbInfo.m_FreqSendInfo.m_sendDataRightNow;
    in.close();
    ret = 0;

    return ret;
}

int CDoc::WriteGlobalConfigInfo(const GlobalInfo &gbInfo)
{
    int ret = -1;
    std::wstring filePath = CSumsAddin::GetAddin()->GetModulePath() + L"GlobalConfig";
    std::ofstream out(filePath);
    if (out)
    {
        out << gbInfo.m_changedSendInfo.m_formatPrecision
            << " " << gbInfo.m_FreqSendInfo.m_defaultSendTimeSpan
            << " " << gbInfo.m_FreqSendInfo.m_sendDataRightNow
            << std::endl;
        out.close();
        ret = 0;
    }
    else
        log_error(helios_module, "[WriteGlobalConfigInfo]open GlobalConfig file failed!");

    return ret;
}

bool CDoc::LoginCDH(const accoutInfo &acInfo)
{
    bool ret = false;
    if (nullptr == m_cliProxy)
    {
        log_error(helios_module, "CDH client not init! Maybe CDoc Initialzie incorrectly.");
        return ret;
    }

    if (!m_cliProxy->isConnected())
    {
        ret = m_cliProxy->Connect(m_confInfo.m_ip, m_confInfo.m_port);
        if (!ret)
            return ret;
    }

    std::string usr = CT2A(acInfo.m_user.GetString());
    std::string pwd = CT2A(acInfo.m_password.GetString());
    ret = m_cliProxy->Login(usr, pwd);
    if (!ret)
        return ret;

    m_cliProxy->getAuth();
    m_is_login = true;
    CSumsAddin::GetAddin()->GetUiMgr()->PostCustomMessage(WM_LOGIN, 0, 0);
    return ret;
}

CDHClient* CDoc::GetCDHClient()
{
    return m_cliProxy;
}
