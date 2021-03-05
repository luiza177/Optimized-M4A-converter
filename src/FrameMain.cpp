#include "FrameMain.h"

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
    m_buttonClearCancel = new wxButton(panelMain, ID_Clear, _("Clear"), wxDefaultPosition, wxSize(70, 20));
    m_buttonClearCancel->Bind(wxEVT_BUTTON, &FrameMain::OnClear, this);

    // list
    m_listViewFiles = new wxListView(panelMain, wxID_ANY, wxDefaultPosition, wxSize(500, 300), wxLC_REPORT);
    m_listViewFiles->Bind(wxEVT_KEY_DOWN, wxKeyEventHandler(FrameMain::OnKeyDown), this); //why?

    m_listViewFiles->AppendColumn(_("File"), wxLIST_FORMAT_LEFT, 400);
    m_listViewFiles->AppendColumn(_("Status"), wxLIST_FORMAT_CENTER, 100);

    // sizers
    wxBoxSizer *sizerVertMain = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerHorMain = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *sizerHorButtons = new wxBoxSizer(wxHORIZONTAL);

    sizerHorMain->Add(m_listViewFiles, 1, wxEXPAND);
    sizerVertMain->Add(sizerHorMain, 1, wxEXPAND);

    sizerHorButtons->Add(m_buttonClearCancel, 0, wxRIGHT, 10);
    sizerHorButtons->Add(m_buttonConvert);

    sizerVertMain->Add(sizerHorButtons, 0, wxALIGN_RIGHT | wxALL, 10);

    panelMain->SetSizer(sizerVertMain); //? move to end after status bar and change to AndFit

    // drag-n-drop
    DropTarget *dropTarget = new DropTarget();
    panelMain->SetDropTarget(dropTarget);
    dropTarget->SetCallback(std::bind(&FrameMain::AddToValidFileList, this, std::placeholders::_1)); // can't just say FrameMain::FillListView

    // status bar
    CreateStatusBar();
    SetStatusText("0 files");

    // converter
    m_converter = new Converter();
    m_converter->SetFileStatusCallback(std::bind(&FrameMain::OnConversionEnd, this, std::placeholders::_1));
    m_converter->SetBatchEndCallback(std::bind(&FrameMain::OnBatchEnd, this));
}

void FrameMain::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void FrameMain::OnAbout(wxCommandEvent &event)
{
    // TODO: elaborate
    wxMessageBox(_("This is a WAV to AAC converter that produces optimized M4A/AAC files"),
                 _("About Optimized M4A Converter"),
                 wxOK | wxICON_INFORMATION);
}

void FrameMain::CreateProcessQueue()
{
    std::list<Process> processList;
    for (int row = 0; row < m_listViewFiles->GetItemCount(); ++row)
    {
        auto listRow = static_cast<long>(row);
        Process process = Process{};
        process.listRow = listRow;
        process.path = m_listViewFiles->GetItemText(listRow);
        processList.push_back(process);
    }
    m_converter->SetListAndConvert(processList);
}

void FrameMain::OnConvert(wxCommandEvent &event)
{
    if (m_listViewFiles->GetItemCount() > 0)
    {
        m_buttonConvert->Disable();
        m_buttonClearCancel->SetLabel(_("Cancel"));
        m_buttonClearCancel->Bind(wxEVT_BUTTON, &FrameMain::OnCancel, this);
        CreateProcessQueue();
    }
    else
    {
        wxLogMessage(_("Nothing to convert!"));
    }
}

void FrameMain::OnClear(wxCommandEvent &event)
{
    if (m_listViewFiles->GetItemCount() > 0)
    {
        m_listViewFiles->DeleteAllItems();
        m_validFileList.clear();
        UpdateStatusBar();
    }
}

void FrameMain::OnCancel(wxCommandEvent &event)
{
    m_converter->Cancel();
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
    wxArrayString validFileList;
    openFileDialog.GetPaths(validFileList); // fills the param fileList
    AddToValidFileList(validFileList);
}

void FrameMain::AddToValidFileList(wxArrayString fileList)
{
    std::for_each(fileList.begin(), fileList.end(), [this](wxString file) { m_validFileList.insert(file); });
    FillListView();
}

void FrameMain::FillListView()
{
    m_listViewFiles->DeleteAllItems();
    std::for_each(m_validFileList.begin(), m_validFileList.end(), [this](wxString file) {
        this->m_listViewFiles->InsertItem(0, file);
    });

    UpdateStatusBar();
}

void FrameMain::UpdateStatusBar()
{
    if (m_listViewFiles->GetItemCount() == 1)
    {
        SetStatusText(_("1 file"));
    }
    else
    {
        wxString filesNumber = wxString::Format(wxT("%d files"), m_listViewFiles->GetItemCount());
        SetStatusText(filesNumber);
    }
}

void FrameMain::OnKeyDown(wxKeyEvent &event)
{
    event.Skip(); // needs to be here to work
    auto key = event.GetKeyCode();
    if (key == WXK_DELETE) //|| key == WXK_BACK)
    {
        auto selectedIdx = m_listViewFiles->GetFirstSelected(); // TODO: understand this
        while (selectedIdx > -1)
        {
            m_validFileList.erase(m_listViewFiles->GetItemText(selectedIdx));
            m_listViewFiles->DeleteItem(selectedIdx);
            selectedIdx = m_listViewFiles->GetNextSelected(-1);
        }
        event.Skip(false);
        UpdateStatusBar();
    }
}

void FrameMain::OnConversionEnd(Process file)
{
    const int STATUS_COL = 1;
    switch (file.status)
    {
    case 0:
    {
        m_listViewFiles->SetItem(file.listRow, STATUS_COL, _("DONE"));
        m_listViewFiles->SetItemTextColour(file.listRow, wxColour("green"));
        break;
    }
    case -1:
    {
        m_listViewFiles->SetItem(file.listRow, STATUS_COL, _("CANCELED"));
        m_listViewFiles->SetItemTextColour(file.listRow, wxColour("red"));
        break;
    }
    case 1:
    {
        m_listViewFiles->SetItem(file.listRow, STATUS_COL, _("ERROR"));
        m_listViewFiles->SetItemTextColour(file.listRow, wxColour("red"));
        break;
    }
    default:
        m_listViewFiles->SetItem(file.listRow, STATUS_COL, _("Unknown error"));
        m_listViewFiles->SetItemTextColour(file.listRow, wxColour("grey"));
    }
}

void FrameMain::OnBatchEnd()
{
    m_buttonConvert->Enable();
    m_buttonClearCancel->SetLabel(_("Clear"));
    m_buttonClearCancel->Bind(wxEVT_BUTTON, &FrameMain::OnClear, this);
}

void FrameMain::OnResize(wxSizeEvent &event)
{
    event.Skip();
    auto windowWidth = GetSize().GetWidth();
    m_listViewFiles->SetColumnWidth(0, windowWidth - 100);
}

//FUTURE
//TODO: progress bar/wxGauge --> capture stderr
//TODO: create folder for output //? Use Boost for file system?

//COSMETIC
//TODO: padding listctrl ??
//TODO: File column stretch on window resize (always width - 100, for other column)
//TODO: ellipsize from left listctrl item