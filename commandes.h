#ifndef COMMANDES_H
#define COMMANDES_H

#include <Arduino.h>

#include "options.h"

#include "configuration.h"

class Commandes
{
public:
    Commandes();
    void gerer();
    void init();
    void volumeInit(uint8_t initValue = 1);
    void volumePlus()
    {
#ifdef USE_MOTORIZED_POT
        moteurDroite();
#endif
#ifdef I2C_VOLUME
        if (mCurrentVolumeLeft < mMaxVolume)
        {
            mCurrentVolumeLeft++;
            setI2cVolumeLeft(mCurrentVolumeLeft);
        }
        if (mCurrentVolumeRight < mMaxVolume)
        {
            mCurrentVolumeRight++;
            setI2cVolumeRight(mCurrentVolumeRight);
        }
#endif
    }
    void volumeMoins()
    {
#ifdef USE_MOTORIZED_POT
        moteurGauche();
#endif
#ifdef I2C_VOLUME
        if (mCurrentVolumeLeft > 0)
        {
            mCurrentVolumeLeft--;
            setI2cVolumeLeft(mCurrentVolumeLeft);
        }
        if (mCurrentVolumeRight > 0)
        {
            mCurrentVolumeRight--;
            setI2cVolumeRight(mCurrentVolumeRight);
        }
#endif
    }
#ifdef USE_MOTORIZED_POT
    void volumeStop()
    {
        moteurStop(true);
    }
#endif
    void selectionnerEntree(uint8_t entree);
//    Configuration::teEntreesAudio entreeSelectionnee();

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

    bool mVolumeGauche;
    bool mVolumeDroite;
#ifdef USE_MOTORIZED_POT
    bool mMoteurAZero;
#endif
    static const uint16_t mDureeActivationVolume = 400; // ms. Attention doit être supérieur à la durée de répétition dela télécommande
    unsigned long mDateDerniereCommandeVolume;

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

#ifdef I2C_VOLUME
    uint8_t mCurrentVolumeLeft;
    uint8_t mCurrentVolumeRight;
    static const uint8_t mMaxVolume = 0x7f;
    static const uint8_t mPcf8574LeftAddress = 0x20;
    static const uint8_t mPcf8574RightAddress = 0x21;
#endif
};

#endif // COMMANDES_H
