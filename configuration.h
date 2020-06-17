#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>

class Configuration
{
public:
    static Configuration* instance();

    bool charger();
    bool sauver();
    void gerer();

    typedef enum
    {
        AucuneEntree = 0,
        EntreeAnalogique_1,
        EntreeAnalogique_2,
        EntreeAnalogique_3,
        EntreeSpdif_1,
        EntreeSpdif_2,
        EntreeSpdif_3,
        EntreeSpdif_4,
        EntreeToslink,
        EntreeUsb,
        NombreEntreeNavigable,

        // Entrée hors liste
        NombreEntrees
    }
    teEntreesAudio;

    String entreeCouranteToString();
    String entreeActiveToString();
    String entreeToString(uint8_t entree);

    void selectionnerEntreeSuivante();
    void selectionnerEntreePrecedente();

    uint8_t entreeCourante()
    {
        return mEntreeCourante;
    }
    uint8_t entreeActive()
    {
        return mEntreeActive;
    }
    void entreeActive(uint8_t entree);
    bool saved()
    {
        bool retour = mSaved;
        mSaved = false;
        return retour;
    }
    void activerEntreeCourante()
    {
        entreeActive(mEntreeCourante);
    }
    bool entreeCouranteAnnulee()
    {
        bool retour = mEntreeCouranteAnnulee;
        mEntreeCouranteAnnulee = false;
        return retour;
    }
    bool mute(bool muted);
    bool toggleMute()
    {
        return mute(!mMuted);
    }

    bool muted()
    {
        return mMuted;
    }

protected:
    Configuration();
private:
    // Singleton
    static Configuration* mInstance;

    // Entree courante
    uint8_t mEntreeCourante;

    // Entree active
    uint8_t mEntreeActive;

    // Le nom des entrées
    String mNomEntrees[NombreEntrees];

    static const uint16_t mDureeAvantSauvegarde = 5000; // ms.
    unsigned long mDateDernierChangementEntreeActive;
    static const uint16_t mDureeAvantAnnulation = 5000; // ms.
    unsigned long mDateDernierChangementEntreeCourante;
    bool mEntreeCouranteAnnulee;

    bool mMuted;

    /// Pour savoir si l'on vient de faire une sauvegarde
    bool mSaved;
//    uint16_t mNbSauvegardes; // Pour test
};

#endif // CONFIGURATION_H
