#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/config.h>
#include <wx/propdlg.h>
#include <wx/choice.h>
#include <ConverterConfig.h>

class SettingsDialog : public wxDialog
{
private:
    wxChoice *m_dropdownBitrate = nullptr;

    void OnOkClicked(wxCommandEvent &event)
    {
        auto bitrate = m_dropdownBitrate->GetStringSelection();
        m_converterConfig.SetBitrate(bitrate);
        this->EndModal(wxID_OK);
    }

public:
    SettingsDialog(ConverterConfig &converterConfig,
                   wxWindow *parent,
                   wxWindowID id,
                   const wxString &title,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   long style = wxDEFAULT_DIALOG_STYLE,
                   const wxString &name = wxDialogNameStr) : wxDialog(parent, id, title, pos, size, style, name),
                                                             m_converterConfig(converterConfig)
    {
        auto bitrateFromConfig = m_converterConfig.GetBitrate();

        auto *sizerVert = new wxBoxSizer(wxVERTICAL);
        auto *sizerBitrate = new wxBoxSizer(wxHORIZONTAL);
        auto bitrateChoices = wxArrayString();
        bitrateChoices.Add(_("64"));
        bitrateChoices.Add(_("96"));
        bitrateChoices.Add(_("128"));
        bitrateChoices.Add(_("192"));
        bitrateChoices.Add(_("224"));
        bitrateChoices.Add(_("320"));

        m_dropdownBitrate = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bitrateChoices);
        m_dropdownBitrate->SetStringSelection(bitrateFromConfig);

        auto *labelBitrate = new wxStaticText(this, wxID_ANY, _("Bitrate"));
        auto *labelKbps = new wxStaticText(this, wxID_ANY, _("Kbps"));
        sizerBitrate->Add(labelBitrate, 0, wxALIGN_CENTER | wxRIGHT, 10);
        sizerBitrate->Add(m_dropdownBitrate, 0, wxALIGN_CENTER);
        sizerBitrate->Add(labelKbps, 0, wxALIGN_CENTER | wxRIGHT, 10);
        sizerBitrate->AddStretchSpacer();

        auto *sizerButtons = new wxBoxSizer(wxHORIZONTAL);
        auto *buttonReset = new wxButton(this, wxID_ANY, "Reset to Default");
        buttonReset->Bind(wxEVT_BUTTON, [&](wxCommandEvent &event) { m_dropdownBitrate->SetSelection(2); });
        auto *buttonOk = new wxButton(this, wxID_OK);
        buttonOk->Bind(wxEVT_BUTTON, &SettingsDialog::OnOkClicked, this); //!
        auto *buttonCancel = new wxButton(this, wxID_CANCEL);
        buttonOk->SetDefault();
        sizerButtons->Add(buttonReset, 0, wxALIGN_CENTER);
        sizerButtons->Add(buttonCancel, 0, wxALIGN_CENTER);
        sizerButtons->Add(buttonOk, 0, wxALIGN_CENTER);

        sizerVert->Add(sizerBitrate, 1, wxEXPAND | wxALL, 10);
        sizerVert->Add(sizerButtons, 0, wxALIGN_RIGHT | wxRIGHT | wxLEFT | wxBOTTOM, 10);
        SetSizerAndFit(sizerVert);
    }
    virtual ~SettingsDialog()
    {
    }

private:
    ConverterConfig &m_converterConfig;
};

#endif