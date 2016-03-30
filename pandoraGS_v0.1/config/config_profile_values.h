/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config_profile_values.h
    Description : config profile constants values
*******************************************************************************/
#define CFG_PRESET_FAST     0
#define CFG_PRESET_STD      1
#define CFG_PRESET_EN2D     2
#define CFG_PRESET_EN3D     3

#define CFG_KEY_ENTER       0
#define CFG_KEY_BACKSPACE   1
#define CFG_KEY_F9          2
#define CFG_KEY_F10         3
#define CFG_KEY_F11         4
#define CFG_KEY_F12         5
#define CFG_KEY_INSERT      6
#define CFG_KEY_DELETE      7
#define CFG_KEY_HOME        8
#define CFG_KEY_END         9
#define CFG_KEY_PGUP        10
#define CFG_KEY_PGDOWN      11
#define CFG_KEY_NUM0        12
#define CFG_KEY_NUM1        13
#define CFG_KEY_NUM2        14
#define CFG_KEY_NUM3        15
#define CFG_KEY_NUM4        16
#define CFG_KEY_NUM5        17
#define CFG_KEY_NUM6        18
#define CFG_KEY_NUM7        19
#define CFG_KEY_NUM8        20
#define CFG_KEY_NUM9        21
#define CFG_KEY_CHAR0       22
#define CFG_KEY_CHAR1       23
#define CFG_KEY_CHAR2       24
#define CFG_KEY_CHAR3       25
#define CFG_KEY_CHAR4       26
#define CFG_KEY_CHAR5       27
#define CFG_KEY_CHAR6       28
#define CFG_KEY_CHAR7       29
#define CFG_KEY_CHAR8       30
#define CFG_KEY_CHAR9       31
#define CFG_KEY_TAB         32
#define CFG_KEY_NUMPLUS     33
#define CFG_KEY_NUMMINUS    34
#define CFG_KEY_NUMSTAR     35
#define CFG_KEY_NUMSLASH    36
#define CFG_KEY_NUMCOMMA    37

#define CFG_TXSM_NONE       0
#define CFG_TXSM_BILINEAR   1
#define CFG_TXSM_ENHANCED   2
#define CFG_TXUS_NONE       0
#define CFG_TXUS_2XSAI      1
#define CFG_TXUS_2XSEAGLE   2
#define CFG_TXUS_HQ2X       3
#define CFG_TXUS_2XBRZ      4
#define CFG_TXUS_HQ3X       5
#define CFG_TXUS_3XBRZ      6

#define CFG_2DSM_NONE       0
#define CFG_2DSM_BILINEAR   1
#define CFG_2DSM_ENHANCED   2
#define CFG_2DUS_NONE       0
#define CFG_2DUS_2XSAI      1
#define CFG_2DUS_2XSEAGLE   2
#define CFG_2DUS_HQ2X       3
#define CFG_2DUS_2XBRZ      4
#define CFG_2DUS_HQ3X       5
#define CFG_2DUS_3XBRZ      6
#define CFG_2DUS_HQ4X       7
#define CFG_2DUS_4XBRZ      8

#define CFG_SCSM_NONE       0
#define CFG_SCSM_BLUR       1
#define CFG_SCSM_HQ3X       2
#define CFG_SCSM_3XBRZ      3
#define CFG_SCSM_HQ4X       4
#define CFG_SCSM_4XBRZ      5
#define CFG_SCSM_4XBRZBLUR  6
#define CFG_SCSM_5XBRZ      7
#define CFG_SCSM_5XBRZBLUR  8

#define CFG_AA_FXAA         0
#define CFG_AA_MSAA4        1
#define CFG_AA_MSAA8        2
#define CFG_AA_NFAA4        3
#define CFG_AA_SMAA4        4
#define CFG_AA_SMAA8        5
#define CFG_AA_SSAA2        6
#define CFG_AA_SSAA4        7

#define CFG_SPECNUMBER      15
#define CFG_SPEC_NV1        0
#define CFG_SPEC_NV2        1
#define CFG_SPEC_CEL1       2
#define CFG_SPEC_CEL2       3
#define CFG_SPEC_CEL3       4
#define CFG_SPEC_CEL4       5
#define CFG_SPEC_STORYM     6
#define CFG_SPEC_STORYC     7
#define CFG_SPEC_BROKEN     8
#define CFG_SPEC_NOISE      9
#define CFG_SPEC_GRAIN      10
#define CFG_SPEC_BLUE       11
#define CFG_SPEC_GREEN      12
#define CFG_SPEC_GRAY       13
#define CFG_SPEC_DESAT      14
#define CFG_SPEC_CUSTOM     -2

#define CFG_LUMA_NORMAL     0
#define CFG_LUMA_WIDER      1
#define CFG_LUMA_PYRAMID    2
#define CFG_LUMA_GAUSSIAN   3
#define CFG_LUMA_HQ         4
#define CFG_BLOOM_SCREEN    0
#define CFG_BLOOM_OVERLAY   1
#define CFG_BLOOM_SOFTLIGHT 2

#define CFG_DITHER_ORDER    0
#define CFG_DITHER_RANDOM   1

#define CFG_INTRES_SCR     0
#define CFG_INTRES_1X      1
#define CFG_INTRES_2X      2
#define CFG_INTRES_4X      3
#define CFG_INTRES_6X      4
#define CFG_INTRES_8X      5

#define CFG_STRETCH_MULT    0
#define CFG_STRETCH_FULL    1
#define CFG_STRETCH_RATIO   2
#define CFG_STRETCH_WIDTH   3
#define CFG_STRETCH_CLOSENC 4
#define CFG_STRETCH_CLOSE   5

#define CFG_SCANLN_HORIZ    0
#define CFG_SCANLN_VERTIC   1
#define CFG_SCANLN_DIAGL    2
#define CFG_SCANLN_DIAGH    3
#define CFG_SCANLN_HORVER   4

#define CFG_GUN_SMALLDOT    0
#define CFG_GUN_LARGEDOT    1
#define CFG_GUN_CIRCLE      2
#define CFG_GUN_CROSSHAIR   3
#define CFG_GUN_CIRCROSS    4
#define CFG_GUN_ARROWS      5

#define CFG_GUNCOLOR_WHITE  0
#define CFG_GUNCOLOR_GRAY   1    
#define CFG_GUNCOLOR_RED    2
#define CFG_GUNCOLOR_GREEN  3
#define CFG_GUNCOLOR_BLUE   4
#define CFG_GUNCOLOR_VIOLET 5
#define CFG_GUNCOLOR_ORANGE 6
#define CFG_GUNCOLOR_YELLOW 7
#define CFG_GUNCOLOR_CYAN   8
        