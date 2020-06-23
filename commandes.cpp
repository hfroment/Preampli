#include <Wire.h>

#include "commandes.h"

Commandes::Commandes() :
    mVolumeGauche(0),
    mVolumeDroite(0)
#ifdef USE_MOTORIZED_POT
    , mMoteurAZero(false)
#endif
    , mDateDerniereCommandeVolume(0)
#ifdef USE_MOTORIZED_POT
    , mIndexTensionCourante(0)
#endif
#ifdef I2C_VOLUME
    , mCurrentVolumeLeft(0)
    , mCurrentVolumeRight(0)
#endif
{
}

void Commandes::gerer()
{
#ifdef USE_MOTORIZED_POT
    if (((mDateDerniereCommandeVolume != 0) && (millis() - mDateDerniereCommandeVolume > mDureeActivationVolume) && !mMoteurAZero) || moteurBloque())
    {
        moteurStop();
        mDateDerniereCommandeVolume = 0;
    }
#endif
}
void Commandes::init()
{
#ifdef USE_MOTORIZED_POT
    // Le moteur
    pinMode(moteurA, OUTPUT);
    pinMode(moteurB, OUTPUT);
#endif
#ifdef I2C_VOLUME
    Wire.begin();
#endif
    //
    pinMode(Entree1DcB1, OUTPUT);
    digitalWrite(Entree1DcB1, EntreeDcB1Off);
    pinMode(Entree2DcB1, OUTPUT);
    digitalWrite(Entree2DcB1, EntreeDcB1Off);
    pinMode(Entree3DcB1, OUTPUT);
    digitalWrite(Entree3DcB1, EntreeDcB1Off);
    pinMode(Entree4DcB1, OUTPUT);
    digitalWrite(Entree4DcB1, EntreeDcB1Off);
    //
    pinMode(Entree1Spdif, OUTPUT);
    digitalWrite(Entree1Spdif, EntreeSpdifOff);
    pinMode(Entree2Spdif, OUTPUT);
    digitalWrite(Entree2Spdif, EntreeSpdifOff);
    pinMode(Entree3Spdif, OUTPUT);
    digitalWrite(Entree3Spdif, EntreeSpdifOff);
    pinMode(Entree4Spdif, OUTPUT);
    digitalWrite(Entree4Spdif, EntreeSpdifOff);
    //
    pinMode(Mute, OUTPUT);
    digitalWrite(Mute, LOW);
    mute(true);

    Serial.begin(115200);
}

#ifdef USE_MOTORIZED_POT
void Commandes::moteurGauche()
{
    if (!mVolumeGauche)
    {
        digitalWrite(moteurA, 1);
        digitalWrite(moteurB, 0);
        mVolumeGauche = true;
        mVolumeDroite = false;
    }
    mDateDerniereCommandeVolume = millis();
}

void Commandes::moteurDroite()
{
    if (!mVolumeDroite)
    {
        digitalWrite(moteurA, 0);
        digitalWrite(moteurB, 1);
        mVolumeDroite = true;
        mVolumeGauche = false;
    }
    mDateDerniereCommandeVolume = millis();
}

void Commandes::moteurStop(bool force)
{
    if (mVolumeDroite || mVolumeGauche || force)
    {
        digitalWrite(moteurA, 0);
        digitalWrite(moteurB, 0);
        mVolumeDroite = false;
        mVolumeGauche = false;
        mMoteurAZero = false;
    }
}

bool Commandes::moteurBloque()
{
    bool retour = false;

    // on est bloqué si on va à gauche et si le fin de course est activé
    if (mVolumeGauche)
    {
        //Serial.println(tensionLueEnLSB());
        retour = (tensionLueEnLSB() > 512);
    }
    //return (analogRead(mesureTension) < seuilTensionBlocage);
    // Consommation moteur non conforme à la spec : ne fonctionne pas
    return retour;
}
#endif

