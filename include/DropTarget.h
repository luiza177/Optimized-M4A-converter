#ifndef DROPTARGET_H
#define DROPTARGET_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dnd.h>
#include <wx/filefn.h>
#include <functional>
#include <boost/filesystem.hpp>

class DropTarget : public wxFileDropTarget
{
public:
    void SetCallback(std::function<void(wxArrayString)> callback) { m_callback = callback; }

private:
    std::function<void(wxArrayString)> m_callback;

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
    {
        wxArrayString validFilesList;
        for (auto file = 0; file < filenames.GetCount(); ++file)
        {
            auto filePath = filenames[file];
            if (filePath.EndsWith(_(".wav")) || filePath.EndsWith(_(".m4a")))
            {
                validFilesList.Add(filePath);
            }
            // else if (wxDirExists(filePath)) { add all valid files }
        }

        if (validFilesList.IsEmpty())
        {
            wxMessageBox("No valid files, must be *.wav (or *.m4a).",
                         "Files - ERROR", wxOK | wxICON_INFORMATION);
            return false;
        }

        if (m_callback)
        {
            m_callback(validFilesList);
        }
        return true;
    }
};

#endif