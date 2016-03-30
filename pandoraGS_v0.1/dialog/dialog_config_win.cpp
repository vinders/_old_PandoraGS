/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   dialog_config_win.cpp
    Description : configuration management WINAPI dialog - main + control events
*******************************************************************************/
#define _INCLUDE_DIALOGAPI
#include "../globals.h"
#if _SYSTEM == _WINDOWS && _DIALOGAPI == _WINAPI
using namespace std;
#include <stdio.h>
#include <string.h>
#include <windowsx.h>
//#include <commctrl.h> // add comctl32.lib to project linking
#include "../pandoraGS.h"
#include "../display/renderer.h"
#include "dialog_config.h"

// char to tchar conversion macro
#define LOADBUFFER 32
#define convertStr(TBUFF,BUFF,ITER) for (ITER = 0; BUFF[ITER] != '\0'; ITER++) \
                                        TBUFF[ITER] = BUFF[ITER]; \
                                    TBUFF[ITER] = '\0';

// main static variables
LangConfig* DialogConfig::_langContent  = NULL;
short DialogConfig::_profilesChange[PROFILE_MAX_NUMBER];
int DialogConfig::_activeProfile = 0;

// pages tab system
int   DialogConfig::_activePage  = 0;
int   DialogConfig::_initialize  = 0;
int   DialogConfig::_tabZoneHeight = 1;
HWND  DialogConfig::_hBtnGeneral = NULL;
HWND  DialogConfig::_hBtnManager = NULL;
HWND  DialogConfig::_hBtnProfile = NULL;
HFONT DialogConfig::_hFontIcons  = NULL;
HBITMAP DialogConfig::_hBmpIcons = NULL;
HBITMAP DialogConfig::_hBmpIconsHover  = NULL;
// dialog pages
HWND DialogConfig::_pageGeneral  = NULL;
HWND DialogConfig::_pageManager  = NULL;
HWND DialogConfig::_pageProfile  = NULL;
        
// external dialogs
HWND DialogConfig::_dialKeyBinding = NULL;
HWND DialogConfig::_dialAdvanced   = NULL;
HWND DialogConfig::_dialBugFixes   = NULL;
HWND DialogConfig::_dialShaders    = NULL;
HWND DialogConfig::_dialAddProfile = NULL;
HWND DialogConfig::_dialImportProfile = NULL;
HWND DialogConfig::_dialExportProfile = NULL;
HWND DialogConfig::_dialAssociations  = NULL;

// draw functions
#include "dialog_configdraw_win.hpp"

/* dialog constructor ------------------------------------------------------- */
DialogConfig::DialogConfig()
{
    for(int i = 0; i < PROFILE_MAX_NUMBER; i++)
    {
        _profilesChange[i] = 0;
    }
}

/* dialog destructor -------------------------------------------------------- */
DialogConfig::~DialogConfig()
{
    // remove config and language content
    ConfigManager::getInstance()->unsetProfile(true);
    ConfigManager::unsetConfig();
    if (_langContent)
        delete _langContent;
    _langContent = NULL;
}

/* save current config ------------------------------------------------------ */
long DialogConfig::saveConfig()
{
    long result = 0;
    Config* globalConfig = ConfigManager::getInstance();
    
    // save global config values
    //!copier la valeur de chaque item dans la config
    //...
    
    // save current unsaved profile (others are saved when deselected)
    saveProfile(); 
    
    result = globalConfig->saveConfigValues(_profilesChange);
    return result;
}

/* save current profile values for profile change --------------------------- */
void DialogConfig::saveProfile()
{
    long result = 0;
    ConfigProfile* currentProfile = ConfigManager::getInstance()->getProfile(_activeProfile);
    //!copier la valeur de chaque item dans le profil
    //...
}


