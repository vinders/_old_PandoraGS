/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   dialog_configdraw_win.hpp
    Description : configuration management WINAPI dialog - draw events functions
*******************************************************************************/
#define _INCLUDE_DIALOGAPI
#include "../globals.h"
#if _SYSTEM == _WINDOWS && _DIALOGAPI == _WINAPI
#ifdef _DIALOG_CONFIG_H // only compile as part of dialog_config_win.cpp

#include "../pandoraGS.h"

// MAIN DIALOG MANAGEMENT
//______________________________________________________________________________

/* set language independant text content ------------------------------------ */
void DialogConfig::setStandardElements(HWND hWindow)
{
    // get config values
    unsigned int errCode = 0;
    HWND hWinControl = NULL;
    int i; char buffer[14]; TCHAR tbuffer[14]; 

    // renderer choice combobox
    if ((hWinControl = GetDlgItem(_pageGeneral, IDC_GEN_RENDER)) != NULL)
    {
        ComboBox_AddString(hWinControl, _T(GLAPI_LABEL1));
        ComboBox_AddString(hWinControl, _T(GLAPI_LABEL41));
        //ComboBox_AddString(hWinControl, _T(GLAPI_LABEL11));
        //ComboBox_AddString(hWinControl, _T(GLAPI_LABEL45));
    }
    else 
        errCode |= 0x1;
    
    // fullscreen resolutions
    if ((hWinControl = GetDlgItem(_pageGeneral, IDC_GEN_DISPRES)) != NULL)
    {
        // get driver resolutions
        DEVMODE dv;
        memset(&dv, 0, sizeof(DEVMODE));
        dv.dmSize = sizeof(DEVMODE);
        for (int dr = 0; EnumDisplaySettings(NULL, dr, &dv); dr++)
        {
            snprintf(buffer, 14, "%4d x %4d", dv.dmPelsWidth, dv.dmPelsHeight);
            convertStr(tbuffer,buffer,i);
            if (ComboBox_FindString(hWinControl, -1, (LPCTSTR)tbuffer) == CB_ERR)
                ComboBox_AddString(hWinControl, tbuffer);
        } 
        
        // add other 4:3 / 5:4
        if (ComboBox_FindString(hWinControl, -1, _T(" 640 x  480")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T(" 640 x  480")); // VGA
        if (ComboBox_FindString(hWinControl, -1, _T(" 800 x  600")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T(" 800 x  600")); // SVGA
        if (ComboBox_FindString(hWinControl, -1, _T("1024 x  768")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1024 x  768")); // XGA
        if (ComboBox_FindString(hWinControl, -1, _T("1280 x  960")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1280 x  960")); // SXGA-
        if (ComboBox_FindString(hWinControl, -1, _T("1280 x 1024")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1280 x 1024")); // SXGA
        if (ComboBox_FindString(hWinControl, -1, _T("1600 x 1200")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1600 x 1200")); // UXGA
        // add other 16:9 / 16:10
        if (ComboBox_FindString(hWinControl, -1, _T("1280 x  720")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1280 x  720")); // WXGA-H
        if (ComboBox_FindString(hWinControl, -1, _T("1280 x  800")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1280 x  800")); // WXGA
        if (ComboBox_FindString(hWinControl, -1, _T("1366 x  768")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1366 x  768")); // HD
        if (ComboBox_FindString(hWinControl, -1, _T("1440 x  900")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1440 x  900")); // WSXGA
        if (ComboBox_FindString(hWinControl, -1, _T("1600 x  900")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1600 x  900")); // HD+
        if (ComboBox_FindString(hWinControl, -1, _T("1920 x 1080")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1920 x 1080")); // fullHD
        if (ComboBox_FindString(hWinControl, -1, _T("1920 x 1200")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("1920 x 1200")); // WUXGA
        if (ComboBox_FindString(hWinControl, -1, _T("2560 x 1600")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("2560 x 1600")); // WQXGA
        if (ComboBox_FindString(hWinControl, -1, _T("4096 x 2304")) == CB_ERR)
            ComboBox_AddString(hWinControl,      _T("4096 x 2304")); // 4K
        
        // add desktop resolution
        int desktopResX = GetSystemMetrics(SM_CXSCREEN);
        int desktopResY = GetSystemMetrics(SM_CYSCREEN);
        if (desktopResX >= 640 && desktopResY >= 480)
        {
            snprintf(buffer, 14, "%4d x %4d", desktopResX, desktopResY);
            convertStr(tbuffer,buffer,i);
            if (ComboBox_FindString(hWinControl, -1, tbuffer) == CB_ERR)
                ComboBox_AddString(hWinControl, (LPCTSTR)tbuffer);
            
            if (ConfigManager::getInstance()->fullResX == 0)
            {
                int index = ComboBox_FindString(hWinControl, -1, (LPCTSTR)tbuffer);
                if (index == CB_ERR)
                    index = 0;
                ComboBox_SetCurSel(hWinControl, index);
            }
        }
        else if (ConfigManager::getInstance()->fullResX == 0)
        {
            snprintf(buffer, 14, " 800 x  600");
            convertStr(tbuffer,buffer,i);
            int index = ComboBox_FindString(hWinControl, -1, (LPCTSTR)tbuffer);
            if (index == CB_ERR)
                index = 0;
            ComboBox_SetCurSel(hWinControl, index);
        }
    }
    else 
        errCode |= 0x1;
    
    // color depth choice combobox
    if ((hWinControl = GetDlgItem(_pageGeneral, IDC_GEN_COLOR)) != NULL)
    {
        ComboBox_AddString(hWinControl, _T("16-bit"));
        ComboBox_AddString(hWinControl, _T("32-bit"));
    }
    else
        errCode |= 0x4;
    
    // profiles management choice combobox
    Config* currentConfig = ConfigManager::getInstance();
    if ((hWinControl = GetDlgItem(_pageManager, IDC_MAN_SELPRO)) != NULL)
    {
        HWND hWinControl2 = NULL;
        if ((hWinControl2 = GetDlgItem(_pageProfile, IDC_PRO_SELPRO)) != NULL)
        {
            ComboBox_AddString(hWinControl, _T("<default>"));
            ComboBox_AddString(hWinControl2, _T("<default>"));
            if (currentConfig->profileNumber && currentConfig->profileNames)
            {
                for (int pr = 0; pr < PROFILE_MAX_NUMBER; pr++)
                {
                    if (currentConfig->profileNames[pr].profileName)
                    {
                        convertStr(tbuffer,currentConfig->profileNames[pr].profileName,i);
                        ComboBox_AddString(hWinControl, tbuffer);
                        ComboBox_AddString(hWinControl2, tbuffer);
                    }
                }
            }
            ComboBox_SetCurSel(hWinControl, 0);
            ComboBox_SetCurSel(hWinControl2, 0);
        }
        else
            errCode |= 0x8;
    }
    else
        errCode |= 0x8;
    
    // error notification
    if (errCode)
    {
        showWarning("GPUconfigure", "failed to load required resources");
        onCloseDialog(hWindow);
    }
}

/* set language dependant text content -------------------------------------- */
void DialogConfig::setLanguageElements(HWND hWindow, bool init)
{
    // STATIC LABELS --------------------------------------------
    
    // main dialog controls translation
    SetWindowText(hWindow, _langContent->dialogTitle);
    SetDlgItemText(hWindow, IDOK, _langContent->buttonOK);
    SetDlgItemText(hWindow, IDCANCEL, _langContent->buttonCancel);
    
    // reset page tabs visuals
    _initialize = 0;
    SendMessage(_hBtnGeneral, WM_SETFONT, (WPARAM)_hFontIcons, (LPARAM)MAKELONG(TRUE, 0));
    SendMessage(_hBtnManager, WM_SETFONT, (WPARAM)_hFontIcons, (LPARAM)MAKELONG(TRUE, 0));
    SendMessage(_hBtnProfile, WM_SETFONT, (WPARAM)_hFontIcons, (LPARAM)MAKELONG(TRUE, 0));
    
    // page1 labels translation
    SetDlgItemText(_pageGeneral, IDS_GEN_GROUP1, _langContent->pageGenGroupDisplaySettings);
    SetDlgItemText(_pageGeneral, IDS_GEN_DISPRES, _langContent->pageGenDisplayResolution);
    SetDlgItemText(_pageGeneral, IDC_GEN_DISPMODE1, _langContent->pageGenDisplayFullscreen);
    SetDlgItemText(_pageGeneral, IDC_GEN_DISPMODE2, _langContent->pageGenDisplayWindow);
    SetDlgItemText(_pageGeneral, IDS_GEN_RENDER, _langContent->pageGenRenderMode);
    SetDlgItemText(_pageGeneral, IDC_GEN_GTEACC, _langContent->pageGenAntiJitterAccuracy);
    SetDlgItemText(_pageGeneral, IDC_GEN_VSYNC, _langContent->pageGenVerticalSync);
    SetDlgItemText(_pageGeneral, IDS_GEN_GROUP2, _langContent->pageGenGroupFramerate);
    SetDlgItemText(_pageGeneral, IDC_GEN_FSKIP, _langContent->pageGenFrameSkipping);
    SetDlgItemText(_pageGeneral, IDC_GEN_FPSLIMIT, _langContent->pageGenFramerateLimit);
    SetDlgItemText(_pageGeneral, IDC_GEN_FPSAUTO, _langContent->pageGenAutoDetect);
    SetDlgItemText(_pageGeneral, IDC_GEN_FPSFIXED, _langContent->pageGenFixed);
    SetDlgItemText(_pageGeneral, IDS_GEN_FPSABBR, _langContent->pageGenFpsAbbr);
    SetDlgItemText(_pageGeneral, IDC_BTN_KEYBINDING, _langContent->pageGenPluginKeysBtn);
    SetDlgItemText(_pageGeneral, IDC_BTN_ADVANCED, _langContent->pageGenAdvancedSettingsBtn);
    // page2 labels translation
    SetDlgItemText(_pageManager, IDS_MAN_GROUP1, _langContent->pageManGroupProfilesManage);
    SetDlgItemText(_pageManager, IDS_MAN_INTRO1, _langContent->pageManIntroLine1);
    SetDlgItemText(_pageManager, IDS_MAN_INTRO2, _langContent->pageManIntroLine2);
    SetDlgItemText(_pageManager, IDS_MAN_INTRO3, _langContent->pageManIntroLine3);
    SetDlgItemText(_pageManager, IDS_MAN_INTRO4, _langContent->pageManIntroLine4);
    SetDlgItemText(_pageManager, IDS_MAN_INTRO5, _langContent->pageManIntroLine5);
    SetDlgItemText(_pageManager, IDC_BTN_REMOVEPROFILE, _langContent->pageManRemoveBtn);
    SetDlgItemText(_pageManager, IDC_BTN_EXPORTPROFILE, _langContent->pageManExportProfileBtn);
    SetDlgItemText(_pageManager, IDS_MAN_PRESET, _langContent->pageManPresetValues);
    SetDlgItemText(_pageManager, IDC_BTN_PRESET, _langContent->pageManApplyPresetBtn);
    SetDlgItemText(_pageManager, IDS_MAN_GROUP2, _langContent->pageManGroupProfilePresets);
    SetDlgItemText(_pageManager, IDC_BTN_ADDPROFILE, _langContent->pageManAddProfileBtn);
    SetDlgItemText(_pageManager, IDC_BTN_IMPORTPROFILE, _langContent->pageManImportProfileBtn);
    SetDlgItemText(_pageManager, IDC_BTN_ASSOCIATIONS, _langContent->pageManGameAssociationsBtn);
    // page3 labels translation
    SetDlgItemText(_pageProfile, IDS_PRO_GROUP1, _langContent->pageProGroupFilters);
    SetDlgItemText(_pageProfile, IDS_PRO_TXFILTER, _langContent->pageProTextureFiltering);
    SetDlgItemText(_pageProfile, IDS_PRO_2DFILTER, _langContent->pagePro2dFiltering);
    SetDlgItemText(_pageProfile, IDS_PRO_SCRFILTER, _langContent->pageProScreenFiltering);
    SetDlgItemText(_pageProfile, IDC_PRO_MDEC, _langContent->pageProMdecFilter);
    SetDlgItemText(_pageProfile, IDC_PRO_FXAA, _langContent->pageProAntiAliasing);
    SetDlgItemText(_pageProfile, IDC_PRO_FXBUMP, _langContent->pageProBumpMapping);
    SetDlgItemText(_pageProfile, IDC_PRO_FXHDR, _langContent->pageProHdRange);
    SetDlgItemText(_pageProfile, IDC_PRO_FXMOTION, _langContent->pageProMotionBlur);
    SetDlgItemText(_pageProfile, IDC_PRO_FXSPECIAL, _langContent->pageProSpecialRendering);
    SetDlgItemText(_pageProfile, IDS_PRO_SPECIALLEVEL, _langContent->pageProRenderLevel);
    SetDlgItemText(_pageProfile, IDS_PRO_GROUP2, _langContent->pageProGroupDisplay);
    SetDlgItemText(_pageProfile, IDS_PRO_INTRES, _langContent->pageProInternalResolution);
    SetDlgItemText(_pageProfile, IDS_PRO_STRETCH, _langContent->pageProScreenStretching);
    SetDlgItemText(_pageProfile, IDC_PRO_MIRROR, _langContent->pageProMirrorScreen);
    SetDlgItemText(_pageProfile, IDC_PRO_SCREXTRA, _langContent->pageProExtraRendering);
    SetDlgItemText(_pageProfile, IDC_PRO_SCRBORDER, _langContent->pageProAntiFlickerBorder);
    SetDlgItemText(_pageProfile, IDS_PRO_PXABBR, _langContent->pageProPxAbbr);
    SetDlgItemText(_pageProfile, IDS_PRO_GROUP3, _langContent->pageProGroupMisc);
    SetDlgItemText(_pageProfile, IDC_PRO_TV, _langContent->pageProTvScanlines);
    SetDlgItemText(_pageProfile, IDC_PRO_CURSOR, _langContent->pageProGunCursor);
    SetDlgItemText(_pageProfile, IDC_BTN_BUGFIXES, _langContent->pageProBugFixesBtn);
    SetDlgItemText(_pageProfile, IDC_BTN_SHADERS, _langContent->pageProAdvancedShaderBtn);
    
    // COMBOBOX LABELS ------------------------------------------
    
    unsigned int errCode = 0;
    HWND hWinControl = NULL;
    int selection = 0;
    
    // config preset choice combobox
    if ((hWinControl = GetDlgItem(_pageManager, IDC_MAN_PRESET)) != NULL)
    {
        selection = 1;
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->presetFastest);
        ComboBox_AddString(hWinControl, _langContent->presetStandard);
        ComboBox_AddString(hWinControl, _langContent->presetEnhanced2D);
        ComboBox_AddString(hWinControl, _langContent->presetEnhanced3D);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x1;
    
    // profiles management choice combobox
    if ((hWinControl = GetDlgItem(_pageManager, IDC_MAN_SELPRO)) != NULL)
    {
        selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_DeleteString(hWinControl, 0);
        ComboBox_InsertString(hWinControl, 0, _langContent->profileDefault);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_SELPRO)) != NULL)
    {
        selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_DeleteString(hWinControl, 0);
        ComboBox_InsertString(hWinControl, 0, _langContent->profileDefault);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    
    // texture filtering
    selection = 0;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_TXFILTER)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureSmoothing0);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureSmoothing1);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureSmoothing2);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x2;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_TXUPSCALE)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureUpscaling0);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureUpscaling1);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureUpscaling2);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureUpscaling3);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureUpscaling4);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureUpscaling5);
        ComboBox_AddString(hWinControl, _langContent->cfgTextureUpscaling6);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x2;
    
    // 2D/sprite filtering
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_2DFILTER)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteSmoothing0);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteSmoothing1);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteSmoothing2);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x4;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_2DUPSCALE)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling0);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling1);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling2);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling3);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling4);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling5);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling6);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling7);
        ComboBox_AddString(hWinControl, _langContent->cfgSpriteUpscaling8);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x4;
    
    // screen filtering
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_SCRFILTER)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing0);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing1);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing2);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing3);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing4);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing5);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing6);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing7);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenSmoothing8);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x8;
    
    // anti-aliasing
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_AAFILTER)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing0);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing1);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing2);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing3);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing4);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing5);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing6);
        ComboBox_AddString(hWinControl, _langContent->cfgAntiAliasing7);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x10;
    
    // high dynamic range
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_HDRFILTER)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgHdRange0);
        ComboBox_AddString(hWinControl, _langContent->cfgHdRange1);
        ComboBox_AddString(hWinControl, _langContent->cfgHdRange2);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x20;
    
    // special rendering
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_SPECIALFILTER)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender0);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender1);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender2);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender3);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender4);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender5);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender6);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender7);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender8);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender9);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender10);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender11);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender12);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender13);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRender14);
        ComboBox_AddString(hWinControl, _langContent->cfgSpecialRenderC);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x40;
    
    // internal resolution
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_INTRESX)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResX0);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResX1);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResX2);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResX3);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResX4);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResX5);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x80;
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_INTRESY)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResY0);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResY1);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResY2);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResY3);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResY4);
        ComboBox_AddString(hWinControl, _langContent->cfgInternalResY5);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x80;
    
    // screen stretching
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_STRETCH)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenStretching0);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenStretching1);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenStretching2);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenStretching3);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenStretching4);
        ComboBox_AddString(hWinControl, _langContent->cfgScreenStretching5);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x80;
    
    // gun cursor
    if ((hWinControl = GetDlgItem(_pageProfile, IDC_PRO_CURSORTYPE)) != NULL)
    {
        if (!init)
            selection = ComboBox_GetCurSel(hWinControl);
        ComboBox_ResetContent(hWinControl);
        ComboBox_AddString(hWinControl, _langContent->cfgGunCursor0);
        ComboBox_AddString(hWinControl, _langContent->cfgGunCursor1);
        ComboBox_AddString(hWinControl, _langContent->cfgGunCursor2);
        ComboBox_AddString(hWinControl, _langContent->cfgGunCursor3);
        ComboBox_AddString(hWinControl, _langContent->cfgGunCursor4);
        ComboBox_AddString(hWinControl, _langContent->cfgGunCursor5);
        ComboBox_SetCurSel(hWinControl, selection);
    }
    else
        errCode |= 0x80;
    
    // FINAL CHECK ----------------------------------------------
    
    // redraw page content
    switch (_activePage)
    {
        case 0: RedrawWindow(_pageGeneral, NULL, NULL, RDW_INVALIDATE); break;
        case 1: RedrawWindow(_pageManager, NULL, NULL, RDW_INVALIDATE); break;
        case 2: RedrawWindow(_pageProfile, NULL, NULL, RDW_INVALIDATE); break;
    }
    // error notification
    if (errCode)
    {
        showWarning("GPUconfigure", "failed to load some resources");
    }
}

