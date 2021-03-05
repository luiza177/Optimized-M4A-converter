#ifndef FRAMEMAIN_H
#define FRAMEMAIN_H

#include "DropTarget.h"
#include "Converter.h"

#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <wx/process.h>

#include <set>
#include <list>

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
    void CreateProcessQueue(); // calls convert() in new class?
    void AddToValidFileList(wxArrayString files);

    wxListView *m_listViewFiles = nullptr;
    wxButton *m_buttonConvert = nullptr;
    wxButton *m_buttonClearCancel = nullptr;
    Converter *m_converter = nullptr;

    std::set<wxString> m_validFileList;

    wxDECLARE_EVENT_TABLE();
};

#endif