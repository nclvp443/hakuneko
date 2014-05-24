#include "KissAnimeCom.h"

KissAnimeCom::KissAnimeCom()
{
    type = CONNECTOR_TYPE_ANIME;
    label = wxT("KissAnime");
    baseURL = wxT("http://kissanime.com");
    referrerURL = wxT("http://kissanime.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("kissanime"), wxT("list"));
    LoadLocalMangaList();
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
    cr.SetAgent(wxT("Android"));
    cr.SetCompression(wxT("gzip"));

    for(wxUint32 i=1; i<75; i++)
    {
        cr.SetUrl(wxString::Format(baseURL + wxT("/AnimeList?page=%i"), i));
        wxString content;
        wxStringOutputStream sos(&content);
        cr.SetOutputStream(&sos);

        if(cr.ExecuteRequest() && !content.IsEmpty())
        {
            int indexStart = content.find(wxT("class=\"listing\""), 0) + 15;
            int indexEnd = 0;

            if(indexStart > -1 && indexEnd >= -1)
            {
                // Example Entry: <div class="post-content"><h2><a href="/Anime/Magi-The-Kingdom-of-Magic">Magi: The Kingdom of Magic</a>
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
        }
        sos.Close();
    }

/*
    Mobile download ~2.5 times slower than from full webpage

    //cr.SetUrl(baseURL + wxT("/Mobile/GetNextUpdateAnime"));

    for(wxUint32 i=0; i<3800; i+=20)
    {
        cr.SetPostData(wxString::Format(wxT("id=%i&sort=popular"), i));
        wxString content;
        wxStringOutputStream sos(&content);
        cr.SetOutputStream(&sos);

        if(cr.ExecuteRequest() && !content.IsEmpty())
        {
            int indexStart = 0;
            int indexEnd = 0;

            if(indexStart > -1 && indexEnd >= -1)
            {
                // Example Entry: <div class="post-content"><h2><a href="/M/Anime/Magi-The-Kingdom-of-Magic">Magi: The Kingdom of Magic</a>
                while((indexStart = content.find(wxT("post-content"), indexEnd)) > -1)
                {
                    indexStart += 12;
                    indexStart = content.find(wxT("<a"), indexStart) + 9; // "<a href='"
                    indexEnd = content.find(wxT("\""), indexStart); // "\">"
                    mangaLink = baseURL + content.Mid(indexStart, indexEnd-indexStart);

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
        }
        sos.Close();
    }
*/
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
    cr.SetAgent(wxT("Android"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("class=\"episode\"")) + 15;
    int indexEnd = content.rfind(wxT("div[data-value]"));

    if(indexStart > 14 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <div class="episode" data-value="55257">Episode 003</div>
        while((indexStart = content.find(wxT("data-value=\""), indexEnd)) > -1)
        {
            indexStart += 12;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            chLink = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 1;
            indexStart = content.find(wxT(">"), indexStart) + 1;
            indexEnd = content.find(wxT("<"), indexStart);
            chTitle = content.Mid(indexStart, indexEnd-indexStart);

            chLink = MangaEntry->Link + wxT("/") + chTitle + wxT("?id=") + chLink;
            chLink.Replace(wxT("/M/"), wxT("/"));
            chLink.Replace(wxT(" "), wxT("-"));

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
    Activate(PageLink);
    CurlRequest cr;
    cr.SetUrl(PageLink);
    cr.SetReferer(PageLink);
    cr.SetAgent(wxT("Android"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // Example Entry: <div style="font-size: 14px; font-weight: bold; padding-top: 15px" id="divDownload">Download (Save link as...): <a target="_blank" href="http://redirector.googlevideo.com/videoplayback?id=a540c0fa20ad7ca2&itag=35&source=picasa&cmo=sensitive_content=yes&ip=0.0.0.0&ipbits=0&expire=1398580126&sparams=id,itag,source,ip,ipbits,expire&signature=388B2E03F17C6511B52F994D8D82575C4ABDCE6B.C034DA1931E17C7A1C0BBB802CF60FE04444793&key=lh1">704x480.flv</a> - <a target="_blank" href="http://redirector.googlevideo.com/videoplayback?id=a540c0fa20ad7ca2&itag=34&source=picasa&cmo=sensitive_content=yes&ip=0.0.0.0&ipbits=0&expire=1398580126&sparams=id,itag,source,ip,ipbits,expire&signature=5A1599D1505FF9B62A51CA3AEDB0F5A9FBE9586E.95C698DA74BB53E1999659BF7BC7953241EE6CC4&key=lh1">528x360.flv</a> - <a target="_blank" href="http://redirector.googlevideo.com/videoplayback?id=a540c0fa20ad7ca2&itag=18&source=picasa&cmo=sensitive_content=yes&ip=0.0.0.0&ipbits=0&expire=1398580126&sparams=id,itag,source,ip,ipbits,expire&signature=71B2743598DC5292109ABFBF121C0763DCA0B77F.6AAF94A7D27229B3461990E8352B76987CAB248F&key=lh1">528x360.mp4</a> - <a target="_blank" href="http://redirector.googlevideo.com/videoplayback?id=a540c0fa20ad7ca2&itag=5&source=picasa&cmo=sensitive_content=yes&ip=0.0.0.0&ipbits=0&expire=1398580126&sparams=id,itag,source,ip,ipbits,expire&signature=B21E011C669D1EE5D09FF723ACD98F9480CDF427.4AF6EB4E801F7F783C09D18228D666DBBC0E77F6&key=lh1">352x240.flv</a></div>
    int indexStart = content.find(wxT("id=\"divDownload\"")) + 16;
    indexStart = content.find(wxT("href=\""), indexStart) + 6;
    int indexEnd = content.find(wxT("\""), indexStart);

    if(indexStart < indexEnd && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return wxEmptyString;
    }
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

wxString KissAnimeCom::CFCookie()
{
    int indexStart;
    int indexEnd;

    CurlRequest cr;
    cr.SetUrl(baseURL);
    cr.SetHeader(true);
    cr.SetReferer(referrerURL);
    cr.SetAgent(wxT("Android"));
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    if(!response.Contains(wxT("chk_jschl")))
    {
        return wxEmptyString;
    }

    indexStart = response.find(wxT("a.value = ")) + 10;
    indexEnd = response.find(wxT(";"), indexStart);
    wxString equation = response.Mid(indexStart, indexEnd-indexStart);
    indexStart = equation.find(wxT("+"));
    indexEnd = equation.find(wxT("*"));
    wxInt32 result = wxAtoi(equation.Mid(indexStart+1, indexEnd-indexStart-1)) * wxAtoi(equation.Mid(indexEnd+1)) + wxAtoi(equation.Mid(0, indexStart)) + 13; // 13 = strlen("kissanime.com")
    indexStart = response.find(wxT("jschl_vc")) + 17;
    indexEnd = response.find(wxT("\""), indexStart);
    wxString token = response.Mid(indexStart, indexEnd-indexStart);

    // TODO: complete the determination of cookies (currently halted)
    // CF-IUAM protection for kissanime was turned of during development

    //response = wxEmptyString;
    cr.SetUrl(wxString::Format(baseURL + wxT("/cdn-cgi/l/chk_jschl?jschl_vc=") + token + wxT("&jschl_answer=%i"), result));
    cr.ExecuteRequest();

    indexStart = response.find(wxT("cf_clearance="));
    indexEnd = response.find(wxT(";"), indexStart) + 1;
    wxString cookies = response.Mid(indexStart, indexEnd-indexStart);
    wxPrintf(cookies);

    //wxPrintf(wxT("\n++++++++++++++++++++++++++++++++++\n\n") + response);
    return wxEmptyString;
}
