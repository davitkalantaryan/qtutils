//
// repo:            qtutils
// file:			qtutils_core_parse_http_multipart.cpp
// path:			src/core/qtutils_core_parse_http_multipart.cpp
// created on:		2025 Jun 20
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <qtutils/core/parse_http_multipart.hpp>

namespace qtutils { namespace core{ namespace httpMPart{

#define QTUTILS_CONT_DISP_STR       "content-disposition:"
#define QTUTILS_CONT_DISP_STR_LEN   (sizeof(QTUTILS_CONT_DISP_STR) - 1)
#define QTUTILS_MPRT_NAME_STR       "name="
#define QTUTILS_MPRT_NAME_STR_LEN   (sizeof(QTUTILS_MPRT_NAME_STR) - 1)
#define QTUTILS_MPRT_FNAME_STR      "filename="
#define QTUTILS_MPRT_FNAME_STR_LEN  (sizeof(QTUTILS_MPRT_FNAME_STR) - 1)


static inline bool ParseHttpPartInline(const QByteArray* CPPUTILS_ARG_NN a_part_p, MultipartMap* CPPUTILS_ARG_NN a_pMap){
    SinglePart nextEntry;
    const QByteArray aPart = a_part_p->toLower();
    //const QByteArray contDispWrd = "Content-Disposition:";
    qsizetype indexOfContentDisposition = aPart.lastIndexOf(QTUTILS_CONT_DISP_STR);
    if(indexOfContentDisposition<0){return false;}
    indexOfContentDisposition += QTUTILS_CONT_DISP_STR_LEN;
    //const QByteArray nameWrd = "name=";
    qsizetype indexOfName = aPart.indexOf(QTUTILS_MPRT_NAME_STR,indexOfContentDisposition);
    if(indexOfName<0){return false;}
    indexOfName += QTUTILS_MPRT_NAME_STR_LEN;
    qsizetype indexOfQuota1 = a_part_p->indexOf('"',indexOfName);
    if((indexOfQuota1++)<0){return false;}
    qsizetype indexOfQuota2 = a_part_p->indexOf('"',indexOfQuota1);
    QByteArray name = a_part_p->mid(indexOfQuota1,indexOfQuota2-indexOfQuota1);
    if(name.startsWith("file")){
        // we have file part
        name = "file";
        //const QByteArray filenameWrd = "filename=";
        qsizetype indexOfFilename = a_part_p->indexOf(QTUTILS_MPRT_FNAME_STR,indexOfContentDisposition);
        if(indexOfFilename<0){return false;}
        indexOfFilename += QTUTILS_MPRT_FNAME_STR_LEN;
        indexOfQuota1 = a_part_p->indexOf('"',indexOfFilename);
        if((indexOfQuota1++)<0){return false;}
        indexOfQuota2 = a_part_p->indexOf('"',indexOfQuota1);
        nextEntry.fileName = a_part_p->mid(indexOfQuota1,indexOfQuota2-indexOfQuota1);
    }
    
    qsizetype indexOfBody = a_part_p->indexOf('\n',indexOfContentDisposition);
    if((indexOfBody++)<0){return false;}
    indexOfBody = a_part_p->indexOf('\n',indexOfBody);
    if((indexOfBody++)<0){return false;}
    nextEntry.dataOffset = indexOfBody;
    nextEntry.data = ::std::move(*a_part_p);
    a_pMap->insert(::std::move(name),::std::move(nextEntry));
    return true;
}


QTUTILS_CORE_EXPORT void ParseMPart(const QByteArray& a_requestBodyBA, const QByteArray& a_boundary, MultipartMap* CPPUTILS_ARG_NN a_map_p)
{
    QByteArray part;
    const qsizetype boundarySize = a_boundary.size();
    qsizetype nextPos, indexOfBndry = 0;
    indexOfBndry = a_requestBodyBA.indexOf(a_boundary, indexOfBndry);
    while(indexOfBndry>=0){
        nextPos = a_requestBodyBA.indexOf(a_boundary, indexOfBndry + boundarySize);
        if (nextPos < 0) {
            part = a_requestBodyBA.mid(indexOfBndry + boundarySize);
            ParseHttpPartInline(&part,a_map_p);
            break; // No more boundaries found
        }
        else{
            part = a_requestBodyBA.mid(indexOfBndry + boundarySize, nextPos - indexOfBndry - boundarySize);
            ParseHttpPartInline(&part,a_map_p);
            indexOfBndry = nextPos;
        }
    }  //  while(indexOfBndry>=0){
}


}}}  //  namespace qtutils { namespace core{ namespace httpMPart{
