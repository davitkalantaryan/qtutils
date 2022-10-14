//
// file:			qtutils_ui_cmdline.cpp
// path:			src/ui/qtutils_ui_cmdline.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/ui/cmdline.hpp>
//#include <qtutils/disable_utils_warnings.h>

namespace qtutils{ namespace ui{


class CPPUTILS_DLL_PRIVATE CmdLine_p
{
public:
    CmdLine_p(CmdLine* a_pParent);

    void ConnectSignals();

public:
    CmdLine*const   m_pParent;
    QTextEdit       m_edit;
};


/*/////////////////////////////////////////////////////////////////////////////////*/


CmdLine::~CmdLine()
{
    delete m_cml_data_p;
}


CmdLine::CmdLine()
    :
      m_cml_data_p(new CmdLine_p(this))
{
}


QTextEdit& CmdLine::textEdit()
{
    return m_cml_data_p->m_edit;
}


const QTextEdit& CmdLine::textEdit()const
{
    return m_cml_data_p->m_edit;
}


void CmdLine::ApplyNewSize(const QSize& a_newSize)
{
    m_cml_data_p->m_edit.resize(a_newSize);
}


/*/////////////////////////////////////////////////////////////////////////////////*/

CmdLine_p::CmdLine_p(CmdLine* a_pParent)
    :
      m_pParent(a_pParent),
      m_edit(a_pParent)
{
    ConnectSignals();
}


void CmdLine_p::ConnectSignals()
{
    //
}


}}  //  namespace qtutils{ namespace ui{
