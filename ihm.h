#ifndef IHM_H
#define IHM_H

#include "actions.h"

class Encoder;
class Bounce;
class LiquidCrystal_I2C;

class IHM
{
public:
    IHM();
    void init(String line1, String line2);
    uint16_t gerer(bool topSeconde);
    void remoteActive(bool active);
    void saved(bool active);
    void displayStatus(String message);
    void backlightOn();
//    void afficherErreur(bool etat);
    void afficherVerouillage(bool etat, bool afficher);
    void afficher96Khz(bool etat, bool afficher);
//    void afficherEtatX(bool etat);
    void afficherTape(bool etat);

private:
    LiquidCrystal_I2C* mLcd;
    static const uint8_t defaultLcdAddress = 0x27;
    static const uint8_t defaultLcdNbCols = 16;//20;
    static const uint8_t defaultLcdNbLines = 2;//4;
    static const uint8_t xTelecommande = defaultLcdNbCols - 1;
    static const uint8_t xBuzy = xTelecommande;
//    static const uint8_t xErreur = defaultLcdNbCols - 2;
    static const uint8_t x96kHz = xBuzy - 1;
    static const uint8_t xVerouillage = x96kHz - 1;
    static const uint8_t xTape = xVerouillage - 1;
    static const uint8_t xDernierSymbole = xTape;
    uint8_t mLcdNbCols;
    uint8_t mLcdNbLines;

    static const uint8_t encoderA = 3;
    static const uint8_t encoderB = 2;
    static const uint8_t encoderButton = 4;

    static const uint8_t nombreDeSecondesAvantExtinction = 30;
    uint8_t mNombreDeSecondesAvantExtinction;

    bool mBacklightOn;

    Encoder* mEncodeur;
    Bounce* mBounce;

    // LCD
    void initLcd(uint8_t adresse = defaultLcdAddress, uint8_t nbCols = defaultLcdNbCols, uint8_t nbLignes = defaultLcdNbLines);
    // Encodeur
    void initEncodeur(uint8_t pinA = encoderA, uint8_t pinB = encoderB, uint8_t buttonPin = encoderButton);

    long mPositionEncodeur;

    uint8_t mDerniereEntreeCouranteAffichee;
    uint8_t mDerniereEntreeActiveAffichee;
    bool mDernierEtatTape;

    static const uint16_t mDureeAppuiLong = 2000; // ms.
    unsigned long mDateDebutAppui;

    typedef enum
    {
        ModeSelectionEntree,
        ModeActionsSecondaires,
        ModeVolume
    }
    teModeAffichage;

    teModeAffichage mModeAffichage;

    enum
    {
        SymboleAntenne = 0,
        SymboleMontre = 1,
        SymboleCadenasFerme = 2,
        SymboleCadenasOuvert = 3,
        SymboleErreur = 4,
        Symbole96k = 5,
        SymboleTape = 6
    };

    class MenuItem
    {
    public:
        String libelle;
        uint16_t action;
    };
    static const uint8_t mNombreEntreeMenuActionsSecondaires = 4; // Mute, tape, corrigé, retour
    MenuItem mMenuActionsSecondaires[mNombreEntreeMenuActionsSecondaires];
    uint8_t mEntreeCouranteMenuActionSecondaires;

    void displayLine(uint8_t lineNumber, String text);
    void backlight(bool on);
    void afficherSymbole(uint8_t symbole, uint8_t position, bool etat);
    void effacerSymbole(uint8_t position);

};


#endif // IHM_H
