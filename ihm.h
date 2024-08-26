#ifndef IHM_H
#define IHM_H

#include "options.h"

#include "actions.h"

class Encoder;
class Bounce;
#ifdef LCD
class LiquidCrystal_I2C;
#endif
#ifdef OLED
class U8G2_SSD1306_128X64_NONAME_1_HW_I2C;
class U8G2_SSD1306_128X64_NONAME_F_HW_I2C;
#endif

class IHM
{
public:
    IHM();
    void init(String line1, String line2);
    uint16_t gerer(bool topSeconde);
    void remoteActive(bool active);
    void saved(bool active);
    void muted(bool active);
    void linked(bool active);
#ifdef USE_MOTORIZED_POT
    void motorOn(bool active);
#endif
    void displayStatus(String message);
    void backlightOn();
    void refresh();
#ifdef OLED
    static const uint8_t mLargeurSymbole = 16;
    static const uint8_t mHauteurSymbole = 16;
    static const uint8_t mHauteurTexte = 24;
#endif
    void gererIt();

private:
#ifdef OLED
    U8G2_SSD1306_128X64_NONAME_1_HW_I2C* mOled;
    void initOled();
    static const uint8_t mNombreSymboles = 8;
    static const uint8_t mXSymboleFugitif = mNombreSymboles - 1;
    static const uint8_t mXSymboleMute = mXSymboleFugitif - 1;
    static const uint8_t mXSymboleLink = mXSymboleMute - 1;
    static const uint8_t mXPremierSymbole = mXSymboleLink;
    String mLigne1;
    String mLigne2;
    uint8_t* mSymboles[mNombreSymboles];
    bool refreshOled(bool full = false);
    void refreshPage();
#endif
    bool mNeedToRefresh;
#ifdef LCD
    LiquidCrystal_I2C* mLcd;
    static const uint8_t defaultLcdAddress = 0x27;
    static const uint8_t defaultLcdNbCols = 16;//20;
    static const uint8_t defaultLcdNbLines = 2;//4;
    static const uint8_t xTelecommande = defaultLcdNbCols - 1;
    static const uint8_t xBuzy = xTelecommande;
    uint8_t mLcdNbCols;
    uint8_t mLcdNbLines;
#endif

    static const uint8_t encoderPrincipalA = 3;
    static const uint8_t encoderPrincipalB = 2;
    static const uint8_t encoderPrincipalButton = 4;
    static const uint8_t encoderSecondaireA = 8;
    static const uint8_t encoderSecondaireB = 12;
    static const uint8_t encoderSecondaireButton = A7;
    static const uint16_t mDureeAppuiLongEncodeurSecondaire = 2000; // ms.
    unsigned long mDateDebutAppuiEncodeurSecondaire;
    /// La tension indiquant l'a présences des servitudes l'appui (moitié de la tension d'alimentation)
    static const uint16_t seuilPresenceServitudes = 1024 / 2;
    static const uint8_t nombreDeSecondesAvantExtinction = 30;
    uint8_t mNombreDeSecondesAvantExtinction;

    bool mBacklightOn;

    Encoder* mEncodeurPrincipal;
    Encoder* mEncodeurSecondaire;
    Bounce* mBounce;

#ifdef LCD
    // LCD
    void initLcd(uint8_t adresse = defaultLcdAddress, uint8_t nbCols = defaultLcdNbCols, uint8_t nbLignes = defaultLcdNbLines);
#endif
    // Encodeur
    void initEncodeurPrincipal(uint8_t pinA = encoderPrincipalA, uint8_t pinB = encoderPrincipalB, uint8_t buttonPin = encoderPrincipalButton);
    void initEncodeurSecondaire(uint8_t pinA = encoderSecondaireA, uint8_t pinB = encoderSecondaireB);
    long mPositionEncodeurPrincipal;
    long mPositionEncodeurSecondaire;
    long mValeurLueEncodeurSecondaire;

    uint8_t mDerniereEntreeCouranteAffichee;
    uint8_t mDerniereEntreeActiveAffichee;

    static const uint16_t mDureeAppuiLongEncodeurPrincipal = 2000; // ms.
    unsigned long mDateDebutAppuiEncodeurPrincipal;

    typedef enum
    {
        ModeSelectionEntree,
        ModeBalance,
    }
    teModeAffichage;

    teModeAffichage mModeAffichage;

    enum
    {
        SymboleAntenne = 0,
        SymboleMontre = 1,
    };

    class MenuItem
    {
    public:
        String libelle;
        uint16_t action;
    };
//    static const uint8_t mNombreEntreeMenuActionsSecondaires = 2; // Mute, retour
//    MenuItem mMenuActionsSecondaires[mNombreEntreeMenuActionsSecondaires];
//    uint8_t mEntreeCouranteMenuActionSecondaires;

    void displayLine(uint8_t lineNumber, String text);
    void backlight(bool on);
#ifndef OLED
    void afficherSymbole(uint8_t symbole, uint8_t position, bool etat);
#else
    void afficherSymbole(const uint8_t* symbole, uint8_t position, bool etat);
#endif
    void effacerSymbole(uint8_t position);

    uint16_t gererEncodeurPrincipal();
    uint16_t gererEncodeurSecondaire();
    void gererVolume(bool plus, uint16_t& action);
    void gererAppuiCourtVolume(uint16_t& action);
    void gererAppuiLongVolume(uint16_t& action);
    void gererSelection(bool plus, uint16_t& action);
    void gererAppuiCourtSelection(uint16_t& action);
    void gererAppuiLongSelection(uint16_t& action);

};




#endif // IHM_H
