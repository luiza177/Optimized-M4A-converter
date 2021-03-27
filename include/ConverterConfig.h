#pragma once

#include <wx/config.h>

class ConverterConfig final
{
public:
    ConverterConfig() : m_config(new wxConfig("m4a-converter")) {}
    ~ConverterConfig()
    {
        delete m_config;
    }
    const wxString GetBitrate() const
    {
        return m_config->Read(BITRATE_KEY, BITRATE_DEFAULT_VALUE);
    }

    void SetBitrate(wxString bitrate)
    {
        m_config->Write(BITRATE_KEY, bitrate);
    }

private:
    const wxString BITRATE_KEY = "bitrate";
    const wxString BITRATE_DEFAULT_VALUE = "128";
    wxConfig *m_config = nullptr;
};