#ifndef EMBY_NET_SOCKETTCP_H
#define EMBY_NET_SOCKETTCP_H

#include "SocketTCPImpl.hh"
#include "NetInterface.hh"
#include "NetUtils.hh"

namespace EmbyNetwork
{

    class SocketTCP
    {

    public:

    	SocketTCP(NetInterface* network);

    	SocketTCP();

        ~SocketTCP();

        SocketTCPImpl* getImpl()
        {
           return &m_impl;
        }

        bool isOpen();

        int32_t open();

        bool close();

        int32_t write(uint8_t* data_ptr, size_t data_len, uint32_t millisTimeout);

        int32_t read(uint8_t* data_ptr, size_t data_len, uint32_t millisTimeout);

        int32_t getStats(SocketTCPStats* stats);

    private:

        bool init();
        int32_t retreiveData(uint8_t* data_ptr, size_t data_len);

        NetInterface* m_network;
        SocketTCPImpl m_impl;

    };

}
#endif
