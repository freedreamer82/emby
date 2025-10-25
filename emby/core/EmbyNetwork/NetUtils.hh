#ifndef EMBY_NETUTILS_NETINTERFACE_H
#define EMBY_NETUTILS_NETINTERFACE_H

#include <EmbyLibs/Callback.hh>
#include <EmbyLibs/String.hh>
#include <EmbyLibs/StringUtils.hh>

namespace EmbyNetwork
{


    struct NetConfig
    {
        bool dhcpEnable;
        EmbyLibs::String ip;
        EmbyLibs::String netmask;
        EmbyLibs::String gw;
        EmbyLibs::String dns;
    };

    struct SocketTCPStats
    {
    	uint32_t tcpPacketsSent;
    	uint32_t tcpPacketsReceived;
    	uint32_t tcpBytesSent;
    	uint32_t tcpBytesReceived;
    	uint32_t tcpRetransmitPackets;
    	uint32_t tcpPacketsQueued;
    	uint32_t tcpChecksumErrors;
        uint32_t status;
    };

    enum class Dhcp : uint8_t
    {
        DHCP_OFF,
        DHCP_START,
        DHCP_WAIT_ADDRESS,
        DHCP_ADDRESS_ASSIGNED,
        DHCP_TIMEOUT,
        DHCP_LINK_DOWN,
    };

    enum class NetError : int32_t
    {
        NoError = 0,
        Timeout = -1,
        GenericError = -2,
		NameResolutionError = -3,
		NoSocketAvailable = -4
     };

    static int32_t net_err(NetError n)
    {
    	return int32_t(n);
    }

    enum class NetworkEventFlag : uint32_t
    {
        Disconnected = 1 << 0,
        Connected,
        Unkwown
    };
    using ConnectionCb = EmbyLibs::Callback<void, NetworkEventFlag, void*>;

	class SocketAddress {
	public:

    	SocketAddress() :
    		m_isIpv4 { false }
    	{}

		SocketAddress(EmbyLibs::String &address, uint16_t port) :
				m_address { address }, m_port { port }, m_isIpv4 { false }
		{
			m_isIpv4 = EmbyLibs::isIpv4(m_address);
		}

		void copy(SocketAddress& original)
		{
			m_port = original.getPort();
			memcpy(&m_address, original.getAddress(), sizeof(*original.getAddress()));
			m_isIpv4 = EmbyLibs::isIpv4(m_address);
		}

		EmbyLibs::String* getAddress()
		{
			return &m_address;
		}

		uint16_t getPort()
		{
			return m_port;
		}

		bool isIpv4()
		{
			return m_isIpv4;
		}

	private:
		EmbyLibs::String m_address;
		uint16_t m_port;
		bool m_isIpv4;
	};


}
#endif //DVC_EVO_PUPS_DETECTOR_NETINTERFACE_H
