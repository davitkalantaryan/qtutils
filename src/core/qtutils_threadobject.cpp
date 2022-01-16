//
// file:            focust_websockscontainer.cpp
// path:			src/core/focust_websockscontainer.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <focust/core/threadobject.hpp>


namespace focust{


QObject* ThreadObject::Sender() const
{
    return QObject::sender();
}


ThreadObject::~ThreadObject()
{
}


}  // namespace focust{
