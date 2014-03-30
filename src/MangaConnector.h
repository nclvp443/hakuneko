#ifndef MANGACONNECTOR_H
#define MANGACONNECTOR_H

#include <wx/string.h>
#include <wx/txtstrm.h>
#include <wx/protocol/http.h>
#include <wx/wfstream.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/msgdlg.h>
#include <wx/arrimpl.cpp>
#include <wx/statusbr.h>
#include <wx/url.h>
#include <wx/filename.h>
#include <wx/sstream.h>
#include <wx/zstream.h>
//#include <wx/datstrm.h>
#include <wx/zipstrm.h>
// #include <wx/tarstrm.h>
#include "CurlRequest.h"

enum MANGA_LIST_TYPE
{
    MANGA_LIST_TYPE_LINKS,
    MANGA_LIST_TYPE_LABELS
};

class MCEntry
{
    public: MCEntry();
    public: MCEntry(wxString Name, wxString Url);
    public: virtual ~MCEntry();

    private: static wxString invalidFileCharacters;
    public: wxString SafeLabel;
    public: wxString Label;
    public: wxString Link;

    private: void SetSafeLabel(wxString Name);
};

WX_DEFINE_ARRAY(MCEntry*, wxArrayMCEntry);

struct MCJob
{
    public: unsigned long ID;
    public: wxString Connector;
    public: wxString MangaLabel;
    public: wxString MangaSafeLabel;
    public: wxString MangaLink;
    public: wxString ChapterLabel;
    public: wxString ChapterSafeLabel;
    public: wxString ChapterLink;
    public: bool DownloadCompleted;
};

WX_DECLARE_STRING_HASH_MAP(wxChar, wxCharHashMap);
WX_DECLARE_HASH_MAP(unsigned long, MCJob, wxIntegerHash, wxIntegerEqual, wxJobHashMap);

WX_DEFINE_ARRAY_LONG(unsigned long, wxArrayJobID);

class MangaConnector
{
    public: MangaConnector();
    public: virtual ~MangaConnector();

    // get the path where the configuration files of the connectors will be stored
    protected: static wxString GetConfigurationPath();
    // create a unique id for a string (max. 4100 characters in string to create 32Bit integer)
    protected: static unsigned long CheckSum(wxString Value);
    // fills the htmlEntities & invalidChars hashmap with values
    private: static void InitializeHtmlEntities();
    // replace illegal url characters
    protected: wxString HtmlEscapeUrl(wxString IllegalUrl);
    // replace html special entities
    protected: wxString HtmlUnescapeString(wxString IllegalString);
    // formats a chapter number into an appealing style (4 digits)
    protected: void FormatChapterNumberStyle(wxString* ChapterNumber);
    // return the connector name
    public: wxString GetLabel();
    // loads all manga titles / links from configuration file
    protected: void LoadLocalMangaList();
    // read content from a http:// file into a string, use gzip transfer for speed improvements (server must support gzip)
    protected: wxString GetHtmlContent(wxString Url, bool UseGzip = false);
    // read content from multiple http:// files containing an incrementable integer (url must contain %i) and a minimum size of each page as abort condition, use gzip transfer for speed improvements (server must support gzip)
    protected: wxString GetHtmlContentF(wxString UrlFormat, int First, int Last, int Increment, size_t AbortSize = 0, bool UseGzip = false);
    // store an image from a website to a stream (i.e. file, archive, ...)
    protected: bool SaveHtmlImage(wxString SourceImageURL, wxBufferedOutputStream* TargetStream, wxString ReferrerURL, wxStatusBar* StatusBar, bool* Abort);
    // store the content from the index list of the website to the local configuraion file
    public: virtual void UpdateMangaList();
    // return all manga titles / links
    public: virtual wxArrayMCEntry GetMangaList();
    // return all chapter titles / links of a manga
    public: virtual wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    // return all page links of a chapter
    public: virtual wxArrayString GetPageLinks(wxString ChapterLink);
    // return the extracted image url from a page
    public: virtual wxString GetImageLink(wxString PageLink);
    // creates a job from manga entry and chapter entry
    public: MCJob CreateJob(MCEntry* MangaEntry, MCEntry* ChapterEntry);
    // creates a unique job id from manga entry and chapter entry
    public: static unsigned long GenerateJobID(MCEntry* ChapterEntry);
    // add a job to the joblist
    public: bool AddJob(MCJob Job);
    // remove a job from the joblist
    public: void RemoveJob(unsigned long JobID);
    // clear all jobs from the joblist
    public: void RemoveAllJobs();
    // check if a job already exists in the joblist
    public: bool ContainsJob(unsigned long JobID);
    // get number of jobs in list
    public: size_t GetJobCount();
    // returns a list of all jobID's whith the download completed flag
    public: wxArrayJobID GetCompletedJobIDs();
    // set the Downloaded Completed Value for a job
    public: void SetJobDownloadCompleted(unsigned long JobID, bool Value);
    // downloads all chapters from the joblist to the base directory, when CompressChapters is true -> .cbz instead of images
    public: wxArrayString DownloadJobs(wxFileName BaseDirectory, wxStatusBar* StatusBar, bool* Abort, bool CompressChapters = false);

    protected: wxString label;
    protected: wxString baseURL;
    protected: wxString referrerURL;
    protected: wxFileName mangaListFile;
    protected: wxArrayMCEntry mangaListEntries;
    protected: wxArrayMCEntry chapterListEntries;
    protected: wxJobHashMap jobList;
    private: static wxCharHashMap htmlEntities;
};

WX_DEFINE_ARRAY(MangaConnector*, wxArrayMangaConnector);

#endif // MANGACONNECTOR_H
