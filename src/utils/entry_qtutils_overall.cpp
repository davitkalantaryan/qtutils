

#ifdef _MSC_VER
#define QTUTILS_DLL_EXPORT	__declspec(dllexport)
#else
#define QTUTILS_DLL_EXPORT
#endif



#ifdef __cplusplus
extern "C" {
#endif

QTUTILS_DLL_EXPORT void entry_qtutils_overall(void)
{
}

#ifdef __cplusplus
}
#endif
