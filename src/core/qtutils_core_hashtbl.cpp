//
// file:            qtutils_core_hashtbl.cpp
// path:			src/core/qtutils_core_hashtbl.cpp
// created on:		2022 Jan 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/hashtbl.hpp>


namespace qtutils {  namespace hashtbl { 


size_t HashByQString::operator()(const QString& a_key)const
{
    return ::cpputils::hashtbl::FHashStr<char>().operator()(a_key.toStdString());
}


}}  // namespace qtutils {  namespace hashtbl { 
