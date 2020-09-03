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
    mCompteurItSeconde(0),
    mCompteurItPrecedent(255),
    mCommandes(*new Commandes()),
    mIhm(*new IHM()),
    mTelecommande(* new Telecommande()),
    mDacActivePrecedent(false),
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
    Serial.println("Bonjour. V " + mVersionString);
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
    gererServitudes();
    mCommandes.mute(Configuration::instance()->muted());
    // On lit le volume sauvegardé pour l'ppliquer
    uint8_t left;
    uint8_t right;
    Configuration::instance()->volumeChanged(left, right);
    mCommandes.changeVolume(left, right);
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
        if ((action & ActionsPreampli::ForceHdmi) != 0)
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
        ActionsServitudes::teTypeActionServitudes actionToslink = ActionsServitudes::Off;
        ActionsServitudes::teTypeActionServitudes actionUsb = ActionsServitudes::Off;
        ActionsServitudes::teTypeActionServitudes alimEcran = ActionsServitudes::Off;
        ActionsServitudes::teTypeActionServitudes alimRpi = ActionsServitudes::Off;
        ActionsServitudes::teTypeActionServitudes alimHdmi = ActionsServitudes::Off;
        ActionsServitudes::teTypeActionServitudes alimD30 = ActionsServitudes::On;
        ActionsServitudes::teCibleActionServitudes entreeHdmi = ActionsServitudes::Hdmi_1;

        switch (Configuration::instance()->entreeActive())
        {
        case Configuration::EntreeAnalogique_1:
        case Configuration::EntreeAnalogique_2:
        case Configuration::EntreeAnalogique_3:
            alimD30 = ActionsServitudes::Off;
        case Configuration::EntreeSpdif_1:
        case Configuration::EntreeSpdif_2:
        case Configuration::EntreeSpdif_3:
        case Configuration::EntreeSpdif_4:
            break;
        case Configuration::EntreeUsb:
            actionUsb = ActionsServitudes::On;
            alimEcran = ActionsServitudes::On;
            alimRpi = ActionsServitudes::On;
            alimHdmi = ActionsServitudes::On;
            entreeHdmi = entreeHdmiRpi;
            break;
        case Configuration::EntreeToslink:
            actionToslink = ActionsServitudes::On;
            alimEcran = ActionsServitudes::On;
            alimHdmi = ActionsServitudes::On;
            entreeHdmi = entreeHdmiChromeCast;
            break;
        default:
            break;
        }
        mCommandes.envoyerCommandeServitude(ActionsServitudes::DacUSB, actionUsb);
        mCommandes.envoyerCommandeServitude(ActionsServitudes::DacToslink, actionToslink);
        mCommandes.envoyerCommandeServitude(ActionsServitudes::AlimKodi, alimRpi);
        mCommandes.envoyerCommandeServitude(ActionsServitudes::AlimD30, alimD30);
        if (mIhm.hdmiCourante() > 0)
        {
            // forçage
            switch (mIhm.hdmiCourante())
            {
            default:
            case 1:
                entreeHdmi = ActionsServitudes::Hdmi_1;
                break;
            case 2:
                entreeHdmi = ActionsServitudes::Hdmi_2;
                break;
            case 3:
                entreeHdmi = ActionsServitudes::Hdmi_3;
                break;
            case 4:
                entreeHdmi = ActionsServitudes::Hdmi_4;
                break;
            case 5:
                entreeHdmi = ActionsServitudes::Hdmi_5;
                break;
            }
            alimEcran = ActionsServitudes::On;
            alimHdmi = ActionsServitudes::On;
        }
        mCommandes.envoyerCommandeServitude(entreeHdmi, ActionsServitudes::On);
        mCommandes.envoyerCommandeServitude(ActionsServitudes::AlimEcran, alimEcran);
        mCommandes.envoyerCommandeServitude(ActionsServitudes::AlimHdmi, alimHdmi);
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
    // On lit le volume sauvegardé pour l'appliquer
    uint8_t left;
    uint8_t right;
    if (mCommandes.volumeChanged(left, right))
    {
        Configuration::instance()->changeVolume(left, right);
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
