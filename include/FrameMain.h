#ifndef FRAMEMAIN_H
#define FRAMEMAIN_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "DropTarget.h"
#include "Converter.h"

#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <wx/process.h>

#include <set>
#include <list>

#include <cmath>

enum
{
    ID_Convert = wxID_LAST + 1,
    ID_Clear
};

class FrameMain : public wxFrame
{
public:
    FrameMain(const wxString &title, const wxPoint &pos, const wxSize &size);
    void FillListView();
    void OnConversionEnd(Process file);
    void OnBatchEnd();
    void UpdateProgress(double percent);

private:
    void OnResize(wxSizeEvent &event);
    void OnConvert(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnClear(wxCommandEvent &event);
    void OnCancel(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void UpdateStatusBar();
    void CreateProcessQueue();
    void AddToValidFileList(wxArrayString files);

    wxListView *m_listViewFiles = nullptr;
    wxButton *m_buttonConvert = nullptr;
    wxButton *m_buttonClearCancel = nullptr;
    Converter m_converter;

    std::set<wxString> m_validFileList;

    wxDECLARE_EVENT_TABLE();
};

#endif