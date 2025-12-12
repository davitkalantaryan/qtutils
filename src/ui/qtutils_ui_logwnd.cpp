//
// file:			qtutils_ui_logwnd.cpp
// path:			src/ui/qtutils_ui_logwnd.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/ui/logwndqu.hpp>
#include <qtutils/core/settings.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <unordered_map>
#include <list>
#include <memory>
#include <stdint.h>
#include <assert.h>
#include <qtutils/disable_utils_warnings.h>
#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>
#include <QGuiApplication>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace ui{ namespace logger{

class CPPUTILS_DLL_PRIVATE CategoryData;
typedef ::std::unordered_map<QString,::std::shared_ptr<CategoryData> >    HashCategories;

static constexpr size_t     s_cunNumberOfLogTypes = static_cast<size_t>(core::logger::LogTypes::Count);
static QColor*              s_defaultColors = nullptr;

static const char* s_setKeyNameExt[s_cunNumberOfLogTypes] = {
    "/debug/",
    "/info/",
    "/warning/",
    "/error/"
};


static inline void QtutilsUiLogwndInitializeInline(void){
    if(s_defaultColors){return;}
    ::qtutils::Settings aSettings;
    s_defaultColors = new QColor[s_cunNumberOfLogTypes];
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)]    = aSettings.value("QtutilsUiLogwndGlobalColors/debug",   QColor(0,0,190))  .value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)]     = aSettings.value("QtutilsUiLogwndGlobalColors/info",    QColor(0,190,0))  .value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)]  = aSettings.value("QtutilsUiLogwndGlobalColors/warning", QColor(100,100,0)).value<QColor>();
    s_defaultColors[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)]    = aSettings.value("QtutilsUiLogwndGlobalColors/error",   QColor(190,0,0))  .value<QColor>();

    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)]   = "/debug/";
    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)]    = "/info/";
    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)] = "/warning/";
    s_setKeyNameExt[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)]   = "/error/";

}


QTUTILS_UI_EXPORT void QtutilsUiLogwndInitialize(void)
{
    QtutilsUiLogwndInitializeInline();
}


QTUTILS_UI_EXPORT void QtutilsUiLogwndCleanup(void)
{
    delete [] s_defaultColors;
    s_defaultColors = nullptr;
}


//#define QTUTILS_UI_LOGWND_BIT_VALUE(_val,_bitNum)   ( ((_val)>>(static_cast<uint32_t>(_bitNum)))&1 )

template <typename BitNumType>
inline bool QTUTILS_UI_LOGWND_BIT_VALUE(uint32_t a_val,const BitNumType& a_bitNum){
    const uint32_t cunVal = (a_val >> (static_cast<uint32_t>(a_bitNum))) & 1;
    return cunVal?true:false;
}

