#include "waypointlist.h"
#include "optionsdialog.h"
#include "waypoint.h"
#include "event.h"

WaypointList::WaypointList()
    :
        QList<Waypoint *>()
{

}

WaypointList::~WaypointList()
{
    while (!isEmpty())
    {
        delete takeLast();
    }
}

void WaypointList::plonkOnTheEnd(Waypoint * newValue)
{
    Waypoint * oldEnd = nullptr;
    if (!isEmpty())
    {
        oldEnd = last();
        oldEnd->next = newValue;
    }
    newValue->previous = oldEnd;
    append(newValue);
}

void WaypointList::insertTroughs(qreal troughLevel, QVector<Event> troughPositions)
{
    double amountFadedInHalfAStrokeTime = 1 - troughLevel;
    double halfAStrokeTime = (double) OptionsDialog::getEstimMaxStrokeLength() / 2;
    double fadeTime = halfAStrokeTime / amountFadedInHalfAStrokeTime;
    int i = length() - 1;
    // Don't crash here by handling last waypoint
    plonkOnTheEnd(new Waypoint(at(i)->timestamp + fadeTime, 0));
    //iterate backwards because it means we don't have to skip values of i
    for (--i /* step back once, we did length - 1 already*/; i >= 0 ; --i)
    {
        //insert a trough _after_ this peak
        qreal lengthBetweenPeaks = (at(i+1)->timestamp - at(i)->timestamp);
        if (lengthBetweenPeaks > (fadeTime * 2))
        {
            //we've got time to completely fade out and back in, so we'll need a waypoint for the end of the fade-out and another for the beginning of the fade-in.
            qreal endOfFadeout = at(i)->timestamp + fadeTime;
            qreal beginningOfFadein = at(i+1)->timestamp - fadeTime;
            squeezeInBetween(i + 1, new Waypoint(beginningOfFadein, 0));
            //use i+1 again
            squeezeInBetween(i + 1, new Waypoint(endOfFadeout, 0));
        }
        else
        {
            // Check if any event from troughPositions falls between the two peaks
            long peakStart = at(i)->timestamp;
            long peakEnd = at(i+1)->timestamp;
            long troughTimestamp = -1;

            for (int j = 0; j < troughPositions.length(); ++j)
            {
                if (troughPositions[j].timestamp > peakStart && troughPositions[j].timestamp < peakEnd)
                {
                    troughTimestamp = troughPositions[j].timestamp;
                    break; // Use the first matching event
                }
            }

            qreal troughPosition;
            if (troughTimestamp != -1)
            {
                // Use the event timestamp for trough position
                troughPosition = troughTimestamp;
            }
            else
            {
                // Fall back to halfway between the peaks (original behavior)
                troughPosition = (at(i)->timestamp + at(i+1)->timestamp) / 2;
            }

            qreal troughValue = troughLevel;
            if (lengthBetweenPeaks < OptionsDialog::getEstimMaxStrokeLength())
            {
                troughValue = std::max(at(i)->multiplier, at(i+1)->multiplier) * troughLevel;
            }
            else
            {
                troughValue = (fadeTime*2 - lengthBetweenPeaks) / (fadeTime*2);
            }
            squeezeInBetween(i+1, new Waypoint(troughPosition, troughValue));
        }
    }
    //i is now -1. Insert the beginning of the fade-in that comes before the first waypoint
    if (at(0)->timestamp > 0)
    {
        qreal startOfFadeIn = 0;
        if (at(0)->timestamp - fadeTime > 0)
            startOfFadeIn = at(0)->timestamp - fadeTime;
        insert(0, new Waypoint(startOfFadeIn, 0));
        at(0)->next = at(1);
        at(1)->previous = at(0);
    }
}

void WaypointList::squeezeInBetween(int insertionIndex, Waypoint *newValue)
{
    Q_ASSERT(insertionIndex < length() - 1);
    insert(insertionIndex, newValue);
    at(insertionIndex-1)->next = at(insertionIndex);
    at(insertionIndex)->previous = at(insertionIndex-1);
    at(insertionIndex)->next = at(insertionIndex+1);
    at(insertionIndex+1)->previous = at(insertionIndex);
}
