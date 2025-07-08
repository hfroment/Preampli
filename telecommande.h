#ifndef TELECOMMANDE_H
#define TELECOMMANDE_H

#include <Arduino.h>

#include "options.h"

#ifndef IRMP
#include <IRremote.h>
#include <IRremoteInt.h>
#endif

#include "actions.h"

#ifndef IRMP
class IRrecv;
#endif

class Telecommande
{
public:
    Telecommande();
    void init();

    typedef enum
    {
        Aucune = 0,

#ifndef IRMP
        Haut = 0x6604CFFA,
        Bas = 0x6604CFC6,
        Gauche = 0x6604CFD6,
        Droite = 0x6604CFE6,
        PowerOff = 0x66666666,
        Enter = 0x6604CFF6
#else
        VolPlus_Harmony = 0x410,
        VolMoins_Harmony = 0x411,
        ChMoins_Harmony = 0x413,
        ChPlus_Harmony = 0x412,
        PowerOff_Harmony = 0x40C,
        Enter_Harmony = 0x422,
        Mute_Harmony = 0x40E,
        //
        Haut_Samsung = 0x410,
        Bas_Samsung = 0x411,
        Gauche_Samsung = 0x413,
        Droite_Samsung = 0x412,
        PowerOff_Samsung = 0x40C,
        Enter_Samsung = 0x422,
        Mute_Samsung = 0x40E
#endif

    }
    teTouches;

    uint16_t gerer();

    static const uint8_t irPin = 11;

private:
#ifndef IRMP
    IRrecv* mReceiver;
    decode_results mResults;

    static const uint16_t mDureeEntreDeuxTouches = 300; // ms
    unsigned long mDateDerniereTouche;
#endif
};


#endif // TELECOMMANDE_H
