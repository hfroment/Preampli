#include <Wire.h>

#include "commandes.h"

Commandes::Commandes() :
    mVolumeGauche(0),
    mVolumeDroite(0),
    mDateDerniereCommandeVolume(0),
    mMoteurAZero(false)
{
}

void Commandes::gerer()
{
    if (((mDateDerniereCommandeVolume != 0) && (millis() - mDateDerniereCommandeVolume > mDureeActivationVolume) && !mMoteurAZero) || moteurBloque())
    {
        moteurStop();
        mDateDerniereCommandeVolume = 0;
    }
}
void Commandes::init()
{
    // Le moteur
    pinMode(moteurA, OUTPUT);
    pinMode(moteurB, OUTPUT);
    pinMode(Entree1DcB1, OUTPUT);
    pinMode(Entree2DcB1, OUTPUT);
    pinMode(Entree3DcB1, OUTPUT);
    pinMode(Entree4DcB1, OUTPUT);
    pinMode(Mute, OUTPUT);
    mute(true);

//    volumeInit(2);
    Serial.begin(9600);
}

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
    //return (analogRead(mesureTension) < seuilTensionBlocage);
    // Consommation moteur non conforme à la spec : ne fonctionne pas
    return false;
}

void Commandes::volumeInit(uint8_t dureeEnSecondes)
{
    volumeMoins();
    delay(12000);
    volumePlus();
    delay(dureeEnSecondes * 1000);
    //    mMoteurAZero = true;
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
        selectionnerEntreeAnalogique(4);
        break;
    case Configuration::EntreeSpdif_1:
        selectionnerEntreeSpdif(1);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::EntreeSpdif_2:
        selectionnerEntreeSpdif(2);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::EntreeSpdif_3:
        selectionnerEntreeSpdif(3);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::EntreeSpdif_4:
        selectionnerEntreeSpdif(4);
        selectionnerEntreeAnalogique(2);
        break;
    }
}

void Commandes::selectionnerEntreeAnalogique(uint8_t entree)
{
    switch (entree)
    {
    case 0:
        // aucune entrée
        digitalWrite(Entree1DcB1, LOW);
        digitalWrite(Entree2DcB1, LOW);
        digitalWrite(Entree3DcB1, LOW);
        digitalWrite(Entree4DcB1, LOW);
        break;
    case 1:
        digitalWrite(Entree2DcB1, LOW);
        digitalWrite(Entree3DcB1, LOW);
        digitalWrite(Entree4DcB1, LOW);
        digitalWrite(Entree1DcB1, HIGH);
        break;
    case 2:
        digitalWrite(Entree1DcB1, LOW);
        digitalWrite(Entree3DcB1, LOW);
        digitalWrite(Entree4DcB1, LOW);
        digitalWrite(Entree2DcB1, HIGH);
        break;
    case 3:
        digitalWrite(Entree1DcB1, LOW);
        digitalWrite(Entree2DcB1, LOW);
        digitalWrite(Entree4DcB1, LOW);
        digitalWrite(Entree3DcB1, HIGH);
        break;
    case 4:
        digitalWrite(Entree1DcB1, LOW);
        digitalWrite(Entree2DcB1, LOW);
        digitalWrite(Entree3DcB1, LOW);
        digitalWrite(Entree4DcB1, HIGH);
        break;
    default:
        // Pas de changement
        break;
    }
}

void Commandes::selectionnerEntreeSpdif(uint8_t entree)
{
}

void Commandes::mute(bool muted)
{
    if (!muted)
    {
        digitalWrite(Mute, LOW);
    }
    else
    {
        digitalWrite(Mute, HIGH);
    }
}

uint16_t Commandes::tensionLueEnMv()
{
    return (uint16_t)((uint32_t)analogRead(mesureTension) * 5000L / 1024L);
}
uint16_t Commandes::tensionRefEnMv()
{
    return (uint16_t)((uint32_t)seuilTensionBlocage * 5000L / 1024L);
}
