/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_pmanager_winview.cpp
Description : configuration dialog page - profiles manager - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "config_pmanager_winview.h"
#include <windowsx.h>

#define MAX_ROWS_WITHOUT_SCROLL 13u
#define LISTVIEW_WIDTH 389
#define LISTVIEW_NAME_WIDTH_NOSCROLL 324
#define LISTVIEW_NAME_WIDTH_SCROLL   307

ConfigPageManagerView* ConfigPageManagerView::s_pCurrentPage = NULL; // current page (static access)

/// <summary>Create page view container</summary>
/// <param name="pController">Controller reference</param>
ConfigPageManagerView::ConfigPageManagerView(ConfigPage* pController) : ConfigPageView(pController)
{
    m_hPage = NULL;
    res_dataTable = NULL;
    res_iconAdd = NULL;
    res_iconEdit = NULL;
    res_iconDel = NULL;
    res_iconIn = NULL;
    res_iconOut = NULL;
    for (int t = 0; t < PMANAGER_TOOLTIPS_NB; ++t)
        res_tooltips[t] = NULL;
}
/// <summary>Destroy dialog view container</summary>
ConfigPageManagerView::~ConfigPageManagerView()
{
    // delete tooltips
    for (int t = 0; t < PMANAGER_TOOLTIPS_NB; ++t)
    {
        if (res_tooltips[t] != NULL) DestroyWindow(res_tooltips[t]);
        res_tooltips[t] = NULL;
    }
    // delete items
    if (res_dataTable != NULL) DestroyWindow(res_dataTable);
    if (res_iconAdd != NULL) DestroyIcon((HICON)res_iconAdd);
    if (res_iconEdit != NULL) DestroyIcon((HICON)res_iconEdit);
    if (res_iconDel != NULL) DestroyIcon((HICON)res_iconDel);
    if (res_iconIn != NULL) DestroyIcon((HICON)res_iconIn);
    if (res_iconOut != NULL) DestroyIcon((HICON)res_iconOut);
    if (m_hPage != NULL)
        DestroyWindow(m_hPage);
    m_hPage = NULL;
}

/// <summary>Create new dialog page</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Page created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigPageManagerView* ConfigPageManagerView::createPage(ConfigPageManager* pController)
{
    ConfigPageManagerView* pPage = new ConfigPageManagerView(pController);
    return pPage;
}

