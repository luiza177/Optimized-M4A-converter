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
    std::list<Process> m_ffmpegProcessList;

    std::function<void(Process process)> m_callbackFileStatus;
    std::function<void()> m_callbackBatchEnd;

public:
    Converter();
    virtual ~Converter() {}
    void SetListAndConvert(std::list<Process> processList);
    void Cancel();
    void SetFileStatusCallback(std::function<void(Process process)> callback);
    void SetBatchEndCallback(std::function<void()> callback);
};

// clang-format off
#endif