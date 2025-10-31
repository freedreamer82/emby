#ifndef EMBY_NET_SOCKETPOOL_H
#define EMBY_NET_SOCKETPOOL_H

#include "EmbyNetwork/SocketTCP.hh"
#include "EmbyNetwork/SocketUDP.hh"

namespace EmbyNetwork {

#define SOCKET_POOL_TCP_SIZE 5
#define SOCKET_POOL_UDP_SIZE 5

class SocketPool {

	public:

		static SocketPool& get();

		~SocketPool();

		SocketTCP* getNewSocketTCP();
		SocketUDP* getNewSocketUDP();

		bool releaseSocketTCP(SocketTCP *s);
		bool releaseSocketUDP(SocketUDP *s);

		uint16_t tcpSocketsInUse();
		uint16_t udpSocketsInUse();

		uint16_t tcpSocketsAvailable();
		uint16_t udpSocketsAvailable();

		uint16_t tcpSocketsTotal();
		uint16_t udpSocketsTotal();

	private:

		struct TCPElement {
			SocketTCP socket;
			bool used;
		};

		struct UDPElement {
			SocketUDP socket;
			bool used;
		};

		SocketPool();

		TCPElement m_poolSocketTCP[SOCKET_POOL_TCP_SIZE];
		UDPElement m_poolSocketUDP[SOCKET_POOL_UDP_SIZE];

};


}
#endif
