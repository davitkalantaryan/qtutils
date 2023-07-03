//
// file:			qtutils_ui_logwnd.cpp
// path:			src/ui/qtutils_ui_logwnd.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/ui/logwndqu.hpp>
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

static constexpr size_t s_cunNumberOfLogTypes = static_cast<size_t>(LogTypes::Count);
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

inline void QTUTILS_UI_LOGWND_SET_BIT_VALUE(uint32_t* a_pVal, const LogTypes& a_bitNum, bool a_newVal){
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
    CategoryData(const QString& a_categoryName, LogWnd_p* a_logwnd_data_p, bool a_defaultEnable);

    inline void SetTypeEnable(const LogTypes& a_type, bool a_isEnable);

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
    inline void EmitCategoryTypeChange(const QString& a_categoryName, const LogTypes& a_type, bool a_isEnabled);

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


void LogWnd::AddLogCategory(const QString& a_categoryName, bool a_defaultEnable)
{
    size_t unHash;
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName,&unHash);
    if(citer!=HashCategories::s_constNullIter){return;} // category is already there

    ::std::shared_ptr<CategoryData> aCategoryData = ::std::shared_ptr<CategoryData>(new CategoryData(a_categoryName,m_logwnd_data_p,a_defaultEnable));
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


void LogWnd::EnableCategoryType(const QString& a_categoryName, const LogTypes& a_type)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName);
    if(citer==HashCategories::s_constNullIter){return;}
    CategoryData* pCategoryData = citer->second.get();
    pCategoryData->SetTypeEnable(a_type,true);
}


void LogWnd::DisableCategoryType(const QString& a_categoryName, const LogTypes& a_type)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName);
    if(citer==HashCategories::s_constNullIter){return;}
    CategoryData* pCategoryData = citer->second.get();
    pCategoryData->SetTypeEnable(a_type,false);
}


bool LogWnd::isEnabledCategoryType(const QString& a_categoryName, const LogTypes& a_type)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(a_categoryName);
    if(citer==HashCategories::s_constNullIter){return false;}
    CategoryData* pCategoryData = citer->second.get();
    return QTUTILS_UI_LOGWND_BIT_VALUE(pCategoryData->m_flags.b.isEnabledVect,a_type);
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
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug);
    case QtWarningMsg:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning);
    case QtInfoMsg:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info);
        break;
    default:
        break;
    }  //  switch(a_msgType){
    return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error);
}


void LogWnd::AddLog(QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg)
{
    static_cast<void>(a_ctx);  // for now this argument is not used

    if(!a_ctx.category){
        return;
    }

    const QString categoryName = QString(a_ctx.category);

    const HashCategories::const_iterator citer = m_logwnd_data_p->m_categories.find(categoryName);
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


inline void LogWnd_p::EmitCategoryTypeChange(const QString& a_categoryName, const LogTypes& a_type, bool a_isEnabled)
{
    const QTUTILS_UI_NTDT_NSP CategoryNoty aCtgTypeNoty({a_categoryName,a_type});

    if(a_isEnabled){
        emit m_pParent->CategoryTypeEnabledSignal(aCtgTypeNoty);
    }
    else{
        emit m_pParent->CategoryTypeDisabledSignal(aCtgTypeNoty);
    }
}


/*/////////////////////////////////////////////////////////////////////////////////*/

CategoryData::CategoryData(const QString& a_categoryName, LogWnd_p* a_logwnd_data_p, bool a_defaultEnable)
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
    isEnabled = aSettings.value(settingsKey,a_defaultEnable).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogTypes::Debug,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].setText("Debug");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Info/isEnabled";
    isEnabled = aSettings.value(settingsKey,a_defaultEnable).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogTypes::Info,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].setText("Info");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Warning/isEnabled";
    isEnabled = aSettings.value(settingsKey,a_defaultEnable).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogTypes::Warning,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].setText("Warning");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Error/isEnabled";
    isEnabled = aSettings.value(settingsKey,a_defaultEnable).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),LogTypes::Error,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].setText("Error");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    m_categoryControlWgt.setMinimumWidth(nX);
    m_categoryControlWgt.setFixedHeight(nMaxHeight);
    m_categoryControlWgt.setVisible(true);
}


static const char* s_setKeyNameExt[s_cunNumberOfLogTypes] = {
    "/Debug/",
    "/Info/",
    "/Warning/",
    "/Error/"
};


inline void CategoryData::SetTypeEnable(const LogTypes& a_type, bool a_isEnable)
{
    const bool isChecked = QTUTILS_UI_LOGWND_BIT_VALUE(m_flags.b.isEnabledVect,a_type);
    if(isChecked!=a_isEnable){
        QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),a_type,a_isEnable);
        m_logwnd_data_p->CategoryVisibilityChanged(m_logwnd_data_p->m_logs.begin());
        ::qtutils::Settings aSettings;
        const QString settingsKey = m_logwnd_data_p->m_settingsKey + "/" + m_categoryName +
                s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(a_type)] + "isEnabled";
        aSettings.setValue(settingsKey,a_isEnable);
        m_logwnd_data_p->EmitCategoryTypeChange(m_categoryName,a_type,a_isEnable);
    }
}


void CategoryData::ConnectSignals()
{
    QCheckBox* pNextCheckBox;

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(LogTypes::Debug,isCheckedByGui);
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(LogTypes::Info,isCheckedByGui);
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(LogTypes::Warning,isCheckedByGui);
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)]);
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(LogTypes::Error,isCheckedByGui);
    });
}


/*/////////////////////////////////////////////////////////////////////////////////*/

QtutilsUiLogwndIniter::QtutilsUiLogwndIniter()
{
    ::qtutils::Settings aSettings;
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)]    = aSettings.value("QtutilsUiLogwndGlobalColors/Debug",   QColor(0,0,190))  .value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)]     = aSettings.value("QtutilsUiLogwndGlobalColors/Info",    QColor(0,190,0))  .value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)]  = aSettings.value("QtutilsUiLogwndGlobalColors/Warning", QColor(100,100,0)).value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)]    = aSettings.value("QtutilsUiLogwndGlobalColors/Error",   QColor(190,0,0))  .value<QColor>();

    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Debug)]   = "/Debug/";
    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Info)]    = "/Info/";
    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Warning)] = "/Warning/";
    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(LogTypes::Error)]   = "/Debug/";

}


}}  //  namespace qtutils{ namespace ui{
