#ifndef CURLREQUEST_H
#define CURLREQUEST_H

#include <wx/string.h>
//#include <wx/txtstrm.h>
//#include <wx/protocol/http.h>
//#include <wx/wfstream.h>
//#include <wx/stdpaths.h>
//#include <wx/textfile.h>
//#include <wx/msgdlg.h>
//#include <wx/arrimpl.cpp>
#include <wx/statusbr.h>
//#include <wx/url.h>
//#include <wx/filename.h>
#include <wx/stream.h>
//#include <wx/zstream.h>
//#include <wx/datstrm.h>
//#include <wx/zipstrm.h>
// #include <wx/tarstrm.h>
//#include <wx/mstream.h>
#include <curl/curl.h>

// this should go in the constructor
//curl_global_init(CURL_GLOBAL_ALL);

// this should go in the destructor
//curl_global_cleanup();

class CurlRequest
{
    public: CurlRequest();
    public: ~CurlRequest();

    public: void SetUrl(wxString Url);
    //public: void SetHost(wxString HostUrl);
    //public: void SetSite(wxString SiteUrl);
    public: void SetCompression(bool UseCompression);
    public: void SetAgent(wxString Agent);
    public: void SetReferer(wxString Referer);
    public: void SetOutputStream(wxOutputStream* Stream);
    public: void SetStatusBar(wxStatusBar* Bar, wxUint32 Field);
    public: void SetAbort(bool* Abort);
    public: bool ExecuteRequest();
    private: static size_t WriteCallback(char* data, size_t chunk_size, size_t chunk_count, void* curl_request);
    private: const char* StringToBytes(wxString String);

    private: CURL* curl;
    private: bool* abort;
    private: wxOutputStream* stream;
    private: wxStatusBar* bar;
    private: wxUint32 bar_field;
    private: bool compression;
};

#endif // CURLREQUEST_H
