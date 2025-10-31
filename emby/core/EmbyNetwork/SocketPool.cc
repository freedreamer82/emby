#include "EmbyNetwork/SocketPool.hh"

namespace EmbyNetwork {


SocketPool &SocketPool::get()
{
    static SocketPool g_instance;
    return g_instance;
}

SocketPool::SocketPool()
{

}

SocketPool::~SocketPool()
{

}

SocketTCP* SocketPool::getNewSocketTCP()
{
	NetInterface& net = NetInterface::get();

	for(int i = 0; i <= SOCKET_POOL_TCP_SIZE; i++)
	{
		if(!m_poolSocketTCP[i].used)
		{
			auto ptr = &m_poolSocketTCP[i].socket;
			new(ptr) SocketTCP(&net);
			m_poolSocketTCP[i].used = true;
			return ptr;
		}
	}

	EmbyDebug_ASSERT_FAIL();
	return nullptr;
}

SocketUDP* SocketPool::getNewSocketUDP()
{
	NetInterface& net = NetInterface::get();

	for(int i = 0; i <= SOCKET_POOL_UDP_SIZE; i++)
	{
		if(!m_poolSocketUDP[i].used)
		{
			auto ptr = &m_poolSocketUDP[i].socket;
			new(ptr) SocketUDP(&net);
			m_poolSocketUDP[i].used = true;
			return ptr;
		}
	}

	EmbyDebug_ASSERT_FAIL();
	return nullptr;
}

bool SocketPool::releaseSocketTCP(SocketTCP* s)
{

	for(int i = 0; i <= SOCKET_POOL_TCP_SIZE; i++)
	{
		if(&m_poolSocketTCP[i].socket == s)
		{
			m_poolSocketTCP[i].used = false;
			return true;
		}
	}

	return false;
}

bool SocketPool::releaseSocketUDP(SocketUDP* s)
{

	for(int i = 0; i <= SOCKET_POOL_UDP_SIZE; i++)
	{
		if(&m_poolSocketUDP[i].socket == s)
		{
			m_poolSocketUDP[i].used = false;
			return true;
		}
	}

	return false;
}

uint16_t SocketPool::tcpSocketsInUse()
{
	uint16_t c = 0;
	for(int i = 0; i <= SOCKET_POOL_TCP_SIZE; i++)
	{
		if(m_poolSocketTCP[i].used)
		{
			c++;
		}
	}

	return c;
}

uint16_t SocketPool::udpSocketsInUse()
{
	uint16_t c = 0;
	for(int i = 0; i <= SOCKET_POOL_UDP_SIZE; i++)
	{
		if(m_poolSocketUDP[i].used)
		{
			c++;
		}
	}

	return c;
}

uint16_t SocketPool::tcpSocketsTotal()
{
	return SOCKET_POOL_TCP_SIZE;
}

uint16_t SocketPool::udpSocketsTotal()
{
	return SOCKET_POOL_UDP_SIZE;
}

uint16_t SocketPool::tcpSocketsAvailable()
{
	return tcpSocketsTotal() - tcpSocketsInUse();
}

uint16_t SocketPool::udpSocketsAvailable()
{
	return udpSocketsTotal() - udpSocketsInUse();
}


}



