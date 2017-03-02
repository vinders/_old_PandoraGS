/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : data table control
*******************************************************************************/
#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <stdexcept>
#if _DIALOGAPI == DIALOGAPI_WIN32
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#endif
#include "../../../res/resource.h"
#include "common.h"
#include "data_table.h"
using namespace config::dialog;
using namespace config::dialog::controls;

#define SCROLLBAR_WIDTH  17
#define ROW_HEIGHT       22


/// @brief Create data table
/// @param[in] instance       Current instance handle
/// @param[in] hWindow        Parent window handle
/// @param[in] resourceId     Table resource identifier
/// @param[in] hasCheckboxes  Add checkboxes to rows
/// @param[in] x              Horizontal position
/// @param[in] y              Vertical position
/// @param[in] width          Table width
/// @param[in] height         Table height
/// @throws runtime_error     Creation failure
DataTable::DataTable(library_instance_t instance, window_handle_t hWindow, const int32_t resourceId, const bool hasCheckboxes,
                     const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    m_hParent = hWindow;
    m_rowCount = 0u;
    m_totalWidth = width - 1;
    m_maxVisibleRows = (height / ROW_HEIGHT) - 1;

    // create table
    m_resourceId = resourceId;
    m_hDataTable = reinterpret_cast<window_handle_t>(CreateWindow(WC_LISTVIEWW, NULL, WS_VISIBLE | WS_CHILD | LVS_REPORT | WS_VSCROLL,
                                                     x, y, width, height, reinterpret_cast<HWND>(hWindow), (HMENU)resourceId, 
                                                     reinterpret_cast<HINSTANCE>(instance), 0));
    if (!m_hDataTable || reinterpret_cast<HWND>(m_hDataTable) == reinterpret_cast<HWND>(INVALID_HANDLE_VALUE))
        throw std::runtime_error("Could not create data table");

    m_headerCheckboxId = 0;
    m_hasCheckboxes = hasCheckboxes;
    if (hasCheckboxes)
    {
        // add column with checkboxes
        m_columnNames.push_back(nullptr);
        m_columnSizes.push_back(24);
        ListView_SetExtendedListViewStyle(m_hDataTable, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        LVCOLUMN lvcSelect;
        ZeroMemory(&lvcSelect, sizeof(LVCOLUMN));
        lvcSelect.mask = LVCF_WIDTH | LVCF_TEXT;
        lvcSelect.cx = 24;
        lvcSelect.pszText = L"";
        SendDlgItemMessage(m_hParent, IDC_MNG_LISTVIEW, LVM_INSERTCOLUMN, 0, (LPARAM)&lvcSelect);

        // set header main checkbox
        HWND hHeader = ListView_GetHeader(m_hDataTable);
        DWORD dwHeaderStyle = ::GetWindowLong(hHeader, GWL_STYLE);
        dwHeaderStyle |= HDS_CHECKBOXES;
        ::SetWindowLong(hHeader, GWL_STYLE, dwHeaderStyle);
        m_headerCheckboxId = GetDlgCtrlID(hHeader);
        // update header format
        HDITEM hdi = { 0 };
        hdi.mask = HDI_FORMAT;
        Header_GetItem(hHeader, 0, &hdi);
        hdi.fmt |= HDF_CHECKBOX | HDF_FIXEDWIDTH;
        Header_SetItem(hHeader, 0, &hdi);
    }
    else
    {
        ListView_SetExtendedListViewStyle(m_hDataTable, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    }
}
#else
{
    //...
}
#endif



/// @brief Add new column to table
/// @param[in] title  Name of header
/// @param[in] width  Column width (0 for auto-sized. Only one auto-sized allowed)
void DataTable::addColumn(std::wstring& title, const uint32_t width)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    // create columns and headers
    LVCOLUMN lvc;
    ZeroMemory(&lvc, sizeof(LVCOLUMN));
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    lvc.pszText = (LPWSTR)title.c_str();
    if (width > 0)
    {
        lvc.cx = width;
        m_columnSizes.push_back(width);
        m_columnNames.push_back(&title);
    }
    else // auto-sized
    {
        uint32_t remainingWidth = m_totalWidth;
        for (uint32_t i = 0; i < m_columnSizes.size(); ++i)
        {
            if (m_columnSizes.at(i) == 0) // if already an auto-sized column, set it to half remaining space
            {
                // resize previous column
                m_columnSizes.at(i) = (remainingWidth >> 1);
                updateColumn(i);
            }
            remainingWidth -= m_columnSizes.at(i);
        }
        // set auto-size
        lvc.cx = remainingWidth;
        m_columnSizes.push_back(0);
        m_columnNames.push_back(&title);
    }
    SendDlgItemMessage(m_hParent, m_resourceId, LVM_INSERTCOLUMN, m_columnSizes.size(), (LPARAM)&lvc);
}
#else
{
    //...
}
#endif


/// @brief Refresh column size and title
/// @param[in] index  Index of column to refresh
void DataTable::updateColumn(const uint32_t index)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    if (index < m_columnSizes.size())
    {
        // set column info
        LVCOLUMN lvcUpdate;
        ZeroMemory(&lvcUpdate, sizeof(LVCOLUMN));
        lvcUpdate.mask = LVCF_WIDTH | LVCF_TEXT;
        if (m_columnNames.at(index) != nullptr)
            lvcUpdate.pszText = (LPWSTR)m_columnNames.at(index)->c_str();
        else
            lvcUpdate.pszText = L"";

        // set column size
        if (m_columnSizes.at(index) == 0) // auto-sized
        {
            uint32_t remainingWidth = m_totalWidth;
            for (uint32_t i = 0; i < index; ++i) 
                remainingWidth -= m_columnSizes.at(i); // only one auto-sized column, so no need to re-check
            if (m_rowCount > m_maxVisibleRows) // remove width for scrollbar, if too many rows
                remainingWidth -= SCROLLBAR_WIDTH;
            lvcUpdate.cx = remainingWidth;
        }
        else // fixed
            lvcUpdate.cx = m_columnSizes.at(index);
        SendDlgItemMessage(m_hParent, m_resourceId, LVM_SETCOLUMN, index, (LPARAM)&lvcUpdate);
    }
}
#else
{
    //...
}
#endif


