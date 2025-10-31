#ifndef EMBY_NET_NETINTERFACE_H
#define EMBY_NET_NETINTERFACE_H

#include <EmbyLibs/Singleton.hh>
#include "NetworkImpl.hh"
#include "NetUtils.hh"

namespace EmbyNetwork
{

    class NetInterface
    {

    public:

        static NetInterface &get();


        ~NetInterface();

        NetworkImpl *getImpl()
        {
            return &m_impl;
        }


        bool setNetworkConfig(NetConfig *netconfig);

        bool getNetworkConfig(NetConfig *netconfig);

        void setConnectionCb(ConnectionCb c)
        {
        	m_cb = c;
        }

        ConnectionCb getConnectionCb()
        {
        	return m_cb;
        }

        bool isInit();

        bool isConnected();

        bool isRunning();

        bool up();

        bool down();

        bool getIp(EmbyLibs::String*);

        bool getMac(EmbyLibs::String*);

    private:

        NetInterface();

        bool init();

        bool enable();

        bool disable();

        NetworkImpl m_impl;
        ConnectionCb m_cb;

    };

}
#endif
