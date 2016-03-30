/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   lang_fr.h
    Description : french text content
*******************************************************************************/
#ifndef _LANG_FR_H
#define _LANG_FR_H
#include "lang.h"

/* CONFIG DIALOG CONTENT ---------------------------------------------------- */
class LangConfigFR : public LangConfig
{
    public:
    LangConfigFR()
    {
        dialogTitle  = _T("PandoraGS Driver - configuration");  // dialog
        buttonOK     = _T("OK");
        buttonCancel = _T("Annuler");
        
        pageGeneralSettings = _T("Options g\x0E9n\x0E9rales");  // pages
        pageProfilesManager = _T("Gestion de profils");
        pageProfileSettings = _T("Options du profil");
        
        pageGenGroupDisplaySettings = _T("Options d'affichage");// page1 labels
        pageGenDisplayResolution    = _T("R\x0E9solution d'affichage");
        pageGenDisplayFullscreen    = _T("plein \x0E9\x063ran");
        pageGenDisplayWindow        = _T("fen\x0EAtre");
        pageGenRenderMode           = _T("Mode de rendu");
        pageGenAntiJitterAccuracy   = _T("Pr\x0E9\x063ision anti-tremblements");
        pageGenVerticalSync         = _T("Synchronisation verticale");
        pageGenGroupFramerate      = _T("Rafra\x0EE\x063hissement");
        pageGenFrameSkipping       = _T("Activer saut d'images");
        pageGenFramerateLimit      = _T("Limiter images par sec.");
        pageGenAutoDetect          = _T("auto-d\x0E9tection");
        pageGenFixed               = _T("fixe (10-200)");
        pageGenFpsAbbr             = _T("FPS");
        pageGenPluginKeysBtn       = _T("Touches de contr\x0F4le du plugin");
        pageGenAdvancedSettingsBtn = _T("Options avanc\x0E9\x065s");
        
        pageManGroupProfilesManage = _T("Gestion des profils"); // page2 labels
        pageManIntroLine1          = _T("Lorem ipsum");
        pageManIntroLine2          = _T("Dolor sit amet");
        pageManIntroLine3          = _T("Lorem ipsum");
        pageManIntroLine4          = _T("Dolor sit amet");
        pageManIntroLine5          = _T("...");
        pageManRemoveBtn           = _T("Effacer");
        pageManExportProfileBtn    = _T("Exporter profil");
        pageManGroupProfilePresets = _T("Presets de profil");
        pageManPresetValues        = _T("Valeurs presets");
        pageManApplyPresetBtn      = _T("Appliquer preset");
        pageManAddProfileBtn       = _T("Ajouter un profil");
        pageManImportProfileBtn    = _T("Importer profil");
        pageManGameAssociationsBtn = _T("G\x0E9rer associations des jeux");
        
        pageProGroupFilters     = _T("Filtres");                // page3 labels
        pageProTextureFiltering = _T("Filtrage de texture");
        pagePro2dFiltering      = _T("Filtrage 2D/sprites");
        pageProScreenFiltering  = _T("Filtrage de l'\x0E9\x063ran");
        pageProMdecFilter       = _T("Filtre vid\x0E9o MDEC");
        pageProAntiAliasing     = _T("Anti-cr\x0E9nelage :");
        pageProBumpMapping      = _T("Bump mapping");
        pageProHdRange          = _T("Large gamme (hdr) :");
        pageProMotionBlur       = _T("Flou de mouvement");
        pageProSpecialRendering = _T("Rendu sp\x0E9\x063ial :");
        pageProRenderLevel      = _T("Niveau (1-4) :");
        pageProGroupDisplay       = _T("Options d'affichage");
        pageProInternalResolution = _T("R\x0E9solution interne");
        pageProScreenStretching   = _T("Agrandissement");
        pageProMirrorScreen       = _T("Inverser \x0E9\x063ran");
        pageProExtraRendering     = _T("Remplacer bords noirs par rendu additionnel (peut causer artefacts)");
        pageProAntiFlickerBorder  = _T("Bords anti-clignotement :");
        pageProPxAbbr             = _T("px");
        pageProGroupMisc         = _T("Divers");
        pageProTvScanlines       = _T("Lignes de balayage TV");
        pageProGunCursor         = _T("Activer viseur d'arme");
        pageProBugFixesBtn       = _T("Corrections sp\x0E9\x063ifiques");
        pageProAdvancedShaderBtn = _T("Options de rendu avanc\x0E9\x065s");
        
        profileDefault   = _T("<par d\x0E9\x066\x061ut>");
        
        // combobox config choices
        
        presetFastest    = _T("Rapide");
        presetStandard   = _T("Standard");
        presetEnhanced2D = _T("2D am\x0E9lior\x0E9\x065");
        presetEnhanced3D = _T("3D am\x0E9lior\x0E9\x065");
        
        cfgTextureSmoothing0 = _T("Au plus proche (natif)");
        cfgTextureSmoothing1 = _T("Filtre standard (bilin\x0E9\x061ire)");
        cfgTextureSmoothing2 = _T("Filtre \x0E9tendu (bilin\x0E9\x061ire)");
        cfgTextureUpscaling0 = _T("R\x0E9solution originale");
        cfgTextureUpscaling1 = _T("Expansion 2xSaI");
        cfgTextureUpscaling2 = _T("Expansion 2xSuper-Eagle");
        cfgTextureUpscaling3 = _T("Expansion HQ2x");
        cfgTextureUpscaling4 = _T("Expansion 2xBRZ");
        cfgTextureUpscaling5 = _T("Expansion HQ3x (lent)");
        cfgTextureUpscaling6 = _T("Expansion 3xBRZ (lent)");
        
        cfgSpriteSmoothing0 = _T("Au plus proche (natif)");
        cfgSpriteSmoothing1 = _T("Filtre standard (bilin\x0E9\x061ire)");
        cfgSpriteSmoothing2 = _T("Filtre \x0E9tendu (bilin\x0E9\x061ire)");
        cfgSpriteUpscaling0 = _T("R\x0E9solution originale");
        cfgSpriteUpscaling1 = _T("Expansion 2xSaI");
        cfgSpriteUpscaling2 = _T("Expansion 2xSuper-Eagle");
        cfgSpriteUpscaling3 = _T("Expansion HQ2x");
        cfgSpriteUpscaling4 = _T("Expansion 2xBRZ");
        cfgSpriteUpscaling5 = _T("Expansion HQ3x");
        cfgSpriteUpscaling6 = _T("Expansion 3xBRZ");
        cfgSpriteUpscaling7 = _T("Expansion HQ4x (lent)");
        cfgSpriteUpscaling8 = _T("Expansion 4xBRZ (lent)");
               
        cfgScreenSmoothing0 = _T("Aucun");
        cfgScreenSmoothing1 = _T("Effet de flou");
        cfgScreenSmoothing2 = _T("Filtre HQ3x");
        cfgScreenSmoothing3 = _T("Filtre 3xBRZ");
        cfgScreenSmoothing4 = _T("Filtre HQ4x");
        cfgScreenSmoothing5 = _T("Filtre 4xBRZ");
        cfgScreenSmoothing6 = _T("Filtre 4xBRZ et flou l\x0E9ger");
        cfgScreenSmoothing7 = _T("Filtre5xBRZ");
        cfgScreenSmoothing8 = _T("Filtre 5xBRZ et flou l\x0E9ger");
        
        cfgAntiAliasing0 = _T("FXAA (rapide, flou)");
        cfgAntiAliasing1 = _T("4x MSAA (standard)");
        cfgAntiAliasing2 = _T("8x MSAA (standard)");
        cfgAntiAliasing3 = _T("4x NFAA (subtil)");
        cfgAntiAliasing4 = _T("4x SMAA (bon)");
        cfgAntiAliasing5 = _T("8x SMAA (bon)");
        cfgAntiAliasing6 = _T("2x SSAA (lent, juste 3D)");
        cfgAntiAliasing7 = _T("4x SSAA (lent, juste 3D)");
        
        cfgHdRange0 = _T("LumaSharpen (bords pr\x0E9\x063is)");
        cfgHdRange1 = _T("Bloom (effets lumineux)");
        cfgHdRange2 = _T("LumaSharpen et bloom");
        
        cfgSpecialRender0 = _T("Natural vision V1 (cathodique)");
        cfgSpecialRender1 = _T("Natural vision V2 (cathodique)");
        cfgSpecialRender2 = _T("Cel-shading V1 (cartoon)");
        cfgSpecialRender3 = _T("Cel-shading V2 (cartoon)");
        cfgSpecialRender4 = _T("Cel-shading V3 (cartoon)");
        cfgSpecialRender5 = _T("Cel-shading V4 (cartoon)");
        cfgSpecialRender6 = _T("Storybook monochrome");
        cfgSpecialRender7 = _T("Storybook en couleur");
        cfgSpecialRender8 = _T("Verre bris\x0E9 (recouvrement)");
        cfgSpecialRender9 = _T("Bruit al\x0E9\x061toire (recouvrement)");
        cfgSpecialRender10= _T("Grain de film vid\x0E9o (recouvrement)");
        cfgSpecialRender11= _T("Bleut\x0E9  (fusion couleurs)");
        cfgSpecialRender12= _T("Verd\x0E2tre (fusion couleurs)");
        cfgSpecialRender13= _T("Gris\x0E9   (fusion couleurs)");
        cfgSpecialRender14= _T("Noir et blanc");
        cfgSpecialRenderC = _T("Personnalis\x0E9 (options avanc\x0E9\x065s)");
        
        cfgInternalResX0 = _T("R\x0E9solution d'affichage");
        cfgInternalResX1 = _T("X : Native (textures 1024px)");
        cfgInternalResX2 = _T("X : Haute (2x)");
        cfgInternalResX3 = _T("X : Tr\x0E8s haute (4x)");
        cfgInternalResX4 = _T("X : Ultra (6x, GPU modernes)");
        cfgInternalResX5 = _T("X : Ultra+ (8x, GPU modernes)");
        cfgInternalResY0 = _T("R\x0E9solution d'affichage");
        cfgInternalResY1 = _T("X : Native (textures 512px)");
        cfgInternalResY2 = _T("X : Haute (2x)");
        cfgInternalResY3 = _T("X : Tr\x0E8s haute (4x)");
        cfgInternalResY4 = _T("X : Ultra (6x, lent)");
        cfgInternalResY5 = _T("X : Ultra+ (8x, lent)");

        cfgScreenStretching0 = _T("Multiple de r\x0E9solution native");
        cfgScreenStretching1 = _T("Taille de fen\x0EAtre (\x0E9tir\x0E9)");
        cfgScreenStretching2 = _T("Conserver ratio (non coup\x0E9)");
        cfgScreenStretching3 = _T("Conserver ratio (largeur fen\x0EAtre)");
        cfgScreenStretching4 = _T("Proche du ratio (faible \x0E9tir.,non coup\x0E9)");
        cfgScreenStretching5 = _T("Proche du ratio (largeur fen\x0EAtre)");
        
        cfgGunCursor0 = _T("Petit point");
        cfgGunCursor1 = _T("Gros point");
        cfgGunCursor2 = _T("Cercle");
        cfgGunCursor3 = _T("Croix");
        cfgGunCursor4 = _T("Croix et cercle");
        cfgGunCursor5 = _T("Fl\x0E8\x063hes");
    }
};

/* INGAME MENU CONTENT ------------------------------------------------------ */
class LangMenuFR : public LangMenu
{
    public:
    LangMenuFR()
    {
        profileSelection = "Choix de profil :";
        profileConfirm   = "confirmer";
        profileHideMenu  = "masquer menu";
        
        pause      = "PAUSE";
        slowMotion = "MODE RALENTI";
        
        debugFPS      = "FPS";
        debugPolys    = "polys";
        debugVertices = "points";
        
        debugLinesMode      = "lignes";
        debugFilledMode     = "plein (sans 2D)";
        debugFilled2DMode   = "plein";
        debugTexturedMode   = "textures (sans 2D)";
        debugTextured2DMode = "textures";
    }
};

#endif