/// @brief Handle data table events
/// @param[in] eventArgs  Event metadata
void DataTable::notifyEvent(uint32_t eventArgs)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    NMHDR* eventHeader = (NMHDR*)eventArgs;
    NMLISTVIEW* eventList = (NMLISTVIEW*)eventHeader;

    static bool busy = false; // prevent recursive calls
    if (!busy && eventHeader->hwndFrom == m_hDataTable && eventHeader->code == LVN_ITEMCHANGED) // if this data table is updated
    {
        busy = true; // lock

        // whole row selected
        if (eventList->uNewState & LVIS_SELECTED) 
        {
            ListView_SetCheckState(m_hDataTable, eventList->iItem, 1); // update checkbox

            // verify if header checkbox should be checked
            bool isTotalCheck = true;
            for (uint32_t nItem = 0; nItem < m_rowCount; ++nItem) //ListView_GetItemCount(m_hDataTable);
            {
                if (!ListView_GetCheckState(m_hDataTable, nItem))
                {
                    isTotalCheck = false;
                    break;
                }
            }
            if (isTotalCheck)
                setHeaderCheckboxState(true);
        }

        // row checkbox changed
        else if (eventList->uNewState & LVIS_STATEIMAGEMASK) 
        {
            if (ListView_GetCheckState(m_hDataTable, eventList->iItem)) // checked
            {
                // verify if header checkbox should be checked
                bool isTotalCheck = true;
                for (uint32_t nItem = 0; nItem < m_rowCount; nItem++)
                {
                    if (!ListView_GetCheckState(m_hDataTable, nItem))
                    {
                        isTotalCheck = false;
                        break;
                    }
                }
                if (isTotalCheck)
                    setHeaderCheckboxState(true);

                ListView_SetItemState(m_hDataTable, eventList->iItem, LVIS_SELECTED, LVIS_SELECTED); // select whole row
            }
            else // unchecked
            {
                setHeaderCheckboxState(false); // at least one row not selected -> unchecked header checkbox
                ListView_SetItemState(m_hDataTable, eventList->iItem, 0, LVIS_SELECTED); // deselect whole row
            }
        }

        // row deselection
        else if ((eventList->uNewState & (LVIS_FOCUSED | LVIS_CUT | LVIS_DROPHILITED | LVIS_OVERLAYMASK)) == 0)
        {
            setHeaderCheckboxState(false);
            ListView_SetCheckState(m_hDataTable, eventList->iItem, 0);
            ListView_SetItemState(m_hDataTable, eventList->iItem, 0, LVIS_SELECTED);
        }

        busy = false; // unlock
    }
}
#else
{
    //...
}
#endif



