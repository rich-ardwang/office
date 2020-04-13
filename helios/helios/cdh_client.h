#pragma once

#include <string>
#include <vector>
#include "gateway_connector.h"
#include "callback.h"
#include "ScContribFormula.h"

class CDHClient
{
private:
    class CDHListener : public CallBack
    {
    public:
        void onDisConnected();
        void onAuthMessage(AuthMessage* pAuthMsg, int len);
        void onKickOut();
    };

public:
    static CDHClient *getInstance()
    {
        static CDHClient instance_;
        return &instance_;
    }

private:
    inline CDHClient() : m_pGtwConnector(nullptr),
                         m_pListener(nullptr),
                         m_bConnected(false),
                         m_pToken(nullptr)
    {
        m_pListener = new CDHListener();
    }
    ~CDHClient();
    CDHClient(const CDHClient &);
    CDHClient &operator=(const CDHClient &);

public:
    bool Connect(const std::string &ip, const int &port);
    bool Login(const std::string &user, const std::string &password);
    bool sendData(const int &funCode, const std::vector<ScContribData> &data);
    void getAuth();

private:
    size_t EncodeSdnMsg(const std::vector<ScContribData> &inData, uint8_t*& outBytes, unsigned int &outLen);

private:
    GatewayConnector*   m_pGtwConnector;
    CDHListener*        m_pListener;
    bool                m_bConnected;
    char*               m_pToken;
};
