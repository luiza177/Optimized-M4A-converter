#include "Converter.h"

#include <wx/txtstrm.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include <thread>

wxDEFINE_EVENT(FFMPEG_THREAD_UPDATE, wxThreadEvent);

Converter::Converter()
{
    this->Bind(wxEVT_END_PROCESS, &Converter::OnConversionEnd, this);
    this->Bind(FFMPEG_THREAD_UPDATE, &Converter::OnLineFeed, this);
}

wxString Converter::GetResourcesDir()
{
#ifdef __APPLE__
    auto mainBundle = CFBundleGetMainBundle();
    auto resourcesUrl = CFBundleCopyResourcesDirectoryURL(mainBundle);

    char resourcesPathCstr[1024];
    if (!CFURLGetFileSystemRepresentation(resourcesUrl, TRUE, (UInt8 *)resourcesPathCstr, 1024))
    {
        // something went wrong
        // abort app?
    }
    CFRelease(resourcesUrl);

    return wxString::FromUTF8(resourcesPathCstr) << "/";
#endif
    return wxString{".\\Resources\\"};
}

wxString Converter::GenerateOutputFileName(wxString inputFile)
{
    auto inputFileName = wxFileName(inputFile);
#ifdef __APPLE__
    auto outputFilePath = inputFileName.GetPath() + "/M4A/";
#else
    auto outputFilePath = inputFileName.GetPath() + "\\M4A\\";
#endif
    if (!wxDirExists(outputFilePath))
    {
        wxMkdir(outputFilePath);
    }
    return outputFilePath + inputFileName.GetName() + ".m4a";
}

wxString Converter::GenerateFfmpegCommand(wxString inputFile)
{
    auto resourcesDir = GetResourcesDir();
    auto outputFile = GenerateOutputFileName(inputFile);

    auto ffmpegCommand = wxString{"ffmpeg -y -i \""}; // -y flag is always overwrite
    // auto ffmpegCommand = wxString{"ffmpeg -hide_banner -y -i \""}; // -y flag is always overwrite
    auto ffmpegFlags = wxString::Format("\" -movflags +faststart -c:a aac -b:a %s \"", m_bitrate);
    ffmpegCommand.Prepend(resourcesDir);
    ffmpegCommand += inputFile + ffmpegFlags + outputFile + "\"";
    return ffmpegCommand;
}

void Converter::Convert()
{
    m_ffmpeg = new wxProcess(this, ID_FFMPEG);
    m_ffmpeg->Redirect();

    wxString ffmpegCommand = GenerateFfmpegCommand(m_ffmpegProcessList.front().path);
    m_ffmpegPID = wxExecute(ffmpegCommand, wxEXEC_ASYNC, m_ffmpeg);

    std::thread ffmpegCaptureThread([&]() {
        auto errorStream = m_ffmpeg->GetErrorStream();

        wxTextInputStream errorTextStream(*errorStream);
        while (!errorStream->Eof())
        {
            auto line = errorTextStream.ReadLine();
            auto event = new wxThreadEvent(FFMPEG_THREAD_UPDATE);
            event->SetPayload(line);
            wxQueueEvent(this, event);
        }
    });
    ffmpegCaptureThread.detach();
}

void Converter::OnConversionEnd(wxProcessEvent &event)
{
    auto file = m_ffmpegProcessList.front();
    file.status = event.GetExitCode();
    m_callbackFileStatus(file);
    m_currentFileDuration = NULL;
    if (m_ffmpegProcessList.size() > 0)
        m_ffmpegProcessList.pop_front();

    if (m_ffmpegProcessList.size() > 0)
    {
        Convert();
    }
    else
    {
        m_callbackBatchEnd();
    }
}

void Converter::SetListAndConvert(std::list<Process> processList)
{
    m_ffmpegProcessList = processList;
    Convert();
}

void Converter::Cancel()
{
    if ((m_ffmpeg != nullptr) && (m_ffmpeg->Exists(m_ffmpegPID)))
    {
        wxKillError err;
        auto isKilled = wxKill(m_ffmpegPID, wxSIGTERM, &err);
        // TODO: delete residue?
    }
}

void Converter::SetFileStatusCallback(std::function<void(Process process)> callback)
{
    m_callbackFileStatus = callback;
}
void Converter::SetBatchEndCallback(std::function<void()> callback)
{
    m_callbackBatchEnd = callback;
}
void Converter::SetUpdateProgressCallback(std::function<void(double)> callback)
{
    m_callbackUpdateProgress = callback;
}

void Converter::SetBitrate(wxString &bitrate)
{
    m_bitrate = bitrate + "000";
}

void Converter::OnLineFeed(wxThreadEvent &event)
{
#define DURATIONCHARCOUNT 10
#define TIMEEQCHARCOUNT 5
#define TIMESTAMPCHARCOUNT 11

    event.Skip();
    auto line = event.GetPayload<wxString>();
    auto durChar = line.Find(_("Duration: "));
    if (durChar != wxNOT_FOUND)
    {
        auto totalDurationStr = line.SubString(durChar + DURATIONCHARCOUNT, durChar + DURATIONCHARCOUNT + TIMESTAMPCHARCOUNT); //HH:MM:SS.cc
        m_currentFileDuration = ParseTimeSpan(totalDurationStr);
    }
    auto updateChar = line.Find(_("time="));
    if (updateChar != wxNOT_FOUND)
    {
        auto convertedTimeStr = line.SubString(updateChar + TIMEEQCHARCOUNT, updateChar + TIMEEQCHARCOUNT + TIMESTAMPCHARCOUNT); //HH:MM:SS.cc
        auto convertedTime = ParseTimeSpan(convertedTimeStr);
        auto progress = CalcConvertedPercentage(convertedTime);

        m_callbackUpdateProgress(progress);
    }
}

wxTimeSpan Converter::ParseTimeSpan(wxString time)
{
    //HH:MM:SS.cc
    auto hourStr = time.SubString(0, 1);
    auto minuteStr = time.SubString(3, 4);
    auto secondStr = time.SubString(6, 7);
    auto centisecStr = time.SubString(9, 10);

    long hour;
    long minute;
    long second;
    long centisec;

    hourStr.ToLong(&hour);
    minuteStr.ToLong(&minute);
    secondStr.ToLong(&second);
    centisecStr.ToLong(&centisec);

    auto timeSpan = wxTimeSpan(hour, minute, second, centisec * 10L);

    return timeSpan;
}

double Converter::CalcConvertedPercentage(wxTimeSpan convertedTime)
{
    auto convertedTimeLL = convertedTime.GetValue();
    auto durationLL = m_currentFileDuration.GetValue();

    return convertedTimeLL.ToDouble() / durationLL.ToDouble() * 100.0;
}