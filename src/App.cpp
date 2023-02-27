#include "../include/FrameMain.h"

class App : public wxApp
{
public:
    virtual bool OnInit()
    {
        FrameMain *frame = new FrameMain(_("Optimized M4A Converter"), wxPoint(50, 50), wxSize(500, 200));
        frame->Show(true);
        return true;
    }
};

// clang-format off
wxBEGIN_EVENT_TABLE(FrameMain, wxFrame)
    EVT_MENU(ID_Convert, FrameMain::OnConvert)
    EVT_MENU(wxID_EXIT, FrameMain::OnExit)
    EVT_MENU(wxID_ABOUT, FrameMain::OnAbout)
    EVT_MENU(wxID_OPEN, FrameMain::OnOpen)
    EVT_MENU(wxID_PREFERENCES, FrameMain::OnPreferences)
    EVT_BUTTON(ID_Convert, FrameMain::OnConvert)
    EVT_SIZE(FrameMain::OnResize)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(App);