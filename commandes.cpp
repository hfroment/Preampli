#include <Wire.h>

#include "commandes.h"

static const uint8_t indirectionVolume[128] PROGMEM = {
    0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120,
    4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124,
    2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122,
    6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126,
    1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121,
    5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125,
    3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123,
    7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127
};

Commandes::Commandes() :
    mVolumeGauche(0),
    mVolumeDroite(0)
#ifdef USE_MOTORIZED_POT
    , mMoteurAZero(false)
    , mDateDerniereCommandeVolume(0)
#endif
#ifdef USE_MOTORIZED_POT
    , mIndexTensionCourante(0)
#endif
#ifdef I2C_VOLUME
    , mCurrentVolumeLeft(0)
    , mCurrentVolumeRight(0)
#endif
    , mVolumeChanged(false)
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

void Commandes::volumePlus()
{
#ifdef USE_MOTORIZED_POT
    moteurDroite();
    mVolumeChanged = true;
#endif
#ifdef I2C_VOLUME
    if (mCurrentVolumeLeft < mMaxVolume)
    {
        mCurrentVolumeLeft++;
    }
    if (mCurrentVolumeRight < mMaxVolume)
    {
        mCurrentVolumeRight++;
    }
    changeVolume(mCurrentVolumeLeft, mCurrentVolumeRight);
#endif
}

void Commandes::volumeMoins()
{
#ifdef USE_MOTORIZED_POT
    moteurGauche();
    mVolumeChanged = true;
#endif
#ifdef I2C_VOLUME
    if (mCurrentVolumeLeft > 0)
    {
        mCurrentVolumeLeft--;
    }
    if (mCurrentVolumeRight > 0)
    {
        mCurrentVolumeRight--;
    }
    changeVolume(mCurrentVolumeLeft, mCurrentVolumeRight);
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

void Commandes::changeVolume(uint8_t left, uint8_t right)
{
#ifdef I2C_VOLUME
    if (left < mMaxVolume)
    {
        setI2cVolumeLeft(left);
        mCurrentVolumeLeft = left;
    }
    if (left < mMaxVolume)
    {
        setI2cVolumeRight(right);
        mCurrentVolumeRight = right;
    }
    mVolumeChanged = true;
#endif
}

bool Commandes::servitudesPresentes()
{
    return (analogRead(PresenceServitudes) > seuilPresenceServitudes);
}

void Commandes::envoyerCommandeServitude(ActionsServitudes::teCibleActionServitudes cible, ActionsServitudes::teTypeActionServitudes type)
{
    if (servitudesPresentes())
    {
        Wire.beginTransmission(DialogDefinition::servitudesI2cId);
        Wire.write(static_cast<uint8_t>(cible));
        Wire.write(static_cast<uint8_t>(type));
        Wire.endTransmission();
        delay(10);
    }
}

void Commandes::lireStatutServitudes()
{
    if (servitudesPresentes())
    {
        Wire.requestFrom(DialogDefinition::servitudesI2cId, 3);    // request 3 bytes from slave device

        while(Wire.available())    // slave may send less than requested
        {
            char c = Wire.read(); // receive a byte as character
            Serial.println(c, HEX);         // print the character
        }
        Serial.println("reçu des servitudes");
    }
    else
    {
        Serial.println("Servitudes absentes");
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
    mMuted = muted;
    if (!muted)
    {
        digitalWrite(Mute, HIGH);
        // On remet le volume
        changeVolume(mCurrentVolumeLeft, mCurrentVolumeRight);
    }
    else
    {
        digitalWrite(Mute, LOW);
        // volume à 0
        setI2cVolumeLeft(0);
        setI2cVolumeRight(0);
    }
}

bool Commandes::volumeChanged(uint8_t &left, uint8_t &right)
{
    if (mVolumeChanged)
    {
        mVolumeChanged = false;
#ifdef I2C_VOLUME
        left = mCurrentVolumeLeft;
        right = mCurrentVolumeRight;
#else
        right = 88;
        left = 88;
#endif
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef I2C_VOLUME
void Commandes::setI2cVolumeLeft(uint8_t volume)
{
    Wire.beginTransmission(mPcf8574LeftAddress);
    sendI2cVolume(volume);
}

void Commandes::setI2cVolumeRight(uint8_t volume)
{
    Wire.beginTransmission(mPcf8574RightAddress);
    sendI2cVolume(volume);
}

void Commandes::sendI2cVolume(uint8_t volume)
{
    uint8_t masqueMute = 0x80;
    if (mMuted)
    {
        masqueMute = 0;
    }
    if (volume > 0)
    {
        Wire.write(pgm_read_byte_near(indirectionVolume + volume) | masqueMute);
    }
    else
    {
        Wire.write(0);
    }
    Wire.endTransmission();
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
