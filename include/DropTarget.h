#ifndef DROPTARGET_H
#define DROPTARGET_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dnd.h>
#include <functional>

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
            auto lastLetter = filenames[file].rbegin();
            std::string fileExtension = "";
            for (auto iter = lastLetter; *iter != '.'; ++iter)
            {
                fileExtension.push_back(*iter);
            }
            std::reverse(fileExtension.begin(), fileExtension.end());
            if (fileExtension == "wav" || fileExtension == "m4a")
            {
                validFilesList.Add(filenames[file]);
            }
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

//TODO: wxValidator?
//TODO: override OnDrop to check for directories??
#endif