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
    void volumeInit(int8_t initValue = 1);
    void volumePlus();
    void volumeMoins();
    void balanceDroite();
    void balanceGauche();
    void selectionnerEntree(uint8_t entree);
    void changeVolume(int8_t volume, int8_t balance);

    bool servitudesPresentes();
    void envoyerCommandeServitude(ActionsServitudes::teCibleActionServitudes cible, ActionsServitudes::teTypeActionServitudes type);
    void lireStatutServitudes();

    void mute(bool muted);

    bool muted()
    {
        return mMuted;
    }
    bool volumeChanged(int8_t &volume, int8_t &balance);

private:
    enum
    {
        EntreeDcB1On = HIGH,
        EntreeDcB1Off = LOW,
    };

    /// Les autres E/S
    static const uint8_t Entree1DcB1 = 5;
    static const uint8_t Entree2DcB1 = A0;
    static const uint8_t Entree3DcB1 = A3;
    static const uint8_t Entree4DcB1 = 7;
    static const uint8_t Mute = A1;

    static const uint8_t PresenceServitudes = A6;

    /// Pour savoir si on est en mute
    bool mMuted;

    void selectionnerEntreeAnalogique(uint8_t entree);

    /// La tension indiquant la présences des servitudes (moitié de la tension d'alimentation)
    static const uint16_t seuilPresenceServitudes = 1024 / 2;

#ifdef I2C_VOLUME
    int8_t mCurrentVolume;
    int8_t mCurrentBalance;
    static const int8_t mMaxVolume = 0x7f;
    static const uint8_t mPcf8574LeftAddress = 0x20;
    static const uint8_t mPcf8574RightAddress = 0x21;

    void setI2cVolumeLeft(int8_t volume);
    void setI2cVolumeRight(int8_t volume);
    void sendI2cVolume(uint8_t adresse, int8_t volume);
#endif

    void muteOn();
    void muteOff();

    bool mVolumeChanged;
};

#endif // COMMANDES_H
