#ifndef COMMANDES_H
#define COMMANDES_H

#include <Arduino.h>

#include "configuration.h"

class Commandes
{
public:
    Commandes();
    void gerer();
    void init();
    void volumePlus()
    {
        moteurDroite();
    }
    void volumeMoins()
    {
        moteurGauche();
    }
    void volumeStop()
    {
        moteurStop();
    }
    void selectionnerEntree(uint8_t entree);
//    Configuration::teEntreesAudio entreeSelectionnee();

    bool dacActive()
    {
        return mDacActive;
    }

    void mute(bool muted);

    uint16_t tensionLueEnMv();
    uint16_t tensionRefEnMv();

private:
    /// les pins du moteur
    static const uint8_t moteurA = 13;
    static const uint8_t moteurB = 12;
    static const uint8_t mesureTension = A7;
    static const uint32_t seuilTensionBlocage = 4450L * 1024L / 5000L;
    void moteurGauche();
    void moteurDroite();
    void moteurStop(bool force = false);
    bool moteurBloque();
    void volumeInit(uint8_t dureeEnSecondes);

    /// Les autres E/S
    static const uint8_t Entree1DcB1 = 5;
    static const uint8_t Entree2DcB1 = A0;
    static const uint8_t Entree3DcB1 = 7;
    static const uint8_t Entree4DcB1 = A3;
    static const uint8_t Mute = A1;

    static const uint8_t Entree1Spdif = 9;
    static const uint8_t Entree2Spdif = 10;
    static const uint8_t Entree3Spdif = 6;
    static const uint8_t EntreeSpdif = A2;

    bool mVolumeGauche;
    bool mVolumeDroite;
    bool mMoteurAZero;

    static const uint16_t mDureeActivationVolume = 400; // ms. Attention doit être supérieur à la durée de répétition dela télécommande
    unsigned long mDateDerniereCommandeVolume;

    /// Pour savoir si le DAC est actif
    bool mDacActive;

    void selectionnerEntreeAnalogique(uint8_t entree);
    void selectionnerEntreeSpdif(uint8_t entree);
};

#endif // COMMANDES_H
