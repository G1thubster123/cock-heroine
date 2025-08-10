#include "triphasevirtualstrokerspeedmodifier.h"
#include "stimsignal/stimsignalsample.h"
#include "mainwindow.h"
#include "globals.h"
#include "optionsdialog.h"
#include "waypoint.h"

TriphaseVirtualStrokerSpeedModifier::TriphaseVirtualStrokerSpeedModifier()
{
    //configuration goes here
}

void TriphaseVirtualStrokerSpeedModifier::modify(StimSignalSample &sample)
{
    //content goes here
}

WaypointList * TriphaseVirtualStrokerSpeedModifier::generateWaypoints(QVector<Event> eventsToUse)
{
    WaypointList * list = new WaypointList();

    if (eventsToUse.length() < 1) {
        return list;
    }

    long maxTransitionLength = OptionsDialog::getEstimStrokerMaxVolumeTransitionLength();
    int fadeOutTime = OptionsDialog::getEstimStrokerFadeOutTime();
    int fadeInTime = OptionsDialog::getEstimStrokerFadeInTime();
    qreal minSpeed = OptionsDialog::getEstimStrokerMinSpeed();
    qreal maxSpeed = OptionsDialog::getEstimStrokerMaxSpeed();
    qreal minVolume = OptionsDialog::getEstimStrokerMinVolume() / 100.0;
    qreal maxVolume = OptionsDialog::getEstimStrokerMaxVolume() / 100.0;

    // Add virtual events at start and end to simplify silent period handling
    QVector<Event> extendedEvents;

    // Virtual start event (fadeInTime before first real event)
    long startTime = eventsToUse[0].timestamp - fadeInTime;
    extendedEvents.append(Event(startTime, 0, eventsToUse[0].value));

    // Add all real events
    extendedEvents.append(eventsToUse);

    // Virtual end event (fadeOutTime after last real event)
    long endTime = eventsToUse.last().timestamp + fadeOutTime;
    extendedEvents.append(Event(endTime, 0, eventsToUse.last().value));

    // Calculate speeds and identify slow periods
    QVector<qreal> speeds;
    QVector<bool> isSilentPeriod;

    for (int i = 0; i < extendedEvents.length() - 1; ++i) {
        long periodDuration = extendedEvents[i + 1].timestamp - extendedEvents[i].timestamp;

        if (periodDuration <= 0) {
            speeds.append(0);
            isSilentPeriod.append(true);
            continue;
        }

        qreal valueChange = qAbs(extendedEvents[i + 1].value - extendedEvents[i].value);
        qreal timeInSeconds = periodDuration / 1000.0;
        qreal speed = valueChange / timeInSeconds;
        speeds.append(speed);
        isSilentPeriod.append(speed < minSpeed);
    }

    // Combine adjacent silent periods by marking intermediate events for removal
    QVector<bool> keepEvent(extendedEvents.length(), true);

    for (int i = 1; i < extendedEvents.length() - 1; ++i) {
        // Remove event if both adjacent periods are silent
        if (i > 0 && i < isSilentPeriod.length() &&
            isSilentPeriod[i - 1] && isSilentPeriod[i]) {
            keepEvent[i] = false;
        }
    }

    // Create final event list and recalculate periods
    QVector<Event> finalEvents;
    QVector<qreal> periodVolumes;
    QVector<bool> finalIsSilent;

    for (int i = 0; i < extendedEvents.length(); ++i) {
        if (keepEvent[i]) {
            finalEvents.append(extendedEvents[i]);
        }
    }

    // Recalculate periods for final event list
    for (int i = 0; i < finalEvents.length() - 1; ++i) {
        long periodDuration = finalEvents[i + 1].timestamp - finalEvents[i].timestamp;

        if (periodDuration <= 0) {
            periodVolumes.append(0);
            finalIsSilent.append(true);
            continue;
        }

        qreal valueChange = qAbs(finalEvents[i + 1].value - finalEvents[i].value);
        qreal timeInSeconds = periodDuration / 1000.0;
        qreal speed = valueChange / timeInSeconds;

        bool isSilent = speed < minSpeed;
        finalIsSilent.append(isSilent);

        if (isSilent) {
            periodVolumes.append(0);
        } else {
            // Map speed to volume
            qreal volume;
            if (speed >= maxSpeed) {
                volume = maxVolume;
            } else {
                qreal speedRatio = (speed - minSpeed) / (maxSpeed - minSpeed);
                volume = minVolume + (speedRatio * (maxVolume - minVolume));
            }
            periodVolumes.append(volume);
        }
    }

    // Generate waypoints
    for (int i = 0; i < finalEvents.length() - 1; ++i) {
        long periodStart = finalEvents[i].timestamp;
        long periodEnd = finalEvents[i + 1].timestamp;
        long periodDuration = periodEnd - periodStart;

        if (finalIsSilent[i]) {
            // Handle silent period
            qreal prevVolume = (i > 0) ? periodVolumes[i - 1] : 0;
            qreal nextVolume = (i < periodVolumes.length() - 1) ? periodVolumes[i + 1] : 0;

            if (periodDuration > fadeOutTime + fadeInTime) {
                // Long silent period - full fade out/in
                list->plonkOnTheEnd(new Waypoint(periodStart, prevVolume));
                list->plonkOnTheEnd(new Waypoint(periodStart + fadeOutTime, 0));
                list->plonkOnTheEnd(new Waypoint(periodEnd - fadeInTime, 0));
                list->plonkOnTheEnd(new Waypoint(periodEnd, nextVolume));
            } else {
                // Short silent period - single intermediate waypoint
                qreal lengthRatio = (qreal)periodDuration / (fadeOutTime + fadeInTime);
                qreal waypointVolume = minVolume * (1.0 - lengthRatio);

                qreal positionRatio = (qreal)fadeOutTime / (fadeOutTime + fadeInTime);
                long waypointTime = periodStart + (long)(periodDuration * positionRatio);

                list->plonkOnTheEnd(new Waypoint(periodStart, prevVolume));
                list->plonkOnTheEnd(new Waypoint(waypointTime, waypointVolume));
                list->plonkOnTheEnd(new Waypoint(periodEnd, nextVolume));
            }
        } else {
            // Handle normal period
            qreal volume = periodVolumes[i];

            // Calculate transition lengths with adjacent non-silent periods
            long prevTransitionLength = 0;
            long nextTransitionLength = 0;

            if (i > 0 && !finalIsSilent[i - 1]) {
                long prevPeriodDuration = periodStart - finalEvents[i - 1].timestamp;
                prevTransitionLength = qMin(prevPeriodDuration, periodDuration);
                prevTransitionLength = qMin(prevTransitionLength, maxTransitionLength);
            }

            if (i < finalEvents.length() - 2 && !finalIsSilent[i + 1]) {
                long nextPeriodDuration = finalEvents[i + 2].timestamp - periodEnd;
                nextTransitionLength = qMin(periodDuration, nextPeriodDuration);
                nextTransitionLength = qMin(nextTransitionLength, maxTransitionLength);
            }

            long firstWaypointTime = periodStart + (prevTransitionLength / 2);
            long lastWaypointTime = periodEnd - (nextTransitionLength / 2);

            if (firstWaypointTime == lastWaypointTime) {
                list->plonkOnTheEnd(new Waypoint(firstWaypointTime, volume));
            } else {
                list->plonkOnTheEnd(new Waypoint(firstWaypointTime, volume));
                list->plonkOnTheEnd(new Waypoint(lastWaypointTime, volume));
            }
        }
    }

    return list;
}
