#include <Encoder.h>
#include <Bounce2.h>

#include "commandes.h"

#define U8G2_WITHOUT_INTERSECTION
#define U8G2_WITHOUT_CLIP_WINDOW_SUPPORT
#define U8G2_WITHOUT_FONT_ROTATION
#define U8G2_WITHOUT_CLIP_WINDOW_SUPPORT
#define U8G2_WITHOUT_UNICODE

#include <U8g2lib.h>

#include "ihm.h"


#ifdef OLED

  U8G2_SSD1306_128X64_NONAME_1_HW_I2C  mOled(U8G2_R0);

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

static const uint8_t iconVide[IHM::mHauteurSymbole * IHM::mLargeurSymbole / 8] U8X8_PROGMEM = {0,};
static const uint8_t iconPlein[IHM::mHauteurSymbole * IHM::mLargeurSymbole / 8] U8X8_PROGMEM = {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};

static const uint8_t iconLink[] U8X8_PROGMEM = {
    0x00, 0x10, 0x00, 0x7c, 0x00, 0x46, 0x00, 0xc1, 0x00, 0x40, 0x80, 0x60,
    0xe0, 0x23, 0x30, 0x18, 0x18, 0x0c, 0xc6, 0x07, 0x06, 0x01, 0x02, 0x00,
    0xc3, 0x00, 0x42, 0x00, 0x3e, 0x00, 0x10, 0x00 };
#endif

IHM::IHM() :
    #ifdef OLED
    //mOled(0),
    #endif
    mNeedToRefresh(true),
    mNombreDeSecondesAvantExtinction(nombreDeSecondesAvantExtinction),
    mBacklightOn(false),
    mEncodeurPrincipal(0)
  , mEncodeurSecondaire(0)
  , mBounce(0),
    mPositionEncodeurPrincipal(0),
    mPositionEncodeurSecondaire(0),
    mValeurLueEncodeurSecondaire(0),
    mDerniereEntreeCouranteAffichee(Configuration::AucuneEntree),
    mDerniereEntreeActiveAffichee(Configuration::AucuneEntree),
    mDateDebutAppuiEncodeurPrincipal(0),
    mDateDebutAppuiEncodeurSecondaire(0),
    mModeAffichage(ModeSelectionEntree)
  //    mEntreeCouranteMenuActionSecondaires(0)
{
}

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

void IHM::initEncodeurSecondaire(uint8_t pinA, uint8_t pinB)
{
    if (mEncodeurSecondaire == 0)
    {
        mEncodeurSecondaire = new Encoder(pinA, pinB);
    }
}

void IHM::displayLine(uint8_t lineNumber, const __FlashStringHelper* text)
{
#ifdef OLED
    //if (mOled != 0)
    {
        switch (lineNumber) {
        case 1:
            strncpy_P(mLigne1, (const char*)text, mNombreMaxChar);
            mLigne1[mNombreMaxChar] = 0;
            break;
        default:
        case 2:
            strncpy_P(mLigne2, (const char*)text, mNombreMaxChar);
            mLigne2[mNombreMaxChar] = 0;
            break;
            //        default:
            //            mLigne3 = text;
            //            break;
        }
        mNeedToRefresh = true;
    }
#endif
}

