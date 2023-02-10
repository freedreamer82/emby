#if !defined( EMBY_BROKER_HH )
#define EMBY_BROKER_HH

#include <EmbyThreading/Worker.hh>
#include <EmbyThreading/Thread.hh>
#include <EmbyThreading/Mutex.hh>
#include <EmbyThreading/ScopedMutex.hh>
#include <EmbyExec/Message.hh>
#include <EmbyLibs/Singleton.hh>
#include <vector>
#include <functional>
#include <EmbyLibs/CircularBuffer.hh>
#include <EmbyLibs/Buffer.hh>
#include <EmbyLibs/Function.hh>
#include <EmbyLibs/CallbackVector.hh>

namespace EmbyExec
{
	class MessageClient
	{
		public:
			static constexpr uint32_t ID_INVALID = 0xFFFFFFFF;

			virtual void onBrokerMessage(Message const&) = 0;

			virtual ~MessageClient() = default;

			inline uint32_t getId() const;

			inline void subscribe();

			inline void unsubscribe() const;

			void sendToSelf(
					Message&& msg,
					bool sendSync = false) const;

			void sendTo(
					Message&& msg,
					uint32_t destId,
					bool sendSync = false) const;

			void sendBroadcast(
					Message&& msg,
					bool sendSync = false) const;

			bool isMessageSync() const;

		protected:
			MessageClient(uint32_t id = ID_INVALID, bool addToBroker = true);

		private:
			uint32_t m_id;
	};

	class MessageBroker: public EmbyThreading::Worker,
			public EmbyLibs::Singleton<MessageBroker>
	{
		public:

			static constexpr uint8_t  MAX_LISTENER_NUMBER = 8;
			static constexpr uint8_t  MAX_MESSAGES = 30;
			static constexpr uint32_t DEST_BROADCAST = 0;

			MessageBroker();

			~MessageBroker() = default;

			/*Send a message to broker.
			 sendSync = force a dispatcher to be synchrounous.
			 you can use this function on ISR , in this case even if sendSync is true will be async by default.
			*/
			bool
			sendMessage(Message&& msg, bool sendSync = false, uint32_t destId = DEST_BROADCAST);

			bool subscribe(MessageClient& client);

			bool unsubscribe(MessageClient const& client);

			bool isMessageSync() const;

		private:
			static size_t const STACK_SIZE = 900;
			static int const PRIORITY = EmbyThreading::PRIORITY_LOWEST;

			struct SenderMessagePair
			{
					SenderMessagePair(uint32_t dest, Message&& msg)
						: destination(dest)
						, message(std::move(msg))
					{
					}

					uint32_t destination;
					Message message;
			};

			virtual void doWork();
			void doStep();

			bool                           m_isMsgSync;
			EmbyThreading::Mutex           m_mutex;
			EmbyThreading::Thread          m_thread;
			std::vector<SenderMessagePair> m_messages;
			std::vector<MessageClient*>    m_clients;
	};

    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::: inline methods :::
    // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	inline MessageClient::MessageClient(uint32_t id, bool addToBroker)
		: m_id(id)
	{
		if(addToBroker)
		{
			subscribe();
		}
	}

	inline uint32_t MessageClient::getId() const
	{
		return m_id;
	}

	inline void MessageClient::subscribe()
	{
		MessageBroker::get().subscribe(*this);
	}

	inline void MessageClient::unsubscribe() const
	{
		MessageBroker::get().unsubscribe(*this);
	}

	inline void MessageClient::sendToSelf(Message&& msg, bool sendSync) const
	{
		MessageBroker::get().sendMessage(std::move(msg), sendSync, getId());
	}

	inline void MessageClient::sendTo(Message&& msg, uint32_t destId, bool sendSync) const
	{
		MessageBroker::get().sendMessage(std::move(msg), sendSync, destId);
	}

	inline void MessageClient::sendBroadcast(Message&& msg, bool sendSync) const
	{
		MessageBroker::get().sendMessage(std::move(msg), sendSync, MessageBroker::DEST_BROADCAST);
	}

	inline bool MessageClient::isMessageSync() const
	{
		return MessageBroker::get().isMessageSync();
	}

	inline bool MessageBroker::subscribe(MessageClient& client)
	{
		EmbyThreading::ScopedMutex lock(m_mutex);

		if(client.getId() != MessageClient::ID_INVALID)
		{
			auto it = std::find_if(
					m_clients.begin(),
					m_clients.end(),
					[&](MessageClient* c)
					{
						return c->getId() == client.getId();
					});

			EmbyDebug_ASSERT(it == m_clients.end());
		}

		m_clients.push_back(&client);
		return true;
	}

	inline bool MessageBroker::unsubscribe(MessageClient const& client)
	{
		EmbyThreading::ScopedMutex lock(m_mutex);

		m_clients.erase(
				std::remove_if(m_clients.begin(), m_clients.end(),
						[&]( MessageClient* c)
						{
							return c == &client;
						}), m_clients.end());
		return false;
	}

	inline bool MessageBroker::isMessageSync() const
	{
		return m_isMsgSync;
	}

} // end of namespace EmbyExec

#endif // EMBY_BROKER_HH
///@}