void Commandes::volumeInit(uint8_t initValue)
{
#ifdef USE_MOTORIZED_POT
    unsigned long dateDebut = millis();
    volumeMoins();
    while ((millis() - dateDebut < 12000) && !moteurBloque());
    volumePlus();
    delay(initValue * 1000);
    volumeStop();
#endif
#ifdef I2C_VOLUME
    setI2cVolumeLeft(initValue);
    setI2cVolumeRight(initValue);
#endif
}

void Commandes::selectionnerEntree(uint8_t entree)
{
    switch (entree)
    {
    case Configuration::AucuneEntree:
    default:
        // On reste en l'état
        break;
    case Configuration::EntreeAnalogique_1:
        selectionnerEntreeSpdif(0);
        selectionnerEntreeAnalogique(1);
        mDacActive = false;
        break;
    case Configuration::EntreeAnalogique_2:
        selectionnerEntreeSpdif(0);
        selectionnerEntreeAnalogique(3);
        mDacActive = false;
        break;
    case Configuration::EntreeAnalogique_3:
        selectionnerEntreeSpdif(0);
        selectionnerEntreeAnalogique(4);
        mDacActive = false;
        break;
    case Configuration::EntreeSpdif_1:
        selectionnerEntreeSpdif(1);
        selectionnerEntreeAnalogique(2);
        mDacActive = true;
        break;
    case Configuration::EntreeSpdif_2:
        selectionnerEntreeSpdif(2);
        selectionnerEntreeAnalogique(2);
        mDacActive = true;
        break;
    case Configuration::EntreeSpdif_3:
        selectionnerEntreeSpdif(3);
        selectionnerEntreeAnalogique(2);
        mDacActive = true;
        break;
    case Configuration::EntreeSpdif_4:
        selectionnerEntreeSpdif(4);
        selectionnerEntreeAnalogique(2);
        mDacActive = true;
        break;
    case Configuration::EntreeToslink:
        selectionnerEntreeSpdif(0);
        selectionnerEntreeAnalogique(2);
        mDacActive = true;
        break;
    case Configuration::EntreeUsb:
        selectionnerEntreeSpdif(0);
        selectionnerEntreeAnalogique(2);
        mDacActive = true;
        break;
    }
}

void Commandes::selectionnerEntreeAnalogique(uint8_t entree)
{
    switch (entree)
    {
    case 0:
        // aucune entrée
        digitalWrite(Entree1DcB1, EntreeDcB1Off);
        digitalWrite(Entree2DcB1, EntreeDcB1Off);
        digitalWrite(Entree3DcB1, EntreeDcB1Off);
        digitalWrite(Entree4DcB1, EntreeDcB1Off);
        break;
    case 1:
        digitalWrite(Entree2DcB1, EntreeDcB1Off);
        digitalWrite(Entree3DcB1, EntreeDcB1Off);
        digitalWrite(Entree4DcB1, EntreeDcB1Off);
        digitalWrite(Entree1DcB1, EntreeDcB1On);
        break;
    case 2:
        digitalWrite(Entree1DcB1, EntreeDcB1Off);
        digitalWrite(Entree3DcB1, EntreeDcB1Off);
        digitalWrite(Entree4DcB1, EntreeDcB1Off);
        digitalWrite(Entree2DcB1, EntreeDcB1On);
        break;
    case 3:
        digitalWrite(Entree1DcB1, EntreeDcB1Off);
        digitalWrite(Entree2DcB1, EntreeDcB1Off);
        digitalWrite(Entree4DcB1, EntreeDcB1Off);
        digitalWrite(Entree3DcB1, EntreeDcB1On);
        break;
    case 4:
        digitalWrite(Entree1DcB1, EntreeDcB1Off);
        digitalWrite(Entree2DcB1, EntreeDcB1Off);
        digitalWrite(Entree3DcB1, EntreeDcB1Off);
        digitalWrite(Entree4DcB1, EntreeDcB1On);
        break;
    default:
        // Pas de changement
        break;
    }
}

