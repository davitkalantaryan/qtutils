//
// file:            qtutils_ui_spinnerbox.cpp
// path:			src/ui/qtutils_ui_spinnerbox.cpp
// created on:		2022 Apr 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
// 
// used docu:		https://stackoverflow.com/questions/5653114/display-image-in-qt-to-fit-label-size
//

#include <qtutils/ui/spinnerbox.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QMovie>


namespace qtutils { namespace ui{

class CPPUTILS_DLL_PRIVATE SpinnerBox_p
{
public:
	SpinnerBox_p();
	
public:
	QMovie	m_movie;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

SpinnerBox::~SpinnerBox()
{
	m_spbox_data_p->m_movie.stop();
	delete m_spbox_data_p;
}


void SpinnerBox::StartSpinning()
{
	m_spbox_data_p->m_movie.start();
}


void SpinnerBox::StopSpinning()
{
	m_spbox_data_p->m_movie.stop();
}


SpinnerBox_p* SpinnerBox::CreateSpinnerBox_p(SpinnerBox* a_pThis)
{
	SpinnerBox_p* pRet = new SpinnerBox_p();
	a_pThis->setMovie(&(pRet->m_movie));
	a_pThis->setScaledContents( true );
	a_pThis->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	return pRet;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

SpinnerBox_p::SpinnerBox_p()
	:
	  m_movie(":/img/qtutils_loader01.gif")
{
}


}}
