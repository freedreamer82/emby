#ifndef EMBY_NET_NETSOCKET_H
#define EMBY_NET_NETSOCKET_H

#include <EmbyTime/Millis.hh>

namespace EmbyNetwork
{
    class NetSocket
    {
    public:
        NetSocket();

        virtual bool send(const void* data, size_t size);
        virtual bool sendTo(const SocketAddress &socketAddress, const void* data, size_t size);

        bool setTimeout(EmbyTime::Millis timeout)
        {
            m_timeout = timeout;
        }

    private:
        EmbyTime::Millis m_timeout;
        SocketAddress m_remoteAddress;
    };
}

#endif