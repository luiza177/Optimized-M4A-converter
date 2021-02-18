// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <algorithm>
#include <functional>

class App : public wxApp
{
public:
    virtual bool OnInit();
};

class DropTarget : public wxFileDropTarget
{
public:
    void SetCallback(std::function<void(wxString)> callback);

private:
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames);
    std::function<void(wxString)> m_callback;
    // wxArrayString m_fileNames; // + get
};

class FrameMain : public wxFrame //, wxFileDropTarget
{
public:
    FrameMain(const wxString &title, const wxPoint &pos, const wxSize &size);
    //virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames);
    void FillListCtrl(wxString file);
    // void FillListCtrl(wxArrayString fileList);

private:
    void OnConvert(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnClear(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);

    wxListCtrl *m_listCtrl = nullptr;

    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_Convert = 1,
    ID_Clear
};

// clang-format off
wxBEGIN_EVENT_TABLE(FrameMain, wxFrame)
    EVT_MENU(ID_Convert, FrameMain::OnConvert)
    EVT_MENU(wxID_EXIT, FrameMain::OnExit)
    EVT_MENU(wxID_ABOUT, FrameMain::OnAbout)
    EVT_MENU(wxID_OPEN, FrameMain::OnOpen)
    EVT_BUTTON(ID_Convert, FrameMain::OnConvert)
    EVT_BUTTON(ID_Clear, FrameMain::OnClear)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(App);
// clang-format on

bool App::OnInit()
{
    FrameMain *frame = new FrameMain(_("Optimized M4A Converter"), wxPoint(50, 50), wxSize(500, 200));
    frame->Show(true);
    return true;
}

FrameMain::FrameMain(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    // SetMinSize(wxSize(150, 100));
    SetMinSize(wxSize(500, 200));

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

    // buttons
    wxButton *buttonConvert = new wxButton(panelMain, ID_Convert, _("Convert"), wxDefaultPosition, wxSize(100, 20));
    buttonConvert->SetDefault();
    wxButton *buttonClear = new wxButton(panelMain, ID_Clear, _("Clear"), wxDefaultPosition, wxSize(50, 20));

    // list
    // wxListCtrl *listCtrl = new wxListCtrl(panelMain, wxID_ANY, wxDefaultPosition, wxSize(500, 300), wxLC_REPORT);
    m_listCtrl = new wxListCtrl(panelMain, wxID_ANY, wxDefaultPosition, wxSize(500, 300), wxLC_REPORT);
    m_listCtrl->AppendColumn(_("File"), wxLIST_FORMAT_LEFT, 300);
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

    panelMain->SetSizer(sizerVertMain);

    // drag-n-drop
    // SetDropTarget(this);
    // panelMain->SetDropTarget(new DropTarget);

    DropTarget *dropTarget = new DropTarget();
    panelMain->SetDropTarget(dropTarget);
    dropTarget->SetCallback(std::bind(&FrameMain::FillListCtrl, this, std::placeholders::_1));

    // status bar
    CreateStatusBar();
    SetStatusText("0 files");
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
    // wxSleep(5);
    // PopStatusText();
}

void FrameMain::OnClear(wxCommandEvent &event)
{
    wxLogMessage("Clear!");
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

    // temp ????
    wxArrayString fileList;
    openFileDialog.GetPaths(fileList);
    if (fileList.Count() == 1)
    {
        PushStatusText("1 file");
    }
    else
    {
        wxString filesNumber = wxString::Format(wxT("%d files"), (int)fileList.Count());
        PushStatusText(filesNumber);
    }
    wxString displayList = "";
    std::for_each(fileList.begin(), fileList.end(), [&displayList](const wxString f) { displayList.Append(f);  displayList.Append("\n"); });
    wxLogMessage(displayList);
}

bool DropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
{
    wxString filesList = "";
    for (auto file = 0; file < filenames.GetCount(); ++file)
    {
        auto lastLetter = filenames[file].rbegin();
        std::string fileExtension = "";
        for (auto iter = lastLetter; *iter != '.'; ++iter)
        {
            fileExtension.push_back(*iter);
        }
        std::reverse(fileExtension.begin(), fileExtension.end());
        if (fileExtension == "wav" || fileExtension == "m4a")
        {
            filesList.Append(filenames[file]);
            filesList.Append("\n");
        }
    }

    if (filesList.IsEmpty())
    {
        wxMessageBox("No valid files, must be *.wav (or *.m4a).",
                     "Files - ERROR", wxOK | wxICON_INFORMATION);
        return false;
    }
    // wxMessageBox(filesList,
    //              "Files", wxOK | wxICON_INFORMATION);
    if (m_callback)
    {
        m_callback(filesList);
    }
    return true;
}

void DropTarget::SetCallback(std::function<void(wxString)> callback)
{
    m_callback = callback;
}

void FrameMain::FillListCtrl(wxString file)
{
    m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), file);
}
// void FrameMain::FillListCtrl(wxArrayString fileList)
// {
//     m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), fileList);
// }

// FIXME: crashes on exit
// bool FrameMain::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
// {
//     wxLogMessage("blah!");
//     return true;
// }
