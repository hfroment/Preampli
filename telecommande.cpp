
#include "telecommande.h"

Telecommande::Telecommande() :
    mDateDerniereTouche(0)
{
    mReceiver = new IRrecv(irPin);
}

void Telecommande::init()
{
    mReceiver->enableIRIn();
}

uint16_t Telecommande::gerer()
{
    uint16_t action = Actions::AucuneAction;
    if ((mDateDerniereTouche == 0) || (millis() - mDateDerniereTouche > mDureeEntreDeuxTouches))
    {
        uint32_t touche = 0;
        if (DECODED == mReceiver->decode(&mResults))
        {
            touche = mResults.value;
            switch(touche)
            {
            case Haut:
                action = Actions::VolumePlus;
                break;
            case Bas:
                action = Actions::VolumeMoins;
                break;
            case Gauche:
                action = Actions::SelectionPrecedente;
                action |= Actions::ActiverEntreeCourante;
                break;
            case Droite:
                action = Actions::SelectionSuivante;
                action |= Actions::ActiverEntreeCourante;
                break;
            case Enter:
                action = Actions::ToggleTape;
                break;
            default:
                break;
            }
            if (action != Actions::AucuneAction)
            {
                mDateDerniereTouche = millis();
            }
            mReceiver->resume();
        }
    }
    else
    {
        mReceiver->resume();
    }
    return action;
}
