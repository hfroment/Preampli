#include <EEPROM.h>

#include "configuration.h"

Configuration* Configuration::mInstance = 0;

Configuration::Configuration() :
    mEntreeCourante(AucuneEntree),
    mEntreeActive(AucuneEntree),
    mDateDernierChangementEntreeActive(0),
    mDateDernierChangementEntreeCourante(0),
    mEntreeCouranteAnnulee(false),
    mMuted(true),
   mSaved(false)
  //, mNbSauvegardes(0)
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
    mNomEntrees[EntreeAnalogique_1] = "CD";
    mNomEntrees[EntreeAnalogique_2] = "Aux 1";
    mNomEntrees[EntreeAnalogique_3] = "Aux 2";
    mNomEntrees[EntreeSpdif_1] = "CD SPDIF";
    mNomEntrees[EntreeSpdif_2] = "Kodi";
    mNomEntrees[EntreeSpdif_3] = "SqueezeBox";
    mNomEntrees[EntreeSpdif_4] = "Aux SPDIF";

    uint16_t position = 0;
    // On lit lentrée active (= courante)
    mEntreeActive = EEPROM.read(position++);
    mEntreeCourante = mEntreeActive;
    mMuted = EEPROM.read(position++);
}

bool Configuration::sauver()
{
    uint16_t position = 0;
    // On sauve la commande courante
    EEPROM.write(position++, mEntreeActive);
    EEPROM.write(position++, mMuted);

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
    if ((mDateDernierChangementEntreeActive != 0) && (millis() - mDateDernierChangementEntreeActive > mDureeAvantSauvegarde))
    {
        sauver();
        mDateDernierChangementEntreeActive = 0;
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
        mDateDernierChangementEntreeActive = millis();
    }
}

bool Configuration::mute(bool muted)
{
    if (mMuted != muted)
    {
        mMuted = muted;
        mDateDernierChangementEntreeActive = millis();
    }
    return mMuted;
}

