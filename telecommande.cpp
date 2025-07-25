#include "options.h"

#include "telecommande.h"

#ifdef IRMP
#define IRMP_INPUT_PIN Telecommande::irPin
#define IRMP_PROTOCOL_NAMES 1
#include <irmpSelectMain15Protocols.h>
#include <irmp.hpp>
IRMP_DATA irmp_data[1];
#endif


Telecommande::Telecommande()
#ifndef IRMP
    : mDateDerniereTouche(0)
    #endif
{
#ifndef IRMP
    mReceiver = new IRrecv(irPin);
#endif
}

void Telecommande::init()
{
#ifndef IRMP
    mReceiver->enableIRIn();
#else
    irmp_init();
#endif
}

uint16_t Telecommande::gerer()
{
    uint16_t action = ActionsPreampli::AucuneAction;
#ifndef IRMP
    if (DECODED == mReceiver->decode(&mResults))
#else
    if (irmp_get_data(&irmp_data[0]))
#endif
    {
        //irmp_result_print(&irmp_data[0]);
        uint32_t touche = Aucune;
#ifndef IRMP
        touche = mResults.value;
#else
        touche = irmp_data[0].command;
#endif
        switch(touche)
        {
        case VolPlus_Harmony:
        case Haut_Samsung:
            action = ActionsPreampli::VolumePlus;
            break;
        case VolMoins_Harmony:
        case Bas_Samsung:
            action = ActionsPreampli::VolumeMoins;
            break;
        case ChMoins_Harmony:
        case Gauche_Samsung:
#ifdef IRMP
            if (!(irmp_data[0].flags & IRMP_FLAG_REPETITION))
#endif
            {
                action = ActionsPreampli::SelectionPrecedente;
                action |= ActionsPreampli::ActiverEntreeCourante;
            }
            break;
        case ChPlus_Harmony:
        case Droite_Samsung:
#ifdef IRMP
            if (!(irmp_data[0].flags & IRMP_FLAG_REPETITION))
#endif
            {
                action = ActionsPreampli::SelectionSuivante;
                action |= ActionsPreampli::ActiverEntreeCourante;
            }
            break;
        case PowerOff_Harmony:
        case PowerOff_Samsung:
            action = ActionsPreampli::PowerOff;
            break;
#ifdef IRMP
        case Mute_Harmony:
        case Enter_Samsung:
            if (!(irmp_data[0].flags & IRMP_FLAG_REPETITION))
            {
                action = ActionsPreampli::ToggleMute;
            }
            break;
#endif
        default:
            break;
        }
#ifndef IRMP
        if ((mDateDerniereTouche != 0) &&
                (millis() - mDateDerniereTouche < mDureeEntreDeuxTouches) &&
                (action != ActionsPreampli::VolumePlus) &&
                (action != ActionsPreampli::VolumeMoins))
        {
            action = ActionsPreampli::AucuneAction;
        }
        if ((action != ActionsPreampli::AucuneAction) &&
                (action != ActionsPreampli::VolumePlus) &&
                (action != ActionsPreampli::VolumeMoins))
        {
            mDateDerniereTouche = millis();
        }
        mReceiver->resume();
#endif
    }
    return action;
}
