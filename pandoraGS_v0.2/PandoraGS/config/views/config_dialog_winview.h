/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog_winview.h
Description : configuration dialog - view
*******************************************************************************/
#ifndef _CONFIG_DIALOG_WINVIEW_H
#define _CONFIG_DIALOG_WINVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32

// dialog colors
#define COLOR_PAGE         RGB(255,255,255)
#define COLOR_BORDER       RGB(210,216,220)
#define COLOR_TAB_BORDER   RGB(217,222,226)
#define COLOR_TAB_BORDER2  RGB(204,212,220)
#define COLOR_TAB_BORDER3  RGB(191,202,215)
#define COLOR_MENU_TEXT        RGB(67,82,97)
#define COLOR_MENU_TEXT_ACTIVE RGB(88,100,116)
#define COLOR_MENUTOP_BORDER_R 227
#define COLOR_MENUTOP_BORDER_G 232
#define COLOR_MENUTOP_BORDER_B 236
#define COLOR_MENUOFFSET_BORDER_R -70.0
#define COLOR_MENUOFFSET_BORDER_G -36.0
#define COLOR_MENUOFFSET_BORDER_B -21.0
#define COLOR_MENUTOP_R    235
#define COLOR_MENUTOP_G    240
#define COLOR_MENUTOP_B    245
#define COLOR_MENUOFFSET_R -79.0 // bottom 156 //152
#define COLOR_MENUOFFSET_G -51.0 // bottom 189 //192
#define COLOR_MENUOFFSET_B -25.0 // bottom 220 //229

#ifndef _CONFIG_DIALOG_H_END
class ConfigDialog;
#endif
#include "config_page_winview.h"


// Configuration dialog box - view
class ConfigDialogView
{
private:
    ConfigPageView** m_pPages; // sub-page views
    ConfigDialog* m_pController; // controller reference
    HWND m_hWindow; // main window handle
public:
    HINSTANCE m_hInstance; // DLL instance handle
    static ConfigDialogView* s_pCurrentWindow; // current instance (static access)

    // onglets
    unsigned int m_activePage;
    int m_initialize;
    HWND res_tabGeneral;
    HWND res_tabManager;
    HWND res_tabProfile;
    HBITMAP res_tabIcons;
    HFONT   res_tabFont;
    int m_menuHeight;

public:
    /// <summary>Create dialog view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigDialogView(ConfigDialog* pController);
    /// <summary>Destroy dialog view container</summary>
    ~ConfigDialogView();

    /// <summary>Create new dialog</summary>
    /// <param name="pController">Controller reference</param>
    /// <returns>Window created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static ConfigDialogView* createWindow(ConfigDialog* pController);

    /// <summary>Display window</summary>
    /// <exception cref="std::exception">No window or event exception</exception>
    void setVisible();

    /// <summary>Get associated controller (event bubbling)</summary>
    /// <returns>Controller reference</returns>
    ConfigDialog* getController()
    {
        return m_pController;
    }

private:
    // -- DIALOG TEMPLATE ------------------------------------------------------

    /// <summary>Retrieve window and instances handles</summary>
    /// <exception cref="std::exception">Definition failure</exception>
    void setDialogHandles();

    /// <summary>Window event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="msg">Event message</param>
    /// <param name="wParam">Command</param>
    /// <param name="lParam">Informations</param>
    /// <returns>Action code</returns>
    static INT_PTR CALLBACK eventHandler(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

    /// <summary>Create and display pages</summary>
    /// <param name="hSubWindow">Form handle</param>
    /// <returns>Action code</returns>
    bool loadPages(HWND hSubWindow);
    /// <summary>Copy UI settings to global configuration</summary>
    void updateConfig();

    // -- EVENTS ---------------------------------------------------------------

    /// <summary>Validation event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <returns>Success</returns>
    static bool onValidation(HWND hWindow);

    /// <summary>Language event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <returns>Changes indicator</returns>
    static INT_PTR onLanguageChange(HWND hWindow);
    /// <summary>Menu event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="tabId">Tab identifier</param>
    void ConfigDialogView::onPageChange(HWND hWindow, int tabId);

    /// <summary>Initialize memory</summary>
    void onBoot();
    /// <summary>Free allocated memory</summary>
    void onClose();

    // -- DISPLAY EVENTS -----------------------------------------------------------

    /// <summary>Drawing init event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="lParam">Parameter</param>
    /// <returns>Changes indicator</returns>
    static INT_PTR onInitDialog(HWND hWindow, LPARAM lParam);
    /// <summary>Paint event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="lParam">Parameter</param>
    /// <returns>Changes indicator</returns>
    static INT_PTR onPaint(HWND hWindow, LPARAM lParam);
    /// <summary>Drawing item event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="lParam">Parameter</param>
    /// <returns>Changes indicator</returns>
    static INT_PTR onDrawItem(HWND hWindow, LPARAM lParam);
    /// <summary>Drawing item event handler</summary>
    /// <param name="hWindow">Window handle</param>
    /// <param name="phDC">Context reference</param>
    /// <param name="pTitleRect">Target rectangle reference</param>
    /// <param name="tabId">Tab identifier</param>
    void drawMenuContent(HWND hWindow, HDC* phDC, RECT* pTitleRect, unsigned int tabId);
};

#define _CONFIG_DIALOG_VIEW_H_END
#include "config_dialog.h"

#endif
#endif
