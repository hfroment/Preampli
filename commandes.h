#ifndef COMMANDES_H
#define COMMANDES_H

#include <Arduino.h>

#include "options.h"

#include "configuration.h"
#include "actions.h"

class Commandes
{
public:
    Commandes();
    void gerer();
    void init();
    void volumeInit(uint8_t initValue = 1);
    void volumePlus();
    void volumeMoins();
#ifdef USE_MOTORIZED_POT
    void volumeStop()
    {
        moteurStop(true);
    }
#endif
    void selectionnerEntree(uint8_t entree);
    void changeVolume(uint8_t left, uint8_t right);
//    Configuration::teEntreesAudio entreeSelectionnee();

    bool servitudesPresentes();
    void envoyerCommandeServitude(ActionsServitudes::teCibleActionServitudes cible, ActionsServitudes::teTypeActionServitudes type);
    void lireStatutServitudes();

#ifdef USE_MOTORIZED_POT
    bool motorOn()
    {
        return (mVolumeDroite || mVolumeGauche);
    }
#endif
    bool dacActive()
    {
        return mDacActive;
    }

    void mute(bool muted);

    bool muted()
    {
        return mMuted;
    }
    bool volumeChanged(uint8_t& left, uint8_t& right);

#ifdef USE_MOTORIZED_POT
    uint16_t tensionLueEnLSB();
    uint16_t tensionMoyenneEnLSB();
    uint16_t tensionMoyenneEnMv();
    uint16_t tensionRefEnMv();
#endif

private:
    enum
    {
        EntreeDcB1On = HIGH,
        EntreeDcB1Off = LOW,
        EntreeSpdifOn = LOW,
        EntreeSpdifOff = HIGH
    };
#ifdef USE_MOTORIZED_POT
    /// les pins du moteur
    static const uint8_t moteurA = 13;
    static const uint8_t moteurB = 12;
    static const uint8_t mesureTension = A7;
    static const uint32_t seuilTensionBlocage = 4450L * 1024L / 5000L;
    void moteurGauche();
    void moteurDroite();
    void moteurStop(bool force = false);
    bool moteurBloque();
#endif
#ifdef I2C_VOLUME
    void setI2cVolumeLeft(uint8_t volume);
    void setI2cVolumeRight(uint8_t volume);
    void sendI2cVolume(uint8_t volume);
#endif
    /// Les autres E/S
    static const uint8_t Entree1DcB1 = 5;
    static const uint8_t Entree2DcB1 = A0;
    static const uint8_t Entree3DcB1 = A3;
    static const uint8_t Entree4DcB1 = 7;
    static const uint8_t Mute = A1;

    static const uint8_t Entree1Spdif = 9;
    static const uint8_t Entree2Spdif = 10;
    static const uint8_t Entree3Spdif = 6;
    static const uint8_t Entree4Spdif = A2;

    static const uint8_t PresenceServitudes = A6;

    bool mVolumeGauche;
    bool mVolumeDroite;
#ifdef USE_MOTORIZED_POT
    bool mMoteurAZero;
    static const uint16_t mDureeActivationVolume = 400; // ms. Attention doit être supérieur à la durée de répétition dela télécommande
    unsigned long mDateDerniereCommandeVolume;
#endif

    /// Pour savoir si le DAC est actif
    bool mDacActive;
    /// Pour savoir si on est en mute
    bool mMuted;

    void selectionnerEntreeAnalogique(uint8_t entree);
    void selectionnerEntreeSpdif(uint8_t entree);

#ifdef USE_MOTORIZED_POT
    static const uint8_t mNombreDeTensions = 20;
    uint16_t mTensionsLues[mNombreDeTensions];
    uint8_t mIndexTensionCourante;
#endif

    /// La tension indiquant la présences des servitudes (moitié de la tension d'alimentation)
    static const uint16_t seuilPresenceServitudes = 1024 / 2;

#ifdef I2C_VOLUME
    uint8_t mCurrentVolumeLeft;
    uint8_t mCurrentVolumeRight;
    static const uint8_t mMaxVolume = 0x7f;
    static const uint8_t mPcf8574LeftAddress = 0x20;
    static const uint8_t mPcf8574RightAddress = 0x21;
#endif

    bool mVolumeChanged;
};

#endif // COMMANDES_H
