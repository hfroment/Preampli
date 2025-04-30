#include <EEPROM.h>

#include "configuration.h"

//Configuration* Configuration::mInstance = 0;

Configuration configurationCourante;

Configuration::Configuration() :
    mEntreeCourante(AucuneEntree),
    mEntreeActive(AucuneEntree),
    mDateDernierChangementConfiguration(0),
    mDateDernierChangementEntreeCourante(0),
    mEntreeCouranteAnnulee(false),
    mMuted(true),
    mSaved(false),
    //, mNbSauvegardes(0)
    mVolume(0),
    mBalance(0),
    mVolumeChanged(true)
{
    pinMode(mPinCongigurationD6, INPUT_PULLUP);
    pinMode(mPinCongigurationA2, INPUT_PULLUP);

    charger();
}

Configuration* Configuration::instance()
{
    return &configurationCourante;
//    if (mInstance == 0)
//    {
//        mInstance = new Configuration();
//    }
//    return mInstance;
}

bool Configuration::charger()
{
    uint16_t position = 0;
    // On lit lentrée active (= courante)
    mEntreeActive = EEPROM.read(position++);
    if (mEntreeActive >= NombreEntreeNavigable)
    {
        mEntreeActive = EntreeAnalogique_1;
    }
    mEntreeCourante = mEntreeActive;
    // L'état de mute
    mMuted = EEPROM.read(position++);
    // Le volume par défaut
    mVolume = mVolumeMax;
    mVolumeParDefaut = EEPROM.read(position++);
    if (mVolumeParDefaut < mVolumeMax)
    {
        mVolume = mVolumeParDefaut;
    }
    position++;
    mBalance = EEPROM.read(position++);
}

bool Configuration::sauver()
{
    uint16_t position = 0;
    // On sauve la commande courante
    EEPROM.write(position++, mEntreeActive);
    EEPROM.write(position++, mMuted);
    EEPROM.write(position++, mVolumeParDefaut);
    EEPROM.write(position++, mBalance);

    //mNbSauvegardes++;
    mSaved = true;
}

void Configuration::selectionnerEntreeSuivante()
{
    mEntreeCourante++;
    if (mEntreeCourante >= NombreEntreeNavigable)
    {
        mEntreeCourante = EntreeAnalogique_1;
    }
    mDateDernierChangementEntreeCourante = millis();
}

void Configuration::selectionnerEntreePrecedente()
{
    if (mEntreeCourante > EntreeAnalogique_1)
    {
        mEntreeCourante--;
    }
    else
    {
        mEntreeCourante = NombreEntreeNavigable - 1;
    }
    mDateDernierChangementEntreeCourante = millis();
}

void Configuration::gerer()
{
    if ((mDateDernierChangementConfiguration != 0) && (millis() - mDateDernierChangementConfiguration > mDureeAvantSauvegarde))
    {
        sauver();
        mDateDernierChangementConfiguration = 0;
    }
    if ((mDateDernierChangementEntreeCourante != 0) && (millis() - mDateDernierChangementEntreeCourante > mDureeAvantSauvegarde))
    {
        mEntreeCouranteAnnulee = true;
        mEntreeCourante = mEntreeActive;
        mDateDernierChangementEntreeCourante = 0;
    }
}

void Configuration::entreeActive(uint8_t entree)
{
    if (mEntreeActive != entree)
    {
        mEntreeActive = entree;
        mDateDernierChangementConfiguration = millis();
    }
}

bool Configuration::mute(bool muted)
{
    if (mMuted != muted)
    {
        mMuted = muted;
        mDateDernierChangementConfiguration = millis();
    }
    return mMuted;
}

void Configuration::changeVolume(int8_t volume, int8_t balance)
{
    if (mVolume != volume)
    {
        mVolume = volume;
        // Pas de sauvegarde sur changement de volume
        //mDateDernierChangementConfiguration = millis();
        mVolumeChanged = true;
    }
    if (mBalance != balance)
    {
        mBalance = balance;
        mDateDernierChangementConfiguration = millis();
        mVolumeChanged = true;
    }
}

bool Configuration::volumeChanged(int8_t &volume, int8_t &balance)
{
    volume = mVolume;
    balance = mBalance;
    if (mVolumeChanged)
    {
        mVolumeChanged = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool Configuration::salon()
{
    return (digitalRead(mPinCongigurationD6) != LOW) &&
           (digitalRead(mPinCongigurationA2) != LOW);
}

bool Configuration::preampRpi()
{
    return (digitalRead(mPinCongigurationD6) == LOW) &&
            (digitalRead(mPinCongigurationA2) != LOW);
}

bool Configuration::preampSimple()
{
    return (digitalRead(mPinCongigurationD6) != LOW) &&
           (digitalRead(mPinCongigurationA2) == LOW);
}

bool Configuration::prempaSimpleEncodeursInverses()
{
    return (digitalRead(mPinCongigurationD6) == LOW) &&
           (digitalRead(mPinCongigurationA2) == LOW);
}

void Configuration::setVolumeParDefaut(int8_t newVolumeParDefaut)
{
    mVolumeParDefaut = newVolumeParDefaut;
    mDateDernierChangementConfiguration = millis();
}