/* grab a part of a sprite-sheet -------------------------------------------- */
HBRUSH DialogConfig::getSpriteBrush(HDC* phDC, bool hover, int coordX, int coordY, int sizeX, int sizeY)
{
    HDC memDC = NULL;
    HDC dstDC = NULL;
    HBITMAP oldMemBmp = NULL; 
    HBITMAP oldDstBmp = NULL;
    HBITMAP dstBmp    = NULL;
    
    // define bitmap contexts
    memDC = CreateCompatibleDC(NULL);
    if (!memDC)
    {
        return NULL;
    }
    dstDC = CreateCompatibleDC(NULL);
    if (!dstDC)
    {
        DeleteDC(memDC);
        return NULL;
    }

    // create bitmap icon
    dstBmp = CreateCompatibleBitmap(*phDC, sizeX, sizeY);
    if (!dstBmp)
    {
        DeleteDC(memDC);
        DeleteDC(dstDC);
        return NULL;
    }
    
    // get part of sprite-sheet image
    if (hover)
        oldMemBmp = (HBITMAP)SelectObject(memDC, _hBmpIconsHover);
    else
        oldMemBmp = (HBITMAP)SelectObject(memDC, _hBmpIcons);
    oldDstBmp = (HBITMAP)SelectObject(dstDC, dstBmp);
    if (oldMemBmp && oldDstBmp)
        BitBlt(dstDC, 0, 0, sizeX, sizeY, memDC, coordX, coordY, SRCCOPY);

    // remove contexts
    if (oldMemBmp)
        SelectObject(memDC, oldMemBmp);
    if (oldDstBmp)
        SelectObject(dstDC, oldDstBmp);
    DeleteDC(memDC);
    DeleteDC(dstDC);

    // create image-pattern brush
    HBRUSH result;
    result = CreatePatternBrush(dstBmp);
    DeleteObject(dstBmp);
    return result;
}



