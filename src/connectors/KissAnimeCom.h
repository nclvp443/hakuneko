#ifndef KISSANIME_H
#define KISSANIME_H

#include "../MangaConnector.h"

class KissAnimeCom : public MangaConnector
{
    public: KissAnimeCom();
    public: virtual ~KissAnimeCom();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);

    private: void Activate(wxString Link);
    private: wxString CFCookie();
};

#endif // KISSANIME_H