inline void QTUTILS_UI_LOGWND_SET_BIT_VALUE(uint32_t* a_pVal, const core::logger::LogTypes& a_bitNum, bool a_newVal){
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

    inline void SetTypeEnable(const core::logger::LogTypes& a_type, bool a_isEnable);

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


struct SLogStr{
    ::std::shared_ptr<CategoryData> ctg;
    size_t                          lgtp;
    QString                         msg;
};


class CPPUTILS_DLL_PRIVATE GuiLogWnd_p final : public QObject
{
public:
    HashCategories          categories;
    QTextEdit               edit;
    ::std::list<SLogStr>    logs;
    size_t                  maxNumberOfLogs;
};


class CPPUTILS_DLL_PRIVATE CoreLogWnd_p final : public core::logger::Base
{
public:
    CoreLogWnd_p(LogWnd_p* CPPUTILS_ARG_NN a_parent_p);
private:
    void LoggerClbk(enum CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen) override;
private:
    LogWnd_p* const m_parent_p;
private:
    CoreLogWnd_p(const CoreLogWnd_p&) = delete;
    CoreLogWnd_p(CoreLogWnd_p&&) = delete;
    CoreLogWnd_p& operator=(const CoreLogWnd_p&) = delete;
    CoreLogWnd_p& operator=(CoreLogWnd_p&&) = delete;
};


class CPPUTILS_DLL_PRIVATE LogWnd_p final
{
public:
    ~LogWnd_p();
    LogWnd_p(LogWnd* a_pParent);

    void ConnectSignals();
    inline void InitAndShowBase();
    inline void ApplyNewSize(const QSize& a_newSize);
    inline void ClearAllCategories();
    inline void EmitCategoryTypeChange(const QString& a_categoryName, const core::logger::LogTypes& a_type, bool a_isEnabled);

public:
    LogWnd*const                m_pParent;
    QString                     m_settingsKey;
    QMetaObject::Connection     m_newLogsConnection;
    GuiLogWnd_p* const          m_gui_p;
    CoreLogWnd_p* const         m_core_p;

private:
    LogWnd_p(const LogWnd_p&) = delete;
    LogWnd_p(LogWnd_p&&) = delete;
    LogWnd_p& operator=(const LogWnd_p&) = delete;
    LogWnd_p& operator=(LogWnd_p&&) = delete;
};


inline void CategoryVisibilityChangedInline(::std::list<SLogStr>::const_iterator a_iter, GuiLogWnd_p* CPPUTILS_ARG_NN a_logData_p)
{
    CategoryData* pCategoryData;
    uint32_t isEnabled;
    a_logData_p->edit.clear();
    ::std::list<SLogStr>::const_iterator iterTmp;

    for(; a_iter!=a_logData_p->logs.cend();){

        const SLogStr& lgDt = *a_iter;
        pCategoryData = lgDt.ctg.get();

        if(pCategoryData->m_flags.all == 0){
            iterTmp = a_iter++;
            a_logData_p->logs.erase(iterTmp);
            continue;
        }

        isEnabled = QTUTILS_UI_LOGWND_BIT_VALUE(pCategoryData->m_flags.b.isEnabledVect,lgDt.lgtp);
        if(isEnabled){
            const QColor aColor = pCategoryData->m_colors[lgDt.lgtp];
            a_logData_p->edit.setTextColor(aColor);
            a_logData_p->edit.append(lgDt.msg);
        }

        ++a_iter;
    }
}


inline void ClearExtraLogsInline(GuiLogWnd_p* CPPUTILS_ARG_NN a_logData_p)
{
    size_t unCurSize = a_logData_p->logs.size();
    if(unCurSize<(a_logData_p->maxNumberOfLogs)){return;}

    unCurSize /= 2;

    ::std::list<SLogStr>::const_iterator iterTmp, iter = a_logData_p->logs.cbegin();

    for(size_t i(0);i<unCurSize;++i){
        iterTmp = iter++;
        a_logData_p->logs.erase(iterTmp);
    }

    CategoryVisibilityChangedInline(iter,a_logData_p);
}

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


void LogWnd::SetDefaultColor(const core::logger::LogTypes& a_type, const QColor& a_newColor)
{
    const size_t cunIndex = QTUTILS_UI_LOGWND_TYPE_TO_INDEX(a_type);
    assert(cunIndex<QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Count));

    s_defaultColors[cunIndex] = a_newColor;
}


void LogWnd::SetCategoryColors(const QString& a_categoryName, const QColor* a_newColors, size_t a_count, size_t a_offset)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_gui_p->categories.find(a_categoryName);
    if(citer==m_logwnd_data_p->m_gui_p->categories.end()){return;}
    CategoryData* pCategoryData = citer->second.get();

    SetColorsInline(a_newColors,a_count,a_offset,pCategoryData->m_colors);
}


void LogWnd::SetCategoryColors(const QString& a_categoryName, const core::logger::LogTypes& a_type, const QColor& a_newColor)
{
    const size_t cunIndex = QTUTILS_UI_LOGWND_TYPE_TO_INDEX(a_type);
    assert(cunIndex<QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Count));

    const HashCategories::const_iterator citer = m_logwnd_data_p->m_gui_p->categories.find(a_categoryName);
    if(citer==m_logwnd_data_p->m_gui_p->categories.end()){return;}
    CategoryData* const pCategoryData = citer->second.get();

    pCategoryData->m_colors[cunIndex] = a_newColor;
}


void LogWnd::AddLogCategory(const QString& a_categoryName, bool a_defaultEnable)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_gui_p->categories.find(a_categoryName);
    if(citer!=m_logwnd_data_p->m_gui_p->categories.end()){return;} // category is already there

    ::std::shared_ptr<CategoryData> aCategoryData = ::std::shared_ptr<CategoryData>(new CategoryData(a_categoryName,m_logwnd_data_p,a_defaultEnable));
    m_logwnd_data_p->m_gui_p->categories.insert(::std::pair<QString,::std::shared_ptr<CategoryData> >(a_categoryName,aCategoryData));
    ApplyNewSize(size());
}


void LogWnd::RemoveCategory(const QString& a_categoryName)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_gui_p->categories.find(a_categoryName);
    if(citer==m_logwnd_data_p->m_gui_p->categories.end()){return;} // category is not there
    CategoryData* const pCategoryData = citer->second.get();

    pCategoryData->m_flags.b.shouldKeep = 0;
    m_logwnd_data_p->m_gui_p->categories.erase(citer);
    CategoryVisibilityChangedInline(m_logwnd_data_p->m_gui_p->logs.cbegin(),m_logwnd_data_p->m_gui_p);
    ApplyNewSize(size());
}