// EVENT HANDLERS - MAIN DIALOG
//______________________________________________________________________________

/* handler - set dialog view ------------------------------------------------ */
BOOL DialogConfig::onInitDialog(HWND hWindow)
{
    #ifdef INSTANCE_SINGLETON
        HINSTANCE hInstance = PandoraGS::getInstance();
        if (hInstance == NULL || hInstance == INVALID_HANDLE_VALUE)
            hInstance = GetModuleHandle(NULL);
      #else
        HINSTANCE hInstance = GetModuleHandle(NULL);   // for executable tester
    #endif
    if (hInstance == NULL || hInstance == INVALID_HANDLE_VALUE)
    {
        showWarning("GPUconfigure", "failed to retrieve dialog handle"); 
        onCloseDialog(hWindow); return FALSE;
    }
    
    // get page dialogs size
    RECT winRect;
    GetClientRect(hWindow, &winRect);
    winRect.left += LOGO_WIDTH;
    winRect.bottom -= 43;
    
    // create pages dialogs
    _pageProfile = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PROFILE_PAGE), 
                                hWindow, (DLGPROC)pageHandler);
    if (_pageProfile)
    {
        SetWindowPos(_pageProfile, NULL, winRect.left, winRect.top, 
                     winRect.right - winRect.left, winRect.bottom - winRect.top, 0);
        ShowWindow(_pageProfile, FALSE);
        EnableWindow(_pageProfile, FALSE);
    }
    else
    {
        showWarning("GPUconfigure", "failed to load page (profile settings)"); 
        onCloseDialog(hWindow); return FALSE;
    }
    
    _pageManager = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MANAGER_PAGE), 
                                hWindow, (DLGPROC)pageHandler);
    if (_pageManager)
    {
        SetWindowPos(_pageManager, NULL, winRect.left, winRect.top, 
                     winRect.right - winRect.left, winRect.bottom - winRect.top, 0);
        ShowWindow(_pageManager, FALSE);
        EnableWindow(_pageManager, FALSE);
    }
    else
    {
        showWarning("GPUconfigure", "failed to load page (profiles manager)"); 
        onCloseDialog(hWindow); return FALSE;
    }
    
    _pageGeneral = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_GENERAL_PAGE), 
                                hWindow, (DLGPROC)pageHandler);
    if (_pageGeneral)
    {
        SetWindowPos(_pageGeneral, NULL, winRect.left, winRect.top, 
                     winRect.right - winRect.left, winRect.bottom - winRect.top, 0);
        ShowWindow(_pageGeneral, TRUE);
        EnableWindow(_pageGeneral, TRUE);
    }
    else
    {
        showWarning("GPUconfigure", "failed to load page (general settings)"); 
        onCloseDialog(hWindow); return FALSE;
    }

    // language choice combobox
    HWND hWinControl = GetDlgItem(hWindow, IDC_CBOX_LANG);
    if (hWinControl)
    {
        ComboBox_AddString(hWinControl, _T(LANG_LABEL0));
        ComboBox_AddString(hWinControl, _T(LANG_LABEL1));
        ComboBox_AddString(hWinControl, _T(LANG_LABEL2));
        ComboBox_AddString(hWinControl, _T(LANG_LABEL3));
        ComboBox_SetCurSel(hWinControl, _langContent->getCode()); 
    }
    else
    {
        showWarning("GPUconfigure", "failed to load language change combobox");
    }
    
    // main menu page tabs
    _hBtnGeneral = CreateWindow(L"button", L"General settings",
                   WS_CHILD | WS_VISIBLE | BS_TEXT | BS_BITMAP | BS_BOTTOM | BS_OWNERDRAW,
                   4, PAGETAB_FIRST_Y, LOGO_WIDTH - 4, PAGETAB_HEIGHT, 
                   hWindow, (HMENU)IDC_BTN_GENERAL, hInstance, NULL);

    _hBtnManager = CreateWindow(L"button", L"Profiles manager",
                   WS_CHILD | WS_VISIBLE | BS_TEXT | BS_BITMAP | BS_BOTTOM | BS_OWNERDRAW,
                   4, PAGETAB_FIRST_Y + PAGETAB_HEIGHT + PAGETAB_INTERVAL, 
                   LOGO_WIDTH - 4, PAGETAB_HEIGHT, hWindow, 
                   (HMENU)IDC_BTN_MANAGER, hInstance, NULL);

    _hBtnProfile = CreateWindow(L"button", L"Profile settings",
                   WS_CHILD | WS_VISIBLE | BS_TEXT | BS_BITMAP | BS_BOTTOM | BS_OWNERDRAW,
                   4, PAGETAB_FIRST_Y + 2*(PAGETAB_HEIGHT + PAGETAB_INTERVAL), 
                   LOGO_WIDTH - 4, PAGETAB_HEIGHT, hWindow, 
                   (HMENU)IDC_BTN_PROFILE, hInstance, NULL);
                   
   if (!_hBtnGeneral || !_hBtnManager || !_hBtnProfile)
   {
       showWarning("GPUconfigure", "failed to load menu tabs"); 
       onCloseDialog(hWindow); return FALSE;
   }
    
    // main menu bitmap icons
    _hBmpIcons      = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_CONFIG_ICONS));
    _hBmpIconsHover = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_CONFIG_ICONS_HOVER));
    if (!_hBmpIcons || !_hBmpIconsHover)
    {
       showWarning("GPUconfigure", "failed to load menu icons bitmap");
    }
    
    // main menu font
    HDC hDC = GetDC(hWindow);
    if (hDC)
    {
        LOGFONT logFont = { 0 };
        logFont.lfHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        logFont.lfWeight = FW_NORMAL;
        //logFont.lfQuality = CLEARTYPE_QUALITY;
        _tcscpy_s(logFont.lfFaceName, _T("Tahoma"));
        //_tcscpy_s(logFont.lfFaceName, _T("MS Sans Serif"));
        _hFontIcons = CreateFontIndirect(&logFont);
        ReleaseDC(hWindow, hDC);
    }
    else
    {
        showWarning("GPUconfigure", "failed to set menu font");
    }
    
    setStandardElements(hWindow);
    setLanguageElements(hWindow, true);
    loadConfig(true);
    return TRUE;
}

