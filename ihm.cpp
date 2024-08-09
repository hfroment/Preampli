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

static const uint8_t iconMuted[] U8X8_PROGMEM = {
    0x00, 0x07, 0x80, 0x27, 0xc0, 0x77, 0xe0, 0x3e, 0x60, 0x1e, 0x3c, 0x0e,
    0x1c, 0x07, 0x84, 0x07, 0xc4, 0x07, 0xfc, 0x06, 0x7c, 0x06, 0x78, 0x06,
    0xdc, 0x06, 0xce, 0x07, 0x84, 0x07, 0x00, 0x07 };

//static const uint8_t iconDac[] U8X8_PROGMEM = {
//    0xe0, 0x07, 0x38, 0x1c, 0x4c, 0x30, 0x76, 0x60, 0x3a, 0x40, 0xdb, 0xc3,
//    0xed, 0x87, 0xe1, 0x87, 0xe1, 0x87, 0xe1, 0xb7, 0xc3, 0xdb, 0x02, 0x5c,
//    0x06, 0x6e, 0x0c, 0x32, 0x38, 0x1c, 0xe0, 0x07 };

#ifdef USE_MOTORIZED_POT
static const uint8_t iconMotor[] U8X8_PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x6e, 0x00, 0xc6, 0x30, 0xc2,
    0xfe, 0x45, 0xce, 0x7f, 0x86, 0x11, 0x87, 0x03, 0x86, 0x01, 0xce, 0x01,
    0xfe, 0x01, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif

static const uint8_t iconVide[IHM::mHauteurSymbole * IHM::mLargeurSymbole / 8] U8X8_PROGMEM = {0,};
static const uint8_t iconPlein[IHM::mHauteurSymbole * IHM::mLargeurSymbole / 8] U8X8_PROGMEM = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};

static const uint8_t iconLink[] U8X8_PROGMEM = {
    0x00, 0x10, 0x00, 0x7c, 0x00, 0x46, 0x00, 0xc1, 0x00, 0x40, 0x80, 0x60,
    0xe0, 0x23, 0x30, 0x18, 0x18, 0x0c, 0xc6, 0x07, 0x06, 0x01, 0x02, 0x00,
    0xc3, 0x00, 0x42, 0x00, 0x3e, 0x00, 0x10, 0x00 };
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
    mEncodeurPrincipal(0)
  #ifdef I2C_VOLUME
  , mEncodeurSecondaire(0)
  #endif
  , mBounce(0),
    mPositionEncodeur(0),
    #ifdef I2C_VOLUME
    mPositionEncodeurVolume(0),
    mValeurLueEncodeurVolume(0),
    #endif
    mDerniereEntreeCouranteAffichee(Configuration::AucuneEntree),
    mDerniereEntreeActiveAffichee(Configuration::AucuneEntree),
    mDateDebutAppui(0),
    mDateDebutAppuiVolume(0),
    mModeAffichage(ModeSelectionEntree)
#ifdef HDMI
    ,     mHdmiCourante(0)
#endif
  //    mEntreeCouranteMenuActionSecondaires(0)
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

void IHM::initEncodeurPrincipal(uint8_t pinA, uint8_t pinB, uint8_t buttonPin)
{
    if (mEncodeurPrincipal == 0)
    {
        mEncodeurPrincipal = new Encoder(pinA, pinB);
    }
    if (mBounce == 0)
    {
        pinMode(buttonPin, INPUT_PULLUP);
        mBounce = new Bounce();
        mBounce->attach(buttonPin);
        mBounce->interval(5);
    }
}

#ifdef I2C_VOLUME
void IHM::initEncodeurSecondaire(uint8_t pinA, uint8_t pinB)
{
    if (mEncodeurSecondaire == 0)
    {
        mEncodeurSecondaire = new Encoder(pinA, pinB);
    }
}
#endif

