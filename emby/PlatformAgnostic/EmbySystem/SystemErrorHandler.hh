/*
 * SystemErrorHandler.hh
 *
 *  Created on: 20/mar/2015
 *      Author: mgarzola
 */

#ifndef SYSTEMERRORHANDLER_HH_
#define SYSTEMERRORHANDLER_HH_



namespace EmbySystem
{
	class SystemErrorHandler
	{
		public:
	        void doWork();

		protected:
			virtual void handler() = 0;
			virtual ~SystemErrorHandler();
	};

}


#endif /* SYSTEMERRORHANDLER_HH_ */
