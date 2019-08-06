#include <Wire.h>

#include "commandes.h"

Commandes::Commandes() :
    mVolumeGauche(0),
    mVolumeDroite(0),
    mDateDerniereCommandeVolume(0),
    mPcf1Data(0),
    mPcf2Data(0),
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

    volumeInit(2);
    // Non appelé car utilisé par l'écran
    // Wire.begin
    Wire.beginTransmission(mPcf2Addr);
    Wire.write(MASQUE_LECTURE);
    Wire.endTransmission();
    Serial.begin(9600);
    //pinMode(alimRelaisSpdif, OUTPUT);
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
    uint8_t pcf1Data = mPcf1Data;

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
    case Configuration::EntreeTape:
        // pas de changement hormis tape
        pcf1Data = mPcf1Data ^ TAPE;
        break;
    case Configuration::EntreeSpdif_1:
        selectionnerEntreeSpdif(1);
        selectionnerEntreeI2s(I2S_SPDIF);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::EntreeSpdif_2:
        selectionnerEntreeSpdif(2);
        selectionnerEntreeI2s(I2S_SPDIF);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::EntreeSpdif_3:
        selectionnerEntreeSpdif(3);
        selectionnerEntreeI2s(I2S_SPDIF);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::EntreeSpdif_4:
        selectionnerEntreeSpdif(4);
        selectionnerEntreeI2s(I2S_SPDIF);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::BlueTooth:
        selectionnerEntreeI2s(I2S_BLUETOOTH);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::EntreeUsb:
        selectionnerEntreeI2s(I2S_USB);
        selectionnerEntreeAnalogique(2);
        break;
    case Configuration::I2sExterne:
        selectionnerEntreeI2s(I2S_I2S);
        selectionnerEntreeAnalogique(2);
        break;
    }
    mPcf1Data = pcf1Data;
    activerPCF();
}

void Commandes::activerPCF()
{
    Wire.beginTransmission(mPcf1Addr);
    Wire.write(mPcf1Data);
    Wire.endTransmission();
    Wire.beginTransmission(mPcf2Addr);
    Wire.write(mPcf2Data);
    Wire.endTransmission();
    //    Serial.print("PCF : 0x");
    //    Serial.print(mPcf1Data, HEX);
    //    Serial.print(", 0x");
    //    Serial.println(mPcf2Data, HEX);
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
    uint8_t pcf2Data = 0;

    // On détemine les codes à envoyer aux PCF7485
    switch (entree)
    {
    default:
        // On reste en l'état
        break;
    case 0:
        // Aucune entrée
        pcf2Data = 0;
        break;
    case 1:
        pcf2Data = SPDIF_1;
        break;
    case 2:
        pcf2Data = SPDIF_2;
        break;
    case 3:
        pcf2Data = SPDIF_3;
        break;
    case 4:
        pcf2Data = SPDIF_4;
        break;
    }
    mPcf2Data = pcf2Data | MASQUE_LECTURE;
    activerPCF();
}

void Commandes::selectionnerEntreeI2s(uint8_t entree)
{
    uint8_t pcf1Data = mPcf1Data ;

    // On détemine les codes à envoyer aux PCF7485
    switch (entree)
    {
    default:
        // On reste en l'état
        break;
    case 0:
        // Aucune entrée
        pcf1Data = ~MASQUE_I2S_INPUTS;
        break;
    case I2S_SPDIF:
        pcf1Data &= I2S_SPDIF | ~MASQUE_I2S_INPUTS;
        break;
    case I2S_BLUETOOTH:
        pcf1Data = I2S_BLUETOOTH | ~MASQUE_I2S_INPUTS;
        break;
    case I2S_USB:
        pcf1Data = I2S_USB | ~MASQUE_I2S_INPUTS;
        break;
    case I2S_I2S:
        pcf1Data = I2S_I2S | ~MASQUE_I2S_INPUTS;
        break;
    }

    mPcf1Data = pcf1Data;
    activerPCF();
}

void Commandes::activerTape(bool actif)
{
    if (actif)
    {
        mPcf1Data |= TAPE;
    }
    else
    {
        mPcf1Data &= ~TAPE;
    }
    activerPCF();
}

void Commandes::etats(bool& verouillage, bool& _96kHz)
{
    Wire.beginTransmission(mPcf2Addr);
    Wire.requestFrom(mPcf2Addr, 1);
    uint8_t etats = Wire.read();
    Wire.endTransmission();
    verouillage = (etats & LECTURE_1) == LECTURE_1;
    _96kHz = (etats & LECTURE_2) == LECTURE_2;
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

void Commandes::extension(bool on)
{
    if (on)
    {
        mPcf1Data |= EXTENSION;
    }
    else
    {
        mPcf1Data &= ~EXTENSION;
    }
}

//void Commandes::corrected(bool corrected)
//{
//    if (corrected)
//    {
//        mPcf2Data |= CORRECTED;
//    }
//    else
//    {
//        mPcf2Data &= ~CORRECTED;
//    }
//    activerPCF();
//}

uint16_t Commandes::tensionLueEnMv()
{
    return (uint16_t)((uint32_t)analogRead(mesureTension) * 5000L / 1024L);
}
uint16_t Commandes::tensionRefEnMv()
{
    return (uint16_t)((uint32_t)seuilTensionBlocage * 5000L / 1024L);
}