void IHM::displayLine(uint8_t lineNumber, const char *text)
{
#ifdef OLED
    //if (mOled != 0)
    {
        switch (lineNumber) {
        case 1:
            strncpy(mLigne1, text, mNombreMaxChar);
            mLigne1[mNombreMaxChar] = 0;
            break;
        default:
        case 2:
            strncpy(mLigne2, text, mNombreMaxChar);
            mLigne2[mNombreMaxChar] = 0;
            break;
            //        default:
            //            mLigne3 = text;
            //            break;
        }
        mNeedToRefresh = true;
    }
#endif
}
void IHM::init(const __FlashStringHelper* line1, const __FlashStringHelper* line2)
{
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
    initEncodeurSecondaire();
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
    action |= gererEncodeurSecondaire();

//    if (Configuration::instance()->entreeActive() != mDerniereEntreeActiveAffichee)
//    {
//        // On affiche l'entrée active
//        String entree = Configuration::instance()->entreeActiveToString();
//        mDerniereEntreeActiveAffichee = Configuration::instance()->entreeActive();
//        displayLine(2, entree);
//        mNeedToRefresh = true;
//    }
    switch(mModeAffichage)
    {
    default:
    case ModeSelectionEntree:
    {
        if (Configuration::instance()->entreeCourante() != mDerniereEntreeCouranteAffichee)
        {
            // On affiche l'entrée courante
            //String entree = Configuration::instance()->entreeCouranteToString();
                switch(Configuration::instance()->entreeCourante())
                {
                    case 1:
                    if (Configuration::instance()->salon())
                    {
                        displayLine(2, F("KODI"));
                    }
                    else
                    {
                        displayLine(2, F("Interne"));
                    }
                        break;
                    case 2:
                    if (Configuration::instance()->salon())
                    {
                        displayLine(2, F("TV"));
                    }
                    else
                    {
                        displayLine(2, F("SPDIF"));
                    }
                        break;
                    case 3:
                        displayLine(2, F("Aux 1"));
                        break;
                    case 4:
                        displayLine(2, F("Aux 2"));
                        break;
                   default:
                        displayLine(2, F("-------"));
                        break;
                }

            mDerniereEntreeCouranteAffichee = Configuration::instance()->entreeCourante();
            mNeedToRefresh = true;
        }
    }
        break;
    case ModeBalance:
    {
      displayBalance();
    }
        break;
    }
    if (mNombreDeSecondesAvantExtinction == 0)
    {
        if (mDerniereEntreeCouranteAffichee != Configuration::AucuneEntree)
        {
            displayLine(2, F(""));
            mDerniereEntreeCouranteAffichee = Configuration::AucuneEntree;
            // On repasse dans le mode nominal
            mModeAffichage = ModeSelectionEntree;
        }
    }
    if (mNeedToRefresh)
    {
#ifdef OLED
        mNeedToRefresh = refreshOled();
#endif
    }

    return action;
}

void IHM::remoteActive(bool active)
{
#ifdef OLED
    afficherSymbole(iconAntenna, mXSymboleFugitif, active);
#endif
}

void IHM::saved(bool active)
{
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


void IHM::displayStatus(const __FlashStringHelper * message)
{
    backlightOn();
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
    if (mEncodeurSecondaire != 0)
    {
        mValeurLueEncodeurSecondaire = mEncodeurSecondaire->read();
    }
}

void IHM::backlight(bool on)
{
#ifdef OLED
    //if (mOled != 0)
    {
        if (on)
        {
            mOled.setPowerSave(0);
            mNeedToRefresh = true;
        }
        else
        {
            mOled.setPowerSave(1);
            // On repasse en mode stanard
            mModeAffichage = ModeSelectionEntree;
        }
    }
#endif
    mBacklightOn = on;
    mNeedToRefresh = true;
}

void IHM::afficherSymbole(const uint8_t *symbole, uint8_t position, bool etat)
{
#ifdef OLED
    //if (mOled != 0)
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
        if (newPosition != mPositionEncodeurPrincipal)
        {
            bool plus = mPositionEncodeurPrincipal < newPosition;
            if (Configuration::instance()->encodeursInverses())
            {
                gererVolume(plus, action);
            }
            else
            {
                gererSelection(plus, action);
            }
            mPositionEncodeurPrincipal = newPosition;
#ifdef SERIAL_ON
            Serial.println(F("mouvement"));
#endif
        }
    }

    if (mBounce != 0)
    {
        if ((mDateDebutAppuiEncodeurPrincipal != 0) && (millis() - mDateDebutAppuiEncodeurPrincipal > mDureeAppuiLongEncodeurPrincipal))
        {
            // Appui long
#ifdef SERIAL_ON
            Serial.println(F("Appui long"));
#endif
            if (Configuration::instance()->encodeursInverses())
            {
                gererAppuiLongVolume(action);
            }
            else
            {
                gererAppuiLongSelection(action);
            }
            mDateDebutAppuiEncodeurPrincipal = 0;
        }
        else if (mBounce->update() == 1)
        {
            /// \todo gérer l'appui long pour la config
            if (mBounce->read() == 0)
            {
                // Falling edge
#ifdef SERIAL_ON
                Serial.println(F("Appui mBounce->read() == 0"));
#endif
                mDateDebutAppuiEncodeurPrincipal = millis();
                mNeedToRefresh = true;
            }
            else
            {
                // Raising edge
#ifdef SERIAL_ON
                Serial.println(F("Appui mBounce->read() != 0"));
#endif
                if (mDateDebutAppuiEncodeurPrincipal != 0)
                {
                    if (Configuration::instance()->encodeursInverses())
                    {
                        gererAppuiCourtVolume(action);
                    }
                    else
                    {
                        gererAppuiCourtSelection(action);
                    }
                    mDateDebutAppuiEncodeurPrincipal = 0;
                }
            }
        }
    }
    return action;
}

