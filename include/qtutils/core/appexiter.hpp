//
// file:            appexiter.hpp
// path:			include/qtutils/core/appexiter.hpp
// created on:		2023 May 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <memory>


namespace qtutils { namespace core{


class QTUTILS_EXPORT AppExiter
{
public:
	~AppExiter();
};


class QTUTILS_EXPORT AppExiterPtr : public ::std::shared_ptr<AppExiter>
{
public:
    AppExiterPtr();
};


}}  // namespace qtutils { namespace core{
