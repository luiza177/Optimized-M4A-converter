#ifndef FRAMEMAIN_H
#define FRAMEMAIN_H

#include "DropTarget.h"

#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <wx/process.h>

#include <queue>
#include <map>

enum
{
    ID_Convert = wxID_LAST + 1,
    ID_Clear,
    ID_FFMPEG
};

struct Process
{
    // wxProcess *process;
    long pid;
    int listRow;
    wxString path;
};

class FrameMain : public wxFrame
{
public:
    FrameMain(const wxString &title, const wxPoint &pos, const wxSize &size);
    void FillListCtrl(wxArrayString fileList);

private:
    void OnConvert(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnClear(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void UpdateStatusBar();
    wxString GenerateFfmpegCommand(wxString inputFile);
    void OnConversionEnd(wxProcessEvent &event);

    wxProcess *m_ffmpeg = nullptr;

    wxListView *m_fileList = nullptr;
    wxButton *m_buttonConvert = nullptr;
    wxButton *m_buttonClear = nullptr;
    wxArrayString m_validFileList;
    long m_ffmpegPID;
    // std::queue<long> m_ffmpegPIDList;
    // std::queue<wxProcess> m_ffmpegProcessList;

    wxDECLARE_EVENT_TABLE();
};

#endif