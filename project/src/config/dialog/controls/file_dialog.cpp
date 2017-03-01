/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file selection dialog control
*******************************************************************************/
#include "../../../globals.h"
#include <string>
#include <stdexcept>
#include <io.h>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <Commdlg.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "dialog.h"
#include "button.hpp"
#include "msg_box.h"
#include "text_field.hpp"
#include "file_dialog.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create dialog box
/// @param[in] instance  Current instance handle
/// @param[in] mode      File selection mode
/// @throws invalid_argument  Invalid instance
FileDialog::FileDialog(library_instance_t instance, file_mode_t mode) 
    : Dialog(instance), m_mode(mode), m_fieldId(0), m_browserId(0), m_filePath(L"./file.csv"s)
{
    Dialog::event_handler_t eventHandler;
    eventHandler.handler = onInit;
    Dialog::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = onCommand;
    Dialog::registerEvent(dialog_event_t::command, eventHandler);
    eventHandler.handler = onConfirm;
    Dialog::registerEvent(dialog_event_t::confirm, eventHandler);
}

/// @brief Display modal dialog box
/// @param[in] dialogResourceId   Dialog description identifier
/// @param[in] fieldResourceId    File path field identifier
/// @param[in] hParentWindow      Parent window handle
/// @param[in] browserResourceId  Browse button identifier
/// @param[in] defaultFile        Default file path
/// @param[in] title              Dialog title
/// @returns Dialog result
/// @throws runtime_error  Dialog creation error or runtime error
Dialog::result_t FileDialog::showDialog(const int32_t dialogResourceId, const int32_t fieldResourceId, window_handle_t hParentWindow, 
                                        const int32_t browserResourceId, const std::wstring& defaultFile, const std::wstring title)
{
    m_filePath = defaultFile;
    m_fieldId = fieldResourceId;
    m_browserId = browserResourceId;
    return Dialog::showDialog(dialogResourceId, hParentWindow, title, false);
}

/// @brief Dialog initialization event handler
EVENT_RETURN FileDialog::onInit(Dialog::event_args_t args)
{
    FileDialog& parent = args.getParent<FileDialog>();
    TextField::setValue(args.window, parent.m_fieldId, parent.m_filePath);
    return EVENT_RETURN_VALID;
}



/// @brief Dialog initialization event handler
EVENT_RETURN FileDialog::onCommand(Dialog::event_args_t args)
{
    if (args.isEventSignedAction(Button::event_t::clicked)) // buttons
    {
        FileDialog& parent = args.getParent<FileDialog>();
        if (args.controlId() == parent.m_browserId)
        {
            // file browser
            switch (parent.m_mode)
            {
                // browse to load a file
                case file_mode_t::load:
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    WCHAR path[MAX_PATH];
                    memset(path, 0, sizeof(path));

                    OPENFILENAME ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = args.window;
                    ofn.lpstrDefExt = nullptr;
                    ofn.lpstrFile = path;
                    ofn.lpstrFile[0] = '\0';
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrFilter = L"CSV files (*.csv)\0*.csv\0All files (*.*)\0*.*\0";
                    ofn.nFilterIndex = 0;
                    ofn.lpstrInitialDir = nullptr;
                    ofn.lpstrTitle = nullptr;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    // open dialog
                    if (GetOpenFileName(&ofn) && path[0])
                    {
                        // get file path
                        parent.m_filePath = std::wstring(path);
                        TextField::setValue(args.window, parent.m_fieldId, parent.m_filePath);
                        return EVENT_RETURN_VALID;
                    }
                    #endif
                    break;
                }

                // browse to save a file
                case file_mode_t::save:
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    WCHAR path[MAX_PATH];
                    memset(path, 0, sizeof(path));

                    OPENFILENAME ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = args.window;
                    ofn.lpstrDefExt = nullptr;
                    ofn.lpstrFile = path;
                    ofn.lpstrFile[0] = '\0';
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrFilter = L"CSV files (*.csv)\0*.csv\0All files (*.*)\0*.*\0";
                    ofn.nFilterIndex = 0;
                    ofn.lpstrInitialDir = nullptr;
                    ofn.lpstrTitle = nullptr;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    // save dialog
                    if (GetSaveFileName(&ofn) && path[0])
                    {
                        // get file path
                        parent.m_filePath = std::wstring(path);
                        TextField::setValue(args.window, parent.m_fieldId, parent.m_filePath);
                        return EVENT_RETURN_VALID;
                    }
                    #endif
                    break;
                }

                // browse for save folder
                case file_mode_t::savePath:
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    BROWSEINFO binf = { 0 };
                    binf.lParam = reinterpret_cast<LPARAM>(parent.m_filePath.c_str());
                    binf.lpfn = folderbrowserEventHandler;
                    LPITEMIDLIST pidl = SHBrowseForFolder(&binf);

                    // get folder path
                    if (pidl != 0)
                    {
                        WCHAR path[MAX_PATH];
                        SHGetPathFromIDList(pidl, path);
                        IMalloc* imalloc = 0;
                        if (SUCCEEDED(SHGetMalloc(&imalloc)))
                        {
                            imalloc->Free(pidl);
                            imalloc->Release();
                        }

                        parent.m_filePath = std::wstring(path);
                        TextField::setValue(args.window, parent.m_fieldId, parent.m_filePath);
                        return EVENT_RETURN_VALID;
                    }
                    #endif
                    break;
                }
            }
        }
    }
    return EVENT_RETURN_IGNORE;
}



/// @brief Dialog confirm event handler - check validity
EVENT_RETURN FileDialog::onConfirm(Dialog::event_args_t args)
{
    FileDialog& parent = args.getParent<FileDialog>();

    // get path
    std::wstring checkedPath;
    if (parent.m_mode == file_mode_t::save) 
    {
        // if file save, extract containing folder path
        size_t pos = parent.m_filePath.find_last_of(L'/');
        if (pos == std::wstring::npos)
            pos = parent.m_filePath.find_last_of(L'\\');
        if (pos == std::wstring::npos)
            checkedPath = parent.m_filePath.substr(0, pos + 1u);
        else
            checkedPath = L"./"s;
    }
    else
    {
        checkedPath = parent.m_filePath;
    }

    // check path validity
    #if _DIALOGAPI == DIALOGAPI_WIN32
    if (_waccess(checkedPath.c_str(), 06) != 0)
    #else
    //...
    #endif
    {
        MsgBox::showMessage(L"Invalid file path..."s, L"The selected file path is invalid, or the application doesn't have the rights to access it."s, 
                            args.window, MsgBox::button_set_t::ok, MsgBox::message_icon_t::error);
        return EVENT_RETURN_ERROR;
    }
    return EVENT_RETURN_VALID;
}



#if _DIALOGAPI == DIALOGAPI_WIN32
/// @brief Folder browser event handler
int CALLBACK FileDialog::folderbrowserEventHandler(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED) {
        LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
        ::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
    }
    return 0;
}
#endif