/* handler - close dialog --------------------------------------------------- */
void DialogConfig::onCloseDialog(HWND hWindow)
{
    // pages tab system
    if (_hBtnGeneral) DestroyWindow(_hBtnGeneral);
    if (_hBtnManager) DestroyWindow(_hBtnManager);
    if (_hBtnProfile) DestroyWindow(_hBtnProfile);
    if (_hBmpIcons)      DeleteObject(_hBmpIcons);
    if (_hBmpIconsHover) DeleteObject(_hBmpIconsHover);
    if (_hFontIcons)     DeleteObject(_hFontIcons);
    _hBtnGeneral = NULL;
    _hBtnManager = NULL;
    _hBtnProfile = NULL;
    _hBmpIcons   = NULL;
    _hFontIcons  = NULL;
    _activePage  = 0;
    _initialize  = 0;
    
    // dialog pages
    if (_pageGeneral) DestroyWindow(_hBtnGeneral);
    if (_pageManager) DestroyWindow(_hBtnManager);
    if (_pageProfile) DestroyWindow(_hBtnProfile);
    _pageGeneral = NULL;
    _pageManager = NULL;
    _pageProfile = NULL;
    
    // external dialogs
    if (_dialKeyBinding) DestroyWindow(_dialKeyBinding);
    if (_dialAdvanced)   DestroyWindow(_dialAdvanced);
    if (_dialBugFixes)   DestroyWindow(_dialBugFixes);
    if (_dialShaders)    DestroyWindow(_dialShaders);
    if (_dialAddProfile) DestroyWindow(_dialAddProfile);
    if (_dialImportProfile) DestroyWindow(_dialImportProfile);
    if (_dialExportProfile) DestroyWindow(_dialExportProfile);
    if (_dialAssociations)  DestroyWindow(_dialAssociations);
    _dialKeyBinding = NULL;
    _dialAdvanced   = NULL;
    _dialBugFixes   = NULL;
    _dialShaders    = NULL;
    _dialAddProfile = NULL;
    _dialImportProfile = NULL;
    _dialExportProfile = NULL;
    _dialAssociations  = NULL;
    
    EndDialog(hWindow, TRUE);
}

