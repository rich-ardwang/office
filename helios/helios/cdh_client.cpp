#include "stdafx.h"
#include "cdh_client.h"
#include "lava_utils_api.h"
#include "sfa/msg/msg.h"
#include "sfa/msg/msg_codec.h"
#include "sfa/msg/value_kv_array.h"
#include "sfa/msg/value_ascii.h"
#include "sfa/msg/value_decimal.h"

#define CDH_KVARRAY_FIELD           1
#define CDH_EXCELADDIN_TYPE         20001
#define CDH_EXCELADDIN_VALUE        "2100"

using namespace sfa::msg;

void CDHClient::CDHListener::onDisConnected()
{
    log_error(helios_module, "Connection to gateway disConnected!");
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

void CDHClient::CDHListener::onKickOut()
{
    log_error(helios_module, "kick out by gateway!");
}

CDHClient::~CDHClient()
{
    if (m_pGtwConnector)
    {
        if (m_pToken)
            m_pGtwConnector->qbLogout(m_pToken);

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
    if (m_bConnected)
        return true;

    if (nullptr == m_pGtwConnector)
    {
        if (!m_pListener)
        {
            log_error(helios_module, "CDH listener pointer is null!");
            return false;
        }
        m_pGtwConnector = new GatewayConnector(m_pListener);
    }

    bool ret = m_pGtwConnector->connect(ip.c_str(), port);
    if (ret)
        m_bConnected = true;
    else
        log_error(helios_module, "connect CDH failed!");

    return m_bConnected;
}

bool CDHClient::Login(const std::string &user, const std::string &password)
{
    bool ret = false;
    if ((!m_bConnected) || (nullptr == m_pGtwConnector))
    {
        log_error(helios_module, "connect first!");
        return ret;
    }

    if (m_pToken)
    {
        m_pGtwConnector->qbLogout(m_pToken);
        m_pToken = nullptr;
    }

    ret = m_pGtwConnector->qbLogin(user.c_str(), password.c_str(), &m_pToken);
    if (ret && m_pToken)
        ret = true;
    else
        log_error(helios_module, "login failed!");

    return ret;
}

bool CDHClient::sendData(const int &funCode, const std::vector<ScContribData> &data)
{
    if (m_pToken && m_pGtwConnector)
    {
        uint8_t *msgBuf = nullptr;
        unsigned int msgLen = 0;
        size_t recordCnt = EncodeSdnMsg(data, msgBuf, msgLen);
        if ((0 == recordCnt) || (nullptr == msgBuf) || (0 == msgLen))
            return false;

        log_info(helios_module, "prepare to send data to CDH, SDN message buf size:[%u].", msgLen);
        bool ret = m_pGtwConnector->syncRequest(m_pToken, funCode, msgBuf, msgLen);
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
    if (m_pToken && m_pGtwConnector)
        m_pGtwConnector->getAuthority(m_pToken);
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
            kvArray.SetColType(CDH_EXCELADDIN_TYPE, FieldType::ASCII);
            auto fidIter = (*iter).fids.begin();
            for (; fidIter != (*iter).fids.end(); ++fidIter)
                kvArray.SetColType(atoi((*fidIter).c_str()), FieldType::DECIMAL);
            initCol = true;
        }

        //add record
        KVArrayRowValue* kvRow = kvArray.AddRow();
        kvRow->SetValue((*iter).mode.first, &AsciiValue((*iter).mode.second));
        kvRow->SetValue(CDH_EXCELADDIN_TYPE, &AsciiValue(CDH_EXCELADDIN_VALUE));
        auto fidIter = (*iter).fids.begin();
        auto valIter = (*iter).vals.begin();
        for (; fidIter != (*iter).fids.end(); ++fidIter, ++valIter)
            kvRow->SetValue(atoi((*fidIter).c_str()), &DecimalValue(*valIter));
    }
    body->AddField(CDH_KVARRAY_FIELD, &kvArray);

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
