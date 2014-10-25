#include "Logger.h"

Logger::Logger()
{
    //
}

Logger::~Logger()
{
    //
}

wxString Logger::LogFile()
{
    wxString LogFile;
    #ifdef PORTABLE
        #ifdef __LINUX__
            LogFile = wxStandardPaths::Get().GetExecutablePath() + wxT(".log");
        #endif
        #ifdef __WINDOWS__
            LogFile = wxStandardPaths::Get().GetExecutablePath() + wxT(".log");
        #endif
    #else
        #ifdef __LINUX__
            // LogFile = wxT("/var/log/hakuneko.log"); // requires write access
            wxGetEnv(wxT("XDG_CONFIG_HOME"), &LogFile);
            LogFile = LogFile.BeforeFirst(':');
            if(LogFile.IsEmpty())
            {
                LogFile = wxStandardPaths::Get().GetUserConfigDir() + wxT("/.config/hakuneko/hakuneko.log");
            }
            else
            {
                LogFile +=  wxT("/hakuneko/hakuneko.log");
            }
        #endif
        #ifdef __WINDOWS__
            LogFile = wxStandardPaths::Get().GetUserDataDir() + wxT("\\hakuneko.log");
        #endif
    #endif
    return LogFile;
}

void Logger::Init()
{
    wxString LogFile = Logger::LogFile();
    if(wxFile::Exists(LogFile))
    {
        bool reset = false;
        wxFile log(LogFile, wxFile::read);
        if(log.Length() > 5242880)
        {
            reset = true;
        }
        log.Close();

        if(reset)
        {
            wxRemoveFile(LogFile);
        }
    }

    Logger::Log(wxEmptyString);
    Logger::Log(wxT("+++++++++++++++++++++++++++++++++"));
    Logger::Log(wxT("+++ ") + wxDateTime::Now().Format(wxT("%Y-%m-%dT%H:%M:%S +0000"), wxDateTime::UTC) + wxT(" +++"));
    Logger::Log(wxT("+++++++++++++++++++++++++++++++++"));
    Logger::Log(wxEmptyString);
}

void Logger::Log(wxString Text)
{
    wxString LogFile = Logger::LogFile();
    // TODO: check if log file exist / was initialized
    wxFile log(LogFile, wxFile::write_append);
    #ifdef __LINUX__
        log.Write(Text + wxT("\n"));
    #endif
    #ifdef __WINDOWS__
        log.Write(Text + wxT("\r\n"));
    #endif
    log.Close();
}