/* handler - paint visual effects ------------------------------------------- */
BOOL DialogConfig::onPaint(HWND hWindow)
{
    PAINTSTRUCT paint;
    HDC hDC = NULL;
    HBRUSH brush = NULL;
    hDC = BeginPaint(hWindow, &paint);
    if (hDC == NULL)
        return FALSE;

    // window bottom border
    RECT rect;
    brush = CreateSolidBrush(RGB(COLOR_BORDER_R, COLOR_BORDER_G, COLOR_BORDER_B));
    if (brush)
    {
        GetClientRect(hWindow, &rect);
        rect.bottom -= 41;
        rect.top = rect.bottom - 1;
        FillRect(hDC, &rect, brush);
        DeleteObject(brush);
        brush = NULL;
    }

    // main menu right border
    brush = CreateSolidBrush(RGB(COLOR_MENUBRD_R, COLOR_MENUBRD_G, COLOR_MENUBRD_B));
    if (brush)
    {
        GetClientRect(hWindow, &rect);
        rect.top    += LOGO_HEIGHT;
        rect.bottom -= 43;
        rect.right = rect.left + LOGO_WIDTH;
        rect.left  = rect.right - 1;
        FillRect(hDC, &rect, brush);
        DeleteObject(brush);
        brush = NULL;
    }

    // main menu background gradient
    RECT line;
    GetClientRect(hWindow, &rect);
    rect.top    += LOGO_HEIGHT;
    rect.bottom -= 43;
    line.left  = rect.left;
    line.right = rect.left + LOGO_WIDTH - 1;
    _tabZoneHeight = rect.bottom - rect.top;
    for (int px = 0; px < _tabZoneHeight; px++)
    {
        line.top = rect.top + px;
        line.bottom = line.top + 1;
        brush = CreateSolidBrush(RGB(COLOR_MENUTOP_R + (px*(COLOR_MENUBTM_R - COLOR_MENUTOP_R) / _tabZoneHeight),
                                     COLOR_MENUTOP_G + (px*(COLOR_MENUBTM_G - COLOR_MENUTOP_G) / _tabZoneHeight), 
                                     COLOR_MENUTOP_B + (px*(COLOR_MENUBTM_B - COLOR_MENUTOP_B) / _tabZoneHeight)));
        FillRect(hDC, &line, brush);
        DeleteObject(brush);
    }

    // page bottom background color
    brush = CreateSolidBrush(RGB(COLOR_PAGE,COLOR_PAGE,COLOR_PAGE));
    if (brush)
    {
        GetClientRect(hWindow, &rect);
        rect.bottom -= 42;
        rect.top = rect.bottom - 1;
        FillRect(hDC, &rect, brush);
        DeleteObject(brush);
        brush = NULL;
    }

    _initialize = 0;
    EndPaint(hWindow, &paint);
    if (hDC)
        ReleaseDC(hWindow, hDC);
    return TRUE;
}

