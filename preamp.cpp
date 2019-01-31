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
//    mErreurPrecedent(false),
    mVerouillagePrecedent(false),
    mDacActivePrecedent(false),
    m96kHzPrecedent(false)
//    mEtatXPrecedent(false)
{
    instance = this;

    pinMode(ledPin, OUTPUT);
    // On met en place l'IT seconde
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(Preamp::instance->cadencer1s);
}
void Preamp::init()
{
    mIhm.init("Shaton Preamp", "version : " + mVersionString);
    // à faire APRES l'IHM à cause de wire
    mCommandes.init();
    mTelecommande.init();
    // On charge la configuration
    Configuration::instance()->charger();
    // On affiche les états
    mCommandes.etats(mVerouillagePrecedent, m96kHzPrecedent);
    mDacActivePrecedent = mCommandes.dacActive();
//    mIhm.afficherErreur(mErreurPrecedent);
    mIhm.afficherVerouillage(mVerouillagePrecedent, mDacActivePrecedent);
    mIhm.afficher96Khz(m96kHzPrecedent, mDacActivePrecedent);
//    mIhm.afficherEtatX(mEtatXPrecedent);

    mCommandes.selectionnerEntree(Configuration::instance()->entreeActive());
    mCommandes.mute(Configuration::instance()->muted());
   // mCommandes.corrected(Configuration::instance()->corrected());
}

bool Preamp::traiterAction(uint16_t action)
{
    bool retour = false;

    if (action != Actions::AucuneAction)
    {
        if ((action & Actions::VolumePlus) == Actions::VolumePlus)
        {
            mCommandes.volumePlus();
        }
        if ((action & Actions::VolumeMoins) == Actions::VolumeMoins)
        {
            mCommandes.volumeMoins();
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
        if ((action & Actions::ActiverTape) != 0)
        {
            mCommandes.activerTape(true);
            retour = true;
        }
        if ((action & Actions::DesactiverTape) != 0)
        {
            mCommandes.activerTape(false);
            retour = true;
        }
        if ((action & Actions::ToggleTape) != 0)
        {
            mCommandes.activerTape(Configuration::instance()->toggleTape());
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
//        if ((action & Actions::ToggleCorrected) != 0)
//        {
//            Configuration::instance()->toggleFreeBool();
//            mCommandes.corrected(Configuration::instance()->corrected());
//            retour = true;
//        }
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
//        // debug
//        mIhm.displayStatus(String(mCommandes.tensionLueEnMv()) + " " + String(mCommandes.tensionRefEnMv()) + "mV");
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
        actionRealisee |= traiterAction(actionTelecommande);
    }
    mCommandes.gerer();
    Configuration::instance()->gerer();
    if (actionRealisee)
    {
        mIhm.backlightOn();
    }
    // On affiche les états
    bool verouillage;
    bool _96kHz;
    mCommandes.etats(verouillage, _96kHz);
    if ((mVerouillagePrecedent != verouillage) || (mDacActivePrecedent != mCommandes.dacActive()))
    {
        mVerouillagePrecedent = verouillage;
        mIhm.afficherVerouillage(verouillage, mCommandes.dacActive());
        mIhm.backlightOn();
    }
    if ((m96kHzPrecedent != _96kHz) || (mDacActivePrecedent != mCommandes.dacActive()))
    {
        m96kHzPrecedent = _96kHz;
        mIhm.afficher96Khz(_96kHz, mCommandes.dacActive());
        mIhm.backlightOn();
    }
    mDacActivePrecedent = mCommandes.dacActive();
    return actionRealisee;
}

void Preamp::cadencer1s()
{
    instance->mCompteurIt++;
}
