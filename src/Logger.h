#ifndef LOGGER_H
#define LOGGER_H

#include <wx/stdpaths.h>
#include <wx/file.h>
// FIXME: Find correct include for wxGetEnv(...)
#include <wx/msgdlg.h>

class Logger
{
    public: Logger();
    public: ~Logger();

    // Get the system and application specific path of the file that will be used for logging
    public: static wxString LogFile();
    // Initialize the log file (reset on size limitation, add session text)
    public: static void Init();
    // write a message to the log file
    public: static void Log(wxString Text);
};

#endif // LOGGER_H
