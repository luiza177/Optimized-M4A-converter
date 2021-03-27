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

wxDECLARE_EVENT(FFMPEG_THREAD_UPDATE, wxThreadEvent);

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
    wxString GenerateOutputFileName(wxString inputFile);
    void Convert();
    void OnConversionEnd(wxProcessEvent &event);
    void OnLineFeed(wxThreadEvent &event);
    wxTimeSpan ParseTimeSpan(wxString time);
    double CalcConvertedPercentage(wxTimeSpan convertedTime);

    wxProcess *m_ffmpeg = nullptr; //TODO: does this need to be a pointer?
    long m_ffmpegPID;
    std::list<Process> m_ffmpegProcessList;
    wxTimeSpan m_currentFileDuration;
    wxString m_bitrate{"128000"};

    std::function<void(Process process)> m_callbackFileStatus;
    std::function<void()> m_callbackBatchEnd;
    std::function<void(double)> m_callbackUpdateProgress;

public:
    Converter();
    virtual ~Converter() {}
    void SetListAndConvert(std::list<Process> processList);
    void Cancel();
    void SetFileStatusCallback(std::function<void(Process process)> callback);
    void SetBatchEndCallback(std::function<void()> callback);
    void SetUpdateProgressCallback(std::function<void(double)> callback);
    void SetBitrate(wxString &bitrate);
};

#endif