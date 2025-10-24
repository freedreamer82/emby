
#if !defined( EMBY_EMBYTHREADWORKER_HH )
#define EMBY_EMBYTHREADWORKER_HH

namespace EmbyThreading
{
	class Worker
	{
		public:
            virtual ~Worker();
			void virtual doWork() = 0;

		protected:
			Worker() {}
	};
}

#endif
