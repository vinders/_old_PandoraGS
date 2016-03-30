/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   lang.cpp
    Description : language specific content
*******************************************************************************/
using namespace std;
#include "lang.h"
#include "lang_en.h"
#include "lang_es.h"
#include "lang_fr.h"
#include "lang_de.h"

/* create config dialog translated text content (factory) ------------------- */
LangConfig* LangConfig::getInstance(unsigned int langCode)
{
    LangConfig* langContent = NULL;
    
    // set language values depending on argument (code)
    switch(langCode)
    {
        case LANGCODE_EN: langContent = new LangConfigEN(); break;
        case LANGCODE_ES: langContent = new LangConfigES(); break;
        case LANGCODE_FR: langContent = new LangConfigFR(); break;
        case LANGCODE_DE: langContent = new LangConfigDE(); break;
        default: langContent = new LangConfigEN(); 
                 langCode = LANGCODE_EN; break;
    }
    langContent->setCode(langCode);
    return langContent;
}

/* create ingame menu translated text content (factory) --------------------- */
LangMenu* LangMenu::getInstance(unsigned int langCode)
{
    LangMenu* langContent = NULL;
    
    // set language values depending on argument (code)
    switch(langCode)
    {
        case LANGCODE_EN: langContent = new LangMenuEN(); break;
        case LANGCODE_ES: langContent = new LangMenuES(); break;
        case LANGCODE_FR: langContent = new LangMenuFR(); break;
        case LANGCODE_DE: langContent = new LangMenuDE(); break;
        default: langContent = new LangMenuEN();
                 langCode = LANGCODE_EN; break;
    }
    langContent->setCode(langCode);
    return langContent;
}

/* language code getters/setters ---------------------------------------------- */
unsigned int LangConfig::getCode()
{
    return _code;
}
void LangConfig::setCode(unsigned int langCode)
{
    _code = langCode;
}
unsigned int LangMenu::getCode()
{
    return _code;
}
void LangMenu::setCode(unsigned int langCode)
{
    _code = langCode;
}
