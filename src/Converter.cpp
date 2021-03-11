#include "Converter.h"

#include <wx/txtstrm.h>
#include <wx/thread.h>
#include <wx/filename.h>

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
    auto ffmpegFlags = wxString{"\" -movflags +faststart -c:a aac -b:a 128000 \""};
    ffmpegCommand.Prepend(resourcesDir);
    ffmpegCommand += inputFile + ffmpegFlags + _(outputFile) + "\"";
    return ffmpegCommand;
}

void Converter::Convert()
{
    m_ffmpeg = new wxProcess(this, ID_FFMPEG);
    m_ffmpeg->Redirect();

    wxString ffmpegCommand = GenerateFfmpegCommand(m_ffmpegProcessList.front().path);
    m_ffmpegPID = wxExecute(ffmpegCommand, wxEXEC_ASYNC, m_ffmpeg);

    wxThread *ffmpegCapture;
    //? wxTimer or multithreading???
    /*  
    auto errorStream = m_ffmpeg->GetErrorStream();

    wxTextInputStream errorTextStream(*errorStream);
    while (!errorStream->Eof())
    {
        std::cout << "stderr = " << errorTextStream.ReadLine() << std::endl;
    } 
    */
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