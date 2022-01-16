//
// file:            qtutils_websockscontainer.cpp
// path:			src/core/qtutils_websockscontainer.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/threadobject.hpp>


namespace qtutils{


QObject* ThreadObject::Sender() const
{
    return QObject::sender();
}


ThreadObject::~ThreadObject()
{
}


}  // namespace qtutils{
