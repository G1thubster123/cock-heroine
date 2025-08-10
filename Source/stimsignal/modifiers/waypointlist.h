#ifndef WAYPOINTLIST_H
#define WAYPOINTLIST_H

class Waypoint;
class Event;

class WaypointList : public QList<Waypoint *>
{
public:
    WaypointList();
    ~WaypointList();

    void plonkOnTheEnd(Waypoint *newValue);
    void insertTroughs(qreal troughLevel, QVector<Event> troughPositions);
    void squeezeInBetween(int insertionIndex, Waypoint * newValue);
};

#endif // WAYPOINTLIST_H
