#include <Encoder.h>
#include <Bounce2.h>

#include "commandes.h"

#include "ihm.h"

#ifdef LCD
#include <LiquidCrystal_I2C.h>
#endif
#ifdef OLED
#include <U8g2lib.h>
#include <U8x8lib.h>
#endif

#ifdef LCD
static const uint8_t customAntenna[8] PROGMEM =
    {
        0b10101,
        0b10101,
        0b01110,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00100
};

static const uint8_t customWatch2[8] PROGMEM =
    {
        0b00000,
        0b01110,
        0b10101,
        0b10111,
        0b10001,
        0b01110,
        0b00000,
        0b00000
};

#endif

#ifdef OLED
static const uint8_t iconAntenna[] U8X8_PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0xfc, 0x3f, 0x1e, 0x78, 0xc7, 0xe3,
       0xf0, 0x0f, 0x3c, 0x3c, 0x8c, 0x31, 0xe0, 0x07, 0x70, 0x0e, 0x00, 0x00,
       0x80, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00};

static const uint8_t iconWatch2[] U8X8_PROGMEM = {0xe0, 0x07, 0xf8, 0x1f, 0x9c, 0x39, 0x06, 0x60, 0x06, 0x60, 0x03, 0xc0,
                                                  0x83, 0xc1, 0x87, 0xe1, 0x87, 0xe1, 0x03, 0xc2, 0x03, 0xc0, 0x06, 0x60,
                                                  0x06, 0x60, 0x9c, 0x39, 0xf8, 0x1f, 0xe0, 0x07};

static const uint8_t iconVide[IHM::mHauteurSymbole * IHM::mLargeurSymbole / 8] U8X8_PROGMEM = {0,};
static const uint8_t iconPlein[IHM::mHauteurSymbole * IHM::mLargeurSymbole / 8] U8X8_PROGMEM = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
#endif

IHM::IHM() :
#ifdef LCD
    mLcd(0),
    mLcdNbCols(0),
    mLcdNbLines(0),
#endif
#ifdef OLED
    mOled(0),
#endif
    mNeedToRefresh(true),
    mNombreDeSecondesAvantExtinction(nombreDeSecondesAvantExtinction),
    mBacklightOn(false),
    mEncodeur(0),
    mPositionEncodeur(0),
    mBounce(0),
    mDerniereEntreeCouranteAffichee(Configuration::AucuneEntree),
    mDerniereEntreeActiveAffichee(Configuration::AucuneEntree),
    mDateDebutAppui(0),
    mModeAffichage(ModeSelectionEntree),
    mEntreeCouranteMenuActionSecondaires(0)
{
}

#ifdef LCD
void IHM::initLcd(uint8_t adresse, uint8_t nbCols, uint8_t nbLignes)
{
    if (mLcd == 0)
    {
        mLcd = new LiquidCrystal_I2C(adresse, nbCols, nbLignes);
        mLcd->init();                      // initialize the lcd
        mLcd->backlight();
        mBacklightOn = true;
        mLcd->createCharFromFlash(SymboleAntenne, customAntenna);
        mLcd->createCharFromFlash(SymboleMontre, customWatch2);
        mLcdNbCols = nbCols;
        mLcdNbLines = nbLignes;
    }
}
#endif

void IHM::initEncodeur(uint8_t pinA, uint8_t pinB, uint8_t buttonPin)
{
    if (mEncodeur == 0)
    {
        mEncodeur = new Encoder(pinA, pinB);
    }
    if (mBounce == 0)
    {
        pinMode(buttonPin, INPUT_PULLUP);
        mBounce = new Bounce();
        mBounce->attach(buttonPin);
        mBounce->interval(5);
    }
}

