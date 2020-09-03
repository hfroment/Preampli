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
    mVolumeLeft(0),
    mVolumeRight(0),
    mVolumeChanged(true)
{
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
    // Valeurs par défaut
    mNomEntrees[AucuneEntree] = "-------";
    mNomEntrees[EntreeAnalogique_1] = "IN 1";
    mNomEntrees[EntreeAnalogique_2] = "IN 2";
    mNomEntrees[EntreeAnalogique_3] = "IN 3";
    mNomEntrees[EntreeSpdif_1] = "CD";
    mNomEntrees[EntreeSpdif_2] = "SPDIF 1";
    mNomEntrees[EntreeSpdif_3] = "SPDIF 3";
    mNomEntrees[EntreeSpdif_4] = "SPDIF 4";
    mNomEntrees[EntreeToslink] = "G Cast";
    mNomEntrees[EntreeUsb] = "Kodi";

    uint16_t position = 0;
    // On lit lentrée active (= courante)
    mEntreeActive = EEPROM.read(position++);
    mEntreeCourante = mEntreeActive;
    // L'état de mute
    mMuted = EEPROM.read(position++);
    // Le volume
    mVolumeLeft = EEPROM.read(position++);
    mVolumeRight = EEPROM.read(position++);
}

bool Configuration::sauver()
{
    uint16_t position = 0;
    // On sauve la commande courante
    EEPROM.write(position++, mEntreeActive);
    EEPROM.write(position++, mMuted);
    EEPROM.write(position++, mVolumeLeft);
    EEPROM.write(position++, mVolumeRight);

    //mNbSauvegardes++;
    mSaved = true;
}

String Configuration::entreeCouranteToString()
{
    return entreeToString(mEntreeCourante);
}

String Configuration::entreeActiveToString()
{
    return entreeToString(mEntreeActive);
}

String Configuration::entreeToString(uint8_t entree)
{
    return mNomEntrees[entree];
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

void Configuration::changeVolume(uint8_t left, uint8_t right)
{
    if (mVolumeLeft != left)
    {
        mVolumeLeft = left;
        mDateDernierChangementConfiguration = millis();
        mVolumeChanged = true;
    }
    if (mVolumeRight != right)
    {
        mVolumeRight = right;
        mDateDernierChangementConfiguration = millis();
        mVolumeChanged = true;
    }
}

bool Configuration::volumeChanged(uint8_t &left, uint8_t &right)
{
    left = mVolumeLeft;
    right = mVolumeRight;
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


