
#if !defined( EMBYLIBS_SINGLETON_HH )
#define EMBYLIBS_SINGLETON_HH

namespace EmbyLibs
{

    /** Singleton
     *
     * This is the singleton template, just to avoid to rewrite singleton logic
     * every time.
     *
     * To make your class 'A' a singleton you have to:
     * @code
     * class A : public EmbyLibs::Singleton<A>
     * {
     *     // class A definition
     * };
     * @endcode
     */

    template<class T> class Singleton
    {
        public:
            static T& get();
        protected:
            Singleton() {};
            virtual ~Singleton() {};
        private:
            Singleton( const Singleton& copy );
            Singleton& operator=( const Singleton& copy );
    };

    template<class T> T&
    Singleton<T>::get()
    {
        static T instance;
        return instance;
    }

}   // namespace EmbyLibs

#endif // EMBYLIBS_SINGLETON_HH
