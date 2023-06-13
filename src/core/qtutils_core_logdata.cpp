//
// file:            focust_core_logdata.cpp
// path:			src/core/focust_core_logdata.cpp
// created on:		2023 Apr 23
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/logdata.hpp>


QTUTILS_CORE_NTDT_NSP_P1


QtutilsLogDataRaw::QtutilsLogDataRaw(const QMessageLogContext& a_ctx)
    :
      ctx(a_ctx.file,a_ctx.line,a_ctx.function,a_ctx.category)
{
    if(this->ctx.category){
        this->categoryStr = ::std::string(this->ctx.category);
    }
    this->ctx.category = this->categoryStr.c_str();
}


QtutilsLogDataRaw::QtutilsLogDataRaw(const QMessageLogContext& a_ctx, const ::std::string& a_category)
    :
      ctx(a_ctx.file,a_ctx.line,a_ctx.function,a_ctx.category),
      categoryStr(a_category)
{
    this->ctx.category = this->categoryStr.c_str();
}


/*//////////////////////////////////////////////////////////////////////////////////////////////////////*/

QtutilsLogData::QtutilsLogData(QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg)
    :
      ::std::shared_ptr<QtutilsLogDataRaw>(new QtutilsLogDataRaw(a_ctx))
{
    static int snIsNotInited = 1;
    if(snIsNotInited){
        qRegisterMetaType< QTUTILS_CORE_NTDT_NSP QtutilsLogData >( "qtutils_QtutilsLogData" );
        snIsNotInited = 0;
    }

    QtutilsLogDataRaw* this_p = get();
    this_p->msgType = a_msgType;
    this_p->msg = a_msg;
}


QtutilsLogData::QtutilsLogData(QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg, const ::std::string& a_category)
    :
      ::std::shared_ptr<QtutilsLogDataRaw>(new QtutilsLogDataRaw(a_ctx,a_category))
{
    QtutilsLogDataRaw* this_p = get();
    this_p->msgType = a_msgType;
    this_p->msg = a_msg;
}


QTUTILS_CORE_NTDT_NSP_P2
