#ifndef CONVERTER_H
#define CONVERTER_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <wx/process.h>
#include <list>
#include <functional>

enum
{
    ID_FFMPEG = wxID_LAST + 1
};

struct Process
{
    int status = 100;
    long listRow;
    wxString path;
};

class Converter : public wxEvtHandler
{
private:
    wxString GetResourcesDir();
    wxString GenerateFfmpegCommand(wxString inputFile);
    void Convert();
    void OnConversionEnd(wxProcessEvent &event);

    wxProcess *m_ffmpeg = nullptr;
    long m_ffmpegPID;
    std::list<Process> m_ffmpegProcessList; // send to or live in new class?
    std::function<void(std::list<Process>)> m_callback;

public:
    Converter();
    virtual ~Converter();
    void CreateProcessQueueAndConvert();
    void Cancel();
    void SetCallback(std::function<void(std::list<Process>)> callback);
};

#endif