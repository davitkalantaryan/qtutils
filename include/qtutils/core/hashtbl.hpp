//
// file:            hashtbl.hpp
// path:			include/qtutils/core/hashtbl.hpp
// created on:		2022 Jan 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <cpputils/hashtbl.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QString>


namespace qtutils {  namespace hashtbl { 


class HashByQString 
{
public:
	size_t	operator()(const QString& key)const;
};

template <typename DataType,typename Hash=HashByQString,size_t templateDefaultSize=DEFAULT_TABLE_SIZE >
using QStrHash = ::cpputils::hashtbl::Base< QString,DataType,Hash,templateDefaultSize >;


}}  // namespace qtutils {  namespace hashtbl {