/* display warning message box ---------------------------------------------- */
void DialogConfig::showWarning(const char* origin, const char* message)
{
    int bufflength = 10 + strlen(origin) + strlen(message);
    char* buffer = new char[bufflength + 1];
    if (buffer == NULL)
        return;
    
    // create final message
    buffer[bufflength] = '\0';
    if (_snprintf(buffer, bufflength, "Error(%s) : %s", origin, message) < 0)
    {
        delete [] buffer;
        bufflength = 27;
        buffer = new char[28];
        if (buffer == NULL)
            return;
        
        // if failure, create generic error message
        buffer[27] = '\0';
        strcpy(buffer, "Error : dialog system error");
    }
    
    // display message box
    wchar_t* wBuffer = new wchar_t[bufflength + 1];
    if (wBuffer)
    {
        MultiByteToWideChar(CP_ACP, 0, buffer, -1, wBuffer, bufflength + 1);
        MessageBox(NULL,wBuffer,(LPCWSTR)L"Configuration dialog",MB_ICONWARNING|MB_OK);
        delete [] wBuffer;
    }
    delete [] buffer;
}



// MAIN DIALOG MANAGEMENT
//______________________________________________________________________________

/* display and manage dialog box -------------------------------------------- */
long DialogConfig::showDialog()
{
    // get config
    ConfigManager::setConfig();
    if (ConfigManager::getInstance() == NULL)
        return -1;
    // get config profile
    ConfigManager::getInstance()->setProfile(DEFAULT_CONFIG, true);
    if (ConfigManager::getInstance()->getProfile() == NULL)
        return -1;
    // get language content
    _langContent = LangConfig::getInstance(ConfigManager::getInstance()->getLangCode());
    if (_langContent == NULL)
        return -1;
    
    HWND hWindow = GetActiveWindow();
    #ifdef INSTANCE_SINGLETON
        if (hWindow == NULL || hWindow == INVALID_HANDLE_VALUE)
            return -1;
        HINSTANCE hInstance = PandoraGS::getInstance();
        if (hInstance == NULL || hInstance == INVALID_HANDLE_VALUE)
            hInstance = GetModuleHandle(NULL);
      #else
        HINSTANCE hInstance = GetModuleHandle(NULL);   // for executable tester
    #endif
    if (hInstance == NULL || hInstance == INVALID_HANDLE_VALUE)
        return -1;
    
    // visual style init (Activation Context API)
    HANDLE hActCtx;
    ACTCTX actCtx;
    ZeroMemory(&actCtx, sizeof(actCtx));
    actCtx.cbSize = sizeof(actCtx);
    actCtx.hModule = hInstance;
    actCtx.lpResourceName = MAKEINTRESOURCE(IDS_ACTIVATION_CONTEXT);
    actCtx.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;
    // visual style activation
    hActCtx = CreateActCtx(&actCtx);
    ULONG_PTR cookie;
    if (hActCtx != INVALID_HANDLE_VALUE) 
        ActivateActCtx(hActCtx, &cookie);
    //INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES };
    //InitCommonControlsEx(&icc);

    // open modal dialog box
    long code = -1;
    _activeProfile = 0;
    _initialize    = 0;
    INT_PTR nResponse = -1;
    nResponse = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CONFIG), 
                          hWindow, (DLGPROC)eventHandler);
    switch (nResponse)
    {
        case 0: 
            showWarning("GPUconfigure", "dialog box unexpected ending"); break;
        case -1: 
            showWarning("GPUconfigure", "dialog box couldn't be created"); break;
        case IDABORT:
            showWarning("GPUconfigure", "dialog box aborted"); break;

        default: code = 0; break;
    };
    
    // disable visual style
    if (hActCtx != INVALID_HANDLE_VALUE) 
    {
        DeactivateActCtx(0, cookie);
        ReleaseActCtx(hActCtx);
    }
    return code;
}


