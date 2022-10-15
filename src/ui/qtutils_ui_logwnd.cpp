//
// file:			qtutils_ui_logwnd.cpp
// path:			src/ui/qtutils_ui_logwnd.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/ui/logwnd.hpp>
#include <qtutils/core/settings.hpp>
#include <cpputils/hash/dllhash.hpp>
#include <list>
#include <memory>
#include <stdint.h>
#include <assert.h>
#include <qtutils/disable_utils_warnings.h>
#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>


namespace qtutils{ namespace ui{

class CPPUTILS_DLL_PRIVATE CategoryData;

class CPPUTILS_DLL_PRIVATE QtutilsUiLogwndIniter{
public:
    QtutilsUiLogwndIniter();
}static s_qtutilsUiLogwndIniter;

typedef ::cpputils::hash::DllHash<QString,::std::shared_ptr<CategoryData> >    HashCategories;

static constexpr size_t s_cunNumberOfLogTypes = static_cast<size_t>(LogWnd::LogTypes::Count);
static QColor           s_defaultColors[s_cunNumberOfLogTypes] = {
    QColor(0,0,190),
    QColor(0,190,0),
    QColor(100,100,0),
    QColor(190,0,0),
};


//#define QTUTILS_UI_LOGWND_BIT_VALUE(_val,_bitNum)   ( ((_val)>>(static_cast<uint32_t>(_bitNum)))&1 )

template <typename BitNumType>
inline bool QTUTILS_UI_LOGWND_BIT_VALUE(uint32_t a_val,const BitNumType& a_bitNum){
    const uint32_t cunVal = (a_val >> (static_cast<uint32_t>(a_bitNum))) & 1;
    return cunVal?true:false;
}

inline void QTUTILS_UI_LOGWND_SET_BIT_VALUE(uint32_t* a_pVal, const LogWnd::LogTypes& a_bitNum, bool a_newVal){
    uint32_t mask = 1<<(static_cast<uint32_t>(a_bitNum));
    if(a_newVal){
        *a_pVal |= mask;
    }
    else{
        mask = ~mask;
        *a_pVal &= mask;
    }
}


class CPPUTILS_DLL_PRIVATE CategoryData final
{
public:
    CategoryData(const QString& a_categoryName, LogWnd_p* a_logwnd_data_p);

private:
    void ConnectSignals();

public:
    const QString       m_categoryName;
    QWidget             m_categoryControlWgt;
    QLabel              m_categoryNameLbl;
    QCheckBox           m_logTypesEnabledCB[s_cunNumberOfLogTypes];
    QColor              m_colors[s_cunNumberOfLogTypes];

    union{
        uint64_t all;
        struct{
            uint32_t isEnabledVect;
            uint32_t shouldKeep         : 1;
            uint32_t reserved01         : 31;
        }b;
    }m_flags;

private:
    LogWnd_p*const      m_logwnd_data_p;

};


struct CPPUTILS_DLL_PRIVATE SLogStr{
    ::std::shared_ptr<CategoryData> ctg;
    size_t                          lgtp;
    QString                         msg;
};


class CPPUTILS_DLL_PRIVATE LogWnd_p final
{
public:
    LogWnd_p(LogWnd* a_pParent);

    void ConnectSignals();
    inline void InitAndShowBase();
    inline void ClearExtraLogs();
    inline void CategoryVisibilityChanged(::std::list<SLogStr>::const_iterator a_iter);
    inline void ApplyNewSize(const QSize& a_newSize);
    inline void ClearAllCategories();

public:
    LogWnd*const            m_pParent;
    HashCategories          m_categories;
    QTextEdit               m_edit;
    QString                 m_settingsKey;
    ::std::list<SLogStr>    m_logs;
    size_t                  m_unMaxNumberOfLogs;

