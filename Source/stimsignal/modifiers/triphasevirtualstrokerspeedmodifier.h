#ifndef TRIPHASEVIRTUALSTROKERSPEEDMODIFIER_H
#define TRIPHASEVIRTUALSTROKERSPEEDMODIFIER_H

#include "../stimsignalmodifier.h"
#include "waypointlist.h"

class Event;
class TriphaseVirtualStrokerSpeedModifier : public StimSignalModifier
{
public:
    TriphaseVirtualStrokerSpeedModifier();
    void modify(StimSignalSample &sample) override;
    static WaypointList * generateWaypoints(QVector<Event> eventsToUse);
};

#endif // TRIPHASEVIRTUALSTROKERSPEEDMODIFIER_H
