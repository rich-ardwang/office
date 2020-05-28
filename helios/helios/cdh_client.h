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
        inline CDHListener(CDHClient* p) : m_parent(p) {}
        inline ~CDHListener() { m_parent = nullptr; }
        void onEvent(const int code);
        void onAuthMessage(AuthMessage* pAuthMsg, int len);

    private:
        enum
        {
            EVENT_CDH_DISCONNECT = 1,
            EVENT_CDH_RECONNECT_OK,
            EVENT_CDH_INVALID_NETWORK,
            EVENT_CDH_KICK_OUT
        };

    private:
        CDHClient   *m_parent;
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
                         m_timeout(5),
                         m_retryTimes(5),
                         m_retryTimeSpan(3),
                         m_cdhLogPath(L"")
    {
        m_pListener = new CDHListener(this);
    }
    ~CDHClient();
    CDHClient(const CDHClient &);
    CDHClient &operator=(const CDHClient &);

public:
    inline void setAutoReconnect(int rtyTimes, int rtyTimeSpan)
    {
        m_retryTimes = rtyTimes;
        m_retryTimeSpan = rtyTimeSpan;
    }
    inline void setTimeout(int timeout) { m_timeout = timeout; }
    inline void setCDHLogPath(const std::wstring &path) { m_cdhLogPath = path; }
    inline bool isConnected() { return m_bConnected; }
    bool Connect(const std::string &ip, const int &port);
    bool Login(const std::string &user, const std::string &password);
    bool sendData(const int &funCode, const std::vector<ScContribData> &data);
    void getAuth();
    inline void clearCurrentAccount();
    inline void clearCurrToken();
    void autoLogin();
    bool autoReconnect();

private:
    size_t EncodeSdnMsg(const std::vector<ScContribData> &inData, uint8_t*& outBytes, unsigned int &outLen);

private:
    typedef struct tagCurrentAccInfo
    {
        std::string         m_curUser;
        std::string         m_curPwd;
        char*               m_pToken;
        tagCurrentAccInfo() : m_curUser(""),
                              m_curPwd(""),
                              m_pToken(nullptr) {}
    } CurrentAccInfo;

    typedef struct tagCurrentGatewayInfo
    {
        std::string     m_gtwIp;
        int             m_gtwPort;
        tagCurrentGatewayInfo() : m_gtwIp(""), m_gtwPort(0) {}
    } CurrentGatewayInfo;

private:
    GatewayConnector*   m_pGtwConnector;
    CDHListener*        m_pListener;
    bool                m_bConnected;
    CurrentAccInfo      m_currAccInfo;
    CurrentGatewayInfo  m_currGtwIngo;
    int                 m_timeout;
    int                 m_retryTimes;
    int                 m_retryTimeSpan;
    std::wstring        m_cdhLogPath;
};
