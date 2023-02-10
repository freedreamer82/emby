/*
 * Callback.hh
 *
 *  Created on: 23/feb/2017
 *      Author: mgarzola
 */

#ifndef Callback_HH_
#define Callback_HH_

/**
 *  @class FP
 *  @brief API for managing Function Pointers
 *
 *  Callback
 *
 */
namespace EmbyLibs
{
	template<typename retT, typename ... argT>
	class Callback
	{
	public:
		/** Create the Callback object - only one callback can be attached to the object, that is
		 *  a member function or a global function, not both at the same time
		 */
		Callback()
		{
			obj_callback = 0;
			method_callback = 0;
			c_callback = 0;
		}

		/** Add a callback function to the object
		 *  @param item - Address of the initialized object
		 *  @param member - Address of the member function (dont forget the scope that the function is defined in)
		 */
		template<class T>
		void attach(T *item, retT (T::*method)(argT...))
		{
			obj_callback = (FPtrDummy *)(item);
			method_callback = (retT (FPtrDummy::*)(argT...))(method);
			return;
		}

		/** Add a callback function to the object
		 *  @param function - The address of a globally defined function
		 */
		void attach(retT (*function)(argT...))
		{
			c_callback = function;
		}

		/** Invoke the function attached to the class
		 *  @param arg - An argument that is passed into the function handler that is called
		 *  @return The return from the function hanlder called by this class
		 */
		retT operator()(argT ... arg) const
		{
			if( c_callback && !obj_callback )
			{
				return (*c_callback)(arg ...);
			}
			else if (obj_callback && method_callback)
			{
				return (obj_callback->*method_callback)(arg ...);
			}
			return (retT)0;
		}

		/** Invoke the function attached to the class
		 *  @param arg - An argument that is passed into the function handler that is called
		 *  @return The return from the function hanlder called by this class
		 */
		retT callback(argT ... arg) const
		{
			if( c_callback && !obj_callback  )
			{
				return (*c_callback)(arg ...);
			}
			else if (obj_callback && method_callback )
			{
				return (obj_callback->*method_callback)(arg ...);
			}
			return (retT)0;
		}

		/** Determine if an callback is currently hooked
		 *  @return 1 if a method is hooked, 0 otherwise
		 */
		bool isAttached()
		{
			return obj_callback || c_callback;
		}

		/** Release a function from the callback hook
		 */
		void detach()
		{
			obj_callback = 0;
			c_callback = 0;
		}

		template<class T>
		bool match(T *item, retT (T::*method)(argT...))
		{
			return  obj_callback	== (FPtrDummy *)(item) &&
					method_callback == (retT (FPtrDummy::*)(argT...))(method);
		}

		bool match(retT (*function)(argT...))
		{
			return c_callback == function;
		}

	private:

		// empty type used for casting
		class FPtrDummy;

		FPtrDummy *obj_callback;

		/**
		 *  @union Function
		 *  @brief Member or global callback function
		 */
		union {
			retT (*c_callback)(argT...);                   /*!< Footprint for a global function */
			retT (FPtrDummy::*method_callback)(argT...);   /*!< Footprint for a member function */
		};
	};
}

#endif /* Callback_HH_ */
