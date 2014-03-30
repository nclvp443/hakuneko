#include "CurlRequest.h"

/*
0x00000000 - 0x0000007F | 0bbbbbbb
0x00000080 - 0x000007FF | 110bbbbb 10bbbbbb
0x00000800 - 0x0000FFFF | 1110bbbb 10bbbbbb 10bbbbbb
0x00010000 - 0x001FFFFF | 11110bbb 10bbbbbb 10bbbbbb 10bbbbbb
0x00200000 - 0x03FFFFFF | 111110bb 10bbbbbb 10bbbbbb 10bbbbbb 10bbbbbb
0x04000000 - 0x7FFFFFFF | 1111110b 10bbbbbb 10bbbbbb 10bbbbbb 10bbbbbb 10bbbbbb
*/
void fix_utf8(char* buffer, size_t bytes)
{
    bool valid = true;
    int utf8_appended_bytes = 0;
    int i = 0;

    while(i<(int)bytes)
    {
        valid = true;

        if    (!(buffer[i] & 0x80))         { utf8_appended_bytes = 0; } // starts with 0 -> 1 byte
        else if((buffer[i] & 0xc0) == 0x80) { utf8_appended_bytes = 0; valid = false; } // starts with 10 -> invalid
        else if((buffer[i] & 0xe0) == 0xc0) { utf8_appended_bytes = 1; } // starts with 110 -> 2 bytes
        else if((buffer[i] & 0xf0) == 0xe0) { utf8_appended_bytes = 2; } // starts with 1110 -> 3 bytes
        else if((buffer[i] & 0xf8) == 0xf0) { utf8_appended_bytes = 3; } // starts with 11110 -> 4 bytes
        else if((buffer[i] & 0xfc) == 0xf8) { utf8_appended_bytes = 4; } // starts with 111110 -> 5 bytes
        else if((buffer[i] & 0xfe) == 0xfc) { utf8_appended_bytes = 5; } // starts with 1111110 -> 6 bytes

        if(i+utf8_appended_bytes < (int)bytes)
        {
            // check the appended bytes
            for(int n=0; n<utf8_appended_bytes; n++)
            {
                // starts not with 10 -> invalid
                if((buffer[i+1+n] & 0xc0) != 0x80)
                {
                    valid = false;
                    break;
                }
            }
        }
        else
        {
            valid = false;
        }

        if(!valid)
        {
            // replace all characters from i to i+utf8_appended_bytes
            for(int n=0; n<1+utf8_appended_bytes; n++)
            {
                buffer[i+n] = '?';
            }
        }

        i += 1+utf8_appended_bytes;
    }
}

CurlRequest::CurlRequest()
{
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    }
    abort = NULL;
    stream = NULL;
    bar = NULL;
    fix_utf = false;
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
}

void CurlRequest::SetUrl(wxString Url)
{
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, (const char*)Url.mb_str(wxConvUTF8));
    }
}

void CurlRequest::SetCompression(wxString Compression)
{
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, (const char*)Compression.mb_str(wxConvUTF8));
    }
}

void CurlRequest::SetAgent(wxString Agent)
{
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, (const char*)Agent.mb_str(wxConvUTF8));
    }
}

void CurlRequest::SetReferer(wxString Referer)
{
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_REFERER, (const char*)Referer.mb_str(wxConvUTF8));
    }
}

void CurlRequest::SetGetData(wxString Data)
{
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_POST, 0);
        if(Data != wxEmptyString)
        {
            // TODO: append this to the url...
        }
    }
}

void CurlRequest::SetPostData(wxString Data)
{
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        if(Data != wxEmptyString)
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (const char*)Data.mb_str(wxConvUTF8)); // "foo=bar&foz=baz"
        }
    }
}

void CurlRequest::SetOutputStream(wxOutputStream* Stream)
{
    stream = Stream;
}

void CurlRequest::SetOutputStreamFixUTF8(bool FixUTF8)
{
    fix_utf = FixUTF8;
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
    if(curl && curl_easy_perform(curl) == CURLE_OK)
    {
        return true;
    }
    //wxPrintf(wxT("FAILED: ") + wxString::FromAscii(curl_easy_strerror(curl_easy_perform(curl))) + wxT("\n\n"));
    return false;
}

size_t CurlRequest::WriteCallback(char* data, size_t chunk_size, size_t chunk_count, void* curl_request)
{
    CurlRequest* cr = (CurlRequest*)curl_request;
    wxUint32 size = chunk_size * chunk_count;

    wxYield();

    if(cr->abort && *(cr->abort))
    {
        return 0 ;// this will stop curl
    }

    if(cr->stream)
    {
        if(cr->fix_utf)
        {
            fix_utf8(data, size);
        }
        cr->stream->Write(data, size);
        size = cr->stream->LastWrite(); // works also for ZipOutputStreams
    }

    if(cr->bar)
    {
        double val;
        int digits = 3;
        wxString status = wxEmptyString;
        if(CURLE_OK == curl_easy_getinfo(cr->curl, CURLINFO_SIZE_DOWNLOAD, &val))
        {
            val /= 1048576.0;
            (val<10 ? digits=2 : (val<100 ? digits=1 : (val<1000 ? digits=1 : digits=0)));
            status.Append(wxString::Format(wxT("%.*f"), digits, val));
        }
        else
        {
            status.Append(wxT("??"));
        }
        status.Append(wxT(" / "));
        if(CURLE_OK == curl_easy_getinfo(cr->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &val))
        {
            val /= 1048576.0;
            (val<10 ? digits=2 : (val<100 ? digits=1 : (val<1000 ? digits=1 : digits=0)));
            status.Append(wxString::Format(wxT("%.*f MB"), digits, val));
        }
        else
        {
            status.Append(wxT("?? MB"));
        }
        status.Replace(wxT(","), wxT("."));
        cr->bar->SetStatusText(status, 1);
    }

    return (size_t)size;
}