/// <summary>Refresh language-dependent page content</summary>
/// <param name="isFirstInit">First time (only labels) or update (all)</param>
void ConfigPageManagerView::resetLanguage(bool isFirstInit)
{
    LanguageDialogResource* pLang = m_pController->getLangResource();

    // set labels
    SetDlgItemText(m_hPage, IDS_MNG_PRESETS, (LPCWSTR)pLang->manager_presets.c_str());
    SetDlgItemText(m_hPage, IDC_MNG_BTN_PRESETS, (LPCWSTR)pLang->manager_btnPresetsApply.c_str());
    // set profile presets
    HWND hPresetList = GetDlgItem(m_hPage, IDC_MNG_PRESETS_LIST);
    if (hPresetList)
    {
        int selection = ComboBox_GetCurSel(hPresetList);
        if (selection < 0)
            selection = 1;
        ComboBox_ResetContent(hPresetList);
        ComboBox_AddString(hPresetList, (LPCTSTR)pLang->manager_preset_fastest.c_str());
        ComboBox_AddString(hPresetList, (LPCTSTR)pLang->manager_preset_standard.c_str());
        ComboBox_AddString(hPresetList, (LPCTSTR)pLang->manager_preset_enhanced2d.c_str());
        ComboBox_AddString(hPresetList, (LPCTSTR)pLang->manager_preset_enhanced3d.c_str());
        ComboBox_SetCurSel(hPresetList, selection);
    }

    // set table header
    if (isFirstInit == false)
    {
        LVCOLUMN lvcProfile; // profile names
        ZeroMemory(&lvcProfile, sizeof(LVCOLUMN));
        lvcProfile.mask = LVCF_WIDTH | LVCF_TEXT;
        lvcProfile.cx = (Config::countProfiles() <= MAX_ROWS_WITHOUT_SCROLL) ? LISTVIEW_NAME_WIDTH_NOSCROLL : LISTVIEW_NAME_WIDTH_SCROLL;
        lvcProfile.pszText = (LPWSTR)pLang->manager_tableColumnProfile.c_str();
        SendDlgItemMessage(m_hPage, IDC_MNG_LISTVIEW, LVM_SETCOLUMN, 2, (LPARAM)&lvcProfile);

        // set tooltips
        updateToolTip(res_tooltips[PMANAGER_TOOLTIP_BTN_ADD], IDC_MNG_BTN_ADD, m_hPage, (PTSTR)pLang->manager_btnAdd_tooltip.c_str());
        updateToolTip(res_tooltips[PMANAGER_TOOLTIP_BTN_EDIT], IDC_MNG_BTN_EDIT, m_hPage, (PTSTR)pLang->manager_btnEdit_tooltip.c_str());
        updateToolTip(res_tooltips[PMANAGER_TOOLTIP_BTN_REMOVE], IDC_MNG_BTN_REMOVE, m_hPage, (PTSTR)pLang->manager_btnRemove_tooltip.c_str());
        updateToolTip(res_tooltips[PMANAGER_TOOLTIP_BTN_IMPORT], IDC_MNG_BTN_IMPORT, m_hPage, (PTSTR)pLang->manager_btnImport_tooltip.c_str());
        updateToolTip(res_tooltips[PMANAGER_TOOLTIP_BTN_EXPORT], IDC_MNG_BTN_EXPORT, m_hPage, (PTSTR)pLang->manager_btnExport_tooltip.c_str());
    }
}
/// <summary>Copy UI settings to global configuration</summary>
void ConfigPageManagerView::updateConfig()
{

}