void IHM::displayLine(uint8_t lineNumber, String text)
{
#ifdef LCD
    if (mLcd != 0)
    {
        mLcd->setCursor(0, lineNumber);
        uint8_t caractereDeFin = (lineNumber == 0 ? xDernierSymbole : mLcdNbCols);
        mLcd->print(text.substring(0,caractereDeFin));
        for (int i = text.length(); i < caractereDeFin; i++)
        {
            mLcd->write(' ');
        }
    }
#endif
#ifdef OLED
    if (mOled != 0)
    {
        switch (lineNumber) {
        case 1:
            mLigne1 = text;
            break;
        default:
        case 2:
            mLigne2 = text;
            break;
//        default:
//            mLigne3 = text;
//            break;
        }
        mNeedToRefresh = true;
    }
#endif
}
void IHM::init(String line1, String line2)
{
#ifdef LCD
    initLcd();
#endif
#ifdef OLED
    initOled();
#endif
    displayLine(1, line1);
    displayLine(2, line2);
    uint8_t i = 0;
    mMenuActionsSecondaires[i].libelle = "Mute";
    mMenuActionsSecondaires[i++].action = Actions::ToggleMute;
    mMenuActionsSecondaires[i].libelle = "Retour";
    mMenuActionsSecondaires[i++].action = Actions::Retour;
    initEncodeur();
}
uint16_t IHM::gerer(bool topSeconde)
{
    uint16_t action = Actions::AucuneAction;

    if (topSeconde)
    {
        saved(Configuration::instance()->saved());
        if (mNombreDeSecondesAvantExtinction > 0)
        {
            mNombreDeSecondesAvantExtinction--;
            if (!mBacklightOn)
            {
                backlight(true);
            }
        }
        else
        {
            if (mBacklightOn)
            {
                backlight(false);
            }
        }
    }
    if (mEncodeur != 0)
    {
        long newPosition = mEncodeur->read() / 4;
        if (newPosition != mPositionEncodeur)
        {
            if (mModeAffichage == ModeSelectionEntree)
            {
                if (mPositionEncodeur > newPosition)
                {
                    action |= Actions::SelectionPrecedente;
                }
                else
                {
                    action |= Actions::SelectionSuivante;
                }
            }
            else if (mModeAffichage == ModeActionsSecondaires)
            {
                if (mPositionEncodeur > newPosition)
                {
                    if (mEntreeCouranteMenuActionSecondaires > 0)
                    {
                        mEntreeCouranteMenuActionSecondaires--;
                    }
                    else
                    {
                        mEntreeCouranteMenuActionSecondaires = mNombreEntreeMenuActionsSecondaires - 1;
                    }
                }
                else
                {
                    mEntreeCouranteMenuActionSecondaires++;
                    if (mEntreeCouranteMenuActionSecondaires >= mNombreEntreeMenuActionsSecondaires)
                    {
                        mEntreeCouranteMenuActionSecondaires = 0;
                    }
                }
                displayLine(2, mMenuActionsSecondaires[mEntreeCouranteMenuActionSecondaires].libelle);
            }
            else if (mModeAffichage == ModeVolume)
            {
                if (mPositionEncodeur > newPosition)
                {
                    action |= Actions::VolumeMoins;
                }
                else
                {
                    action |= Actions::VolumePlus;
                }
            }
            mPositionEncodeur = newPosition;
            //displayStatus(String(mPositionEncodeur));
        }
    }
    if (mBounce != 0)
    {
        if ((mDateDebutAppui != 0) && (millis() - mDateDebutAppui > mDureeAppuiLong))
        {
            mDateDebutAppui = 0;
            switch (mModeAffichage)
            {
            case ModeSelectionEntree:
                mModeAffichage = ModeActionsSecondaires;
                mEntreeCouranteMenuActionSecondaires = 0;
                break;
            case ModeActionsSecondaires:
                mModeAffichage = ModeVolume;
                break;
            case ModeVolume:
                mModeAffichage = ModeSelectionEntree;
                mDerniereEntreeActiveAffichee = Configuration::AucuneEntree;
                break;
            default:
                mModeAffichage = ModeSelectionEntree;
                break;
            }
        }
        else if (mBounce->update() == 1)
        {
            /// \todo gérer l'appui long pour la config
            if (mBounce->read() == 0)
            {
                // Falling edge
                mDateDebutAppui = millis();
            }
            else
            {
                // Raising edge
                if (mDateDebutAppui != 0)
                {
                    switch (mModeAffichage)
                    {
                    case ModeSelectionEntree:
                        action |= Actions::ActiverEntreeCourante;
                        break;
                    case ModeActionsSecondaires:
                        if (mMenuActionsSecondaires[mEntreeCouranteMenuActionSecondaires].action == Actions::Retour)
                        {
                            mModeAffichage = ModeSelectionEntree;
                            mDerniereEntreeActiveAffichee = Configuration::AucuneEntree;
                        }
                        else
                        {
                            action |= mMenuActionsSecondaires[mEntreeCouranteMenuActionSecondaires].action;
                        }
                        break;
                    case ModeVolume:
                        break;
                    default:
                        break;
                    }
                    //action |= Actions::ActiverEntreeCourante;
                    //Configuration::instance()->activerEntreeCourante();
                    mDateDebutAppui = 0;
                }
            }
        }
    }
    if (Configuration::instance()->entreeActive() != mDerniereEntreeActiveAffichee)
    {
        // On affiche l'entrée courante
        String entree = Configuration::instance()->entreeActiveToString();
        mDerniereEntreeActiveAffichee = Configuration::instance()->entreeCourante();
        if (mModeAffichage == ModeVolume)
        {
            entree = "Volume";
            mDerniereEntreeActiveAffichee = Configuration::AucuneEntree;
        }
        displayLine(1, entree);
    }
    if (mNombreDeSecondesAvantExtinction > 0)
    {
        if (Configuration::instance()->entreeCourante() != mDerniereEntreeCouranteAffichee)
        {
            // On affiche l'entrée courante
            String entree = Configuration::instance()->entreeCouranteToString();
            displayLine(2, entree);
            mDerniereEntreeCouranteAffichee = Configuration::instance()->entreeCourante();
        }
    }
    else
    {
        if (mDerniereEntreeCouranteAffichee != Configuration::AucuneEntree)
        {
            displayLine(2, "");
            mDerniereEntreeCouranteAffichee = Configuration::AucuneEntree;
            // On repasse dans le mode nominal
            mModeAffichage = ModeSelectionEntree;
        }
    }
    if (mNeedToRefresh)
    {
#ifdef OLED
        refresOled();
#endif
        mNeedToRefresh = false;
    }

    return action;
}

