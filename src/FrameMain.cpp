#include "../include/FrameMain.h"
#include <iostream>

FrameMain::FrameMain(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    // menu bar [wxMenuBar, wxMenu, (wxMenuItem)]
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Convert, _("&Convert...\tCtrl-Shift-C"), _("Convert shown files")); // "\t" tells a shortcut will follow
    menuFile->Append(wxID_OPEN, _("&Open...\tCtrl-O"), _("Select files to be converted"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT); // doesn't have default shortcut on Windows

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File")); // "&" turns "F" into an 'accelerator', for Windows' Alt + letter
    menuBar->Append(menuHelp, _("&Help"));
    SetMenuBar(menuBar);

    // main panel
    wxPanel *panelMain = new wxPanel(this, wxID_ANY);
    SetMinSize(wxSize(500, 200));

    // drag-n-drop
    DropTarget *dropTarget = new DropTarget();
    panelMain->SetDropTarget(dropTarget);
    dropTarget->SetCallback(std::bind(&FrameMain::FillListCtrl, this, std::placeholders::_1)); // can't just say FrameMain::FillListCtrl

    // status bar
    CreateStatusBar();
    SetStatusText("0 files");

    // buttons
    wxButton *buttonConvert = new wxButton(panelMain, ID_Convert, _("Convert"), wxDefaultPosition, wxSize(100, 20));
    buttonConvert->SetDefault();
    wxButton *buttonClear = new wxButton(panelMain, ID_Clear, _("Clear"), wxDefaultPosition, wxSize(50, 20));

    // list
    m_listCtrl = new wxListCtrl(panelMain, wxID_ANY, wxDefaultPosition, wxSize(500, 300), wxLC_REPORT);
    m_listCtrl->AppendColumn(_("File"), wxLIST_FORMAT_LEFT, 400);
    m_listCtrl->AppendColumn(_("Status"), wxLIST_FORMAT_CENTER, 100);

    // sizers
    wxBoxSizer *sizerVertMain = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerHorMain = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *sizerHorButtons = new wxBoxSizer(wxHORIZONTAL);

    sizerHorMain->Add(m_listCtrl, 1, wxEXPAND);
    sizerVertMain->Add(sizerHorMain, 1, wxEXPAND);

    sizerHorButtons->Add(buttonClear, 0, wxRIGHT, 10);
    sizerHorButtons->Add(buttonConvert);

    sizerVertMain->Add(sizerHorButtons, 0, wxALIGN_RIGHT | wxALL, 10);

    panelMain->SetSizerAndFit(sizerVertMain);
}

void FrameMain::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void FrameMain::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(_("This is a WAV to AAC converter that produces optimized M4A/AAC files"),
                 _("About Optimized M4A Converter"),
                 wxOK | wxICON_INFORMATION);
}

void FrameMain::OnConvert(wxCommandEvent &event)
{
    PushStatusText(_("DONE!"));
    // PopStatusText();
}

void FrameMain::OnClear(wxCommandEvent &event)
{
    m_listCtrl->DeleteAllItems();
    PopStatusText();
}

void FrameMain::OnOpen(wxCommandEvent &event)
{
    wxFileDialog openFileDialog(this,
                                _("Select files to be converted"),
                                "",
                                "",
                                "WAV or M4A files (*.wav; *.m4a)|*.wav;*.m4a",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    openFileDialog.GetPaths(m_validFileList); // fills the param fileList
    FillListCtrl(m_validFileList);
}

void FrameMain::FillListCtrl(wxArrayString fileList)
{
    std::for_each(fileList.begin(), fileList.end(), [this](wxString file) { this->m_listCtrl->InsertItem(0, file); });
    UpdateStatusBar();
}

void FrameMain::UpdateStatusBar()
{
    if (m_listCtrl->GetItemCount() == 1)
    {
        PushStatusText(_("1 file"));
    }
    else
    {
        wxString filesNumber = wxString::Format(wxT("%d files"), m_listCtrl->GetItemCount());
        PushStatusText(filesNumber);
    }
}

// void FrameMain::OnDeleteItem(wxListEvent &event)
// {
//     std::cout << "blah\n";
//     if (m_listCtrl->GetItemCount())
//     {
//         std::cout << event.GetId();
//         m_listCtrl->DeleteItem(event.GetId());
//     }
//     else
//     {
//         wxLogMessage(_("Nothing to delete"));
//     }
// }
//TODO: check if already exists
//TODO: padding listctrl text
//TODO: ellipsize from left listctrl item
//TODO: implement ability to delete file list
//TODO: drag n drop directories
//TODO: onResize adjust column width