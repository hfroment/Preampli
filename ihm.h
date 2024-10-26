#ifndef IHM_H
#define IHM_H

#include "options.h"

#include "actions.h"

class Encoder;
class Bounce;
#ifdef OLED
class U8G2_SSD1306_128X64_NONAME_1_HW_I2C;
#endif

class IHM
{
public:
    IHM();
    void init(const __FlashStringHelper *line1, const __FlashStringHelper *line2);
    uint16_t gerer(bool topSeconde);
    void remoteActive(bool active);
    void saved(bool active);
    void muted(bool active);
    void linked(bool active);
    void displayStatus(const __FlashStringHelper  *message);
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
    static const uint8_t mNombreMaxChar = 8;
    char mLigne1[mNombreMaxChar + 1];
    char mLigne2[mNombreMaxChar + 1];
    uint8_t* mSymboles[mNombreSymboles];
    bool refreshOled(bool full = false);
    void refreshPage();
#endif
    bool mNeedToRefresh;

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

    // Encodeur
    void initEncodeurPrincipal(uint8_t pinA = encoderPrincipalA, uint8_t pinB = encoderPrincipalB, uint8_t buttonPin = encoderPrincipalButton);
    void initEncodeurSecondaire(uint8_t pinA = encoderSecondaireA, uint8_t pinB = encoderSecondaireB);
    long mPositionEncodeurPrincipal;
    long mPositionEncodeurSecondaire;
    long mValeurLueEncodeurSecondaire;
    bool mEtatPrecedentEncodeurSecondaire;

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

//    class MenuItem
//    {
//    public:
//        const char* libelle;
//        uint16_t action;
//    };
//    static const uint8_t mNombreEntreeMenuActionsSecondaires = 2; // Mute, retour
//    MenuItem mMenuActionsSecondaires[mNombreEntreeMenuActionsSecondaires];
//    uint8_t mEntreeCouranteMenuActionSecondaires;

    void displayLine(uint8_t lineNumber, const __FlashStringHelper *text);
    void displayLine(uint8_t lineNumber, const char *text);
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

    void displayBalance();
    void displayEntree(uint8_t ligne, uint8_t entree);
};




#endif // IHM_H