void IHM::remoteActive(bool active)
{
#ifdef LCD
    afficherSymbole(SymboleAntenne, xTelecommande, active);
#endif
#ifdef OLED
    afficherSymbole(iconAntenna, mXSymboleFugitif, active);
#endif
}

void IHM::saved(bool active)
{
#ifdef LCD
    afficherSymbole(xBuzy, SymboleMontre, active);
#endif
#ifdef OLED
    afficherSymbole(iconWatch2, mXSymboleFugitif, active);
#endif
}

void IHM::displayStatus(String message)
{
    backlightOn();
#ifdef LCD
    displayLine(mLcdNbLines - 1, message);
#endif
#ifdef OLED
    displayLine(1, message);
#endif
}
void IHM::backlightOn()
{
    backlight(true);
    mNombreDeSecondesAvantExtinction = nombreDeSecondesAvantExtinction;
    mNeedToRefresh = true;
}

void IHM::backlight(bool on)
{
#ifdef LCD
    if (mLcd != 0)
    {
        if (on)
        {
            mLcd->backlight();
        }
        else
        {
            mLcd->noBacklight();
        }
    }
#endif
#ifdef OLED
    if (mOled != 0)
    {
        if (on)
        {
            mOled->setPowerSave(0);
            mNeedToRefresh = true;
        }
        else
        {
            mOled->setPowerSave(1);
        }
    }
#endif
    mBacklightOn = on;
}

#ifndef OLED
void IHM::afficherSymbole(uint8_t symbole, uint8_t position, bool etat)
#else
void IHM::afficherSymbole(const uint8_t *symbole, uint8_t position, bool etat)
#endif
{
#ifdef LCD
    if (mLcd != 0)
    {
        mLcd->setCursor(position, 0);
        if (etat)
        {
            mLcd->write(symbole);
        }
        else
        {
            mLcd->write(' ');
        }
    }
#endif
#ifdef OLED
    if (mOled != 0)
    {
        if (position < mNombreSymboles)
        {
            if (etat)
            {
                mSymboles[position] = symbole;
            }
            else
            {
                mSymboles[position] = iconVide;
            }
            mNeedToRefresh = true;
        }
    }
#endif
}

void IHM::effacerSymbole(uint8_t position)
{
#ifdef LCD
    if (mLcd != 0)
    {
        mLcd->setCursor(position, 0);
        mLcd->write(' ');
    }
#endif
#ifdef OLED
    afficherSymbole(iconVide, position, false);
#endif
}

#ifdef OLED
void IHM::initOled()
{
    for (int i = 0; i < mNombreSymboles; i++)
    {
        mSymboles[i] = iconVide;
    }
    mOled = new U8G2_SSD1306_128X64_NONAME_1_HW_I2C(U8G2_R0);
    mOled->begin();
    mOled->setPowerSave(0);
}

void IHM::refresOled()
{
    mOled->firstPage();
    do {
        mOled->setFont(u8g2_font_ncenB14_tr);
        mOled->setCursor(0, mHauteurSymbole);
        mOled->print(mLigne1);
//        for (int i = 0; i < mNombreSymboles; i++)
        {
            int i = mNombreSymboles - 1;
            mOled->drawXBMP(i * mLargeurSymbole, 0, mLargeurSymbole, mHauteurSymbole, mSymboles[i]);
        }
        mOled->setFont(u8g2_font_ncenB24_tr);
        mOled->setCursor(0, mHauteurSymbole + mHauteurTexte);
        mOled->print(mLigne2);

    } while (mOled->nextPage());
}
#endif