/* load config for dialog elements ------------------------------------------ */
void DialogConfig::loadConfig(bool init)
{
    HWND hWinControl = NULL;
    Config* currentConfig = ConfigManager::getInstance();
    ConfigProfile* currentProfile = currentConfig->getProfile(_activeProfile);
    if (currentProfile == NULL)
    {
        currentConfig->setProfile(_activeProfile, true);
        ConfigProfile* currentProfile = currentConfig->getProfile(_activeProfile);
        if (currentProfile == NULL)
        {
            showWarning("loadConfig", "Profile couldn't be properly loaded. Please try again.");
            return;
        }
    }
    char buffer[LOADBUFFER];
    TCHAR tbuffer[LOADBUFFER];
    int errCode = 0;
    int i = 0;
    
    // GENERAL CONFIG --------------------------------------------
    if (init)
    {
        // display mode
        if (currentConfig->isFullscreen())
            CheckDlgButton(_pageGeneral, IDC_GEN_DISPMODE1, BST_CHECKED);
        else
            CheckDlgButton(_pageGeneral, IDC_GEN_DISPMODE2, BST_CHECKED);
        
        // fullscreen resolution
        if (currentConfig->fullResX)
        {
            if ((hWinControl = GetDlgItem(_pageGeneral, IDC_GEN_DISPRES)) != NULL)
            {
                snprintf(buffer, LOADBUFFER, "%4d x %4d", currentConfig->fullResX, currentConfig->fullResY);
                convertStr(tbuffer,buffer,i);
                int index = ComboBox_FindString(hWinControl, -1, (LPCTSTR)tbuffer);
                if (index == CB_ERR)
                    index = 0;
                ComboBox_SetCurSel(hWinControl, index);
            }
            else errCode |= 0x1;
        }
        
        // window resolution
        snprintf(buffer, LOADBUFFER, "%d", currentConfig->windowResX);
        convertStr(tbuffer,buffer,i);
        SetDlgItemText(_pageGeneral, IDC_GEN_DISPWINX, (LPCTSTR)tbuffer);
        snprintf(buffer, LOADBUFFER, "%d", currentConfig->windowResY);
        convertStr(tbuffer,buffer,i);
        SetDlgItemText(_pageGeneral, IDC_GEN_DISPWINY, (LPCTSTR)tbuffer);
        
        // renderer choice
        if ((hWinControl = GetDlgItem(_pageGeneral, IDC_GEN_RENDER)) != NULL)
        {
            switch (currentConfig->getRenderer())
            {
                case GLAPICODE_SOFTD3D: 
                    ComboBox_SetCurSel(hWinControl, GLAPI_INDEX1); 
                    onRendererChange(hWinControl);
                    break;
                case GLAPICODE_OGL4: 
                /*{
                    switch(currentConfig->getRendererMinor())
                    {
                        case GLAPI_MINOR41: ComboBox_SetCurSel(hWinControl, GLAPI_INDEX41); break;
                        case GLAPI_MINOR45: ComboBox_SetCurSel(hWinControl, GLAPI_INDEX45); break;
                        default: */ComboBox_SetCurSel(hWinControl, GLAPI_INDEX41); break;
                    /*}
                    break;
                }
                case GLAPICODE_DX11: ComboBox_SetCurSel(hWinControl, GLAPI_INDEX11); break;*/
                default: ComboBox_SetCurSel(hWinControl, GLAPI_INDEX41);
            }
        }
        else errCode |= 0x2;
        
        // color depth
        if ((hWinControl = GetDlgItem(_pageGeneral, IDC_GEN_COLOR)) != NULL)
            ComboBox_SetCurSel(hWinControl, currentConfig->colorDepth);
        else errCode |= 0x4;
        
        // float accuracy + vsync + frame skip
        if (currentConfig->bFloatAccuracy)
            CheckDlgButton(_pageGeneral, IDC_GEN_GTEACC, BST_CHECKED);
        if (currentConfig->bVerticalSync)
            CheckDlgButton(_pageGeneral, IDC_GEN_VSYNC, BST_CHECKED);
        if (currentConfig->bFrameSkipping)
            CheckDlgButton(_pageGeneral, IDC_GEN_FSKIP, BST_CHECKED);
        
        // framerate limit
        if (currentConfig->bFramerateLimit)
            CheckDlgButton(_pageGeneral, IDC_GEN_FPSLIMIT, BST_CHECKED);
        if (currentConfig->bFramerateLimitAuto)
            CheckDlgButton(_pageGeneral, IDC_GEN_FPSAUTO, BST_CHECKED);
        else
            CheckDlgButton(_pageGeneral, IDC_GEN_FPSFIXED, BST_CHECKED);
        // framerate value
        snprintf(buffer, LOADBUFFER, "%.2f", currentConfig->framerateLimitFixed);
        convertStr(tbuffer,buffer,i);
        SetDlgItemText(_pageGeneral, IDC_GEN_FPSVAL, (LPCTSTR)tbuffer);
    }
    
    // PROFILE CONFIG --------------------------------------------
    
    // texture filtering
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_TXFILTER)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->textureSmoothing);
    else errCode |= 0x8;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_TXUPSCALE)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->textureUpscaling);
    else errCode |= 0x8;
    // 2D/sprite filtering
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_2DFILTER)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->spriteSmoothing);
    else errCode |= 0x8;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_2DUPSCALE)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->spriteUpscaling);
    else errCode |= 0x8;
    // screen filtering
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_SCRFILTER)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->screenSmoothing);
    else errCode |= 0x8;
    if (currentProfile->bMdecFilter)
        CheckDlgButton(_pageProfile, IDC_PRO_MDEC, BST_CHECKED);
    else
        CheckDlgButton(_pageProfile, IDC_PRO_MDEC, BST_UNCHECKED);
    
    // anti-aliasing
    if (currentProfile->antiAliasing != -1)
    {
        CheckDlgButton(_pageProfile, IDC_PRO_FXAA, BST_CHECKED);
        if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_AAFILTER)) != NULL)
            ComboBox_SetCurSel(hWinControl, currentProfile->antiAliasing);
        else errCode |= 0x10;
    }
    else
    {
        CheckDlgButton(_pageProfile, IDC_PRO_FXAA, BST_UNCHECKED);
    }
    // bump mapping
    if (currentProfile->bBumpMapping)
        CheckDlgButton(_pageProfile, IDC_PRO_FXBUMP, BST_CHECKED);
    else
        CheckDlgButton(_pageProfile, IDC_PRO_FXBUMP, BST_UNCHECKED);
    // high dynamic range
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_HDRFILTER)) != NULL)
    {
        if (currentProfile->bHdrLumaSharpen)
        {
            if (currentProfile->bHdrBloom)
                ComboBox_SetCurSel(hWinControl, 2);
            else
                ComboBox_SetCurSel(hWinControl, 0);
            CheckDlgButton(_pageProfile, IDC_PRO_FXHDR, BST_CHECKED);
        }
        else if (currentProfile->bHdrBloom)
        {
            ComboBox_SetCurSel(hWinControl, 1);
            CheckDlgButton(_pageProfile, IDC_PRO_FXHDR, BST_CHECKED);
        }
        else
        {
            ComboBox_SetCurSel(hWinControl, 1);
            CheckDlgButton(_pageProfile, IDC_PRO_FXHDR, BST_UNCHECKED);
        }
    }
    else errCode |= 0x20;
    // motion blur
    if (currentProfile->bMotionBlur)
        CheckDlgButton(_pageProfile, IDC_PRO_FXMOTION, BST_CHECKED);
    else
        CheckDlgButton(_pageProfile, IDC_PRO_FXMOTION, BST_UNCHECKED);
    
    // special rendering
    if (currentProfile->specialFilter != -1)
    {
        CheckDlgButton(_pageProfile, IDC_PRO_FXSPECIAL, BST_CHECKED);
        if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_SPECIALFILTER)) != NULL)
        {
            if (currentProfile->specialFilter == CFG_SPEC_CUSTOM)
                ComboBox_SetCurSel(hWinControl, CFG_SPECNUMBER);
            else
                ComboBox_SetCurSel(hWinControl, currentProfile->specialFilter);
        }
        else errCode |= 0x40;
    }
    else
    {
        CheckDlgButton(_pageProfile, IDC_PRO_FXSPECIAL, BST_UNCHECKED);
    }
    // special rendering level
    snprintf(buffer, LOADBUFFER, "%u", currentProfile->specialLevel);
    convertStr(tbuffer,buffer,i);
    SetDlgItemText(_pageProfile, IDC_PRO_SPECIALLEVEL, (LPCTSTR)tbuffer);
    
    // internal resolution
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_INTRESX)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->internalResX);
    else errCode |= 0x80;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_INTRESY)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->internalResY);
    else errCode |= 0x80;
    
    // screen stretching
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_STRETCH)) != NULL)
        ComboBox_SetCurSel(hWinControl, currentProfile->screenStretching);
    else errCode |= 0x100;
    // mirror screen
    if (currentProfile->bMirrorScreen)
        CheckDlgButton(_pageProfile, IDC_PRO_MIRROR, BST_CHECKED);
    else
        CheckDlgButton(_pageProfile, IDC_PRO_MIRROR, BST_UNCHECKED);
    // extra rendering
    if (currentProfile->bExtraRendering)
        CheckDlgButton(_pageProfile, IDC_PRO_SCREXTRA, BST_CHECKED);
    else
        CheckDlgButton(_pageProfile, IDC_PRO_SCREXTRA, BST_UNCHECKED);
    // anti-flicker border
    if (currentProfile->blackBorder)
    {
        CheckDlgButton(_pageProfile, IDC_PRO_SCRBORDER, BST_CHECKED);
        snprintf(buffer, LOADBUFFER, "%d", currentProfile->blackBorder);
    }
    else
    {
        CheckDlgButton(_pageProfile, IDC_PRO_SCRBORDER, BST_UNCHECKED);
        snprintf(buffer, LOADBUFFER, "10");
    }
    convertStr(tbuffer,buffer,i);
    SetDlgItemText(_pageProfile, IDC_PRO_SCRBORDERVAL, (LPCTSTR)tbuffer);
    
    // TV scanlines
    if (currentProfile->bTvScanlines)
        CheckDlgButton(_pageProfile, IDC_PRO_TV, BST_CHECKED);
    else
        CheckDlgButton(_pageProfile, IDC_PRO_TV, BST_UNCHECKED);
    // gun cursor
    if (currentProfile->gunCursor != -1)
    {
        CheckDlgButton(_pageProfile, IDC_PRO_CURSOR, BST_CHECKED);
        if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_CURSORTYPE)) != NULL)
            ComboBox_SetCurSel(hWinControl, currentProfile->gunCursor);
        else errCode |= 0x200;
    }
    else
    {
        CheckDlgButton(_pageProfile, IDC_PRO_CURSOR, BST_UNCHECKED);
        if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_CURSORTYPE)) != NULL)
            ComboBox_SetCurSel(hWinControl, CFG_GUN_CROSSHAIR);
        else errCode |= 0x200;
    }
    
    // error notification
    if (errCode)
    {
        showWarning("GPUconfigure", "failed to load all config values");
    }
}



