#ifndef EMBY_NET_SOCKETUDP_H
#define EMBY_NET_SOCKETUDP_H

#include "SocketUDPImpl.hh"
#include "NetInterface.hh"
#include "NetUtils.hh"

namespace EmbyNetwork
{

    class SocketUDP
    {

    public:

    	static constexpr uint16_t DEFAULT_TIMEOUT_MS = 500;

    	SocketUDP(NetInterface* network);

    	SocketUDP();

        ~SocketUDP();

        void setDestinationAddr(SocketAddress& socketAddress)
        {
        	m_destinationAddress.copy(socketAddress);
        }

        SocketUDPImpl* getImpl()
        {
           return &m_impl;
        }

        bool isOpen();

        bool open(uint16_t port);

        bool close();

        int32_t write(uint8_t* data_ptr, size_t data_len, uint32_t millisTimeout = DEFAULT_TIMEOUT_MS)
        {
        	return send(&m_destinationAddress, data_ptr, data_len, millisTimeout);
        }

        int32_t write(SocketAddress *socketAddress, uint8_t* data_ptr, size_t data_len, uint32_t millisTimeout = DEFAULT_TIMEOUT_MS)
        {
        	return send(socketAddress, data_ptr, data_len, millisTimeout);
        }

        int32_t read(uint8_t* data_ptr, uint32_t millisTimeout = DEFAULT_TIMEOUT_MS);

    private:

        bool init();

        int32_t send(SocketAddress *socketAddress, uint8_t* data_ptr, size_t data_len, uint32_t millisTimeout);

        NetInterface* m_network;
        SocketUDPImpl m_impl;
        uint16_t m_bind_port;
        SocketAddress m_destinationAddress;

    };

}
#endif
