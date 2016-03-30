/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   console_display.h
    Description : console display settings
*******************************************************************************/
#ifndef _CONSOLE_DISPLAY_H
#define _CONSOLE_DISPLAY_H

/* CONSOLE DISPLAY SETTINGS ------------------------------------------------- */
class ConsoleDisplay
{
  private:
    bool bCurrentStatus;    // display status
    bool bPreviousStatus;
    
    DwPoint_t currentSizeMode;   // console display size (original)
    DwPoint_t previousSizeMode;
    DwPoint_t currentPosition;   // console display position
    DwPoint_t previousPosition;
    DwPoint_t currentDisplayEnd; // console display limit
    DwPoint_t previousDisplayEnd;
    
    WRect_t   currentRange;      // real screen range
    WRect_t   previousRange;
    WRect_t   drawArea;          // real screen drawing area
    DwPoint_t drawOffset;        // real screen drawing offset
    
  public:
    ConsoleDisplay();
    bool isBeingEnabled(bool);
    // display setter methods
    void resetDisplay();
    void setDisplayFixes(bool);
    void setDisplayInfo(unsigned long);
    void setDisplayPosition(unsigned long, unsigned long);
    void setDisplayWidth(unsigned long);
    bool setDisplayHeight(unsigned long);
    void setVisualVibration(unsigned long, unsigned long, unsigned int);
    
    // public display settings
    int  gameColorDepth;   // original color depth (15 / 24 bits)    
    int  gameLocalization; // original localization (NTSC / PAL)
    bool gameInterlacing;  // original interlacing
    int  multiplier;       // height multiplier (simple/double/...)
    int  rumbleStrength;
    int  rumbleTime;
    
    // enum variables
    static short displayWidths[8]; // console display widths
};

#endif