// EVENT HANDLERS - MAIN DIALOG
//______________________________________________________________________________

/* main dialog events handler ----------------------------------------------- */
BOOL CALLBACK DialogConfig::eventHandler(HWND hWindow, UINT uEvent, WPARAM wParam, LPARAM lParam)
{
    switch(uEvent)
    {
        // dialog settings and drawing
        case WM_INITDIALOG: return onInitDialog(hWindow);
        case WM_PAINT:      return onPaint(hWindow);
        case WM_DRAWITEM:   return onDrawItem(hWindow, lParam);

        // dialog controls
        case WM_COMMAND:
        {
            switch(LOWORD(wParam)) 
            {
                // save and close button
                case IDOK:  if (saveConfig() == -1)
                                showWarning("GPUconfigure", "couldn't save configuration");
                            else
                                onCloseDialog(hWindow);
                            return TRUE;

                // cancel and close button
                case IDCANCEL:  onCloseDialog(hWindow); return TRUE;
                
                // language combobox change
                case IDC_CBOX_LANG: if (HIWORD(wParam) == CBN_SELCHANGE)
                                        return onLanguageChange(hWindow);
                                    break;
            } 
            break;
        }
        // close x button or shortcut
        case WM_CLOSE: onCloseDialog(hWindow); return TRUE; 
    }
    return FALSE;
}


