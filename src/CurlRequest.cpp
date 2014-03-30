#include "CurlRequest.h"

CurlRequest::CurlRequest()
{
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    }
    abort = NULL;
    stream = NULL;
    bar = NULL;
    //bar_field = 0;
}

CurlRequest::~CurlRequest()
{
    if(curl)
    {
        curl_easy_cleanup(curl);
    }
    abort = NULL;
    stream = NULL;
    bar = NULL;
    //bar_field = 0;
}

void CurlRequest::SetUrl(wxString Url)
{
    // TODO: convert wxString to char* and apply
    //StringToBytes(Url);
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/db.sqlite");
    }
}

void CurlRequest::SetCompression(bool UseCompression)
{
    // TODO: set header to use gzip
    compression = UseCompression;
}

void CurlRequest::SetAgent(wxString Agent)
{
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, StringToBytes(Agent));
    }
}

void CurlRequest::SetReferer(wxString Referer)
{
    //
}

void CurlRequest::SetOutputStream(wxOutputStream* Stream)
{
    stream = Stream;
}

void CurlRequest::SetStatusBar(wxStatusBar* Bar, wxUint32 Field)
{
    bar = Bar;
    bar_field = Field;
}

void CurlRequest::SetAbort(bool* Abort)
{
    abort = Abort;
}

bool CurlRequest::ExecuteRequest()
{
    if(curl)
    {
        return (curl_easy_perform(curl) == CURLE_OK);
    }
    return false;
}

const char* CurlRequest::StringToBytes(wxString String)
{
    //wxByte* NameBuffer = (wxByte*)memcpy(new wxByte[2*NameLength], Name.mb_str(enc), 2*NameLength);
    return "xxx";
}

size_t CurlRequest::WriteCallback(char* data, size_t chunk_size, size_t chunk_count, void* curl_request)
{
    CurlRequest* cr = (CurlRequest*)curl_request;
    wxUint32 pos_start = 0;
    wxUint32 pos_end = chunk_size * chunk_count;

    wxYield();

    // TODO: check for abort
    if(cr->abort && *(cr->abort))
    {
        // TODO: stop curl
        wxPrintf(wxT("Aborting...\n"));
        return 0 ;// this will stop curl?
    }

    if(cr->stream)
    {
        // TODO: check for gzip data
        // maybe curl processes gzip automagically?
        pos_start = cr->stream->TellO();
        cr->stream->Write(data, pos_end);
        pos_end = cr->stream->TellO();
    }

    // TODO: update statusbar
    if(cr->bar)
    {
        double val;
        wxString status = wxEmptyString;
        if(CURLE_OK == curl_easy_getinfo(cr->curl, CURLINFO_SIZE_DOWNLOAD, &val))
        {
            status.Append(wxString::Format(wxT("%0.1f"), val/1048576.0));
        }
        else
        {
            status.Append(wxT("???"));
        }
        status.Append(wxT("/"));
        if(CURLE_OK == curl_easy_getinfo(cr->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &val))
        {
            status.Append(wxString::Format(wxT("%0.1f MB"), val/1048576.0));
        }
        else
        {
            status.Append(wxT("??? MB"));
        }
        //wxPrintf(status + wxT("\n"));
        cr->bar->SetStatusText(status, 1);
    }

    // return the processed bytes, otherwise curl will fail
    return (size_t)(pos_end - pos_start);
}