void Commandes::selectionnerEntreeSpdif(uint8_t entree)
{
    switch (entree)
    {
    case 0:
        // aucune entrée
        digitalWrite(Entree1Spdif, EntreeSpdifOff);
        digitalWrite(Entree2Spdif, EntreeSpdifOff);
        digitalWrite(Entree3Spdif, EntreeSpdifOff);
        digitalWrite(Entree4Spdif, EntreeSpdifOff);
        break;
    case 1:
        // aucune entrée
        digitalWrite(Entree2Spdif, EntreeSpdifOff);
        digitalWrite(Entree3Spdif, EntreeSpdifOff);
        digitalWrite(Entree4Spdif, EntreeSpdifOff);
        digitalWrite(Entree1Spdif, EntreeSpdifOn);
        break;
    case 2:
        // aucune entrée
        digitalWrite(Entree1Spdif, EntreeSpdifOff);
        digitalWrite(Entree3Spdif, EntreeSpdifOff);
        digitalWrite(Entree4Spdif, EntreeSpdifOff);
        digitalWrite(Entree2Spdif, EntreeSpdifOn);
        break;
    case 3:
        // aucune entrée
        digitalWrite(Entree1Spdif, EntreeSpdifOff);
        digitalWrite(Entree2Spdif, EntreeSpdifOff);
        digitalWrite(Entree4Spdif, EntreeSpdifOff);
        digitalWrite(Entree3Spdif, EntreeSpdifOn);
        break;
    case 4:
        // aucune entrée
        digitalWrite(Entree1Spdif, EntreeSpdifOff);
        digitalWrite(Entree2Spdif, EntreeSpdifOff);
        digitalWrite(Entree3Spdif, EntreeSpdifOff);
        digitalWrite(Entree4Spdif, EntreeSpdifOn);
        break;
    default:
        // Pas de changement
        break;
    }
}

void Commandes::mute(bool muted)
{
    if (!muted)
    {
        digitalWrite(Mute, HIGH);
    }
    else
    {
        digitalWrite(Mute, LOW);
    }
    mMuted = muted;
}

bool Commandes::volumeChanged(uint8_t &left, uint8_t &right)
{
#ifdef I2C_VOLUME
    left = mCurrentVolumeLeft;
    right = mCurrentVolumeRight;

    return true;
#else
    return false;
#endif
}

#ifdef I2C_VOLUME
void Commandes::setI2cVolumeLeft(uint8_t volume)
{
    if (volume != mCurrentVolumeLeft)
    {
        Wire.beginTransmission(mPcf8574LeftAddress);
        Wire.write(volume);
        Wire.endTransmission();
    }
}

void Commandes::setI2cVolumeRight(uint8_t volume)
{
    if (volume != mCurrentVolumeRight)
    {
        Wire.beginTransmission(mPcf8574RightAddress);
        Wire.write(volume);
        Wire.endTransmission();
    }
}
#endif

#ifdef USE_MOTORIZED_POT
uint16_t Commandes::tensionLueEnLSB()
{
    uint16_t tension = (uint16_t)analogRead(mesureTension);
    mTensionsLues[mIndexTensionCourante++] = tension;
    mIndexTensionCourante %= mNombreDeTensions;
    return tension;
}

uint16_t Commandes::tensionMoyenneEnMv()
{
    return (uint32_t)tensionMoyenneEnLSB() * 5000L / 1024L;
}
uint16_t Commandes::tensionMoyenneEnLSB()
{
    uint16_t somme = 0;
    for (int var = 0; var < mNombreDeTensions; ++var)
    {
        somme += mTensionsLues[var];
    }
    return somme / mNombreDeTensions;
}
uint16_t Commandes::tensionRefEnMv()
{
   return (uint16_t)((uint32_t)seuilTensionBlocage * 5000L / 1024L);
}
#endif