/* handler - change language ------------------------------------------------ */
BOOL DialogConfig::onLanguageChange(HWND hWindow)
{
    int langSelection = SendMessage(GetDlgItem(hWindow, IDC_CBOX_LANG), 
                                    CB_GETCURSEL, NULL, NULL);
    if (langSelection != CB_ERR)
    {
        // get new language content
        LangConfig* newLangContent = LangConfig::getInstance(langSelection);
        if (newLangContent == NULL)
            return FALSE;
        
        // set language content
        if (_langContent)
            delete _langContent;
        _langContent = newLangContent;
        setLanguageElements(hWindow, false);
    }
    else
    {
        MessageBox(NULL, L"An error occurred during selection. Please try again.", 
                         L"Language selection error", NULL);
    }
    return TRUE;
}

/* handler - page changer --------------------------------------------------- */
void DialogConfig::onPageChange(HWND hWindow, int active)
{
    // disable previous page
    switch(_activePage)
    {
        case 0: ShowWindow(_pageGeneral, SW_HIDE); EnableWindow(_pageGeneral, FALSE); break;
        case 1: ShowWindow(_pageManager, SW_HIDE); EnableWindow(_pageManager, FALSE); break;
        case 2: ShowWindow(_pageProfile, SW_HIDE); EnableWindow(_pageProfile, FALSE); break;
    }

    // set new page
    _activePage = active;
    switch(_activePage)
    {
        case 0: ShowWindow(_pageGeneral, SW_SHOW); EnableWindow(_pageGeneral, TRUE); break;
        case 1: ShowWindow(_pageManager, SW_SHOW); EnableWindow(_pageManager, TRUE); break;
        case 2: ShowWindow(_pageProfile, SW_SHOW); EnableWindow(_pageProfile, TRUE); break;
    }
}



