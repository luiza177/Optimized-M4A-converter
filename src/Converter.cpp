#include "Converter.h"

Converter::Converter()
{
    this->Bind(wxEVT_END_PROCESS, &Converter::OnConversionEnd, this);
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

wxString Converter::GenerateFfmpegCommand(wxString inputFile)
{
    auto resourcesDir = GetResourcesDir();
    auto outputFile = inputFile;
    auto extension = inputFile.find_last_of('.');
    outputFile.replace(extension, 4, ".m4a");
    auto ffmpegCommand = wxString{"ffmpeg -y -i \""}; // -y flag is always overwrite
    auto ffmpegFlags = wxString{"\" -movflags +faststart -c:a aac -b:a 128000 \""};
    ffmpegCommand.Prepend(resourcesDir);
    ffmpegCommand += inputFile + ffmpegFlags + _(outputFile) + "\"";
    return ffmpegCommand;
}

void Converter::Convert()
{
    m_ffmpeg = new wxProcess(this, ID_FFMPEG);
    wxString ffmpegCommand = GenerateFfmpegCommand(m_ffmpegProcessList.front().path);
    m_ffmpegPID = wxExecute(ffmpegCommand, wxEXEC_ASYNC, m_ffmpeg);
}

void Converter::OnConversionEnd(wxProcessEvent &event)
{
    auto file = m_ffmpegProcessList.front();
    file.status = event.GetExitCode();
    m_callbackFileStatus(file);
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