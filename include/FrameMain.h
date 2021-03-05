#ifndef FRAMEMAIN_H
#define FRAMEMAIN_H

#include "DropTarget.h"

#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <wx/process.h>

#include <set>
#include <list>

enum
{
    ID_Convert = wxID_LAST + 1,
    ID_Clear,
    ID_FFMPEG
};

struct Process
{
    long listRow;
    wxString path;
};

class FrameMain : public wxFrame
{
public:
    FrameMain(const wxString &title, const wxPoint &pos, const wxSize &size);
    void FillListView();

private:
    void OnConvert(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnClear(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void UpdateStatusBar();
    wxString GenerateFfmpegCommand(wxString inputFile);
    void Convert();
    void OnConversionEnd(wxProcessEvent &event);
    void CreateProcessQueue();
    wxString GetResourcesDir();
    void AddToValidFileList(wxArrayString files);
    void OnResize(wxSizeEvent &event);

    wxProcess *m_ffmpeg = nullptr;

    wxListView *m_listViewFiles = nullptr; //? rename to m_listViewFiles
    wxButton *m_buttonConvert = nullptr;
    wxButton *m_buttonClear = nullptr;
    std::set<wxString> m_validFileList;
    long m_ffmpegPID;
    std::list<Process> m_ffmpegProcessList;

    wxDECLARE_EVENT_TABLE();
};

#endif