/// @brief Insert multiple rows at once
/// @param[in] data  Array of rows (one row = vector of column values)
void DataTable::insertRows(std::vector<std::vector<std::wstring>>& data)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    if (data.empty())
        return;
    uint32_t firstColumn = (m_hasCheckboxes) ? 1u : 0u;

    // insert rows
    LVITEM lviDef;
    ZeroMemory(&lviDef, sizeof(LVITEM));
    lviDef.mask = 0;
    lviDef.iSubItem = 0;
    for (uint32_t p = 0; p < data.size(); ++p)
    {
        lviDef.iItem = p; // index in table
        SendDlgItemMessage(m_hParent, m_resourceId, LVM_INSERTITEM, 0, (LPARAM)&lviDef);

        uint32_t col = firstColumn;
        std::vector<std::wstring>& rowData = data.at(p);
        for (auto it = rowData.begin(); it != rowData.end(); ++it)
        {
            ListView_SetItemText(m_hDataTable, p, col, (LPWSTR)it->c_str());
            ++col;
        }
    }

    // increase count (update display if scrollbar added)
    uint32_t previousCount = m_rowCount;
    m_rowCount += data.size();
    if (m_rowCount > m_maxVisibleRows && previousCount <= m_maxVisibleRows)
    {
        for (uint32_t i = 0; i < m_columnSizes.size(); ++i)
        {
            if (m_columnSizes.at(i) == 0u)
            {
                updateColumn(i);
                break;
            }
        }
    }
}
#else
{
    //...
}
#endif


/// @brief Insert one row
/// @param[in] row  Vector of column values
/// @param[in] pos  Position in table (-1 to append)
void DataTable::insertRow(const std::vector<std::wstring>& row, const int32_t pos)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    // insert row
    int32_t newIndex = (pos >= 0) ? pos : m_rowCount;
    LVITEM lviDef;
    ZeroMemory(&lviDef, sizeof(LVITEM));
    lviDef.mask = 0;
    lviDef.iSubItem = 0;
    lviDef.iItem = newIndex; // index in table
    SendDlgItemMessage(m_hParent, m_resourceId, LVM_INSERTITEM, 0, (LPARAM)&lviDef);

    uint32_t col = (m_hasCheckboxes) ? 1u : 0u;
    for (auto it = row.begin(); it != row.end(); ++it)
    {
        ListView_SetItemText(m_hDataTable, newIndex, col, (LPWSTR)it->c_str());
        ++col;
    }

    // increase count (update display if scrollbar added)
    ++m_rowCount;
    if (m_rowCount == 1 + m_maxVisibleRows)
    {
        for (uint32_t i = 0; i < m_columnSizes.size(); ++i)
        {
            if (m_columnSizes.at(i) == 0u)
            {
                updateColumn(i);
                break;
            }
        }
    }
}
#else
{
    //...
}
#endif



