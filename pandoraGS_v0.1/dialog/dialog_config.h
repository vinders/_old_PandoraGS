/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   dialog_config.h
    Description : configuration management dialog - main
*******************************************************************************/
#ifndef _DIALOG_CONFIG_H
#define _DIALOG_CONFIG_H

#include "../lang/lang.h"
#include "../config/config_manager.h"

#define PAGES_NUMBER 3


/* CONFIG DIALOG ------------------------------------------------------------ */
#if _SYSTEM == _WINDOWS && _DIALOGAPI == _WINAPI                    // WIN API    
    class DialogConfig
    {       
      private:
        static LangConfig* _langContent;
        static int _activeProfile;
        static short _profilesChange[PROFILE_MAX_NUMBER];  // 0=ignore / -1=removed / 1=updated/new
        
        // pages tab system
        static int   _initialize;
        static int   _activePage;
        static int   _tabZoneHeight;
        static HWND  _hBtnGeneral;
        static HWND  _hBtnManager;
        static HWND  _hBtnProfile;
        static HFONT _hFontIcons;
        static HBITMAP _hBmpIcons;
        static HBITMAP _hBmpIconsHover;
        // dialog pages
        static HWND _pageGeneral;
        static HWND _pageManager;
        static HWND _pageProfile;
        
        // external dialogs
        static HWND _dialKeyBinding;
        static HWND _dialAdvanced;
        static HWND _dialAddProfile;
        static HWND _dialImportProfile;
        static HWND _dialExportProfile;
        static HWND _dialAssociations;
        static HWND _dialBugFixes;
        static HWND _dialShaders;

      public:
        DialogConfig();
        ~DialogConfig();
        static long saveConfig();
        static void saveProfile();
        static void showWarning(const char*, const char*);
        
        // main dialog management
        long showDialog(); // 0=OK/cancel / -1=error
        static void loadConfig(bool);
        static BOOL CALLBACK eventHandler(HWND, UINT, WPARAM, LPARAM);
        static BOOL CALLBACK pageHandler(HWND, UINT, WPARAM, LPARAM);
        
        // event handlers - main dialog
        static BOOL onLanguageChange(HWND);
        static void onPageChange(HWND, int);
        // event handlers - page dialogs
        static BOOL onRendererChange(HWND);
        static BOOL onKeyBinding(HWND);
        static BOOL onAdvancedSettings(HWND);
        static BOOL onBugFixes(HWND);
        static BOOL onAdvancedShader(HWND);
        static BOOL onProfileAdd(HWND);
        static BOOL onProfileRemove(HWND);
        static BOOL onProfileImport(HWND);
        static BOOL onProfileExport(HWND);
        static BOOL onAssociations(HWND);
        
        // main dialog draw functions
        static void setStandardElements(HWND);
        static void setLanguageElements(HWND, bool);
        static HBRUSH getSpriteBrush(HDC*, bool, int, int, int, int);
        // event handlers - draw functions
        static BOOL onInitDialog(HWND);
        static void onCloseDialog(HWND);
        static BOOL onPaint(HWND);
        static BOOL onDrawItem(HWND, LPARAM);
        static void onDrawInactivePageTab(HWND, HDC*, int, RECT*);
        static void onDrawPageTabTitle(HWND, HDC*, RECT*, int);
        static BOOL onPagePaint(HWND);
    };

#else                                                               // QT API
    class DialogConfig
    {
      private:
        LangConfig* _langContent;
        
        // dialog pages
        //...

      public:
        DialogConfig();
        ~DialogConfig();
        
        // dialog management
        long saveConfig();
        long showDialog(); // 1=OK / 0=cancel / -1=error
        void showWarning(const char*, const char*);
        
        //...
    };
    
#endif
#endif