    union{
        uint64_t all;
        struct{
            uint64_t instanceNumber : 32;
            uint64_t reserved01     : 32;
        }b;
    }m_flags;

private:
    static uint64_t  ms_nInstances;
};


uint64_t LogWnd_p::ms_nInstances = 0;

/*/////////////////////////////////////////////////////////////////////////////////*/


LogWnd::~LogWnd()
{
    m_logwnd_data_p->ClearAllCategories();

    delete m_logwnd_data_p;
}


LogWnd_p* LogWnd::ConstructLogger(LogWnd* a_pLogWnd)
{
    return new LogWnd_p(a_pLogWnd);
}

void LogWnd::ApplyNewSize(const QSize& a_newSize)
{
    m_logwnd_data_p->ApplyNewSize(a_newSize);
}


void LogWnd::showEvent(QShowEvent* a_event)
{
    m_logwnd_data_p->InitAndShowBase();
    ::qtutils::ui::SizeApplyWindow<QWidget>::showEvent(a_event);
}


void LogWnd::InitSettings()
{
    m_logwnd_data_p->InitAndShowBase();
}


const QColor* LogWnd::defaultColors()
{
    return s_defaultColors;
}


static inline void SetColorsInline(const QColor* a_newColors, size_t a_count, size_t a_offset, QColor a_colorsDest[s_cunNumberOfLogTypes]){
    for(size_t indArg(0), indArray(a_offset); (indArg<a_count)&&(indArray<s_cunNumberOfLogTypes);++indArg,++indArray){
        a_colorsDest[indArray] = a_newColors[indArg];
    }
}


void LogWnd::SetDefaultColors(const QColor* a_newColors, size_t a_count, size_t a_offset)
{
    SetColorsInline(a_newColors,a_count,a_offset,s_defaultColors);
}


void LogWnd::SetDefaultColor(const LogTypes& a_type, const QColor& a_newColor)
{
    const size_t cunIndex = QTUTILS_UI_LOGWND_TYPE_TO_INDEX(a_type);
    assert(cunIndex<QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Count));

    s_defaultColors[cunIndex] = a_newColor;
}


void LogWnd::SetCategoryColors(const QString& a_categoryName, const QColor* a_newColors, size_t a_count, size_t a_offset)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName);
    if(citer==HashCategories::s_constNullIter){return;}
    CategoryData* pCategoryData = citer->second.get();

    SetColorsInline(a_newColors,a_count,a_offset,pCategoryData->m_colors);
}


void LogWnd::SetCategoryColors(const QString& a_categoryName, const LogTypes& a_type, const QColor& a_newColor)
{
    const size_t cunIndex = QTUTILS_UI_LOGWND_TYPE_TO_INDEX(a_type);
    assert(cunIndex<QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Count));

    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName);
    if(citer==HashCategories::s_constNullIter){return;}
    CategoryData* pCategoryData = citer->second.get();

    pCategoryData->m_colors[cunIndex] = a_newColor;
}


void LogWnd::AddLogCategory(const QString& a_categoryName)
{
    size_t unHash;
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName,&unHash);
    if(citer!=HashCategories::s_constNullIter){return;} // category is already there

    ::std::shared_ptr<CategoryData> aCategoryData = ::std::shared_ptr<CategoryData>(new CategoryData(a_categoryName,m_logwnd_data_p));
    m_logwnd_data_p->m_categories.AddEntryWithKnownHashC(::std::pair<QString,::std::shared_ptr<CategoryData> >(a_categoryName,aCategoryData),unHash);
    ApplyNewSize(size());

}


void LogWnd::RemoveCategory(const QString& a_categoryName)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName);
    if(citer==HashCategories::s_constNullIter){return;} // category is not there
    CategoryData* pCategoryData = citer->second.get();

    pCategoryData->m_flags.b.shouldKeep = 0;
    m_logwnd_data_p->m_categories.RemoveEntryRaw(citer);
    m_logwnd_data_p->CategoryVisibilityChanged(m_logwnd_data_p->m_logs.cbegin());
    ApplyNewSize(size());
}


void LogWnd::ClearAllCategories()
{
    m_logwnd_data_p->ClearAllCategories();
    m_logwnd_data_p->CategoryVisibilityChanged(m_logwnd_data_p->m_logs.cbegin());
    m_logwnd_data_p->ApplyNewSize(size());
}


static inline size_t QtMsgTypeToIndex(const QtMsgType& a_msgType){
    // enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtInfoMsg, QtSystemMsg = QtCriticalMsg };
    switch(a_msgType){
    case QtDebugMsg:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug);
    case QtWarningMsg:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning);
    case QtInfoMsg:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info);
        break;
    default:
        break;
    }  //  switch(a_msgType){
    return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error);
}