void LogWnd::EnableCategoryType(const QString& a_categoryName, const core::logger::LogTypes& a_type)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_gui_p->categories.find(a_categoryName);
    if(citer==m_logwnd_data_p->m_gui_p->categories.end()){return;}
    CategoryData* const pCategoryData = citer->second.get();
    pCategoryData->SetTypeEnable(a_type,true);
}


void LogWnd::DisableCategoryType(const QString& a_categoryName, const core::logger::LogTypes& a_type)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_gui_p->categories.find(a_categoryName);
    if(citer==m_logwnd_data_p->m_gui_p->categories.end()){return;}
    CategoryData* const pCategoryData = citer->second.get();
    pCategoryData->SetTypeEnable(a_type,false);
}


bool LogWnd::isEnabledCategoryType(const QString& a_categoryName, const core::logger::LogTypes& a_type)
{
    const HashCategories::const_iterator citer = m_logwnd_data_p->m_gui_p->categories.find(a_categoryName);
    if(citer==m_logwnd_data_p->m_gui_p->categories.end()){return false;}
    CategoryData* const pCategoryData = citer->second.get();
    return QTUTILS_UI_LOGWND_BIT_VALUE(pCategoryData->m_flags.b.isEnabledVect,a_type);
}


void LogWnd::ClearAllCategories()
{
    m_logwnd_data_p->ClearAllCategories();
    CategoryVisibilityChangedInline(m_logwnd_data_p->m_gui_p->logs.cbegin(),m_logwnd_data_p->m_gui_p);
    m_logwnd_data_p->ApplyNewSize(size());
}


static inline size_t QtMsgTypeToIndex(const CinternalLogCategory& a_msgType){
    // enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtInfoMsg, QtSystemMsg = QtCriticalMsg };
    switch(a_msgType){
    case CinternalLogCategoryDebug:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug);
    case CinternalLogCategoryWarning:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning);
    case CinternalLogCategoryInfo:
        return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info);
        break;
    default:
        break;
    }  //  switch(a_msgType){
    return QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error);
}


void LogWnd::SetMaxNumberOfLogs(size_t a_unMaxNumberOfLogs)
{
    m_logwnd_data_p->m_gui_p->maxNumberOfLogs = a_unMaxNumberOfLogs;
}


/*/////////////////////////////////////////////////////////////////////////////////*/


LogWnd_p::~LogWnd_p()
{
    QObject::disconnect(m_newLogsConnection);
    delete m_core_p;
    m_gui_p->deleteLater();
}


LogWnd_p::LogWnd_p(LogWnd* a_pParent)
    :
      m_pParent(a_pParent),
      m_gui_p(new GuiLogWnd_p()),
      m_core_p(new CoreLogWnd_p(this))
{
    m_gui_p->edit.setParent(a_pParent);
    QtutilsUiLogwndInitializeInline();
    m_gui_p->maxNumberOfLogs = QTUTILS_UI_LOGWND_DEFAULT_MAX_NUMBER_OF_LOGS;
    m_gui_p->edit.setReadOnly(true);
    InitAndShowBase();
    ConnectSignals();
}


void LogWnd_p::ConnectSignals()
{
    m_newLogsConnection = ::QObject::connect(m_pParent,&LogWnd::NewLogAvailableSignal,m_pParent,[this](qtutils_core_logger_LoggerData a_newLogData){
        const QString categoryName = "default";

        const HashCategories::const_iterator citer = m_gui_p->categories.find(categoryName);
        if(citer==(m_gui_p->categories.end())){return;}
        const CategoryData* const pCategoryData = citer->second.get();

        const size_t cunIndex = QtMsgTypeToIndex(a_newLogData.m_categoryEnm);
        const uint32_t isEnabled = QTUTILS_UI_LOGWND_BIT_VALUE(pCategoryData->m_flags.b.isEnabledVect,cunIndex);
        if(isEnabled){
            const QColor aColor = pCategoryData->m_colors[cunIndex];
            m_gui_p->edit.setTextColor(aColor);
            m_gui_p->edit.append(a_newLogData.m_log);
            m_gui_p->logs.push_back({citer->second,cunIndex,a_newLogData.m_log});
            ClearExtraLogsInline(m_gui_p);
        }
        else if(pCategoryData->m_flags.b.shouldKeep){
            m_gui_p->logs.push_back({citer->second,cunIndex,a_newLogData.m_log});
            ClearExtraLogsInline(m_gui_p);
        }
    });
}


inline void LogWnd_p::InitAndShowBase()
{
    static int instanceNumber = 0;
    m_settingsKey = typeid(*this).name()+QString::number(++instanceNumber);
}


CoreLogWnd_p::CoreLogWnd_p(LogWnd_p* CPPUTILS_ARG_NN a_parent_p)
    :
    m_parent_p(a_parent_p)
{
}


