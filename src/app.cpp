#include "app.h"

IMPLEMENT_APP(MangaDownloaderApp);

bool MangaDownloaderApp::OnInit()
{
    wxInitAllImageHandlers();
    Frame = new MangaDownloaderFrame(NULL);
    Frame->Show();
    SetTopWindow(Frame);

    return true;
}

int MangaDownloaderApp::OnRun()
{
    wxString parameter;
    for(int i=1; i<argc; i++)
    {
        parameter = wxString(argv[i], wxConvUTF8);
        if(parameter.IsSameAs(wxT("-t")) || parameter.IsSameAs(wxT("--test")))
        {
            Frame->ErrorDetectionTest();
            break;
        }
    }

    // call the main loop for GUI processing (not required for modal dialogs)
    wxApp::OnRun();

    return 0;
}

int MangaDownloaderApp::OnExit()
{
    return 0;
}
