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
    mMuted(false)
#ifdef I2C_VOLUME
    , mCurrentVolume(0)
    , mCurrentBalance(0)
#endif
    , mVolumeChanged(false)
{
}

void Commandes::gerer()
{
}
void Commandes::init()
{
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
    pinMode(Mute, OUTPUT);
    digitalWrite(Mute, LOW);
    mute(true);
}


void Commandes::volumeInit(int8_t initValue)
{
#ifdef I2C_VOLUME
    setI2cVolumeLeft(initValue);
    setI2cVolumeRight(initValue);
#endif
}

void Commandes::volumePlus()
{
#ifdef I2C_VOLUME
    if (mCurrentVolume < mMaxVolume)
    {
        mCurrentVolume++;
    }
    changeVolume(mCurrentVolume, mCurrentBalance);
#endif
}

void Commandes::volumeMoins()
{
#ifdef I2C_VOLUME
    if (mCurrentVolume > 0)
    {
        mCurrentVolume--;
    }
    changeVolume(mCurrentVolume, mCurrentBalance);
#endif
}

void Commandes::balanceDroite()
{
#ifdef I2C_VOLUME
    if (mCurrentBalance < mMaxVolume)
    {
        mCurrentBalance++;
        changeVolume(mCurrentVolume, mCurrentBalance);
    }
#endif
}

void Commandes::balanceGauche()
{
#ifdef I2C_VOLUME
    if (mMaxVolume + mCurrentBalance > 0)
    {
        mCurrentBalance--;
        changeVolume(mCurrentVolume, mCurrentBalance);
    }
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
        selectionnerEntreeAnalogique(1);
        break;
    case Configuration::EntreeAnalogique_2:
        selectionnerEntreeAnalogique(3);
        break;
    case Configuration::EntreeAnalogique_3:
        selectionnerEntreeAnalogique(3);
        break;
    case Configuration::EntreeAnalogique_4:
        selectionnerEntreeAnalogique(4);
        break;
    }
}

void Commandes::changeVolume(int8_t volume, int8_t balance)
{
#ifdef I2C_VOLUME
    if ((volume < mMaxVolume) && (volume > 0))
    {
        setI2cVolumeLeft(volume);
        mCurrentVolume = volume;
    }
    if ((volume + balance < mMaxVolume) && (volume + balance > 0))
    {
        setI2cVolumeRight(volume + balance);
        mCurrentBalance = balance;
    }
    if ((volume <= 0) || (volume + balance <= 0))
    {
        // Un des volume est à 0, on mute
        muteOn();
    }
    else if (!muted())
    {
        muteOff();
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
//        Wire.beginTransmission(DialogDefinition::servitudesI2cId);
//        Wire.write(static_cast<uint8_t>(cible));
//        Wire.write(static_cast<uint8_t>(type));
//        Wire.endTransmission();
//        delay(10);
    }
}

void Commandes::lireStatutServitudes()
{
    if (servitudesPresentes())
    {
//        Wire.requestFrom(DialogDefinition::servitudesI2cId, 3);    // request 3 bytes from slave device

//        while(Wire.available())    // slave may send less than requested
//        {
//            char c = Wire.read(); // receive a byte as character
//#ifdef SERIAL_ON
//            Serial.println(c, HEX);         // print the character
//#endif
//        }
//#ifdef SERIAL_ON
//        Serial.println(F("reçu des servitudes"));
//#endif
//    }
//    else
//    {
//#ifdef SERIAL_ON
//        Serial.println(F("Servitudes absentes"));
//#endif
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



void Commandes::mute(bool muted)
{
    mMuted = muted;
    if (!muted)
    {
        muteOff();
        // On remet le volume
#ifdef I2C_VOLUME
        changeVolume(mCurrentVolume, mCurrentBalance);
#endif
    }
    else
    {
        muteOn();
        // volume à 0
#ifdef I2C_VOLUME
        setI2cVolumeLeft(0);
        setI2cVolumeRight(0);
#endif
    }
}

bool Commandes::volumeChanged(int8_t &volume, int8_t &balance)
{
    if (mVolumeChanged)
    {
        mVolumeChanged = false;
#ifdef I2C_VOLUME
        volume = mCurrentVolume;
        balance = mCurrentBalance;
#endif
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef I2C_VOLUME
void Commandes::setI2cVolumeLeft(int8_t volume)
{
    sendI2cVolume(mPcf8574LeftAddress, volume);
}

void Commandes::setI2cVolumeRight(int8_t volume)
{
    sendI2cVolume(mPcf8574RightAddress, volume);
}

void Commandes::sendI2cVolume(uint8_t adresse, int8_t volume)
{
    uint8_t masqueMute = 0x80;
    if (mMuted)
    {
        masqueMute = 0;
    }
    Wire.beginTransmission(adresse);
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

void Commandes::muteOn()
{
    digitalWrite(Mute, LOW);
}

void Commandes::muteOff()
{
    digitalWrite(Mute, HIGH);
}

