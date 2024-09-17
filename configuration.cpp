#include <EEPROM.h>

#include "configuration.h"

Configuration* Configuration::mInstance = 0;

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
    pinMode(mPinSalon, INPUT_PULLUP);
    if (!salon())
    {
        pinMode(mPinInversionEncodeurs, INPUT_PULLUP);
    }
    charger();
}

Configuration* Configuration::instance()
{
    if (mInstance == 0)
    {
        mInstance = new Configuration();
    }
    return mInstance;
}

bool Configuration::charger()
{
    // Nom des entrées
    if (salon())
    {
        mNomEntrees[AucuneEntree] = "-------";
        mNomEntrees[EntreeAnalogique_1] = "KODI";
        mNomEntrees[EntreeAnalogique_2] = "TV";
        mNomEntrees[EntreeAnalogique_3] = "Aux 2";
        mNomEntrees[EntreeAnalogique_4] = "Aux 1";
    }
    else
    {
        mNomEntrees[AucuneEntree] = "-------";
        mNomEntrees[EntreeAnalogique_1] = "Internal";
        mNomEntrees[EntreeAnalogique_2] = "SPDIF";
        mNomEntrees[EntreeAnalogique_3] = "IN 1";
        mNomEntrees[EntreeAnalogique_4] = "IN 2";
    }


    uint16_t position = 0;
    // On lit lentrée active (= courante)
    mEntreeActive = EEPROM.read(position++);
    if (mEntreeActive >= NombreEntreeNavigable)
    {
        mEntreeActive = AucuneEntree;
    }
    mEntreeCourante = mEntreeActive;
    // L'état de mute
    mMuted = EEPROM.read(position++);
    // Le volume
    mVolume = mVolumeParDefaut;//EEPROM.read(position++);
    position++;
    mBalance = EEPROM.read(position++);
}

bool Configuration::sauver()
{
    uint16_t position = 0;
    // On sauve la commande courante
    EEPROM.write(position++, mEntreeActive);
    EEPROM.write(position++, mMuted);
    EEPROM.write(position++, mVolume);
    EEPROM.write(position++, mBalance);

    //mNbSauvegardes++;
    mSaved = true;
}

String Configuration::entreeCouranteToString()
{
    if (mEntreeCourante < NombreEntrees)
    {
        return entreeToString(mEntreeCourante);
    }
    else
    {
        return entreeToString(AucuneEntree);
    }
}

String Configuration::entreeActiveToString()
{
    if (mEntreeActive < NombreEntrees)
    {
        return entreeToString(mEntreeActive);
    }
    else
    {
        return entreeToString(AucuneEntree);
    }
}

String Configuration::entreeToString(uint8_t entree)
{
    if (entree < NombreEntrees)
    {
        return entreeToString(entree);
    }
    else
    {
        return entreeToString(AucuneEntree);
    }
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
        mDateDernierChangementConfiguration = millis();
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
    return (digitalRead(mPinSalon) != LOW);
}

bool Configuration::encodeursInverses()
{
    if (!salon())
    {
        return (digitalRead(mPinInversionEncodeurs) != LOW);
    }
    else
    {
        return false;
    }
}


