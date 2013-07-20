#include "SubmangaCom.h"

SubmangaCom::SubmangaCom()
{
    label = wxT("Submanga");
    baseURL = wxT("http://submanga.com");
    referrerURL = wxT("http://submanga.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("submanga"), wxT("list"));
    LoadLocalMangaList();
}

SubmangaCom::~SubmangaCom()
{
    //
}

void SubmangaCom::UpdateMangaList()
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

    wxString content = GetHtmlContent(baseURL + wxT("/series/n"), true);

    // only update local list, if connection successful...
    if(!content.IsEmpty())
    {
        int indexStart = content.find(wxT("<div class=\"b468\">")) + 18;
        int indexEnd = content.rfind(wxT("<div class=\"b250 bmr0\">"));

        if(indexStart > 17 && indexEnd >= -1)
        {
            content = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd = 0;

            // Example Entry: <td><a href="http://submanga.com/zzz"><b class="xs">11.936.</b> zzz</a></td>
            while((indexStart = content.find(wxT("<td><a href=\""), indexEnd)) > -1)
            {
                indexStart += 13;
                indexEnd = content.find(wxT("\""), indexStart); // "\">"
                mangaLink = content.Mid(indexStart, indexEnd-indexStart) + wxT("/completa");

                indexStart = indexEnd + 2;
                indexStart = content.find(wxT("b>"), indexStart) + 3; // "</b> "
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

wxArrayMCEntry SubmangaCom::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chNumber;
    wxString chScangroup;
    wxString chTitle;
    wxString chLink;

    wxString content = GetHtmlContent(MangaEntry->Link, true);

    int indexStart = content.find(wxT("class=\"r\"")) + 9;
    int indexEnd = content.find(wxT("class=\"r\""), indexStart);

    if(indexStart > 8 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <td class="s"><a href="http://submanga.com/One_Piece/684/172521">One Piece <strong>684</strong></a></td><td><a class="grey s" href="http://submanga.com/scanlation/Shinshin_Fansub" rel="nofollow">Shinshin Fansub</a></td>
        while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
        {
            indexStart += 9;
            indexEnd = content.find(wxT("\""), indexStart); // "\">"
            chLink = content.Mid(indexStart, indexEnd-indexStart);
            chLink = baseURL + wxT("/c/") + chLink.AfterLast('/');

            indexStart = indexEnd + 2;
            indexStart = content.find(wxT("g>"), indexStart) + 2; // "<strong>"
            indexEnd = content.find(wxT("<"), indexStart); // "</a> : "
            chTitle = content.Mid(indexStart, indexEnd-indexStart);
            chNumber = chTitle;

            // submanga don't use explicite restrictions for chapter number
            // assume a chapter number, when description is a number without any spaces...
            if(chNumber.Find(wxT(" ")) < 0 && (chNumber.IsSameAs (wxT("0")) || wxAtoi(chNumber) != 0))
            {
                FormatChapterNumberStyle(&chNumber);
            }

            indexStart = indexEnd + 4;
            indexStart = content.find(wxT("w\">"), indexStart) + 3; // "el=\"nofollow\">"
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chScangroup = content.Mid(indexStart, indexEnd-indexStart);

            // NOTE: submanga uses global chapter numbering, where the chapter numbers are unique (there aren't volumes anyway)
            // -> ignore volume prefix

            chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" by [") + chScangroup + wxT("]")), chLink));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString SubmangaCom::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    wxString content = GetHtmlContent(ChapterLink);

    int indexStart = content.find(wxT("<select")) + 7;
    int indexEnd = content.find(wxT("</select>"), indexStart);

    if(indexStart > 6 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option selected value="2">2</option>
        while((indexStart = content.find(wxT("<option"), indexEnd)) > -1)
        {
            indexStart += 7;
            indexStart = content.find(wxT("\""), indexStart) + 1; // "\""
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            pageLinks.Add(ChapterLink + wxT("/") + content.Mid(indexStart, indexEnd-indexStart));

            //wxYield();
        }
    }

    return pageLinks;
}

wxString SubmangaCom::GetImageLink(wxString PageLink)
{
    wxString content = GetHtmlContent(PageLink);

    // Example Entry: <img src="http://img6.submanga.com/pages/105/1056821bb/2.jpg"/>
    int indexStart = content.rfind(wxT("<img src=\"")) + 10;
    int indexEnd = content.find(wxT("\""), indexStart);

    if(indexStart > 9 && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return wxEmptyString;
    }
}
