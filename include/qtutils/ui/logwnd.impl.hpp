//
// file:			logwnd.impl.hpp
// path:			include/ui/logwnd.impl.hpp
// created on:		2022 Oct 14
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_HPP
#include "logwndqu.hpp"
#endif


namespace qtutils { namespace ui{ namespace logger{


template<typename... Targs>
LogWnd::LogWnd(Targs... a_args)
    :
      ::qtutils::ui::SizeApplyWindow<QWidget>(a_args...),
      m_logwnd_data_p(ConstructLogger(this))
{
}


}}}  //  namespace qtutils { namespace ui{ namespace logger{

#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_IMPL_HPP
