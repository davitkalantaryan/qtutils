//
// file:			cmdline.hpp
// path:			include/ui/cmdline.hpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/ui/resiziblewindow.hpp>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QTextEdit>

namespace qtutils{ namespace ui{


class CPPUTILS_DLL_PRIVATE CmdLine_p;


class QTUTILS_EXPORT CmdLine : public ::qtutils::ui::SizeApplyWindow<::qtutils::ui::ResizibleWindow<QWidget> >
{
public:
    virtual ~CmdLine() override;
    CmdLine();

    QTextEdit& textEdit();
    const QTextEdit& textEdit()const;

private:
    void ApplyNewSize(const QSize& a_newSize) override;

private:
    CmdLine_p*const   m_cml_data_p;
};


}}  //  namespace qtutils{ namespace ui{
