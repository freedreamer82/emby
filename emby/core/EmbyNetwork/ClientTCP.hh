#ifndef EMBY_NET_CLIENTTCP_H
#define EMBY_NET_CLIENTTCP_H

#include "NetInterface.hh"
#include "NetUtils.hh"
#include "SocketTCP.hh"

namespace EmbyNetwork
{

    class ClientTCP
    {

    public:

    	static constexpr uint16_t DEFAULT_TIMEOUT_MS = 500;

    	ClientTCP(NetInterface* network);

        ~ClientTCP();

        int32_t start(uint32_t port);

        int32_t stop();

        int32_t connect(SocketAddress* serverAddress, uint32_t millisTimeout = DEFAULT_TIMEOUT_MS);

        int32_t disconnect(uint32_t millisTimeout = DEFAULT_TIMEOUT_MS);

        int32_t send(uint8_t* data_ptr, size_t data_len, uint32_t millisTimeout = DEFAULT_TIMEOUT_MS);

        int32_t receive(uint8_t* data_ptr, size_t data_len, uint32_t millisTimeout = DEFAULT_TIMEOUT_MS);

        bool isConnected()
        {
        	return m_isUp && m_isConnected;
        }

        bool isRunning()
        {
        	return m_isUp;
        }

    private:

        bool init();

        NetInterface* m_network;
        SocketTCP* m_tcpSocket;
        uint32_t m_bind_port;
        uint8_t m_isUp :1;
        uint8_t m_isConnected :1;

    };

}
#endif