/* handler - draw custom items ---------------------------------------------- */
BOOL DialogConfig::onDrawItem(HWND hWindow, LPARAM lParam)
{
    // identify element to draw
    LPDRAWITEMSTRUCT pDraw = (LPDRAWITEMSTRUCT)lParam;
    int number = -1;
    switch (pDraw->CtlID)
    {
        case IDC_BTN_GENERAL: number = 0; break;
        case IDC_BTN_MANAGER: number = 1; break;
        case IDC_BTN_PROFILE: number = 2; break;
        default: return TRUE;
    }
    
    // prepare draw components
    HDC  hDC = pDraw->hDC;
    if (hDC == NULL)
        return FALSE;
    HBRUSH brushBack   = NULL;
    HBRUSH brushBorder = NULL;
    brushBorder = CreateSolidBrush(RGB(COLOR_MENUBRD_R, COLOR_MENUBRD_G, COLOR_MENUBRD_B));
    if (brushBorder == NULL)
        return FALSE;
    RECT itemRect = pDraw->rcItem;
    RECT titleRect = { itemRect.left, itemRect.top + PAGETAB_TEXT_Y, 
                       itemRect.right - 4, itemRect.bottom };
    
    // init drawing (used once per button)
    SetBkMode(hDC, TRANSPARENT);
    if (_initialize < PAGES_NUMBER)
    {
        _initialize++;
        if (_activePage == number)  // active tab
        {
            FrameRect(hDC, &itemRect, brushBorder);  // border
            brushBack = CreateSolidBrush(RGB(COLOR_PAGE,COLOR_PAGE,COLOR_PAGE));
            if (brushBack)
            {
                itemRect.left   += 1;
                itemRect.top    += 1;
                itemRect.bottom -= 1;
                FillRect(hDC, &itemRect, brushBack);
                DeleteObject(brushBack);
            }
            onDrawPageTabTitle(hWindow, &hDC, &titleRect, number);
        }
        else  // inactive tabs
        {
            onDrawInactivePageTab(hWindow, &hDC, number, &itemRect);
            onDrawPageTabTitle(hWindow, &hDC, &titleRect, number);
        }
    }
    // redraw selection
    else
    {
        // change selection
        BOOL bIsPressed = (pDraw->itemState & ODS_SELECTED);
        if (bIsPressed)
        {
            // change selection + toggle dialogs
            int prevTab = _activePage;
            onPageChange(hWindow, number);
            
            // change visual selection
            _initialize = 0;
            if (prevTab == 0 || _activePage == 0)
                SendMessage(_hBtnGeneral, WM_SETFONT, (WPARAM)_hFontIcons, (LPARAM)MAKELONG(TRUE, 0));
            if (prevTab == 1 || _activePage == 1)
                SendMessage(_hBtnManager, WM_SETFONT, (WPARAM)_hFontIcons, (LPARAM)MAKELONG(TRUE, 0));
            if (prevTab == 2 || _activePage == 2)
                SendMessage(_hBtnProfile, WM_SETFONT, (WPARAM)_hFontIcons, (LPARAM)MAKELONG(TRUE, 0));
        }
    }
    DeleteObject(brushBorder);
    ReleaseDC(hWindow, hDC);
    return TRUE;
}

