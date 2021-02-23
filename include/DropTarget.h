#ifndef DROPTARGET_H
#define DROPTARGET_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dnd.h>
#include <wx/filefn.h>
#include <functional>

class DropTarget : public wxFileDropTarget
{
public:
    void SetCallback(std::function<void(wxArrayString)> callback) { m_callback = callback; }

private:
    std::function<void(wxArrayString)> m_callback;

    bool IsValidFile(wxString file)
    {
        //? file.find_last_of(".");
        // TODO: use wxString functions
        auto lastLetter = file.rbegin();
        std::string fileExtension = "";
        for (auto iter = lastLetter; *iter != '.'; ++iter)
        {
            if (*iter == '/' || *iter == '\\')
            {
                //! Directories are not accepted //TODO: deal with directory
                // if (wxDirExists(file))
                // {
                return false;
                // }
            }
            fileExtension.push_back(*iter);
        }
        std::reverse(fileExtension.begin(), fileExtension.end());

        return (fileExtension == "wav" || fileExtension == "m4a");
    }

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
    {
        wxArrayString validFilesList;
        for (auto file = 0; file < filenames.GetCount(); ++file)
        {
            auto fileName = filenames[file];
            if (IsValidFile(fileName))
            {
                validFilesList.Add(fileName);
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
#endif