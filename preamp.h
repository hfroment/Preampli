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

    static const String mVersionString;

    static const ActionsServitudes::teCibleActionServitudes entreeHdmiRpi = ActionsServitudes::Hdmi_2;
    static const ActionsServitudes::teCibleActionServitudes entreeHdmiChromeCast = ActionsServitudes::Hdmi_3;

    bool traiterAction(uint16_t action);
    void gererServitudes();

    Commandes& mCommandes;

    IHM& mIhm;

    Telecommande& mTelecommande;

    bool mDacActivePrecedent;
    bool mMutedPrecedent;
    bool mServitudesOnPrecedent;

    static const long mPeriodeTimer1 = 1000; // 1 ms
    static const long mCompteurPour1s = 1000000 / mPeriodeTimer1; // 1 ms

#ifdef USE_MOTORIZED_POT
    bool mMotorOnPrecedent;
#endif
};

#endif // PREAMP_H
