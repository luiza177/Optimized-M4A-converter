#ifndef FRAMEMAIN_H
#define FRAMEMAIN_H

// #include <wx/wxprec.h>
// #ifndef WX_PRECOMP
// #include <wx/wx.h>
// #endif

#include "DropTarget.h"
#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <wx/process.h>

enum
{
    ID_Convert = 1,
    ID_Clear,
    ID_DeleteItem
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

    wxProcess *m_ffmpeg = nullptr;

    wxListView *m_listCtrl = nullptr;
    wxArrayString m_validFileList;

    wxDECLARE_EVENT_TABLE();
};

#endif