void IHM::displayLine(uint8_t lineNumber, String text)
{
#ifdef LCD
    if (mLcd != 0)
    {
        mLcd->setCursor(0, lineNumber);
        uint8_t caractereDeFin = (/*lineNumber == 0 ? xDernierSymbole :*/ mLcdNbCols);
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
#ifdef OLED
    refreshOled(true);
#endif
    //    uint8_t i = 0;
    //    mMenuActionsSecondaires[i].libelle = "Mute";
    //    mMenuActionsSecondaires[i++].action = ActionsPreampli::ToggleMute;
    //    mMenuActionsSecondaires[i].libelle = "Retour";
    //    mMenuActionsSecondaires[i++].action = ActionsPreampli::Retour;
    initEncodeurPrincipal();
#ifdef I2C_VOLUME
    initEncodeurSecondaire();
#endif
}
uint16_t IHM::gerer(bool topSeconde)
{
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
    uint16_t action = gererEncodeurPrincipal();
    action |= gererEncodeurVolume(topSeconde);

    if (Configuration::instance()->entreeActive() != mDerniereEntreeActiveAffichee)
    {
        // On affiche l'entrée active
        String entree = Configuration::instance()->entreeActiveToString();
        mDerniereEntreeActiveAffichee = Configuration::instance()->entreeCourante();
        displayLine(1, entree);
        mNeedToRefresh = true;
    }
    switch(mModeAffichage)
    {
    default:
    case ModeSelectionEntree:
    {
        if (Configuration::instance()->entreeCourante() != mDerniereEntreeCouranteAffichee)
        {
            // On affiche l'entrée courante
            String entree = Configuration::instance()->entreeCouranteToString();
            displayLine(2, entree);
            mDerniereEntreeCouranteAffichee = Configuration::instance()->entreeCourante();
            mNeedToRefresh = true;
        }
    }
        break;
    case ModeBalance:
    {
        int8_t volume;
        int8_t balance;
        Configuration::instance()->volumeChanged(volume, balance);
        displayLine(2, String(volume) + " / " + String(volume + balance));
    }
        break;
#ifdef HDMI
    case ModeSelectionHdmi:
        if (mHdmiCourante > 0)
        {
            displayLine(2, "HDMI : " + String(mHdmiCourante));
        }
        else
        {
            displayLine(2, "HDMI : *");
        }
        break;
#endif
    }
    if (mNombreDeSecondesAvantExtinction == 0)
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
        mNeedToRefresh = refreshOled();
#else
        mNeedToRefresh = false;
#endif
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

void IHM::muted(bool active)
{
#ifdef OLED
    afficherSymbole(iconMuted, mXSymboleMute, active);
#endif
}

void IHM::linked(bool active)
{
#ifdef OLED
    afficherSymbole(iconLink, mXSymboleLink, active);
#endif
}

#ifdef USE_MOTORIZED_POT
void IHM::motorOn(bool active)
{
#ifdef OLED
    afficherSymbole(iconMotor, mXSymboleFugitif, active);
#endif
}
#endif

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

void IHM::refresh()
{
#ifdef OLED
    refreshOled(true);
#endif
}

void IHM::gererIt()
{
#ifdef I2C_VOLUME
    if (mEncodeurSecondaire != 0)
    {
        mValeurLueEncodeurVolume = mEncodeurSecondaire->read();
    }
#endif
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
            // On repasse en mode stanard
            mModeAffichage = ModeSelectionEntree;
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
            // On repasse en mode stanard
            mModeAffichage = ModeSelectionEntree;
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

uint16_t IHM::gererEncodeurPrincipal()
{
    uint16_t action = ActionsPreampli::AucuneAction;

    if (mEncodeurPrincipal != 0)
    {
        long newPosition = mEncodeurPrincipal->read() / 4;
        if (newPosition != mPositionEncodeur)
        {
            bool plus = mPositionEncodeur < newPosition;
            switch(mModeAffichage)
            {
            default:
            case ModeSelectionEntree:
                if (plus)
                {
                    action |= ActionsPreampli::SelectionSuivante;
                }
                else
                {
                    action |= ActionsPreampli::SelectionPrecedente;
                }
                break;
            case ModeBalance:
                break;
#ifdef HDMI
            case ModeSelectionHdmi:
                if (plus)
                {
                    if (mHdmiCourante < 5)
                    {
                        mHdmiCourante++;
                    }
                    else
                    {
                        mHdmiCourante = 0;
                    }
                }
                else
                {
                    if (mHdmiCourante > 0)
                    {
                        mHdmiCourante--;
                    }
                    else
                    {
                        mHdmiCourante = 5;
                    }
                }
                action |= ActionsPreampli::ForceHdmi;
                break;
#endif
            }
            mPositionEncodeur = newPosition;
            mNeedToRefresh = true;
        }
    }

    if (mBounce != 0)
    {
        if ((mDateDebutAppui != 0) && (millis() - mDateDebutAppui > mDureeAppuiLong))
        {
            // Appui long
            Serial.println(F("Appui long"));
            mDateDebutAppui = 0;
#ifdef HDMI
            // Refresh sur changement de mode
            if (mModeAffichage != ModeSelectionHdmi)
            {
                mModeAffichage = ModeSelectionHdmi;
            }
            else
            {
                mDerniereEntreeCouranteAffichee = Configuration::AucuneEntree;
                mModeAffichage = ModeSelectionEntree;
            }
#endif
            mNeedToRefresh = true;
        }
        else if (mBounce->update() == 1)
        {
            /// \todo gérer l'appui long pour la config
            if (mBounce->read() == 0)
            {
                // Falling edge
                Serial.println(F("Appui mBounce->read() == 0"));
                mDateDebutAppui = millis();
                mNeedToRefresh = true;
            }
            else
            {
                // Raising edge
                Serial.println(F("Appui mBounce->read() != 0"));
                if (mDateDebutAppui != 0)
                {
                    Serial.println(F("Appui court"));
                    switch (mModeAffichage)
                    {
                    case ModeSelectionEntree:
                        action |= ActionsPreampli::ActiverEntreeCourante;
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
    return action;
}

uint16_t IHM::gererEncodeurVolume(bool seconde)
{
    uint16_t action = ActionsPreampli::AucuneAction;
#ifdef I2C_VOLUME
    if (mEncodeurSecondaire != 0)
    {
        long newPosition = mValeurLueEncodeurVolume / 4;
        if (newPosition != mPositionEncodeurVolume)
        {
            bool plus = mPositionEncodeurVolume < newPosition;
            if (mModeAffichage != ModeBalance)
            {
                if (plus)
                {
                    action |= ActionsPreampli::VolumePlus;
                }
                else
                {
                    action |= ActionsPreampli::VolumeMoins;
                }
            }
            else
            {
                if (plus)
                {
                    action |= ActionsPreampli::BalanceDroite;
                }
                else
                {
                    action |= ActionsPreampli::BalanceGauche;
                }
            }
            mPositionEncodeurVolume = newPosition;
        }
    }
    if ((mDateDebutAppuiVolume != 0) && (millis() - mDateDebutAppuiVolume > mDureeAppuiLongVolume))
    {
        // Appui long
        Serial.println(F("Appui long volume"));
        mDateDebutAppuiVolume = 0;
        if (mModeAffichage != ModeBalance)
        {
            mModeAffichage = ModeBalance;
        }
        else
        {
            mDerniereEntreeCouranteAffichee = Configuration::AucuneEntree;
            mModeAffichage = ModeSelectionEntree;
        }
        // Refresh sur changement de mode
        mNeedToRefresh = true;
        action |= ActionsPreampli::Refresh;
    }
    else if (analogRead(encoderSecondaireButton) < seuilPresenceServitudes)
    {
        if (mDateDebutAppuiVolume == 0)
        {
            Serial.println(F("Appui volume"));
            mDateDebutAppuiVolume = millis();
        }
    }
    else
    {
        if (mDateDebutAppuiVolume != 0)
        {
            mDateDebutAppuiVolume = 0;
        }
    }
#endif
    return action;
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

bool IHM::refreshOled(bool full)
{
    bool retour = !full;
    if (full)
    {
        mOled->firstPage();
        do {
            refreshPage();
        } while (mOled->nextPage());
        mOled->firstPage();
    }
    else
    {
        refreshPage();
        if (!mOled->nextPage())
        {
            retour = false;
            mOled->firstPage();
        }
    }
    return retour;
}

void IHM::refreshPage()
{
    // On récupère le volume
    int8_t volume;
    int8_t balance;
    Configuration::instance()->volumeChanged(volume, balance);
    mOled->setFont(u8g2_font_ncenB14_tr);
    mOled->setCursor(0, mHauteurSymbole);
    mOled->print(mLigne1);
    for (int i = mXPremierSymbole; i < mNombreSymboles; i++)
    {
        mOled->drawXBMP(i * mLargeurSymbole, 0, mLargeurSymbole, mHauteurSymbole, mSymboles[i]);
    }
    mOled->setFont(u8g2_font_ncenB18_tr);
    mOled->setCursor(0, mHauteurSymbole + mHauteurTexte);
    mOled->print(mLigne2);

    mOled->drawBox(0, 50, volume, 6);
    mOled->drawBox(0, 58, volume + balance, 6);
}
#endif