/// @brief Update a row
/// @param[in] rowIndex  Row index
/// @param[in] row       Vector of column values
void DataTable::updateRow(const uint32_t rowIndex, const std::vector<std::wstring>& row)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    if (rowIndex >= m_rowCount)
        return;

    // update row
    LVITEM lviDef;
    ZeroMemory(&lviDef, sizeof(LVITEM));
    lviDef.mask = 0;
    lviDef.iSubItem = 0;
    lviDef.iItem = rowIndex; // index in table
    SendDlgItemMessage(m_hParent, m_resourceId, LVM_SETITEM, 0, (LPARAM)&lviDef);

    uint32_t col = (m_hasCheckboxes) ? 1u : 0u;
    for (auto it = row.begin(); it != row.end() && col < m_columnSizes.size(); ++it)
    {
        ListView_SetItemText(m_hDataTable, rowIndex, col, (LPWSTR)it->c_str());
        ++col;
    }
}
#else
{
    //...
}
#endif


/// @brief Update a row
/// @param[in] rowIndex     Row index
/// @param[in] columnIndex  Column index
/// @param[in] value        New value for field
void DataTable::updateRow(const uint32_t rowIndex, const uint32_t columnIndex, const std::wstring& value)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    if (rowIndex >= m_rowCount || columnIndex >= m_columnSizes.size())
        return;

    // update row
    LVITEM lviDef;
    ZeroMemory(&lviDef, sizeof(LVITEM));
    lviDef.mask = 0;
    lviDef.iSubItem = 0;
    lviDef.iItem = rowIndex; // index in table
    SendDlgItemMessage(m_hParent, m_resourceId, LVM_SETITEM, 0, (LPARAM)&lviDef);
    ListView_SetItemText(m_hDataTable, rowIndex, columnIndex, (LPWSTR)value.c_str());
}
#else
{
    //...
}
#endif


/// @brief Remove a row
/// @param[in] rowIndex  Row index
void DataTable::removeRow(const uint32_t rowIndex)
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    if (rowIndex >= m_rowCount)
        return;

    // delete row
    SendDlgItemMessage(m_hParent, m_resourceId, LVM_DELETEITEM, rowIndex, 0);

    // decrease count (update display if scrollbar removed)
    --m_rowCount;
    if (m_rowCount == m_maxVisibleRows)
    {
        for (uint32_t i = 0; i < m_columnSizes.size(); ++i)
        {
            if (m_columnSizes.at(i) == 0u)
            {
                updateColumn(i);
                break;
            }
        }
    }
}
#else
{
    //...
}
#endif



/// @brief Get indexes of all selected rows
/// @param[out] outIndexes  Vector of indexes to fill
void DataTable::getSelectedIndexes(std::vector<uint32_t>& outIndexes) noexcept
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    outIndexes.clear();

    int32_t pos = ListView_GetNextItem(m_hDataTable, -1, LVNI_SELECTED);
    while (pos != -1)
    {
        outIndexes.push_back(static_cast<uint32_t>(pos));
        pos = ListView_GetNextItem(m_hDataTable, pos, LVNI_SELECTED);
    }
}
#else
{
    //...
}
#endif


/// @brief Get index of first selected row
/// @returns First row selected (or -1 if no selection)
int32_t DataTable::getFirstSelectedIndex() noexcept
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    return ListView_GetNextItem(m_hDataTable, -1, LVNI_SELECTED);
}
#else
{
    //...
    return 0;
}
#endif



/// @brief Check/uncheck main header checkbox
/// @param[in] isChecked  Checked or not
void DataTable::setHeaderCheckboxState(const bool isChecked) noexcept
#if _DIALOGAPI == DIALOGAPI_WIN32
{
    HWND header = ListView_GetHeader(m_hDataTable);
    HDITEM hdi = { 0 };
    hdi.mask = HDI_FORMAT;
    Header_GetItem(header, 0, &hdi);

    // set state
    if (isChecked)
        hdi.fmt |= HDF_CHECKED;
    else
        hdi.fmt &= ~HDF_CHECKED;
    Header_SetItem(header, 0, &hdi);
}
#else
{
    //...
}
#endif
