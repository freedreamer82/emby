#include "MessageBroker.hh"
#include <EmbyThreading/ScopedMutex.hh>
#include <EmbySystem/System.hh>
#include <EmbyLog/Log.hh>

#ifdef EMBY_BROKER_DEBUG
EmbyLog_MODULE_LOG_CLASS("mb");
#endif // EMBY_BROKER_DEBUG

using namespace EmbyExec;

MessageBroker::MessageBroker()
    : m_isMsgSync(false)
    , m_mutex()
    , m_thread(this, "Broker", STACK_SIZE, PRIORITY)
    , m_messages()
{
	m_messages.reserve(MAX_MESSAGES);
}

void MessageBroker::doWork()
{
	doStep();
	EmbySystem::delayMs(5);
}

bool
MessageBroker::sendMessage(Message&& msg, bool sendSync, uint32_t destId)
{
	EmbyDebug_ASSERT(destId != MessageClient::ID_INVALID);

	bool retval = true;

	if(EmbySystem::isInInterrupt())
	{
		//on critical section to avoid more interrupts
		EmbySystem_BEGIN_CRITICAL_SECTION;
			if(m_messages.size() < MAX_MESSAGES)
			{
				SenderMessagePair m(destId, std::move(msg));
				m_messages.push_back(std::move(m));
				retval = true;
			}
			else
			{
				retval = false;
			}
		EmbySystem_END_CRITICAL_SECTION;

		return retval;
	}

#ifdef EMBY_BROKER_DEBUG
	// We log here because we can't log on interrupt
	log_debug("send msg: %d, %d", msg.getType(), destId);
#endif // EMBY_BROKER_DEBUG

	// no ISR messages
	if(sendSync)
	{
		EmbyThreading::ScopedMutex lock(m_mutex);

    	//call the message for every listener and then pop
		for(auto client : m_clients)
		{
			if(destId == DEST_BROADCAST ||
			   destId == client->getId())
			{
				m_isMsgSync = true;
				//broadcast msg or filtered for listener
				client->onBrokerMessage(msg);
			}
		}
	}
	else
	{
		EmbyThreading::ScopedMutex lock(m_mutex);

		if(m_messages.size() < MAX_MESSAGES)
		{
			SenderMessagePair m(destId, std::move(msg));
			m_messages.push_back(std::move(m));
		}
		else
		{
			retval = false;
		}
	}

	m_isMsgSync = false;

//	EmbyDebug_ASSERT(retval);

	return retval;
}

void
MessageBroker::doStep()
{
	EmbyThreading::ScopedMutex lock(m_mutex);

	while(m_messages.size() != 0)
    {
    	//release
		auto msg = std::move(m_messages.back());
		m_messages.pop_back();

#ifdef EMBY_BROKER_DEBUG
		log_debug("pop msg: %d, %d", msg.message.getType(), msg.destination);
#endif // EMBY_BROKER_DEBUG

    	//call the message for every listener and then pop
		for(auto client : m_clients)
		{
			if(msg.destination == DEST_BROADCAST ||
			   msg.destination == client->getId())
			{
				m_isMsgSync = false;
				//broadcast msg or filtered for listener
				client->onBrokerMessage(msg.message);
			}
		}
	}
}
