#include <EEPROM.h>

#include "configuration.h"

Configuration* Configuration::mInstance = 0;

Configuration::Configuration() :
    mEntreeCourante(AucuneEntree),
    mEntreeActive(AucuneEntree),
    mDateDernierChangementEntreeActive(0),
    mDateDernierChangementEntreeCourante(0),
    mEntreeCouranteAnnulee(false),
    mTapeActive(false),
    mMuted(true),
    mFreeBool(false),
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
    mNomEntrees[EntreeAnalogique_2] = "Kodi";
    mNomEntrees[EntreeAnalogique_3] = "IR715";
    mNomEntrees[EntreeUsb] = "USB";
    mNomEntrees[EntreeTape] = "Tape";
    mNomEntrees[EntreeSpdif_1] = "CD SPDIF";
    mNomEntrees[EntreeSpdif_2] = "Kodi SPDIF";
    mNomEntrees[EntreeSpdif_3] = "IR715 SPDIF";
    mNomEntrees[EntreeSpdif_4] = "SPDIF 4";

    uint16_t position = 0;
    // On lit lentrée active (= courante)
    mEntreeActive = EEPROM.read(position++);
    mEntreeCourante = mEntreeActive;
    mTapeActive = EEPROM.read(position++);
    mMuted = EEPROM.read(position++);
    mFreeBool = EEPROM.read(position++);
}

bool Configuration::sauver()
{
    uint16_t position = 0;
    // On sauve la commande courante
    EEPROM.write(position++, mEntreeActive);
    EEPROM.write(position++, mTapeActive);
    EEPROM.write(position++, mMuted);
    EEPROM.write(position++, mFreeBool);

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
    if (mEntreeCourante > DerniereEntreeNavigable)
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
        mEntreeCourante = DerniereEntreeNavigable;
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

bool Configuration::activerTape(bool actif)
{
     if (mTapeActive != actif)
     {
         mTapeActive = actif;
         mDateDernierChangementEntreeActive = millis();
     }
     return mTapeActive;
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

bool Configuration::freeBool(bool corrected)
{
    if (mFreeBool != corrected)
    {
        mFreeBool = corrected;
        mDateDernierChangementEntreeActive = millis();
    }
    return mFreeBool;
}
