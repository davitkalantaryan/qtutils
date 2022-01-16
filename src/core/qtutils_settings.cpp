//
// file:            monitor_settings.cpp
// path:			src/core/basic/monitor_settings.cpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <focust/core/settings.hpp>
#ifdef FOCUST_EMSCRIPTEN_IS_USED
#include <cpputils/hashtbl.hpp>
#include <mutex>


namespace focust{

static ::cpputils::hashtbl::Base<QString,QVariant> s_values;
static ::std::mutex     s_mutex;

bool Settings::contains(const QString & a_key)const
{
    ::cpputils::hashtbl::Base<QString,QVariant>::const_iterator iter;
    {
        ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
        iter = s_values.FindEntry(a_key);
    }
    
    return iter?true:false;
}

QVariant Settings::value(const QString & a_key, const QVariant & a_defaultValue) const
{
    ::cpputils::hashtbl::Base<QString,QVariant>::const_iterator iter;
    {
        ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
        iter = s_values.FindEntry(a_key);
        if(iter){
            return iter->second;
        }
    }
    return a_defaultValue;
}

void Settings::setValue(const QString &a_key, const QVariant &a_value)
{
    ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
    s_values.AddOrReplaceEntry(a_key,a_value);
}


void Settings::setDefaultFormat( FOCUST_QT_NSP QSettings::Format a_format)
{
	static_cast<void>(a_format);
}


}  // namespace focust{


#endif  // #ifdef FOCUST_EMSCRIPTEN_IS_USED

