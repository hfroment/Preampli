#ifndef TELECOMMANDE_H
#define TELECOMMANDE_H

#include <Arduino.h>

#ifndef IRMP
#include <IRremote.h>
#include <IRremoteInt.h>
#endif

#include "options.h"

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
        Haut = 0x97,
        Bas = 0x98,
        Gauche = 0x9A,
        Droite = 0x99,
        PowerOff = 0x40C,
        Enter = 0x422,
        Mute = 0x40E
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
