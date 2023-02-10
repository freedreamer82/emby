/*
 * SystemErrorHandler.hh
 *
 *  Created on: 20/mar/2015
 *      Author: mgarzola
 */

#include <EmbySystem/SystemErrorHandler.hh>

namespace EmbySystem
{
    SystemErrorHandler::~SystemErrorHandler()
    {
    }

    void SystemErrorHandler::doWork()
    {
        handler();
    }
}