/// <summary>Load page content</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="phInstance">Instance handle reference</param>
/// <param name="pPageSize">Page limits (rectangle)</param>
/// <param name="isVisible">Visibility (show/hide)</param>
/// <exception cref="std::exception">Creation failure</exception>
void ConfigPageManagerView::loadPage(HWND hWindow, HINSTANCE* phInstance, RECT* pPageSize, bool isVisible)
{
    // load page
    s_pCurrentPage = this;
    m_hPage = CreateDialog(*phInstance, MAKEINTRESOURCE(IDD_MANAGER_PAGE), hWindow, (DLGPROC)eventHandler);
    if (!m_hPage)
        throw std::exception();
    SetWindowPos(m_hPage, NULL, pPageSize->left, pPageSize->top,
        pPageSize->right - pPageSize->left, pPageSize->bottom - pPageSize->top, 0);
    // set colors
    if (isVisible)
    {
        ShowWindow(m_hPage, SW_SHOW);
        EnableWindow(m_hPage, TRUE);
    }
    else
    {
        ShowWindow(m_hPage, SW_HIDE);
        EnableWindow(m_hPage, FALSE);
    }
    // set button icons
    res_iconAdd = LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_ADD), IMAGE_ICON, 24, 24, NULL);
    res_iconEdit= LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_EDIT), IMAGE_ICON, 24, 24, NULL);
    res_iconDel = LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_DEL), IMAGE_ICON, 24, 24, NULL);
    res_iconIn =  LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_IN), IMAGE_ICON, 24, 24, NULL);
    res_iconOut = LoadImage(*phInstance, MAKEINTRESOURCE(IDI_CONFIG_BUTTON_OUT), IMAGE_ICON, 24, 24, NULL);
    if (res_iconAdd)
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_ADD, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconAdd);
    else
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_ADD, WM_SETTEXT, 0, (LPARAM)L"Add");
    if (res_iconEdit)
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EDIT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconEdit);
    else
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EDIT, WM_SETTEXT, 0, (LPARAM)L"Edit");
    if (res_iconDel)
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_REMOVE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconDel);
    else
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_REMOVE, WM_SETTEXT, 0, (LPARAM)L"X");
    if (res_iconIn)
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_IMPORT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconIn);
    else
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_IMPORT, WM_SETTEXT, 0, (LPARAM)L"In");
    if (res_iconOut)
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EXPORT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)res_iconOut);
    else
        SendDlgItemMessage(m_hPage, IDC_MNG_BTN_EXPORT, WM_SETTEXT, 0, (LPARAM)L"Out");
    // set tooltips
    LanguageDialogResource* pLang = m_pController->getLangResource();
    res_tooltips[PMANAGER_TOOLTIP_BTN_ADD] = createToolTip(IDC_MNG_BTN_ADD, m_hPage, phInstance, (PTSTR)pLang->manager_btnAdd_tooltip.c_str());
    res_tooltips[PMANAGER_TOOLTIP_BTN_EDIT] = createToolTip(IDC_MNG_BTN_EDIT, m_hPage, phInstance, (PTSTR)pLang->manager_btnEdit_tooltip.c_str());
    res_tooltips[PMANAGER_TOOLTIP_BTN_REMOVE] = createToolTip(IDC_MNG_BTN_REMOVE, m_hPage, phInstance, (PTSTR)pLang->manager_btnRemove_tooltip.c_str());
    res_tooltips[PMANAGER_TOOLTIP_BTN_IMPORT] = createToolTip(IDC_MNG_BTN_IMPORT, m_hPage, phInstance, (PTSTR)pLang->manager_btnImport_tooltip.c_str());
    res_tooltips[PMANAGER_TOOLTIP_BTN_EXPORT] = createToolTip(IDC_MNG_BTN_EXPORT, m_hPage, phInstance, (PTSTR)pLang->manager_btnExport_tooltip.c_str());

    // set data table
    res_dataTable = CreateWindow(WC_LISTVIEWW, L"blabla", WS_VISIBLE | WS_CHILD | LVS_REPORT | WS_VSCROLL,
                                30, 54, LISTVIEW_WIDTH, 287, m_hPage, (HMENU)IDC_MNG_LISTVIEW, *phInstance, 0);
    if (res_dataTable)
    {
        // set columns
        ListView_SetExtendedListViewStyle(res_dataTable, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        LVCOLUMN lvcSelect; // checkboxes
        ZeroMemory(&lvcSelect, sizeof(LVCOLUMN));
        lvcSelect.mask = LVCF_WIDTH | LVCF_TEXT;
        lvcSelect.cx = 24;
        lvcSelect.pszText = L"";
        SendDlgItemMessage(m_hPage, IDC_MNG_LISTVIEW, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcSelect);
        LVCOLUMN lvcId; // ID
        ZeroMemory(&lvcId, sizeof(LVCOLUMN));
        lvcId.mask = LVCF_WIDTH | LVCF_TEXT;
        lvcId.cx = 40;
        lvcId.pszText = L"#";
        SendDlgItemMessage(m_hPage, IDC_MNG_LISTVIEW, LVM_INSERTCOLUMN, 1, (LPARAM)&lvcId);
        LVCOLUMN lvcProfile; // profile names
        ZeroMemory(&lvcProfile, sizeof(LVCOLUMN));
        lvcProfile.mask = LVCF_WIDTH | LVCF_TEXT;
        lvcProfile.cx = (Config::countProfiles() <= MAX_ROWS_WITHOUT_SCROLL) ? LISTVIEW_NAME_WIDTH_NOSCROLL : LISTVIEW_NAME_WIDTH_SCROLL;
        lvcProfile.pszText = (LPWSTR)pLang->manager_tableColumnProfile.c_str();
        SendDlgItemMessage(m_hPage, IDC_MNG_LISTVIEW, LVM_INSERTCOLUMN, 2, (LPARAM)&lvcProfile);
        // set header checkbox
        HWND hHeader = ListView_GetHeader(res_dataTable);
        DWORD dwHeaderStyle = ::GetWindowLong(hHeader, GWL_STYLE);
        dwHeaderStyle |= HDS_CHECKBOXES;
        ::SetWindowLong(hHeader, GWL_STYLE, dwHeaderStyle);
        m_headerCheckboxId = ::GetDlgCtrlID(hHeader); // store control ID
        // update header format - display checkbox
        HDITEM hdi = { 0 };
        hdi.mask = HDI_FORMAT;
        Header_GetItem(hHeader, 0, &hdi);
        hdi.fmt |= HDF_CHECKBOX | HDF_FIXEDWIDTH;
        Header_SetItem(hHeader, 0, &hdi);

        // insert rows
        LVITEM lviDef;
        ZeroMemory(&lviDef, sizeof(LVITEM));
        lviDef.mask = 0;
        lviDef.iSubItem = 0;
        std::wstring* profileNames = m_pController->getProfileNames();
        for (uint32_t p = 0; p < Config::countProfiles(); ++p)
        {
            lviDef.iItem = p; // index
            SendDlgItemMessage(m_hPage, IDC_MNG_LISTVIEW, LVM_INSERTITEM, 0, (LPARAM)&lviDef);
            std::wstring index = std::to_wstring(p);
            ListView_SetItemText(res_dataTable, p, 1, (LPWSTR)index.c_str());
            ListView_SetItemText(res_dataTable, p, 2, (LPWSTR)profileNames[p].c_str());
        }
    }
    else
        throw std::exception();

    // set language
    resetLanguage(true);
}