// EVENT HANDLERS - PAGE DIALOGS
//______________________________________________________________________________

/* page dialog event handler ------------------------------------------------ */
BOOL CALLBACK DialogConfig::pageHandler(HWND hWindow, UINT uEvent, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH hBrushColor = NULL; 
    switch(uEvent)
    {
        // dialog page drawing
        case WM_PAINT:  return onPagePaint(hWindow);
        case WM_CTLCOLORSTATIC: // controls text color
        { 
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            SetBkColor(hdcStatic, RGB(COLOR_PAGE,COLOR_PAGE,COLOR_PAGE));
            if (!hBrushColor)
                hBrushColor = CreateSolidBrush(RGB(COLOR_PAGE,COLOR_PAGE,COLOR_PAGE));
            return (LRESULT)hBrushColor; //return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        case WM_DESTROY:
        {
            if (hBrushColor) 
                DeleteObject(hBrushColor);
            return TRUE;
        }
        
        // controls interaction
        case WM_COMMAND:
        {
            // combobox
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                switch(LOWORD(wParam)) 
                {
                    // renderer selection combobox
                    case IDC_GEN_RENDER: return onRendererChange((HWND)lParam);
                    
                    // internal resolution combobox
                    case IDC_PRO_INTRESX:
                    case IDC_PRO_INTRESY:
                    {
                        // get both comboboxes handles
                        HWND hWinControl = (HWND)lParam;
                        HWND hOtherControl = NULL;
                        if (LOWORD(wParam) == IDC_PRO_INTRESX)
                            hOtherControl = GetDlgItem(_pageProfile, IDC_PRO_INTRESY);
                        else
                            hOtherControl = GetDlgItem(_pageProfile, IDC_PRO_INTRESX);
                        // update other internal res combobox
                        if (hOtherControl)
                        {
                            if (ComboBox_GetCurSel(hWinControl) == CFG_INTRES_SCR)
                                ComboBox_SetCurSel(hOtherControl, CFG_INTRES_SCR);
                            else if(ComboBox_GetCurSel(hOtherControl) == CFG_INTRES_SCR)
                                ComboBox_SetCurSel(hOtherControl, CFG_INTRES_1X);
                        }
                        return TRUE;
                    }
                    
                    // change profile selection (combobox)
                    case IDC_MAN_SELPRO:
                    case IDC_PRO_SELPRO:
                    {
                        // get selected profile ID
                        int index = 0;
                        HWND hWinControl = (HWND)lParam;
                        index = ComboBox_GetCurSel(hWinControl);
                        int id = ConfigManager::getInstance()->getProfileId(index);
                        if (id != _activeProfile)
                        {
                            // save current + load profile
                            saveProfile();
                            _activeProfile = id;
                            loadConfig(false);
                            
                            // update other profile combobox
                            HWND hOtherControl = NULL;
                            if (LOWORD(wParam) == IDC_MAN_SELPRO)
                                hOtherControl = GetDlgItem(_pageProfile, IDC_PRO_SELPRO);
                            else
                                hOtherControl = GetDlgItem(_pageManager, IDC_MAN_SELPRO);
                            if (hOtherControl)
                                ComboBox_SetCurSel(hOtherControl, index);
                        }
                        return TRUE;
                    }
                } 
                break;
            }
            // button
            else
            {
                switch(LOWORD(wParam))
                {
                    // apply config preset to current profile
                    case IDC_BTN_PRESET:
                    {
                        int preset = CFG_PRESET_STD;
                        HWND hWinControl = GetDlgItem(_pageManager, IDC_MAN_PRESET);
                        if (hWinControl != NULL)
                            preset = ComboBox_GetCurSel(hWinControl);
                        ConfigManager::getInstance()->getProfile(_activeProfile)->loadPreset(preset);
                        loadConfig(false);
                        return TRUE;
                    }
                    
                    //open popup dialogs
                    case IDC_BTN_KEYBINDING: return onKeyBinding(hWindow);
                    case IDC_BTN_ADVANCED:   return onAdvancedSettings(hWindow);
                    case IDC_BTN_BUGFIXES:   return onBugFixes(hWindow);
                    case IDC_BTN_SHADERS:    return onAdvancedShader(hWindow);
                    case IDC_BTN_ADDPROFILE:    return onProfileAdd(hWindow);
                    case IDC_BTN_REMOVEPROFILE: return onProfileRemove(hWindow);
                    case IDC_BTN_IMPORTPROFILE: return onProfileImport(hWindow);
                    case IDC_BTN_EXPORTPROFILE: return onProfileExport(hWindow);
                    case IDC_BTN_ASSOCIATIONS:  return onAssociations(hWindow);
                }
                break;
            }
        } // WM_COMMAND
    } // uEvent
    return FALSE;
}


