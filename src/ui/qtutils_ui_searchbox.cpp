//
// file:            qtutils_ui_searchbox.cpp
// path:			src/ui/qtutils_ui_searchbox.cpp
// created on:		2022 Sep 26
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/searchbox.hpp>
#include <qtutils/ui/global_functions.hpp>
#include <qtutils/core/logger.hpp>
#include <assert.h>
#include <qtutils/disable_utils_warnings.h>
#include <QLabel>
#include <QMouseEvent>


namespace qtutils { namespace ui{


class CPPUTILS_DLL_PRIVATE SearchBox_p final
{
public:
    SearchBox_p(SearchBox* a_pParent);

public:
    SearchBox*const     m_pParent;
    QLineEdit           m_lnEdit;
};

/*////////////////////////////////////////////////////////////////////////////////////*/


SearchBox::~SearchBox()
{
    delete m_sb_data_p;
}


SearchBox_p* SearchBox::Init(SearchBox* a_pThis)
{
    SearchBox_p* pSBdata = new SearchBox_p(a_pThis);
    return pSBdata;
}


void SearchBox::SetNewSearchItemImage(const QString& a_imgPath)
{
    QLabel* pSrchBox = wgtInLeft();
    assert(pSrchBox);
    pSrchBox->setPixmap( QPixmap( a_imgPath ) );
}


QLineEdit* SearchBox::lnEdit()const
{
    return &(m_sb_data_p->m_lnEdit);
}


void SearchBox::ApplyNewSize(const QSize& a_newSize)
{
    WgtWithWgtInLeft<QWidget,QLabel>::ApplyNewSize(a_newSize);
    const int cnIconHeight(m_pWgtInLeft->height());
    m_sb_data_p->m_lnEdit.resize(a_newSize.width()-cnIconHeight-2,a_newSize.height()-2);
    m_sb_data_p->m_lnEdit.move(cnIconHeight+1,1);
}


/*////////////////////////////////////////////////////////////////////////////////////*/

#define QU_SB_BACKGROUND_COLOR_STR  "white;"
#define QU_SB_LINE_EDIT_STYLESHEET  \
    "background-color: " QU_SB_BACKGROUND_COLOR_STR         \
    "border: 0px solid " QU_SB_BACKGROUND_COLOR_STR
#define QU_SB_WHOLE_STYLESHEET  \
    "background-color: " QU_SB_BACKGROUND_COLOR_STR         \
    "border: 1px solid black;"                              \
    "border-style: inset;"

SearchBox_p::SearchBox_p(SearchBox* a_pParent)
    :
      m_pParent(a_pParent),
      m_lnEdit(a_pParent)
{
    QLabel*const pSrchBox = new QLabel;
    SetProperParamsToImagedLbl(pSrchBox);
    pSrchBox->setPixmap( QPixmap( ":/qtutils/search_box01.png" ) );
    static_cast<WgtWithWgtInLeft<QWidget,QLabel>*>(m_pParent)->SetWgtInLeft(pSrchBox);
    const QSize szHint(m_lnEdit.sizeHint());
    m_pParent->setFixedSize(szHint.width()*5/4+4,szHint.height()+4);
    m_lnEdit.setStyleSheet(QU_SB_LINE_EDIT_STYLESHEET);
    m_pParent->setStyleSheet(QU_SB_WHOLE_STYLESHEET);
    pSrchBox->setStyleSheet("border: 0px solid " QU_SB_BACKGROUND_COLOR_STR);
}



}}  // namespace qtutils { namespace ui{
