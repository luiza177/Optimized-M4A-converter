#include "../include/FrameMain.h"
#include <iostream>

FrameMain::FrameMain(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    SetMinSize(wxSize(500, 200)); //? Move to panel creation

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
    m_buttonConvert = new wxButton(panelMain, ID_Convert, _("Convert"), wxDefaultPosition, wxSize(100, 20));
    m_buttonConvert->SetDefault();
    m_buttonClear = new wxButton(panelMain, ID_Clear, _("Clear"), wxDefaultPosition, wxSize(70, 20));

    // list
    m_fileList = new wxListView(panelMain, wxID_ANY, wxDefaultPosition, wxSize(500, 300), wxLC_REPORT);
    m_fileList->Bind(wxEVT_KEY_DOWN, wxKeyEventHandler(FrameMain::OnKeyDown), this); //why?

    m_fileList->AppendColumn(_("File"), wxLIST_FORMAT_LEFT, 400);
    m_fileList->AppendColumn(_("Status"), wxLIST_FORMAT_CENTER, 100);

    // sizers
    wxBoxSizer *sizerVertMain = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerHorMain = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *sizerHorButtons = new wxBoxSizer(wxHORIZONTAL);

    sizerHorMain->Add(m_fileList, 1, wxEXPAND);
    sizerVertMain->Add(sizerHorMain, 1, wxEXPAND);

    sizerHorButtons->Add(m_buttonClear, 0, wxRIGHT, 10);
    sizerHorButtons->Add(m_buttonConvert);

    sizerVertMain->Add(sizerHorButtons, 0, wxALIGN_RIGHT | wxALL, 10);

    panelMain->SetSizer(sizerVertMain); //? move to end after status bar and change to AndFit

    // drag-n-drop
    DropTarget *dropTarget = new DropTarget();
    panelMain->SetDropTarget(dropTarget);
    dropTarget->SetCallback(std::bind(&FrameMain::FillListCtrl, this, std::placeholders::_1)); // can't just say FrameMain::FillListCtrl

    // status bar
    CreateStatusBar();
    SetStatusText("0 files");

    m_ffmpeg = new wxProcess(this, ID_FFMPEG);
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

wxString FrameMain::GenerateFfmpegCommand(wxString inputFile)
{
    wxString ffmpegCommand = "ffmpeg -y -i \""; // -y flag is always overwrite
    //TODO: create folder for output
    std::string outputFile = inputFile.ToStdString();
    //TODO: refactor
    outputFile.pop_back();
    outputFile.pop_back();
    outputFile.pop_back();
    outputFile.append("m4a");
    wxString ffmpegFlags = "\" -movflags +faststart -c:a aac -b:a 128000 \"";
    ffmpegCommand += inputFile + ffmpegFlags + _(outputFile) + "\"";
    return ffmpegCommand;
}

void FrameMain::OnConvert(wxCommandEvent &event)
{
    if (m_fileList->GetItemCount() > 0)
    {
        //for each listctrl item do

        wxString ffmpegCommand = GenerateFfmpegCommand(m_fileList->GetItemText(0));
        // wxArrayString output;
        m_buttonConvert->Disable();
        m_buttonClear->SetLabel(_("Cancel"));
        m_ffmpegPID = wxExecute(ffmpegCommand, wxEXEC_ASYNC, m_ffmpeg);
    }
    else
    {
        wxLogMessage(_("Nothing to convert!"));
    }
}

void FrameMain::OnClear(wxCommandEvent &event)
{

    if (m_ffmpeg->Exists(m_ffmpegPID))
    {
        wxKillError *err;
        wxKill(m_ffmpegPID, wxSIGTERM, err);
        // TODO: delete residue?
    }
    else
    {
        m_fileList->DeleteAllItems();
        PopStatusText();
    }
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
    std::for_each(fileList.begin(), fileList.end(), [this](wxString file) { this->m_fileList->InsertItem(0, file); });
    UpdateStatusBar();
}

void FrameMain::UpdateStatusBar()
{
    if (m_fileList->GetItemCount() == 1)
    {
        PushStatusText(_("1 file"));
    }
    else
    {
        wxString filesNumber = wxString::Format(wxT("%d files"), m_fileList->GetItemCount());
        PushStatusText(filesNumber);
    }
}

void FrameMain::OnKeyDown(wxKeyEvent &event)
{
    event.Skip(); // needs to be here to work
    auto key = event.GetKeyCode();
    if (key == WXK_DELETE) //|| key == WXK_BACK)
    {
        auto selectedIdx = m_fileList->GetFirstSelected(); // TODO: understand this
        while (selectedIdx > -1)
        {
            m_fileList->DeleteItem(selectedIdx);
            selectedIdx = m_fileList->GetNextSelected(-1);
        }
        event.Skip(false);
    }
}

void FrameMain::OnConversionEnd(wxProcessEvent &event)
{
    PushStatusText(_("DONE!"));
    m_buttonConvert->Enable();
    m_buttonClear->SetLabel(_("Clear"));
}

//MVP
//TODO: drag n drop directories

//FUNCTIONAL
//TODO: capture stdout (or stderr)
//TODO: get exit code

//FUTURE
//TODO: check if already exists --> std::map or std::set

//COSMETIC
//TODO: padding listctrl ??
//TODO: File column stretch on window resize (always width - 100, for other column)
//TODO: ellipsize from left listctrl item