/* handler - change selected renderer --------------------------------------- */
BOOL DialogConfig::onRendererChange(HWND hWindow)
{
    // enable/disable elements display
    int cmdShow = SW_SHOW;
    if (ComboBox_GetCurSel(hWindow) == GLAPI_INDEX1)  // software mode limits
    {
        cmdShow = SW_HIDE;
    }

    // apply to elements
    HWND hWinControl = NULL;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_FXBUMP)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_FXHDR)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_HDRFILTER)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_FXMOTION)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_FXSPECIAL)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_SPECIALFILTER)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    if ((hWinControl = GetDlgItem(_pageProfile, IDS_PRO_SPECIALLEVEL)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_SPECIALLEVEL)) != NULL)
        ShowWindow(hWinControl, cmdShow);
    return TRUE;
}

/* handler - open key binding sub-dialog ------------------------------------ */
BOOL DialogConfig::onKeyBinding(HWND hWindow)
{
    //...
    return TRUE;
}
/* handler - open advanced general settings sub-dialog ---------------------- */
BOOL DialogConfig::onAdvancedSettings(HWND hWindow)
{
    //...
    return TRUE;
}
/* handler - open bug fixes sub-dialog -------------------------------------- */
BOOL DialogConfig::onBugFixes(HWND hWindow)
{
    //...
    return TRUE;
}
/* handler - open advanced shader settings sub-dialog ----------------------- */
BOOL DialogConfig::onAdvancedShader(HWND hWindow)
{
    //...
    return TRUE;
}

/* handler - add new profile ------------------------------------------------ */
BOOL DialogConfig::onProfileAdd(HWND hWindow)
{
    //...
    return TRUE;
}
/* handler - remove profile ------------------------------------------------- */
BOOL DialogConfig::onProfileRemove(HWND hWindow)
{
    //...
    return TRUE;
}

/* handler - import profile ------------------------------------------------- */
BOOL DialogConfig::onProfileImport(HWND hWindow)
{
    //...
    return TRUE;
}
/* handler - export profile ------------------------------------------------- */
BOOL DialogConfig::onProfileExport(HWND hWindow)
{
    //...
    return TRUE;
}

/* handler - open game associations sub-dialog ------------------------------ */
BOOL DialogConfig::onAssociations(HWND hWindow)
{
    //...
    return TRUE;
}

#endif
