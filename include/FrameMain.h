#ifndef FRAMEMAIN_H
#define FRAMEMAIN_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "DropTarget.h"
#include "Converter.h"
#include "SettingsDialog.h"

#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <wx/propdlg.h>
#include <wx/process.h>
#include <wx/choice.h>
#include <wx/config.h>

#include <set> //TODO: unordered_set?
#include <list>

#include <cmath>

enum
{
    ID_Convert = wxID_LAST + 1,
    ID_Clear,
    ID_Reset
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
    void OnPreferences(wxCommandEvent &event);
    void OnClear(wxCommandEvent &event);
    void OnCancel(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void UpdateStatusBar();
    void CreateProgressBar();
    void CreateProcessQueue();
    void AddToValidFileList(wxArrayString files);
    std::unique_ptr<wxConfig> GetConfig();

    wxListView *m_listViewFiles = nullptr;
    wxButton *m_buttonConvert = nullptr;
    wxButton *m_buttonClearCancel = nullptr;
    wxStatusBar *m_statusBar = nullptr;
    wxGauge *m_progressBar = nullptr;
    Converter m_converter;

    std::set<wxString> m_validFileList;

    wxDECLARE_EVENT_TABLE();
};

#endif