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

    static void cadencer1s();

private:
    static Preamp *instance;

    uint8_t mCompteurIt;
    uint8_t mCompteurItPrecedent;

    static const String mVersionString;

    static const uint8_t ledPin = 8;

    bool traiterAction(uint16_t action);

    Commandes& mCommandes;

    IHM& mIhm;

    Telecommande& mTelecommande;

    bool mDacActivePrecedent;
    bool mMutedPrecedent;
#ifdef USE_MOTORIZED_POT
    bool mMotorOnPrecedent;
#endif
};

#endif // PREAMP_H