void LogWnd::AddLog(const QString& a_categoryName, QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg)
{
    static_cast<void>(a_ctx);  // for now this argument is not used

    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName);
    if(citer==HashCategories::s_constNullIter){return;}
    const CategoryData* pCategoryData = citer->second.get();

    const size_t cunIndex = QtMsgTypeToIndex(a_msgType);
    const uint32_t isEnabled = QTUTILS_UI_LOGWND_BIT_VALUE(pCategoryData->m_flags.b.isEnabledVect,cunIndex);
    if(isEnabled){
        const QColor aColor = pCategoryData->m_colors[cunIndex];
        m_logwnd_data_p->m_edit.setTextColor(aColor);
        m_logwnd_data_p->m_edit.append(a_msg);
        m_logwnd_data_p->m_logs.push_back({citer->second,cunIndex,a_msg});
        m_logwnd_data_p->ClearExtraLogs();
    }
    else if(pCategoryData->m_flags.b.shouldKeep){
        m_logwnd_data_p->m_logs.push_back({citer->second,cunIndex,a_msg});
        m_logwnd_data_p->ClearExtraLogs();
    }
}


void LogWnd::SetMaxNumberOfLogs(size_t a_unMaxNumberOfLogs)
{
    m_logwnd_data_p->m_unMaxNumberOfLogs = a_unMaxNumberOfLogs;
}


/*/////////////////////////////////////////////////////////////////////////////////*/

LogWnd_p::LogWnd_p(LogWnd* a_pParent)
    :
      m_pParent(a_pParent),
      m_edit(a_pParent)
{
    m_flags.all = 0;
    m_flags.b.instanceNumber = ms_nInstances++;
    m_unMaxNumberOfLogs = QTUTILS_UI_LOGWND_DEFAULT_MAX_NUMBER_OF_LOGS;
    m_edit.setReadOnly(true);
    InitAndShowBase();
    ConnectSignals();
}


void LogWnd_p::ConnectSignals()
{
    // inline void ResizibleWindowRaw<WidgetType>::InitAndShowBase()
    // m_settingsKey = typeid(*this).name()+QString::number(m_flags.b.instanceNumber);
}


inline void LogWnd_p::InitAndShowBase()
{
    m_settingsKey = typeid(*this).name()+QString::number(m_flags.b.instanceNumber);
}


inline void LogWnd_p::ClearExtraLogs()
{
    size_t unCurSize = m_logs.size();
    if(unCurSize<m_unMaxNumberOfLogs){return;}

    unCurSize /= 2;

    ::std::list<SLogStr>::const_iterator iterTmp, iter = m_logs.cbegin();

    for(size_t i(0);i<unCurSize;++i){
        iterTmp = iter++;
        m_logs.erase(iterTmp);
    }

    CategoryVisibilityChanged(iter);
}


inline void LogWnd_p::CategoryVisibilityChanged(::std::list<SLogStr>::const_iterator a_iter)
{
    CategoryData* pCategoryData;
    uint32_t isEnabled;
    m_edit.clear();
    ::std::list<SLogStr>::const_iterator iterTmp;

    for(; a_iter!=m_logs.cend();){

        const SLogStr& lgDt = *a_iter;
        pCategoryData = lgDt.ctg.get();

        if(pCategoryData->m_flags.all == 0){
            iterTmp = a_iter++;
            m_logs.erase(iterTmp);
            continue;
        }

        isEnabled = QTUTILS_UI_LOGWND_BIT_VALUE(pCategoryData->m_flags.b.isEnabledVect,lgDt.lgtp);
        if(isEnabled){
            const QColor aColor = pCategoryData->m_colors[lgDt.lgtp];
            m_edit.setTextColor(aColor);
            m_edit.append(lgDt.msg);
        }

        ++a_iter;
    }
}


inline void LogWnd_p::ApplyNewSize(const QSize& a_newSize)
{
    int nY = 3;
    CategoryData* pCategoryData;
    HashCategories::const_iterator citer = m_categories.begin();

    for(;citer!=HashCategories::s_constNullIter;++citer){
        pCategoryData = citer->second.get();
        pCategoryData->m_categoryControlWgt.move(3,nY);
        nY += pCategoryData->m_categoryControlWgt.height();
    }

    nY += 3;
    m_edit.move(0,nY++);
    m_edit.setFixedSize(a_newSize.width(),a_newSize.height()-nY);
    m_pParent->setMinimumHeight(nY);
}


inline void LogWnd_p::ClearAllCategories()
{
    m_categories.clear();
}


