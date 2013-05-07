#include "BatotoNet.h"

#include <wx/timer.h>

BatotoNet::BatotoNet()
{
    label = wxT("Batoto");
    baseURL = wxT("http://www.batoto.net");
    referrerURL = wxT("http://www.batoto.net");
    mangaListFile.Assign(CONNECTOR_CONFIGURATION_PATH, wxT("batoto"), wxT("list"));
    LoadLocalMangaList();
}

BatotoNet::~BatotoNet()
{
    //
}

void BatotoNet::UpdateMangaList()
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

    wxString content = GetHtmlContentF(baseURL + wxT("/search_ajax?p=%i"), 1, 500, 1, 600, true);

    // only update local list, if connection successful...
    if(!content.IsEmpty())
    {
        int indexStart = 0;// = content.find(wxT("<div class=\"manga_list\">")) + 24;
        int indexEnd = 0;// = content.rfind(wxT("<div id=\"footer\">"));

        if(indexStart > -1 && indexEnd >= -1)
        {
            //content = content.Mid(indexStart, indexEnd-indexStart);
            //indexEnd = 0;

            // Example Entry: <strong><a href="http://www.batoto.net/comic/_/zettai-kareshi-r3650"><img src="http://www.batoto.net/forums/public/style_images/master/book.png" alt="Status"/> Zettai Kareshi</a></strong>
            while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
            {
                indexStart += 9;
                indexEnd = content.find(wxT("\""), indexStart); // "\">"
                mangaLink = content.Mid(indexStart, indexEnd-indexStart);

                indexEnd += 2;
                indexStart = content.find(wxT(">"), indexEnd) + 2; // "\"/> "
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

wxArrayMCEntry BatotoNet::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chNumber;
    wxString chEntry;
    wxString chLanguage;
    wxString chScangroup;
    wxString chTitle;
    wxString chLink;

    wxString content = GetHtmlContent(MangaEntry->Link, true);

    int posStart, posEnd;
    int indexStart = content.find(wxT("ipb_table chapters_list")) + 23;
    int indexEnd = content.rfind(wxT("comments"));

    if(indexStart > 22 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry:
        /*
        <tr class="row lang_English">
        <td style="border-top:0;"><a href="http://www.batoto.net/read/_/134827/one-piece_ch683_by_mangarule"><img src="http://www.batoto.net/book_open.png" style="vertical-align:middle;"/> Ch.683: A Woman like Ice</a></td>
        <td style="border-top:0;"><div title="English" style="display: inline-block; width:16px; height: 12px; margin: 2px 0; padding:0; background-image:url('http://www.batoto.net/images/all_flags.png'); background-position: -284px -173px; "></div></td>
        <td style="border-top:0;"><a href="http://www.batoto.net/group/_/mangarule-r149">Mangarule</a>        </td>
        */
        while((indexStart = content.find(wxT("class=\"row"), indexEnd)) > -1)
        {
            indexStart += 10;
            indexStart = content.find(wxT("_"), indexStart) + 1;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            chLanguage = content.Mid(indexStart, indexEnd-indexStart);

            indexStart += 1;
            indexStart = content.find(wxT("<a"), indexStart) + 9; // "<a href=\""
            indexEnd = content.find(wxT("\""), indexStart); // "\">"
            chLink = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 2;
            indexStart = content.find(wxT(">"), indexStart) + 1; // "\"/>"
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chEntry = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 4;
            indexStart = content.find(wxT("<a"), indexStart) + 9; // "<a href=\""
            indexStart = content.find(wxT(">"), indexStart) + 1; // "\">"
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chScangroup = content.Mid(indexStart, indexEnd-indexStart);

            // parse chEntry for: volumePrefix, chNumber, chTitle

            posStart = chEntry.find(wxT("Vol."));
            if(posStart > -1)
            {
                posEnd = chEntry.find(wxT(" "), posStart);
                volumePrefix = chEntry.Mid(posStart, posEnd-posStart);
            }

            posStart = chEntry.find(wxT("Ch.")) + 3;
            if(posStart > 2)
            {
                if(chEntry.EndsWith(wxT("Read Online")))
                {
                    posEnd = chEntry.rfind(wxT(" R"));

                    chTitle = wxT(""); // overwrite previous title
                }
                else
                {
                    // NOTE: wrong data when text before chapter number contains ': ' (i.e. ch.Text: Text 10: Chapter Title)
                    posEnd = chEntry.find(wxT(": "), posStart);

                    chTitle = chEntry.Mid(posEnd+2);
                }

                chNumber = chEntry.Mid(posStart, posEnd-posStart);

                FormatChapterNumberStyle(&chNumber);
            }

            // NOTE: batoto seems to use global chapter numbering, where the chapter numbers are unique (volumes are optional)
            // -> ignore volume prefix

            chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" - ") + chTitle + wxT(" [") + chLanguage + wxT("] by [") + chScangroup + wxT("]")), chLink));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString BatotoNet::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    wxString content = GetHtmlContent(ChapterLink, true);

    int indexStart = content.find(wxT("<select name=\"page_select\"")) + 26;
    int indexEnd = content.find(wxT("</select>"), indexStart);

    if(indexStart > 25 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option value="http://www.batoto.net/read/_/132664/one-piece_ch682_by_mangarule/3" >page 3</option>
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

wxString BatotoNet::GetImageLink(wxString PageLink)
{
    wxString content = GetHtmlContent(PageLink, true);

    // Example Entry: <img id="comic_page" style="max-width: 100%;" src="http://img.batoto.net/comics/2012/09/23/o/read505ee533070ca/img000003.png" alt="One Piece - ch 682 Page 3 | Batoto!" onerror="this.src='http://img.batoto.net/comics/misc/Img-Error.jpg'" onload="adjust_page_width();" />
    int indexStart = content.find(wxT("<img id=\"comic_page\"")) + 20;
    indexStart = content.find(wxT("src=\""), indexStart) + 5;
    int indexEnd = content.find(wxT("\""), indexStart);

    if(indexStart > 4 && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return wxT("");
    }
}
