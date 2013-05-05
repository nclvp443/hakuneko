#ifndef MANGACONNECTORCOLLECTION_H
#define MANGACONNECTORCOLLECTION_H

#include <wx/string.h>
#include <wx/txtstrm.h>
#include <wx/protocol/http.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include <wx/msgdlg.h>
#include <wx/hashmap.h>

#include "connectors/VirtualManga.h"
#include "connectors/MangareaderNet.h"
#include "connectors/MangafoxMe.h"
#include "connectors/MangahereCom.h"
#include "connectors/BatotoNet.h"
#include "connectors/MeinmangaCom.h"
#include "connectors/SubmangaCom.h"
#include "MangaConnector.h"
#include "ErrorLogFrame.h"

class MangaConnectorCollection
{
    public: MangaConnectorCollection();
    public: virtual ~MangaConnectorCollection();

    private: wxArrayMangaConnector MangaConnectors;

    public: wxArrayString GetConnectorLabels();
    private: int GetConnectorIndexFromLabel(wxString ConnectorLabel);
    public: void UpdateMangaLists();
    public: wxArrayMCEntry GetMangaList(wxString ConnectorLabel);
    public: wxArrayMCEntry GetChapterList(wxString ConnectorLabel, MCEntry* MangaEntry);
    public: MCJob CreateJob(wxString ConnectorLabel, MCEntry* MangaEntry, MCEntry* ChapterEntry);
    public: unsigned long GenerateJobID(MCEntry* ChapterEntry);
    public: bool AddJob(MCJob Job);
    public: void RemoveJob(unsigned long JobID);
    public: void RemoveAllJobs();
    public: bool ContainsJob(unsigned long JobID);
    public: size_t GetJobCount();
    public: wxArrayJobID GetCompletedJobIDs();
    public: void SetJobDownloadCompleted(unsigned long JobID, bool Value);
    public: void DownloadJobs(wxFileName BaseDirectory, wxStatusBar* StatusBar, bool* Abort);
};

#endif // MANGACONNECTORCONNECTION_H