void CoreLogWnd_p::LoggerClbk(enum CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen)
{
    if(!a_log){
        return;
    }
    const qtutils_core_logger_LoggerData newLog({a_categoryEnm,a_categoryStr,a_log,a_logStrLen});
    emit m_parent_p->m_pParent->NewLogAvailableSignal(newLog);
}


inline void LogWnd_p::ApplyNewSize(const QSize& a_newSize)
{
    int nY = 3;
    CategoryData* pCategoryData;
    HashCategories::const_iterator citer = m_gui_p->categories.begin();
    const HashCategories::const_iterator citerEnd = m_gui_p->categories.end();

    for(;citer!=citerEnd;++citer){
        pCategoryData = citer->second.get();
        pCategoryData->m_categoryControlWgt.move(3,nY);
        nY += pCategoryData->m_categoryControlWgt.height();
    }

    nY += 3;
    m_gui_p->edit.move(0,nY++);
    m_gui_p->edit.setFixedSize(a_newSize.width(),a_newSize.height()-nY);
    m_pParent->setMinimumHeight(nY);
}


inline void LogWnd_p::ClearAllCategories()
{
    m_gui_p->categories.clear();
}


inline void LogWnd_p::EmitCategoryTypeChange(const QString& a_categoryName, const core::logger::LogTypes& a_type, bool a_isEnabled)
{
    const qtutils_core_logger_CategoryNoty aCtgTypeNoty({a_categoryName,a_type});

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
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),core::logger::LogTypes::Debug,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].setText("Debug");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Info/isEnabled";
    isEnabled = aSettings.value(settingsKey,a_defaultEnable).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),core::logger::LogTypes::Info,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].setText("Info");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Warning/isEnabled";
    isEnabled = aSettings.value(settingsKey,a_defaultEnable).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),core::logger::LogTypes::Warning,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].setText("Warning");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    settingsKey = a_logwnd_data_p->m_settingsKey + "/" + a_categoryName + "/Error/isEnabled";
    isEnabled = aSettings.value(settingsKey,a_defaultEnable).toBool();
    QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),core::logger::LogTypes::Error,isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].setParent(&m_categoryControlWgt);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].setChecked(isEnabled);
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].setText("Error");
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].
            setFixedSize(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].sizeHint());
    m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].move(nX,0);
    nX += m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].width();
    nNextHeight = m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)].height();
    nMaxHeight = (nNextHeight>nMaxHeight)?nNextHeight:nMaxHeight;

    m_categoryControlWgt.setMinimumWidth(nX);
    m_categoryControlWgt.setFixedHeight(nMaxHeight);
    m_categoryControlWgt.setVisible(true);
}


inline void CategoryData::SetTypeEnable(const core::logger::LogTypes& a_type, bool a_isEnable)
{
    const bool isChecked = QTUTILS_UI_LOGWND_BIT_VALUE(m_flags.b.isEnabledVect,a_type);
    if(isChecked!=a_isEnable){
        QTUTILS_UI_LOGWND_SET_BIT_VALUE(&(m_flags.b.isEnabledVect),a_type,a_isEnable);
        CategoryVisibilityChangedInline(m_logwnd_data_p->m_gui_p->logs.begin(),m_logwnd_data_p->m_gui_p);
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

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Debug)]);
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
#else
    QObject::connect(pNextCheckBox,&QCheckBox::checkStateChanged,&m_categoryControlWgt,[this](int a_state){
#endif
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(core::logger::LogTypes::Debug,isCheckedByGui);
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Info)]);
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
#else
    QObject::connect(pNextCheckBox,&QCheckBox::checkStateChanged,&m_categoryControlWgt,[this](int a_state){
#endif
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(core::logger::LogTypes::Info,isCheckedByGui);
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Warning)]);
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
#else
    QObject::connect(pNextCheckBox,&QCheckBox::checkStateChanged,&m_categoryControlWgt,[this](int a_state){
#endif
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(core::logger::LogTypes::Warning,isCheckedByGui);
    });

    pNextCheckBox = &(m_logTypesEnabledCB[QTUTILS_UI_LOGWND_TYPE_TO_INDEX(core::logger::LogTypes::Error)]);
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    QObject::connect(pNextCheckBox,&QCheckBox::stateChanged,&m_categoryControlWgt,[this](int a_state){
#else
    QObject::connect(pNextCheckBox,&QCheckBox::checkStateChanged,&m_categoryControlWgt,[this](int a_state){
#endif
        const bool isCheckedByGui = (a_state != Qt::Unchecked);
        SetTypeEnable(core::logger::LogTypes::Error,isCheckedByGui);
    });
}


}}}  //  namespace qtutils { namespace ui{ namespace logger{