uint16_t IHM::gererEncodeurSecondaire()
{
    uint16_t action = ActionsPreampli::AucuneAction;
    if (mEncodeurSecondaire != 0)
    {
        long newPosition = mValeurLueEncodeurSecondaire / 4;
        if (newPosition != mPositionEncodeurSecondaire)
        {
            bool plus = mPositionEncodeurSecondaire < newPosition;
            if (Configuration::instance()->encodeursInverses())
            {
                gererSelection(plus, action);
            }
            else
            {
                gererVolume(plus, action);
            }
            mPositionEncodeurSecondaire = newPosition;
        }
    }
    if ((mDateDebutAppuiEncodeurSecondaire != 0) && (millis() - mDateDebutAppuiEncodeurSecondaire > mDureeAppuiLongEncodeurSecondaire))
    {
        // Appui long
        if (Configuration::instance()->encodeursInverses())
        {
            gererAppuiLongSelection(action);
        }
        else
        {
            gererAppuiLongVolume(action);
        }
    }
    else if (analogRead(encoderSecondaireButton) < seuilPresenceServitudes)
    {
        if (mDateDebutAppuiEncodeurSecondaire == 0)
        {
            if (Configuration::instance()->encodeursInverses())
            {
                gererAppuiCourtSelection(action);
            }
            else
            {
                gererAppuiCourtVolume(action);
            }
            mDateDebutAppuiEncodeurSecondaire = millis();
        }
    }
    else
    {
        if (mDateDebutAppuiEncodeurSecondaire != 0)
        {
            mDateDebutAppuiEncodeurSecondaire = 0;
        }
    }
    return action;
}

void IHM::gererVolume(bool plus, uint16_t &action)
{
#ifdef SERIAL_ON
            Serial.println(F("gererVolume"));
#endif
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
}

void IHM::gererAppuiCourtVolume(uint16_t &action)
{
  return;
#ifdef SERIAL_ON
    Serial.println(F("Appui volume"));
#endif
    action |= ActionsPreampli::ToggleMute;
}

void IHM::gererAppuiLongVolume(uint16_t &action)
{
  return;
#ifdef SERIAL_ON
    Serial.println(F("Appui long volume"));
#endif
    mDateDebutAppuiEncodeurSecondaire = 0;
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

void IHM::gererSelection(bool plus, uint16_t &action)
{
#ifdef SERIAL_ON
            Serial.println(F("gererSelection"));
#endif
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
    }
    mNeedToRefresh = true;
}

void IHM::gererAppuiCourtSelection(uint16_t &action)
{
#ifdef SERIAL_ON
    Serial.println(F("Appui court"));
#endif
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
}

void IHM::gererAppuiLongSelection(uint16_t &action)
{
            mNeedToRefresh = true;
}

#ifdef OLED
void IHM::initOled()
{
    for (int i = 0; i < mNombreSymboles; i++)
    {
        mSymboles[i] = iconVide;
    }
    mOled.begin();
    mOled.setPowerSave(0);
}

bool IHM::refreshOled(bool full)
{
    bool retour = !full;
    if (full)
    {
        mOled.firstPage();
        do {
            refreshPage();
        } while (mOled.nextPage());
        mOled.firstPage();
    }
    else
    {
        refreshPage();
        if (!mOled.nextPage())
        {
            retour = false;
            mOled.firstPage();
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
    mOled.setFont(u8g2_font_ncenB14_tr);
    mOled.setCursor(0, mHauteurSymbole);
    mOled.print(mLigne1);
    for (int i = mXPremierSymbole; i < mNombreSymboles; i++)
    {
        mOled.drawXBMP(i * mLargeurSymbole, 0, mLargeurSymbole, mHauteurSymbole, mSymboles[i]);
    }
    mOled.setFont(u8g2_font_ncenB18_tr);
    mOled.setCursor(0, mHauteurSymbole + mHauteurTexte);
    mOled.print(mLigne2);

    mOled.drawBox(0, 50, volume, 6);
    mOled.drawBox(0, 58, volume + balance, 6);
}

void IHM::displayBalance()
{
        int8_t volume;
        int8_t balance;
        Configuration::instance()->volumeChanged(volume, balance);
        sprintf(mLigne2, "%d / %d", volume, volume + balance);
        mNeedToRefresh = true;
}

#endif
