#include "KissAnimeCom.h"

KissAnimeCom::KissAnimeCom()
{
    type = CONNECTOR_TYPE_ANIME;
    label = wxT("KissAnime");
    baseURL = wxT("http://kissanime.com");
    referrerURL = wxT("http://kissanime.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("kissanime"), wxT("list"));
    LoadLocalMangaList();
    agent = wxT("Android");
    CFCookie();
}

KissAnimeCom::~KissAnimeCom()
{
    //
}

void KissAnimeCom::UpdateMangaList()
{
    wxTextFile f;
/*
    if(!mangaListFile.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nConfiguration directory: ") + mangaListFile.GetPath());
        return;
    }
*/
    mangaListFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    // create file, or open if already exists
    if(!f.Create(mangaListFile.GetFullPath()))
    {
        f.Open(mangaListFile.GetFullPath());
        f.Clear();
    }

    wxString mangaLink;
    wxString mangaLabel;

    CurlRequest cr;
    cr.SetAgent(agent);
    cr.SetCookies(cookies);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    for(wxUint32 i=1; i<150; i++)
    {
        cr.SetUrl(wxString::Format(baseURL + wxT("/AnimeList?page=%i"), i));
        content = wxEmptyString;

        if(cr.ExecuteRequest() && !content.IsEmpty())
        {
            int indexStart = content.find(wxT("class=\"listing\"")) + 15;
            int indexEnd = 0;//content.rfind(wxT("</table>"));

            if(indexStart > -1 && indexEnd >= -1)
            {
                // Example Entry: <a class="bigChar" href="/Anime/hack-G-U-Returner">.hack//G.U. Returner</a>
                while((indexStart = content.find(wxT("class=\"bigChar\""), indexEnd)) > -1)
                {
                    indexStart += 15;
                    indexStart = content.find(wxT("=\""), indexStart) + 2; // "<a href='"
                    indexEnd = content.find(wxT("\""), indexStart); // "\">"
                    mangaLink = baseURL + +wxT("/M") + content.Mid(indexStart, indexEnd-indexStart);

                    indexStart = indexEnd + 2;
                    indexEnd = content.find(wxT("<"), indexStart); // "</a>"
                    mangaLabel = content.Mid(indexStart, indexEnd-indexStart);

                    if(!mangaLabel.IsEmpty())
                    {
                        f.AddLine(HtmlUnescapeString(mangaLabel) + wxT("\t") + mangaLink);
                    }

                    //wxYield();
                }
            }
            else
            {
                // empty page -> break loop
                i = 9999;
            }
        }
        sos.Close();
    }

    f.Write();
    f.Close();
    LoadLocalMangaList();
}

wxArrayMCEntry KissAnimeCom::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString chNumber;
    wxString chEntry;
    wxString chTitle;
    wxString chLink;

    CurlRequest cr;
    cr.SetUrl(MangaEntry->Link);
    cr.SetAgent(agent);
    cr.SetCookies(cookies);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("asp.wrap(\"")) + 10;
    int indexEnd = content.find(wxT("\");"), indexStart);

    if(indexStart > -1 && indexEnd >= -1)
    {
        content = Base64::Decode(content.Mid(indexStart, indexEnd-indexStart), wxCSConv(wxFONTENCODING_UTF8));
        indexStart = 0;
        indexEnd = 0;

        // Example Entry: <div class="episode" data-value="55257">Episode 003</div>
        while((indexStart = content.find(wxT("data-value=\""), indexStart)) > -1)
        {
            indexStart += 12;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            chLink = baseURL + wxT("/Mobile/GetEpisode?eID=") + content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 1;
            indexStart = content.find(wxT(">"), indexStart) + 1;
            indexEnd = content.find(wxT("<"), indexStart);
            chTitle = content.Mid(indexStart, indexEnd-indexStart);

            chapterList.Add(new MCEntry(HtmlUnescapeString(chTitle), HtmlEscapeUrl(chLink)));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString KissAnimeCom::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;
    pageLinks.Add(ChapterLink);
    return pageLinks;
}

wxString KissAnimeCom::GetImageLink(wxString PageLink)
{
    //Activate(PageLink);
    CurlRequest cr;
    cr.SetUrl(PageLink.BeforeLast('?'));
    cr.SetPostData(PageLink.AfterLast('?'));
    cr.SetAgent(agent);
    cr.SetCookies(cookies);
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    content = Base64::Decode(content, wxCSConv(wxFONTENCODING_UTF8));

    int indexStart = 0;
    int indexEnd = 0;

    wxString link = wxEmptyString;
    wxString linkTemp = wxEmptyString;
    int resolution = 0;
    int resolutionTemp = 0;

    while((indexStart = content.find(wxT("href=\""), indexStart)) > -1)
    {
        indexStart += 6;
        indexEnd = content.find(wxT("\""), indexStart); // "\""
        linkTemp = content.Mid(indexStart, indexEnd-indexStart);

        indexStart = indexEnd + 1;
        indexStart = content.find(wxT(">"), indexStart) + 1;
        indexEnd = content.find(wxT("<"), indexStart);
        resolutionTemp = wxAtoi(content.Mid(indexStart, indexEnd-indexStart).AfterLast('x').BeforeLast('.'));

        // prefer video closest to 720p or lower
        if(link.IsEmpty() || (resolutionTemp <= 800 && resolutionTemp > resolution))
        {
            link = linkTemp;
            resolution = resolutionTemp;
        }
    }

    return link;
}

void KissAnimeCom::Activate(wxString Link)
{
    wxString tmp = Link + wxT("0xb"); // TODO: get offset from online source in case it changes periodically
    wxString hash = wxEmptyString;
    unsigned char sum[32];
    SHA256((const unsigned char*)tmp.ToAscii().data(), tmp.Len(), sum); // Scripts/google.js (= CryptoJS)
    for(int i = 0; i < 32; i++)
    {
        hash.Append(wxString::Format(wxT("%02x"), sum[i]));
    }
    CurlRequest cr;
    cr.SetUrl(baseURL + wxT("/0xba93?key=") + hash);
    cr.SetReferer(Link);
    cr.SetPostData(wxT("key=") + hash);
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();
}

wxString KissAnimeCom::JSRiddle(wxString Script)
{
    CurlRequest cr(false);
    cr.SetUrl(wxT("http://jsapp.us/ajax"));
    cr.SetPostData(wxT("{\"actions\":[{\"action\":\"test\",\"code\":\"") + Script + wxT("\"}]}"));
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // open url from response
    cr.SetUrl(response.BeforeLast('"').AfterLast('"'));
    response = wxEmptyString;
    cr.ExecuteRequest();

    return response;
}

void KissAnimeCom::CFCookie()
{
    int indexStart;
    int indexEnd;

    CurlRequest cr(false);
    cr.SetUrl(baseURL);
    cr.SetHeader(true);
    cr.SetAgent(agent);
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // get initial cookies
    indexStart = response.find(wxT("__cfduid=")) + 11;
    indexEnd = response.find(wxT(";"), indexStart);
    cookies = response.Mid(indexStart, indexEnd-indexStart) + wxT(";test_enable_cookie=cookie_value");

    // get jschl_answer equation script
    indexStart = response.find(wxT("var t,r,a,f,"), indexEnd) + 12;
    indexEnd = response.find(wxT("}, 8000);"), indexStart);
    wxString temp = response.Mid(indexStart, indexEnd-indexStart).Trim(false);
    wxString key = temp.BeforeFirst('=');
    wxString jschl_script = wxT("require('http').createServer(function(req,res){");
    while(temp.Contains(wxT(";")))
    {
        if(temp.StartsWith(key) || temp.StartsWith(wxT("a.value"))) {
            jschl_script += temp.BeforeFirst(';').Trim(false) + wxT(";");
        }
        temp = temp.AfterFirst(';').Trim(false);
    }
    jschl_script.Replace(wxT("a.value = "), wxT("res.writeHead(200);res.end(("));
    jschl_script.Replace(wxT("t.length"), wxT("13)+'');}).listen()"));
    jschl_script.Replace(wxT("\""), wxT("'"));
    jschl_script.Replace(wxT(" "), wxT(""));
    wxString jschl_answer = JSRiddle(jschl_script);

    // get chk_jschl url
    indexStart = response.find(wxT("challenge-form"), indexEnd) + 14;
    indexStart = response.find(wxT("action=\""), indexStart) + 8;
    indexEnd = response.find(wxT("\""), indexStart);
    wxString jschl_chk = baseURL + response.Mid(indexStart, indexEnd-indexStart);

    // get jschl_vc
    indexStart = response.find(wxT("name=\"jschl_vc\""), indexEnd) + 23;
    indexEnd = response.find(wxT("\""), indexStart);
    wxString jschl_vc = response.Mid(indexStart, indexEnd-indexStart);

    // get pass
    indexStart = response.find(wxT("name=\"pass\"")) + 19;
    indexEnd = response.find(wxT("\""), indexStart);
    wxString pass = response.Mid(indexStart, indexEnd-indexStart);

    // wait a bit before sending solution of jsriddle (bypass cloudflare's "to fast soled" protection)
    wxSleep(3);
    cr.SetUrl(jschl_chk + wxT("?jschl_vc=") + jschl_vc + wxT("&pass=") + pass + wxT("&jschl_answer=") + jschl_answer);
    cr.SetCookies(cookies);
    response = wxEmptyString;
    cr.ExecuteRequest();

    // get cf_clearance cookie
    indexStart = response.find(wxT("cf_clearance="));
    indexEnd = response.find(wxT(";"), indexStart);
    cookies = response.Mid(indexStart, indexEnd-indexStart) + wxT(";usingHTML5V1=true;playerTypeMobile=html5");
}
