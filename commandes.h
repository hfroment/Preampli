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

    void activerTape(bool actif);

    void etats(bool& verouillage, bool& _96kHz);

    bool dacActive()
    {
        return mDacActive;
    }

    void mute(bool muted);
    void extension(bool on);
    //void corrected(bool corrected);

    uint16_t tensionLueEnMv();
    uint16_t tensionRefEnMv();

private:

    /// définition ports premier PCF8574
    enum
    {
        I2S_SPDIF = 0x01, // 0
        // 1
        // 2
        I2S_BLUETOOTH = 0x08, // 3
        I2S_USB = 0x10, // 4
        I2S_I2S = 0x20, // 5
        MASQUE_I2S_INPUTS = 0x3f,

        TAPE = 0x40, // 6
        EXTENSION = 0x80 // 7
    };
    /// définition ports second PCF8574
    enum
    {
        SPDIF_1 = 0x01,
        SPDIF_2 = 0x02,
        SPDIF_3 = 0x04,
        SPDIF_4 = 0x08,
        MASQUE_SPDIF = 0x0f,

        LECTURE_1 = 0x10,
        LECTURE_2 = 0x20,
        LECTURE_3 = 0x40,
        LECTURE_4 = 0x80,
        MASQUE_LECTURE = 0xf0,
    };
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

    bool mVolumeGauche;
    bool mVolumeDroite;
    bool mMoteurAZero;

    static const uint16_t mDureeActivationVolume = 400; // ms. Attention doit être supérieur à la durée de répétition dela télécommande
    unsigned long mDateDerniereCommandeVolume;

    /// Les adresses des PCF
    static const uint8_t mPcf1Addr = 0x20;
    static const uint8_t mPcf2Addr = 0x21;
    /// La valeur des bits des PCF
    uint8_t mPcf1Data;
    uint8_t mPcf2Data;

    void activerPCF();

    //static const uint8_t alimRelaisSpdif = 9;
    /// \todo voir s'il faut gérer l'alim du DAC

    /// Pour savoir si le DAC est actif
    bool mDacActive;

    void selectionnerEntreeAnalogique(uint8_t entree);
    void selectionnerEntreeSpdif(uint8_t entree);
    void selectionnerEntreeI2s(uint8_t entree);
};

#endif // COMMANDES_H