/*/////////////////////////////////////////////////////////////////////////////////*/

CategoryData::CategoryData(const QString& a_categoryName, LogWnd_p* a_logwnd_data_p)
    :
      m_categoryName(a_categoryName),
      m_categoryControlWgt(a_logwnd_data_p->m_pParent),
      m_logwnd_data_p(a_logwnd_data_p)
{
    m_flags.all = 0;
    m_flags.b.shouldKeep = 1;
    ::qtutils::Settings aSettings;
    QString settingsKey;
    bool isEnabled;

    ConnectSignals();
    SetColorsInline(s_defaultColors,s_cunNumberOfLogTypes,0,m_colors);

    int nMaxHeight, nNextHeight;
    int nX = 10;

    m_categoryNameLbl.setParent(&m_categoryControlWgt);
    m_categoryNameLbl.setText(a_categoryName + ":");
    m_categoryNameLbl.setFixedSize(150,m_categoryNameLbl.sizeHint().height());
    m_categoryNameLbl.setAlignment(Qt::AlignVCenter);
    m_categoryNameLbl.move(nX,0);
    nX += m_categoryNameLbl.width();
    nMaxHeight = m_categoryNameLbl.height();

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Debug/isEnabled";
    isEnabled = aSettings.value(settingsKey,true).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Debug,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].setText("Debug");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Info/isEnabled";
    isEnabled = aSettings.value(settingsKey,true).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Info,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].setText("Info");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Warning/isEnabled";
    isEnabled = aSettings.value(settingsKey,true).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Warning,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].setText("Warning");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Error/isEnabled";
    isEnabled = aSettings.value(settingsKey,true).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Error,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].setText("Error");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    m_categoryControlWgt.setMinimumWidth(nX);
    m_categoryControlWgt.setFixedHeight(nMaxHeight);
    m_categoryControlWgt.setVisible(true);
}


void CategoryData::ConnectSignals()
{
    QCheckBox* pNextCheckBox;

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        const bool isChecked = QTUTILS_UI_LOGWND_BIT_VALUE(m_flags.b.isEnabledVect,LogWnd::LogTypes::Debug);
        if(isChecked!=isCheckedByGui){
            QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Debug,isCheckedByGui);
            m_logwnd_data_p->CategoryVisibilityChanged(m_logwnd_data_p->m_logs.begin());
        }
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        const bool isChecked = QTUTILS_UI_LOGWND_BIT_VALUE(m_flags.b.isEnabledVect,LogWnd::LogTypes::Info);
        if(isChecked!=isCheckedByGui){
            QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Info,isCheckedByGui);
            m_logwnd_data_p->CategoryVisibilityChanged(m_logwnd_data_p->m_logs.begin());
        }
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        const bool isChecked = QTUTILS_UI_LOGWND_BIT_VALUE(m_flags.b.isEnabledVect,LogWnd::LogTypes::Warning);
        if(isChecked!=isCheckedByGui){
            QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Warning,isCheckedByGui);
            m_logwnd_data_p->CategoryVisibilityChanged(m_logwnd_data_p->m_logs.begin());
        }
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        const bool isChecked = QTUTILS_UI_LOGWND_BIT_VALUE(m_flags.b.isEnabledVect,LogWnd::LogTypes::Error);
        if(isChecked!=isCheckedByGui){
            QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogWnd::LogTypes::Error,isCheckedByGui);
            m_logwnd_data_p->CategoryVisibilityChanged(m_logwnd_data_p->m_logs.begin());
        }
    });
}


/*/////////////////////////////////////////////////////////////////////////////////*/

QtutilsUiLogwndIniter::QtutilsUiLogwndIniter()
{
    ::qtutils::Settings aSettings;
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Debug)]    = aSettings.value("QtutilsUiLogwndGlobalColors/Debug",   QColor(0,0,190))  .value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Info)]     = aSettings.value("QtutilsUiLogwndGlobalColors/Info",    QColor(0,190,0))  .value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Warning)]  = aSettings.value("QtutilsUiLogwndGlobalColors/Warning", QColor(100,100,0)).value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogWnd::LogTypes::Error)]    = aSettings.value("QtutilsUiLogwndGlobalColors/Error",   QColor(190,0,0))  .value<QColor>();
}


}}  //  namespace qtutils{ namespace ui{
