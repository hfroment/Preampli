#include <TimerOne.h>

#include "ihm.h"
#include "commandes.h"
#include "configuration.h"
#include "telecommande.h"

#include "preamp.h"


Preamp *Preamp::instance = NULL;
const String Preamp::mVersionString = "0.0.1";

Preamp::Preamp() :
    mCompteurIt(0),
    mCompteurItPrecedent(255),
    mCommandes(*new Commandes()),
    mIhm(*new IHM()),
    mTelecommande(* new Telecommande()),
    mDacActivePrecedent(false),
    mMutedPrecedent(false)
#ifdef USE_MOTORIZED_POT
    , mMotorOnPrecedent(false)
#endif
{
    instance = this;

    pinMode(ledPin, OUTPUT);
    // On met en place l'IT seconde
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(Preamp::instance->cadencer1s);
}
void Preamp::init()
{
    mIhm.init("Bonjour", "V " + mVersionString);
    // à faire APRES l'IHM à cause de wire
    mCommandes.init();
    mMutedPrecedent = mCommandes.muted();
    mIhm.muted(mMutedPrecedent);
    mIhm.refresh();
    mCommandes.volumeInit();
    mTelecommande.init();
    // On charge la configuration
    Configuration::instance()->charger();

    // On gère les états
    mCommandes.selectionnerEntree(Configuration::instance()->entreeActive());
    mDacActivePrecedent = mCommandes.dacActive();
    mIhm.dacActivated(mDacActivePrecedent);
    mCommandes.mute(Configuration::instance()->muted());
#ifdef USE_MOTORIZED_POT
    mMotorOnPrecedent = mCommandes.motorOn();
#endif
}

bool Preamp::traiterAction(uint16_t action)
{
    bool retour = false;

    if (action != Actions::AucuneAction)
    {
        if ((action & Actions::VolumePlus) == Actions::VolumePlus)
        {
            mCommandes.volumePlus();
            retour = true;
        }
        if ((action & Actions::VolumeMoins) == Actions::VolumeMoins)
        {
            mCommandes.volumeMoins();
            retour = true;
        }
        if ((action & Actions::SelectionPrecedente) != 0)
        {
            Configuration::instance()->selectionnerEntreePrecedente();
            //mCommandes.selectionnerEntree(Configuration::instance()->entreeCourante());
            retour = true;
        }
        if ((action & Actions::SelectionSuivante) != 0)
        {
            Configuration::instance()->selectionnerEntreeSuivante();
            //mCommandes.selectionnerEntree(Configuration::instance()->entreeCourante());
            retour = true;
        }
        if ((action & Actions::ActiverEntreeCourante) != 0)
        {
            Configuration::instance()->activerEntreeCourante();
            mCommandes.selectionnerEntree(Configuration::instance()->entreeActive());
            retour = true;
        }
        if ((action & Actions::ToggleMute) != 0)
        {
            Configuration::instance()->toggleMute();
            mCommandes.mute(Configuration::instance()->muted());
            retour = true;
        }
    }
    return retour;
}

bool Preamp::gerer()
{
    bool actionRealisee = false;
    uint16_t actionIhm = Actions::AucuneAction;
    if (mCompteurIt != mCompteurItPrecedent)
    {
        digitalWrite(ledPin, 0);
        mCompteurItPrecedent = mCompteurIt;
        mIhm.remoteActive(false);
        actionIhm = instance->mIhm.gerer(true);
        digitalWrite(ledPin, 1);
    }
    else
    {
        actionIhm = instance->mIhm.gerer(false);
    }
    actionRealisee |= traiterAction(actionIhm);
    uint16_t actionTelecommande = mTelecommande.gerer();
    if (actionTelecommande != Actions::AucuneAction)
    {
        mIhm.remoteActive(true);
        //mIhm.backlightOn();
        actionRealisee |= traiterAction(actionTelecommande);
    }
    mCommandes.gerer();
    Configuration::instance()->gerer();
    if (actionRealisee)
    {
        mIhm.backlightOn();
    }
    if (mDacActivePrecedent != mCommandes.dacActive())
    {
        mDacActivePrecedent = mCommandes.dacActive();
        mIhm.dacActivated(mDacActivePrecedent);
    }
    if (mMutedPrecedent != mCommandes.muted())
    {
        mMutedPrecedent = mCommandes.muted();
        mIhm.muted(mMutedPrecedent);
    }
#ifdef USE_MOTORIZED_POT
    if (mMotorOnPrecedent != mCommandes.motorOn())
    {
        mMotorOnPrecedent = mCommandes.motorOn();
        mIhm.motorOn(mMotorOnPrecedent);
    }
#endif
    return actionRealisee;
}

void Preamp::cadencer1s()
{
    instance->mCompteurIt++;
}
