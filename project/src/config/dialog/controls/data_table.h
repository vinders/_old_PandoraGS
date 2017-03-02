/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : data table control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#include <vector>
#include "common.h"

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
            /// @class DataTable
            /// @brief Data table control
            class DataTable
            {
            private:
                window_handle_t m_hParent;      ///< Parent window handle
                window_handle_t m_hDataTable;   ///< Data table handle
                int32_t m_resourceId;           ///< Table resource ID

                int32_t m_headerCheckboxId;     ///< Header checkbox resource ID
                bool m_hasCheckboxes;           ///< Rows have checkboxes
                uint32_t m_rowCount;            ///< Number of rows
                uint32_t m_maxVisibleRows;      ///< Max visible rows without scrolling
                uint32_t m_totalWidth;          ///< Remaining width for new columns
                std::vector<std::wstring*> m_columnNames;///< Vector with names of columns
                std::vector<uint32_t> m_columnSizes;     ///< Vector with widths of columns


            public:
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
                DataTable(library_instance_t instance, window_handle_t hWindow, const int32_t resourceId, const bool hasCheckboxes,
                          const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
                /// @brief Destroy data table
                ~DataTable()
                {
                    #if _DIALOGAPI == DIALOGAPI_WIN32
                    if (m_hDataTable != NULL)
                        DestroyWindow(reinterpret_cast<HWND>(m_hDataTable));
                    #endif
                }


                /// @brief Add new column to table
                /// @param[in] title  Name of header
                /// @param[in] width  Column width (0 for auto-sized. Only one auto-sized allowed)
                void addColumn(std::wstring& title, const uint32_t width);
                /// @brief Refresh column size and title
                /// @param[in] index  Index of column to refresh
                void updateColumn(const uint32_t index);

                /// @brief Handle data table events
                /// @param[in] eventArgs  Event metadata
                void notifyEvent(uint32_t eventArgs);


                /// @brief Insert multiple rows at once
                /// @param[in] data  Array of rows (one row = vector of column values)
                void insertRows(std::vector<std::vector<std::wstring>>& data);
                /// @brief Insert one row
                /// @param[in] row  Vector of column values
                /// @param[in] pos  Position in table (-1 to append)
                void insertRow(const std::vector<std::wstring>& row, const int32_t pos = -1);

                /// @brief Update a row
                /// @param[in] rowIndex  Row index
                /// @param[in] row       Vector of column values
                void updateRow(const uint32_t rowIndex, const std::vector<std::wstring>& row);
                /// @brief Update a row
                /// @param[in] rowIndex     Row index
                /// @param[in] columnIndex  Column index
                /// @param[in] value        New value for field
                void updateRow(const uint32_t rowIndex, const uint32_t columnIndex, const std::wstring& value);
                /// @brief Remove a row
                /// @param[in] rowIndex  Row index
                void removeRow(const uint32_t rowIndex);

                /// @brief Remove all rows
                void clearRows()
                {
                    for (int32_t i = m_rowCount - 1; i >= 0; --i)
                        removeRow(i);
                }

                /// @brief Get indexes of all selected rows
                /// @param[out] outIndexes  Vector of indexes to fill
                void getSelectedIndexes(std::vector<uint32_t>& outIndexes) noexcept;
                /// @brief Get index of first selected row
                /// @returns First row selected (or -1 if no selection)
                int32_t getFirstSelectedIndex() noexcept;


            private:
                /// @brief Check/uncheck main header checkbox
                /// @param[in] isChecked  Checked or not
                void setHeaderCheckboxState(const bool isChecked) noexcept;
            };
        }
    }
}