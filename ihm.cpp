#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Bounce2.h>

#include "commandes.h"

#include "ihm.h"

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

static const uint8_t customCadenasFerme[8] PROGMEM =
{
    14,17,17,31,27,27,31,0
};
static const uint8_t customCadenasOuvert[8] PROGMEM =
{
    14,16,16,31,27,27,31,0
};
static const uint8_t customErreur[8] PROGMEM =
{
    4,14,14,14,31,4,0
};
static const uint8_t custom96k[8] PROGMEM =
{
    0,15,3,5,9,16,0
};
static const uint8_t customTape[8] PROGMEM =
{
    0x2,0x3,0x2,0x2,0xe,0x1e,0xc,0x0
};


IHM::IHM() :
    mLcd(0),
    mLcdNbCols(0),
    mLcdNbLines(0),
    mNombreDeSecondesAvantExtinction(nombreDeSecondesAvantExtinction),
    mBacklightOn(false),
    mEncodeur(0),
    mPositionEncodeur(0),
    mBounce(0),
    mDerniereEntreeCouranteAffichee(Configuration::AucuneEntree),
    mDerniereEntreeActiveAffichee(Configuration::AucuneEntree),
    mDernierEtatTape(false),
    mDateDebutAppui(0),
    mModeAffichage(ModeSelectionEntree),
    mEntreeCouranteMenuActionSecondaires(0)
{
}

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
        mLcd->createCharFromFlash(SymboleCadenasFerme, customCadenasFerme);
        mLcd->createCharFromFlash(SymboleCadenasOuvert, customCadenasOuvert);
        mLcd->createCharFromFlash(SymboleErreur, customErreur);
        mLcd->createCharFromFlash(Symbole96k, custom96k);
        mLcd->createCharFromFlash(SymboleTape, customTape);
        mLcdNbCols = nbCols;
        mLcdNbLines = nbLignes;
    }
}
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
}
void IHM::init(String line1, String line2)
{
    initLcd();
    displayLine(0, line1);
    displayLine(1, line2);
    mDernierEtatTape = Configuration::instance()->tapeActive();
    afficherTape(mDernierEtatTape);
    uint8_t i = 0;
    mMenuActionsSecondaires[i].libelle = "Mute";
    mMenuActionsSecondaires[i++].action = Actions::ToggleMute;
    mMenuActionsSecondaires[i].libelle = "Tape";
    mMenuActionsSecondaires[i++].action = Actions::ToggleTape;
    //    mMenuActionsSecondaires[i].libelle = "Correction";
    //    mMenuActionsSecondaires[i++].action = Actions::ToggleCorrected;
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
                displayLine(1, mMenuActionsSecondaires[mEntreeCouranteMenuActionSecondaires].libelle);
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
        displayLine(0, entree);
    }
    if (Configuration::instance()->tapeActive() !=  mDernierEtatTape)
    {
        mDernierEtatTape = Configuration::instance()->tapeActive();
        afficherTape(mDernierEtatTape);
    }
    if (mNombreDeSecondesAvantExtinction > 0)
    {
        if (Configuration::instance()->entreeCourante() != mDerniereEntreeCouranteAffichee)
        {
            // On affiche l'entrée courante
            String entree = Configuration::instance()->entreeCouranteToString();
            displayLine(1, entree);
            mDerniereEntreeCouranteAffichee = Configuration::instance()->entreeCourante();
        }
    }
    else
    {
        if (mDerniereEntreeCouranteAffichee != Configuration::AucuneEntree)
        {
            displayLine(1, "");
            mDerniereEntreeCouranteAffichee = Configuration::AucuneEntree;
            // On repasse dans le mode nominal
            mModeAffichage = ModeSelectionEntree;
        }
    }

    return action;
}

void IHM::remoteActive(bool active)
{
    afficherSymbole(SymboleAntenne, xTelecommande, active);
}

void IHM::saved(bool active)
{
    afficherSymbole(xBuzy, SymboleMontre, active);
}

void IHM::displayStatus(String message)
{
    backlightOn();
    displayLine(mLcdNbLines - 1, message);
}
void IHM::backlightOn()
{
    backlight(true);
    mNombreDeSecondesAvantExtinction = nombreDeSecondesAvantExtinction;
}

void IHM::backlight(bool on)
{
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
    mBacklightOn = on;
}

void IHM::afficherSymbole(uint8_t symbole, uint8_t position, bool etat)
{
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
}

void IHM::effacerSymbole(uint8_t position)
{
    if (mLcd != 0)
    {
        mLcd->setCursor(position, 0);
        mLcd->write(' ');
    }
}

//void IHM::afficherErreur(bool etat)
//{
//    if (mLcd != 0)
//    {
//        mLcd->setCursor(xErreur, 0);
//        if (etat)
//        {
//            mLcd->write(SymboleErreur);
//        }
//        else
//        {
//            mLcd->write(' ');
//        }
//    }
//}

void IHM::afficherVerouillage(bool etat, bool afficher)
{
    if (afficher)
    {
        if (etat)
        {
            afficherSymbole(SymboleCadenasFerme, xVerouillage, true);
        }
        else
        {
            afficherSymbole(SymboleErreur, xVerouillage, true);
        }
    }
    else
    {
        effacerSymbole(xVerouillage);
    }

}

void IHM::afficher96Khz(bool etat, bool afficher)
{
    if (afficher)
    {
        afficherSymbole(Symbole96k, x96kHz, etat);
    }
    else
    {
        effacerSymbole(x96kHz);
    }
}

//void IHM::afficherEtatX(bool etat)
//{

//}

void IHM::afficherTape(bool etat)
{
    afficherSymbole(SymboleTape, xTape, etat);
}