/// <summary>Page event handler</summary>
/// <param name="hWindow">Window handle</param>
/// <param name="msg">Event message</param>
/// <param name="wParam">Command</param>
/// <param name="lParam">Informations</param>
/// <returns>Action code</returns>
INT_PTR CALLBACK ConfigPageManagerView::eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH hBrushColor = NULL;
    switch (msg)
    {
        case WM_PAINT: // page background color
        {
            PAINTSTRUCT paint;
            HDC hDC = BeginPaint(hWindow, &paint);
            RECT rect;
            if (!hBrushColor)
                hBrushColor = CreateSolidBrush(COLOR_PAGE);
            if (hBrushColor)
            {
                GetClientRect(hWindow, &rect);
                FillRect(hDC, &rect, hBrushColor);
            }
            EndPaint(hWindow, &paint);
            if (hDC)
                ReleaseDC(hWindow, hDC);
            return (INT_PTR)TRUE; break;
        }
        case WM_CTLCOLORSTATIC: // controls text color
        {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkColor(hdcStatic, COLOR_PAGE);
            if (!hBrushColor)
                hBrushColor = CreateSolidBrush(COLOR_PAGE);
            return (LRESULT)hBrushColor; break;
        }
        case WM_DESTROY:
        {
            if (hBrushColor)
                DeleteObject(hBrushColor);
            hBrushColor = NULL;
            return (INT_PTR)TRUE; break;
        }

        // selection changed
        case WM_NOTIFY:
            if (lParam)
            {
                NMHDR* header = (NMHDR*)lParam;
                NMLISTVIEW* nmlist = (NMLISTVIEW*)lParam;

                static bool busy = false; // prevent recursive calls
                if (!busy && header->hwndFrom == s_pCurrentPage->res_dataTable && header->code == LVN_ITEMCHANGED)
                {
                    busy = true;
                    if (nmlist->uNewState & LVIS_SELECTED) // row selected
                    {
                        ListView_SetCheckState(s_pCurrentPage->res_dataTable, nmlist->iItem, 1);
                        bool isTotalCheck = true;
                        int rowCount = ListView_GetItemCount(s_pCurrentPage->res_dataTable);
                        for (int nItem = 0; nItem < rowCount; nItem++)
                        {
                            if (!ListView_GetCheckState(s_pCurrentPage->res_dataTable, nItem))
                            {
                                isTotalCheck = false;
                                break;
                            }
                        }
                        if (isTotalCheck)
                            setTableHeaderChecked(true);
                    }
                    else if (nmlist->uNewState & LVIS_STATEIMAGEMASK) // checkbox changed
                    {
                        if (ListView_GetCheckState(s_pCurrentPage->res_dataTable, nmlist->iItem))
                        {
                            bool isTotalCheck = true;
                            int rowCount = ListView_GetItemCount(s_pCurrentPage->res_dataTable);
                            for (int nItem = 0; nItem < rowCount; nItem++)
                            {
                                if (!ListView_GetCheckState(s_pCurrentPage->res_dataTable, nItem))
                                {
                                    isTotalCheck = false;
                                    break;
                                }
                            }
                            if (isTotalCheck)
                                setTableHeaderChecked(true);
                            ListView_SetItemState(s_pCurrentPage->res_dataTable, nmlist->iItem, LVIS_SELECTED, LVIS_SELECTED);
                        }
                        else
                        {
                            setTableHeaderChecked(false);
                            ListView_SetItemState(s_pCurrentPage->res_dataTable, nmlist->iItem, 0, LVIS_SELECTED);
                        }
                    }
                    else if ((nmlist->uNewState & (LVIS_FOCUSED | LVIS_CUT | LVIS_DROPHILITED | LVIS_OVERLAYMASK)) == 0)
                    {
                        setTableHeaderChecked(false);
                        ListView_SetCheckState(s_pCurrentPage->res_dataTable, nmlist->iItem, 0);
                        ListView_SetItemState(s_pCurrentPage->res_dataTable, nmlist->iItem, 0, LVIS_SELECTED);
                    }
                    busy = false;
                }
            }
            break;

        // controls interaction
        case WM_COMMAND:
        {
            // buttons
            if (HIWORD(wParam) != CBN_SELCHANGE)
            {
                switch (LOWORD(wParam))
                {
                    case IDC_MNG_BTN_PRESETS: break;// return onPreset(hWindow); break;
                    case IDC_MNG_BTN_ADD:     break;// if(onProfileAdd(hWindow)) setTableHeaderChecked(false); if (Config::countProfiles() > MAX_ROWS_WITHOUT_SCROLL) SendDlgItemMessage(m_hPage, IDC_MNG_LISTVIEW, LVM_SETCOLUMNWIDTH, 2, LISTVIEW_NAME_WIDTH_SCROLL); return TRUE;
                    case IDC_MNG_BTN_EDIT:    break;// return onProfileEdit(hWindow); break;
                    case IDC_MNG_BTN_REMOVE:  break;// onProfileRemove(hWindow); if (Config::countProfiles() <= MAX_ROWS_WITHOUT_SCROLL) SendDlgItemMessage(m_hPage, IDC_MNG_LISTVIEW, LVM_SETCOLUMNWIDTH, 2, LISTVIEW_NAME_WIDTH_NOSCROLL); return TRUE;
                    case IDC_MNG_BTN_IMPORT:  break;// return onProfileImport(hWindow); break;
                    case IDC_MNG_BTN_EXPORT:  break;// return onProfileExport(hWindow); break;
                }
            }
            break;
        } // WM_COMMAND

        default: return DefWindowProc(hWindow, msg, wParam, lParam); break;
    }
    return (INT_PTR)FALSE;
}

/// <summary>Update table header checkbox</summary>
/// <param name="isChecked">Checkbox status</param>
void ConfigPageManagerView::setTableHeaderChecked(bool isChecked)
{
    HWND header = ListView_GetHeader(s_pCurrentPage->res_dataTable);
    HDITEM hdi = { 0 };
    hdi.mask = HDI_FORMAT;
    Header_GetItem(header, 0, &hdi);
    if (isChecked)
        hdi.fmt |= HDF_CHECKED;
    else
        hdi.fmt &= ~HDF_CHECKED;
    Header_SetItem(header, 0, &hdi);
}

#endif
