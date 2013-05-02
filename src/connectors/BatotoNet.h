#ifndef BATOTO_H
#define BATOTO_H

#include "../MangaConnector.h"

class BatotoNet : public MangaConnector
{
    public: BatotoNet();
    public: virtual ~BatotoNet();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // BATOTO_H
