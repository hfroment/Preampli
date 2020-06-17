#ifndef TELECOMMANDE_H
#define TELECOMMANDE_H

#include <Arduino.h>
#include <IRremote.h>
#include <IRremoteInt.h>

#include "actions.h"

class IRrecv;

class Telecommande
{
public:
    Telecommande();
    void init();

    typedef enum
    {
        Haut = 0x6604CFFA,
        Bas = 0x6604CFC6,
        Gauche = 0x6604CFD6,
        Droite = 0x6604CFE6,
        Enter = 0x6604CFF6

    }
    teTouches;

    uint16_t gerer();

private:
    IRrecv* mReceiver;
    decode_results mResults;

    static const uint8_t irPin = 11;

    static const uint16_t mDureeEntreDeuxTouches = 300; // ms
    unsigned long mDateDerniereTouche;
};


#endif // TELECOMMANDE_H
