#include "options.h"

#include "telecommande.h"

#ifdef IRMP
#define IRMP_INPUT_PIN Telecommande::irPin
#define IRMP_PROTOCOL_NAMES 1
#include <irmpSelectMain15Protocols.h>
#include <irmp.c.h>
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
            uint32_t touche = Aucune;
#ifndef IRMP
            touche = mResults.value;
#else
            touche = irmp_data[0].command;
#endif
            switch(touche)
            {
            case Haut:
                action = ActionsPreampli::VolumePlus;
                break;
            case Bas:
                action = ActionsPreampli::VolumeMoins;
                break;
            case Gauche:
                if (!(irmp_data[0].flags & IRMP_FLAG_REPETITION))
                {
                action = ActionsPreampli::SelectionPrecedente;
                action |= ActionsPreampli::ActiverEntreeCourante;
                }
                break;
            case Droite:
                if (!(irmp_data[0].flags & IRMP_FLAG_REPETITION))
                {
                action = ActionsPreampli::SelectionSuivante;
                action |= ActionsPreampli::ActiverEntreeCourante;
                }
                break;
            case PowerOff:
                action = ActionsPreampli::PowerOff;
                break;
#ifdef IRMP
            case Mute:
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
