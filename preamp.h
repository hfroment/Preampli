#ifndef PREAMP_H
#define PREAMP_H

#include <Arduino.h>

#include "options.h"

#include "actions.h"

class IHM;
class Commandes;
class Telecommande;

class Preamp
{
public:
    Preamp();

    void init();
    // Fonction périodique
    bool gerer();

    static void itFunction();

private:
    static Preamp *instance;

    long mCompteurIt;
    uint8_t mCompteurItSeconde;
    uint8_t mCompteurItPrecedent;

    bool traiterAction(uint16_t action);
    void gererServitudes();

    Commandes& mCommandes;

    IHM& mIhm;

    Telecommande& mTelecommande;

    bool mMutedPrecedent;
    bool mServitudesOnPrecedent;

    static const long mPeriodeTimer1 = 1000; // 1 ms
    static const long mCompteurPour1s = 1000000 / mPeriodeTimer1; // 1 ms

};

#endif // PREAMP_H
