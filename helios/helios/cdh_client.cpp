#include "stdafx.h"
#include "cdh_client.h"
#include "lava_utils_api.h"
#include "SumsAddin.h"
#include "sfa/msg/msg.h"
#include "sfa/msg/msg_codec.h"
#include "sfa/msg/value_kv_array.h"
#include "sfa/msg/value_ascii.h"
#include "sfa/msg/value_decimal.h"

#define HELIOS_CDH_KVARRAY_FIELD           1
#define HELIOS_CDH_EXCELADDIN_TYPE         20001
#define HELIOS_CDH_EXCELADDIN_VALUE        "2100"

using namespace sfa::msg;

void CDHClient::CDHListener::onEvent(const int code)
{
    switch (code)
    {
    case EVENT_CDH_DISCONNECT:
        m_parent->m_bConnected = false;
        m_parent->clearCurrToken();
        log_warn(helios_module, "Disconnected! Auto reconnect...");
        break;

    case EVENT_CDH_RECONNECT_OK:
        m_parent->m_bConnected = true;
        log_info(helios_module, "Reconnect success. Auto login...");
        m_parent->autoLogin();
        break;

    case EVENT_CDH_INVALID_NETWORK:
        m_parent->m_bConnected = false;
        m_parent->clearCurrentAccount();
        log_error(helios_module, "Invalid network! Please contact your network administrator for help.");
        break;

    case EVENT_CDH_KICK_OUT:
        m_parent->m_bConnected = false;
        m_parent->clearCurrToken();
        log_warn(helios_module, "Kick out by CDH! Auto reconnect and login...");
        if (m_parent->autoReconnect())
            m_parent->autoLogin();
        break;

    default:
        break;
    }
}

void CDHClient::CDHListener::onAuthMessage(AuthMessage* pAuthMsg, int len)
{
    if (nullptr == pAuthMsg)
    {
        log_warn(helios_module, "Auth message is null!");
        return;
    }
    for (int i = 0; i < len; ++i)
    {
        log_info(helios_module, "Auth info catgeory: %s, subCategory: %s, sourceIds: %s.",
            pAuthMsg[i].category, pAuthMsg[i].subCategory, pAuthMsg[i].sourceIds);
    }
}

CDHClient::~CDHClient()
{
    if (m_pGtwConnector)
    {
        if (m_currAccInfo.m_pToken)
        {
            m_pGtwConnector->qbLogout(m_currAccInfo.m_pToken);
            m_currAccInfo.m_pToken = nullptr;
        }

        delete m_pGtwConnector;
        m_pGtwConnector = nullptr;

        if (m_pListener)
        {
            delete m_pListener;
            m_pListener = nullptr;
        }
    }
}

bool CDHClient::Connect(const std::string &ip, const int &port)
{
    if (m_currAccInfo.m_pToken)
    {
        m_pGtwConnector->qbLogout(m_currAccInfo.m_pToken);
        m_bConnected = false;
        clearCurrentAccount();
    }

    if (m_bConnected)
        return true;

    if (nullptr == m_pGtwConnector)
    {
        if (!m_pListener)
        {
            log_error(helios_module, "CDH listener pointer is null!");
            return false;
        }

        if (m_cdhLogPath.empty())
        {
            m_pGtwConnector = new GatewayConnector(m_pListener);
            log_info(helios_module, "Do not open CDH log.");
        }
        else
        {
            m_pGtwConnector = new GatewayConnector(m_pListener, m_cdhLogPath.c_str());
            log_info(helios_module, "Open CDH log.");
        }

        if (m_pGtwConnector)
            m_pGtwConnector->setReconnectArgs(m_retryTimes, m_retryTimeSpan);
        else
        {
            log_error(helios_module, "create gateway connector failed!");
            return false;
        }
    }

    bool ret = m_pGtwConnector->connect(ip.c_str(), port, m_timeout);
    if (ret)
    {
        m_bConnected = true;
        m_currGtwIngo.m_gtwIp = ip;
        m_currGtwIngo.m_gtwPort = port;
    }
    else
        log_error(helios_module, "connect CDH failed!");

    return m_bConnected;
}

bool CDHClient::Login(const std::string &user, const std::string &password)
{
    if ((!m_bConnected) || (nullptr == m_pGtwConnector))
    {
        log_error(helios_module, "connect first!");
        return false;
    }

    if ((m_currAccInfo.m_curUser == user)
        && (m_currAccInfo.m_curPwd == password)
        && (m_currAccInfo.m_pToken))
    {
        log_info(helios_module, "this account has already login, username:%s, token:%s",
            user.c_str(), m_currAccInfo.m_pToken);
        return true;
    }

    bool ret = m_pGtwConnector->qbLogin(user.c_str(), password.c_str(), &m_currAccInfo.m_pToken, m_timeout);
    if (ret && m_currAccInfo.m_pToken)
    {
        m_currAccInfo.m_curUser = user;
        m_currAccInfo.m_curPwd = password;
        CSumsAddin::GetAddin()->GetDoc()->SetLoginResult(true);
        return true;
    }

    m_currAccInfo.m_pToken = nullptr;
    log_error(helios_module, "login failed!");
    CSumsAddin::GetAddin()->GetDoc()->SetLoginResult(false);
    return false;
}

