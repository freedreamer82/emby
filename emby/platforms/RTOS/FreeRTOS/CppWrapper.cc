
#include <FreeRTOS.h>
#include <new>          // std::bad_alloc

/*
* Define the 'new' operator for C++ to use the freeRTOS memory management
*/
void *operator new(size_t size)
{
    void *p=pvPortMalloc(size);
#ifdef	__EXCEPTIONS
    if (p==0) // did pvPortMalloc succeed?
        throw std::bad_alloc(); // ANSI/ISO compliant behavior
#endif
    return p;
}

/*
* Define the 'delete' operator for C++ to use the freeRTOS memory management
*/
void operator delete(void *p)
{
    vPortFree( p );
}

void operator delete(void* p, size_t )
{
    vPortFree( p );
}

void *operator new[](size_t size)
{
    void *p=pvPortMalloc(size);
#ifdef	__EXCEPTIONS
    if (p==0) // did pvPortMalloc succeed?
        throw std::bad_alloc(); // ANSI/ISO compliant behavior
#endif
    return p;
}


void operator delete[](void *p)
{
    vPortFree( p );
}

void operator delete[](void* p, size_t )
{
    vPortFree( p );
}

extern "C"
{
int atexit(void (*)(void))
{
    return 0;
}
}
