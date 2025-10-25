#ifndef EMBY_NET_SERVERTCP_H
#define EMBY_NET_SERVERTCP_H

#include "NetInterface.hh"
#include "ServerTCPImpl.hh"
#include "NetUtils.hh"
#include "SocketTCP.hh"

namespace EmbyNetwork
{

    class ServerTCP
    {

    public:

    	static constexpr uint16_t DEFAULT_MAX_CLIENTS = 3;
    	static constexpr uint16_t DEFAULT_TIMEOUT_MS = 500;

    	ServerTCP(NetInterface* network, uint16_t maxClients = DEFAULT_MAX_CLIENTS);

        ~ServerTCP();

        int32_t start(uint16_t port);

        int32_t stop();

        int32_t listen(); //torna id della socket in listen

        SocketTCP* accept(uint32_t millisTimeout = DEFAULT_TIMEOUT_MS); //getSocketFromId

        int16_t release(SocketTCP* socket);

        bool isRunning()
        {
        	return m_isUp;
        }

    private:

        bool init();

        NetInterface* m_network;

        ServerTCPImpl m_impl;

        uint16_t m_bind_port;
        uint8_t m_isUp :1;

    };

}
#endif