/* handler - draw custom inactive page tab ---------------------------------- */
void DialogConfig::onDrawInactivePageTab(HWND hWindow, HDC* phDC, int tabNb, RECT* pItemRect)
{
    // window metrics (for gradient)
    int winHeight = _tabZoneHeight;
    if (winHeight == 1)
        winHeight = PAGETAB_FIRST_Y - LOGO_HEIGHT + 3*(PAGETAB_HEIGHT + PAGETAB_INTERVAL) + 70;
    int top = tabNb*(PAGETAB_HEIGHT + PAGETAB_INTERVAL);
    
    // background gradient
    HBRUSH brushBack = NULL;
    RECT line;
    line.left = pItemRect->left;
    line.right = pItemRect->right - 1;
    for (int px = 0; px < PAGETAB_HEIGHT; px++)
    {
        line.top = px;
        line.bottom = line.top + 1;
        brushBack = CreateSolidBrush(RGB(COLOR_MENUTOP_R + ((top+px)*(COLOR_MENUBTM_R - COLOR_MENUTOP_R) / winHeight),
                                         COLOR_MENUTOP_G + ((top+px)*(COLOR_MENUBTM_G - COLOR_MENUTOP_G) / winHeight),
                                         COLOR_MENUTOP_B + ((top+px)*(COLOR_MENUBTM_B - COLOR_MENUTOP_B) / winHeight)));
        FillRect(*phDC, &line, brushBack);
        DeleteObject(brushBack);
    }
    
    // right border
    HBRUSH brushBorder = NULL;
    brushBorder = CreateSolidBrush(RGB(COLOR_MENUBRD_R, COLOR_MENUBRD_G, COLOR_MENUBRD_B));
    if (brushBorder)
    {
        pItemRect->left = -1 + pItemRect->right;
        FillRect(*phDC, pItemRect, brushBorder);
        DeleteObject(brushBorder);
    }
}

