/*
 * Listener.hh
 *
 *  Created on: 21/ott/2016
 *      Author: mgarzola
 */

#ifndef LISTENER_HANDLER_HH_
#define LISTENER_HANDLER_HH_


#include <vector>

namespace EmbyExec
{
	template<typename T>
	class Listener
	{
		public:
		  virtual void onEvent(T t) = 0;
	};


	template<typename T>
	class ListenersHandler
	{
		public:

		  ListenersHandler(){};

		  bool addListener(T t)
		  {
			  m_listeners.push_back(t);
			  return true;
		  }

		  bool removeListener(T t)
		  {
			  //todo: implement me
			  return false;
		  }

		  std::vector<T> & getListeners()
		  {
			  return m_listeners;
		  }
		protected:
		  std::vector<T> m_listeners;
	};
}





#endif /* LISTENER_HH_ */
