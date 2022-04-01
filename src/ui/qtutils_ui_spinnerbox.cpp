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


void SpinnerBox::CenterSpinner(QWidget* a_pToCenter,int a_margins)
{
	QWidget*const pParent = parentWidget();
	const int marginsDouble = a_margins + a_margins;
	const int widthThis = a_pToCenter->width();
	const int heigthThis = a_pToCenter->height();
	int squareSize = (widthThis<heigthThis)?widthThis:heigthThis;
	int nOffsetX, nOffsetY;
	if(squareSize>=marginsDouble){squareSize-=marginsDouble;}
	
	nOffsetX = (widthThis-squareSize)>>1;
	nOffsetY = (heigthThis-squareSize)>>1;
	
	QPoint spinnerInTheParent;
	if(pParent){
		if(pParent==a_pToCenter){
			spinnerInTheParent = QPoint(nOffsetX,nOffsetY);
		}
		else{
			const QPoint spinnerInTheDesktop = a_pToCenter->mapToGlobal(QPoint(nOffsetX,nOffsetY));
			spinnerInTheParent = pParent->mapFromGlobal(spinnerInTheDesktop);
		}
	}
	else{
		spinnerInTheParent = a_pToCenter->mapToGlobal(QPoint(nOffsetX,nOffsetY));
	}
	
	this->resize(squareSize,squareSize);
	this->move(spinnerInTheParent);
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
