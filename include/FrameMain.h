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
    wxString GetResourcesDir(); // to new class

    void OnResize(wxSizeEvent &event);
    void OnConvert(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnClear(wxCommandEvent &event);
    void OnCancel(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void UpdateStatusBar();
    wxString GenerateFfmpegCommand(wxString inputFile); // to new class
    void Convert();                                     // to new class
    void OnConversionEnd(wxProcessEvent &event);        // to new class
    void CreateProcessQueue();                          // calls convert() in new class?
    void AddToValidFileList(wxArrayString files);

    wxProcess *m_ffmpeg = nullptr; // to new class
    long m_ffmpegPID;              // to new class?

    wxListView *m_listViewFiles = nullptr;
    wxButton *m_buttonConvert = nullptr;
    wxButton *m_buttonClearCancel = nullptr;

    std::set<wxString> m_validFileList;
    std::list<Process> m_ffmpegProcessList; // send to or live in new class?

    wxDECLARE_EVENT_TABLE();
};

#endif