bool CDHClient::sendData(const int &funCode, const std::vector<ScContribData> &data)
{
    if (!m_bConnected)
    {
        log_warn(helios_module, "connect state is invalid!");
        return false;
    }

    if (m_currAccInfo.m_pToken && m_pGtwConnector)
    {
        uint8_t *msgBuf = nullptr;
        unsigned int msgLen = 0;
        size_t recordCnt = EncodeSdnMsg(data, msgBuf, msgLen);
        if ((0 == recordCnt) || (nullptr == msgBuf) || (0 == msgLen))
            return false;

        bool ret = false;
        log_info(helios_module, "prepare to send data to CDH, SDN message buf size:[%u].", msgLen);
        try
        {
            ret = m_pGtwConnector->syncRequest(m_currAccInfo.m_pToken, funCode, msgBuf, msgLen, m_timeout);
        }
        catch (std::exception & e)
        {
            log_error(helios_module, "exception happened in syncRequest: %s", e.what());
        }

        ReleaseMsgBuf(msgBuf);
        if (ret)
        {
            log_info(helios_module, "send data to CDH success. records:[%d].", recordCnt);
            return true;
        }
        else
        {
            log_error(helios_module, "syncRequest failed!");
            return false;
        }
    }
    log_error(helios_module, "no connection or not login!");
    return false;
}

void CDHClient::getAuth()
{
    if (m_currAccInfo.m_pToken && m_pGtwConnector)
        m_pGtwConnector->getAuthority(m_currAccInfo.m_pToken);
}

void CDHClient::clearCurrentAccount()
{
    m_currAccInfo.m_curUser.clear();
    m_currAccInfo.m_curPwd.clear();
    m_currAccInfo.m_pToken = nullptr;
}

void CDHClient::clearCurrToken() 
{ 
    m_currAccInfo.m_pToken = nullptr; 
    CSumsAddin::GetAddin()->GetDoc()->SetLoginResult(false);
}

void CDHClient::autoLogin()
{
    if ( !m_currAccInfo.m_curUser.empty() && !m_currAccInfo.m_curPwd.empty() )
    {
        bool ret = Login(m_currAccInfo.m_curUser, m_currAccInfo.m_curPwd);
        if (ret)
        {
            log_info(helios_module, "Auto login to CDH success.");
            return;
        }
    }
    log_error(helios_module, "Can not auto login to CDH!");
}

bool CDHClient::autoReconnect()
{
    if (!m_bConnected)
    {
        bool ret = Connect(m_currGtwIngo.m_gtwIp, m_currGtwIngo.m_gtwPort);
        if (ret)
        {
            log_info(helios_module, "Auto reconnect to CDH success.");
            return true;
        }
        else
        {
            log_error(helios_module, "Auto reconnect to CDH failed!");
            return false;
        }
    }
    log_info(helios_module, "Already connect to CDH.");
    return true;
}

size_t CDHClient::EncodeSdnMsg(const std::vector<ScContribData> &inData, uint8_t*& outBytes, unsigned int &outLen)
{
    size_t records = 0;
    if (inData.empty())
    {
        log_error(helios_module, "inData empty!");
        return records;
    }
    if (outBytes)
    {
        log_error(helios_module, "outBytes must be nullptr!");
        return records;
    }

    IMessage* message = NewMessage();
    IMessageContext *context = NewMessageContext();
    IMessageHeader *header = message->GetHeader();
    header->SetMessageType(IMessageHeader::MessageType::FULL);
    IMessageBody *body = message->GetBody();

    KVArrayValue kvArray;
    bool initCol = false;
    auto iter = inData.begin();
    for (; iter != inData.end(); ++iter)
    {
        //init col type according to field id
        if (!initCol)
        {
            kvArray.SetColType((*iter).mode.first, FieldType::ASCII);
            kvArray.SetColType(HELIOS_CDH_EXCELADDIN_TYPE, FieldType::ASCII);
            auto fidIter = (*iter).fids.begin();
            for (; fidIter != (*iter).fids.end(); ++fidIter)
                kvArray.SetColType(atoi((*fidIter).c_str()), FieldType::DECIMAL);
            initCol = true;
        }

        //add record
        KVArrayRowValue* kvRow = kvArray.AddRow();
        kvRow->SetValue((*iter).mode.first, &AsciiValue((*iter).mode.second));
        kvRow->SetValue(HELIOS_CDH_EXCELADDIN_TYPE, &AsciiValue(HELIOS_CDH_EXCELADDIN_VALUE));
        auto fidIter = (*iter).fids.begin();
        auto valIter = (*iter).vals.begin();
        for (; fidIter != (*iter).fids.end(); ++fidIter, ++valIter)
        {
            double value = *valIter;
            if (0x8000000000000000 == *((uint64_t*)&value))
            {
                // empty value : -0.0
                kvRow->SetValue(atoi((*fidIter).c_str()), &DecimalValue(FieldDesc::Null));
            }
            else
            {
                // need not set field desc, the default value is FieldDesc:NORMAL
                kvRow->SetValue(atoi((*fidIter).c_str()), &DecimalValue(value));
            }
        }
    }
    body->AddField(HELIOS_CDH_KVARRAY_FIELD, &kvArray);

    bool ret = false;
    outLen = 0;
    ret = EncodeMessage(message, context, outBytes, outLen);
    if (ret)
    {
#ifdef _DEBUG
        /*
        IMessage* decMsg = nullptr;
        ret = DecodeMessage(outBytes, outLen, context, decMsg);
        std::string decMsgOut = decMsg->ToString();
        log_info(helios_module, "send SDN message:[%s].", decMsgOut.c_str());
        DeleteMessage(&decMsg);
        */
#endif
        records = inData.size();
    }
    else
        log_error(helios_module, "encode SDN message failed!");

    //release memory
    DeleteMessageContext(&context);
    DeleteMessage(&message);

    return records;
}
