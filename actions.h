#ifndef ACTIONS_H
#define ACTIONS_H

class Actions
{
public:
    typedef enum
    {
        AucuneAction = 0,
        VolumePlus = 0x01,
        VolumeMoins = 0x02,
        SelectionSuivante = 0x04,
        SelectionPrecedente = 0x10,
        ActiverTape = 0x20,
        DesactiverTape = 0x40,
        ToggleTape = 0x80,
        ActiverEntreeCourante = 0x0100,
        ToggleMute = 0x0200,
        //ToggleCorrected = 0x0400,

        Retour = 0x8000
    }
    teAction;
};

#endif // ACTIONS_H
