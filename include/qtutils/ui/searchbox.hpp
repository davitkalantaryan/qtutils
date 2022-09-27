//
// file:            searchbox.hpp
// path:			include/qtutils/ui/searchbox.hpp
// created on:		2022 Sep 26
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_HPP


#include <qtutils/export_symbols.h>
#include <qtutils/ui/wgtwithwgtinleft.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QLineEdit>
#include <QLabel>


namespace qtutils { namespace ui{

class CPPUTILS_DLL_PRIVATE SearchBox_p;

class QTUTILS_EXPORT SearchBox : public WgtWithWgtInLeft<QWidget,QLabel>
{
public:
    template<typename... Targs>
    SearchBox(Targs... a_args);
    virtual ~SearchBox() override;

    void SetNewSearchItemImage(const QString& a_imgPath);
    QLineEdit* lnEdit()const;

protected:
    virtual void ApplyNewSize(const QSize& a_newSize) override;

private:
    void SetWgtInLeft(QLabel* a_pLabelInRight) = delete;
    static SearchBox_p* Init(SearchBox* a_pThis);

private:
    SearchBox_p*const   m_sb_data_p;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_IMPL_HPP
#include "searchbox.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_HPP
