#include <TimerOne.h>

#include "ihm.h"
#include "commandes.h"
#include "configuration.h"
#include "telecommande.h"

#include "preamp.h"


Preamp *Preamp::instance = NULL;
const String Preamp::mVersionString = "1.1.0";

Preamp::Preamp() :
    mCompteurIt(0),
    mCompteurItSeconde(0),
    mCompteurItPrecedent(255),
    mCommandes(*new Commandes()),
    mIhm(*new IHM()),
    mTelecommande(* new Telecommande()),
    mMutedPrecedent(false),
    mServitudesOnPrecedent(false)
#ifdef USE_MOTORIZED_POT
    , mMotorOnPrecedent(false)
#endif
{
    instance = this;

    //pinMode(ledPin, OUTPUT);
    // On met en place l'IT seconde
    Timer1.initialize(mPeriodeTimer1);
    Timer1.attachInterrupt(Preamp::instance->itFunction);

    Serial.begin(115200);

}
void Preamp::init()
{
    if (Configuration::instance()->salon())
    {
#ifdef SERIAL_ON
        Serial.println("Salon V " + mVersionString);
#endif
        mIhm.init("Salon", "V " + mVersionString);
    }
    else
    {
#ifdef SERIAL_ON
        Serial.println("Bonjour V " + mVersionString);
#endif
        mIhm.init("Bonjour", "V " + mVersionString);
    }
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
    gererServitudes();
    mCommandes.mute(Configuration::instance()->muted());
    // On lit le volume sauvegardé pour l'ppliquer
    int8_t volume;
    int8_t balance;
    Configuration::instance()->volumeChanged(volume, balance);
    mCommandes.changeVolume(volume, balance);
#ifdef USE_MOTORIZED_POT
    mMotorOnPrecedent = mCommandes.motorOn();
#endif
}

bool Preamp::traiterAction(uint16_t action)
{
    bool retour = false;

    if (action != ActionsPreampli::AucuneAction)
    {
        if ((action & ActionsPreampli::PowerOff) == ActionsPreampli::PowerOff)
        {
            mCommandes.envoyerCommandeServitude(ActionsServitudes::PowerOff, ActionsServitudes::On);
            retour = true;
        }
        if ((action & ActionsPreampli::VolumePlus) == ActionsPreampli::VolumePlus)
        {
            mCommandes.volumePlus();
            retour = true;
        }
        if ((action & ActionsPreampli::VolumeMoins) == ActionsPreampli::VolumeMoins)
        {
            mCommandes.volumeMoins();
            retour = true;
        }
        if ((action & ActionsPreampli::BalanceDroite) == ActionsPreampli::BalanceDroite)
        {
            mCommandes.balanceDroite();
            retour = true;
        }
        if ((action & ActionsPreampli::BalanceGauche) == ActionsPreampli::BalanceGauche)
        {
            mCommandes.balanceGauche();
            retour = true;
        }
        if ((action & ActionsPreampli::SelectionPrecedente) != 0)
        {
            Configuration::instance()->selectionnerEntreePrecedente();
            retour = true;
        }
        if ((action & ActionsPreampli::SelectionSuivante) != 0)
        {
            Configuration::instance()->selectionnerEntreeSuivante();
            retour = true;
        }
        if ((action & ActionsPreampli::ActiverEntreeCourante) != 0)
        {
            Configuration::instance()->activerEntreeCourante();
            mCommandes.selectionnerEntree(Configuration::instance()->entreeActive());
            retour = true;
        }
        if ((action & ActionsPreampli::ToggleMute) != 0)
        {
            Configuration::instance()->toggleMute();
            mCommandes.mute(Configuration::instance()->muted());
            retour = true;
        }
        if ((action & ActionsPreampli::Refresh) != 0)
        {
            // Juste pour garder l'affichage actif
            retour = true;
        }
    }
    return retour;
}

void Preamp::gererServitudes()
{
    if (mCommandes.servitudesPresentes())
    {
        if (!mServitudesOnPrecedent)
        {
            mServitudesOnPrecedent = true;
            mIhm.linked(mServitudesOnPrecedent);
        }
        // analyser l'état du préampli et piloter les servitudes en conséquence
        // ==> on ne fait plus

        // On allume la lumière
        mCommandes.envoyerCommandeServitude(ActionsServitudes::Aux12V_1, ActionsServitudes::On);
        mCommandes.envoyerCommandeServitude(ActionsServitudes::Aux12V_2, ActionsServitudes::On);
    }
    else
    {
        if (mServitudesOnPrecedent)
        {
            mServitudesOnPrecedent = false;
            mIhm.linked(mServitudesOnPrecedent);
        }
    }
}

bool Preamp::gerer()
{
    bool actionRealisee = false;
    uint16_t actionIhm = ActionsPreampli::AucuneAction;
    bool seconde = mCompteurItSeconde != mCompteurItPrecedent;
    if (seconde)
    {
        mCompteurItPrecedent = mCompteurItSeconde;
        mIhm.remoteActive(false);
        actionIhm = instance->mIhm.gerer(true);
    }
    else
    {
        actionIhm = instance->mIhm.gerer(false);
    }
    actionRealisee |= traiterAction(actionIhm);
    uint16_t actionTelecommande = mTelecommande.gerer();
    if (actionTelecommande != ActionsPreampli::AucuneAction)
    {
        mIhm.remoteActive(true);
        //mIhm.backlightOn();
        actionRealisee |= traiterAction(actionTelecommande);
    }
    if (actionRealisee)
    {
        mIhm.backlightOn();
    }
    if (seconde)
    {
        gererServitudes();
    }
    mCommandes.gerer();
    Configuration::instance()->gerer();
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
    // On lit le volume sauvegardé pour l'appliquer
    int8_t volume;
    int8_t balance;
    if (mCommandes.volumeChanged(volume, balance))
    {
        Configuration::instance()->changeVolume(volume, balance);
    }

    return actionRealisee;
}

void Preamp::itFunction()
{
    instance->mCompteurIt++;
    if (instance->mCompteurIt == instance->mCompteurPour1s)
    {
        instance->mCompteurIt = 0;
        instance->mCompteurItSeconde++;
    }
    instance->mIhm.gererIt();
}
