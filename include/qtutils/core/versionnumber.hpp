//
// repo:            qtutils
// file:            versionnumber.hpp
// path:			include/qtutils/core/versionnumber.hpp
// created on:		2023 Jul 08
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QVersionNumber>
#include <QList>
#include <QVarLengthArray>


namespace qtutils { namespace core{


class QTUTILS_EXPORT VersionNumber
{
public:
    virtual ~VersionNumber();
    VersionNumber();
    VersionNumber(const QVersionNumber& a_qvn);
    VersionNumber(const VersionNumber& a_cM);
    VersionNumber(VersionNumber&& a_mM);
    VersionNumber& operator=(const VersionNumber& a_cM);
    VersionNumber& operator=(VersionNumber&& a_mM);
    void CopyFrom(const VersionNumber& a_cM);
    void ReplaceWithOther(VersionNumber* CPPUTILS_ARG_NN a_pmM);
    void ReplaceWithOther(VersionNumber&& a_mM);
    QVersionNumber* qVersionNumber()const;
    QVersionNumber* operator->()const;
    const QVersionNumber& operator*()const;

protected:
    QVersionNumber*     m_vn_data_p;
};


}}  //  namespace qtutils { namespace core{
