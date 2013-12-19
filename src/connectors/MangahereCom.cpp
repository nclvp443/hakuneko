#include "MangahereCom.h"

MangahereCom::MangahereCom()
{
    label = wxT("MangaHere");
    baseURL = wxT("http://www.mangahere.com");
    referrerURL = wxT("http://www.mangahere.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("mangahere"), wxT("list"));
    LoadLocalMangaList();
}

MangahereCom::~MangahereCom()
{
    //
}

void MangahereCom::UpdateMangaList()
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

    wxString content = GetHtmlContent(baseURL + wxT("/mangalist/"), false);

    // only update local list, if connection successful...
    if(!content.IsEmpty())
    {
        int indexStart = content.find(wxT("<div class=\"sort_list\">")) + 23;
        int indexEnd = content.rfind(wxT("<footer>"));

        if(indexStart > 22 && indexEnd >= -1)
        {
            content = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd = 0;

            // Example Entry: <li><a class="manga_info" rel="&quot;Gokko&quot;" href="http://www.mangahere.com/manga/gokko/"><span></span>&quot;Gokko&quot;</a></li>
            while((indexStart = content.find(wxT("manga_info"), indexEnd)) > -1)
            {
                indexStart += 10;
                indexStart = content.find(wxT("href="), indexStart); // "href=\""
                indexStart += 6;
                indexEnd = content.find(wxT("\""), indexStart); // "\">"
                mangaLink = content.Mid(indexStart, indexEnd-indexStart);

                indexStart = indexEnd + 2 + 13; // skip <span></span>
                indexEnd = content.find(wxT("<"), indexStart); // "</a>"
                mangaLabel = content.Mid(indexStart, indexEnd-indexStart);

                if(!mangaLabel.IsEmpty())
                {
                    f.AddLine(HtmlUnescapeString(mangaLabel) + wxT("\t") + mangaLink);
                }

                //wxYield();
            }
        }

        f.Write();
        f.Close();
        LoadLocalMangaList();
    }
    else
    {
        f.Close();
    }
}

wxArrayMCEntry MangahereCom::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chNumber;
    wxString chTitle;
    wxString chLink;

    wxString content = GetHtmlContent(MangaEntry->Link);

    int indexStart = content.find(wxT("<div class=\"detail_list\">")) + 25;
    int indexEnd = content.find(wxT("<ul class=\"tab_comment clearfix\">"), indexStart);

    if(indexStart > 24 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <li><span class="left"><a class="color_0077" href="http://www.mangahere.com/manga/one_piece/v60/c683/" >              One Piece 683           </a>                     <span class="mr6">Vol 60</span>A girl like ice</span><span class="right">Oct 2, 2012</span></li>
        while((indexStart = content.find(wxT("color_0077"), indexEnd)) > -1)
        {
            indexStart += 10;
            indexStart = content.find(wxT("href="), indexStart); // "href=\""
            indexStart += 6;
            indexEnd = content.find(wxT("\""), indexStart); // "\" >"
            chLink = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 3;
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chNumber = content.Mid(indexStart, indexEnd-indexStart).Trim().AfterLast(' ');

            FormatChapterNumberStyle(&chNumber);

            indexStart = indexEnd + 4;
            indexStart = content.find(wxT(">"), indexStart); // "mr6">"
            indexStart += 1;
            indexEnd = content.find(wxT("<"), indexStart); // "</span>"
            volumePrefix = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 7;
            indexEnd = content.find(wxT("<"), indexStart); // "</span>"
            chTitle = content.Mid(indexStart, indexEnd-indexStart);

            // NOTE: mangahere seems to use global chapter numbering, where the chapter numbers are unique (i.e. vol1-ch1, vol2-ch2)
            // -> ignore volume prefix

            // in case chTitle is NULL or whitespaced, whitespaces will be removed bei MCEntry::SetSafeLabel()
            chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" - ") + chTitle), chLink));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString MangahereCom::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    wxString content = GetHtmlContent(ChapterLink);

    int indexStart = content.find(wxT("<select class=\"wid60\" onchange=\"change_page(this)\">")) + 51;
    // ignore last option (comments -> value="0")
    int indexEnd = content.find(wxT("</select>"), indexStart);

    if(indexStart > 50 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option value="http://www.mangahere.com/manga/ranma_1_2/c022/2.html" >2</option>
        while((indexStart = content.find(wxT("<option value=\""), indexEnd)) > -1)
        {
            indexStart += 15;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            pageLinks.Add(content.Mid(indexStart, indexEnd-indexStart));

            //wxYield();
        }
    }

    return pageLinks;
}

wxString MangahereCom::GetImageLink(wxString PageLink)
{
    wxString content = GetHtmlContent(PageLink);

    // Example Entry: <img src="http://c.mhcdn.net/store/manga/9464/022.0/compressed/i22.08.jpg?v=11349820721" onerror="this.src=\'http://m.mhcdn.net/store/manga/9464/022.0/compressed/i22.08.jpg?v=11349820721\'" width="800" id="image" alt="Ranma 1/2 22 Page 8" />
    // use the onerror-server instead the src-server (to save bandwith for web-browsing users which will read from src-server)
    int indexStart = content.find(wxT("id=\"viewer\"")) + 11;
    indexStart = content.find(wxT("<img src=\""), indexStart) + 10;
    int indexEnd = content.find(wxT("\""), indexStart);

    if(indexStart > 10 && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return wxEmptyString;
    }
}