/* handler - draw custom page tab title (text and icon) ----------------------------- */
void DialogConfig::onDrawPageTabTitle(HWND hWindow, HDC* phDC, RECT* pTitleRect, int tabNb)
{
    if (_hFontIcons)
    {
        // set color
        if (tabNb == _activePage)
            SetTextColor(*phDC, RGB(80, 80, 80));
        else
            SetTextColor(*phDC, RGB(70, 90, 100));
        
        // draw text
        switch (tabNb)
        {
            case 0: DrawText(*phDC, _langContent->pageGeneralSettings, -1, pTitleRect, DT_CENTER); break;
            case 1: DrawText(*phDC, _langContent->pageProfilesManager, -1, pTitleRect, DT_CENTER); break;
            case 2: DrawText(*phDC, _langContent->pageProfileSettings, -1, pTitleRect, DT_CENTER); break;
        }
    }

    if (_hBmpIcons && _hBmpIconsHover)
    {
        // set icon position
        RECT iconRect;
        iconRect.left = pTitleRect->left + (LOGO_WIDTH/2) - (4+ICONS_SIZE/2);
        iconRect.right = pTitleRect->right + (4 + ICONS_SIZE / 2) - (LOGO_WIDTH / 2);
        iconRect.bottom = pTitleRect->top - 4;
        iconRect.top = iconRect.bottom - ICONS_SIZE;
        
        // draw icon
        HBRUSH curIcon = getSpriteBrush(phDC, (tabNb==_activePage), tabNb*ICONS_SIZE, 0, ICONS_SIZE, ICONS_SIZE);
        if (curIcon)
        {
            SetBrushOrgEx(*phDC, iconRect.left, iconRect.top, NULL);
            FillRect(*phDC, &iconRect, curIcon);
            DeleteObject(curIcon);
        }
    }
}



// EVENT HANDLERS - PAGE DIALOGS
//______________________________________________________________________________

/* handler - page background painter ---------------------------------------- */
BOOL DialogConfig::onPagePaint(HWND hWindow)
{
    PAINTSTRUCT paint;
    HDC hDC = BeginPaint(hWindow, &paint);

    // page background color
    RECT rect;
    HBRUSH brush = CreateSolidBrush(RGB(COLOR_PAGE,COLOR_PAGE,COLOR_PAGE));
    if (brush)
    {
        GetClientRect(hWindow, &rect);
        FillRect(hDC, &rect, brush);
        DeleteObject(brush);
    }
    
    EndPaint(hWindow, &paint);
    if (hDC)
        ReleaseDC(hWindow, hDC);
    return TRUE;
}

#endif
#endif
