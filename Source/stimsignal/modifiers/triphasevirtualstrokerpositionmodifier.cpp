#include "triphasevirtualstrokerpositionmodifier.h"
#include "stimsignal/stimsignalsample.h"
#include "mainwindow.h"

TriphaseVirtualStrokerPositionModifier::TriphaseVirtualStrokerPositionModifier()
{
    //possibly no configuration here
}

void TriphaseVirtualStrokerPositionModifier::modify(StimSignalSample &sample)
{
    // Default to no rotation
    qreal phaseDifference = 0;

    // Find the waypoints before and after the current sample
    Event* nextEvent = mainWindow->getNextEventAfter(sample.totalTimestamp());
    Event* previousEvent = mainWindow->getLastEventBefore(sample.totalTimestamp());

    // If we have both waypoints, we can interpolate between them
    if (nextEvent != nullptr && previousEvent != nullptr)
    {
        long nextEventTimestamp = nextEvent->timestamp;
        long previousEventTimestamp = previousEvent->timestamp;

        // Get the percentage values from the waypoints (1-100)
        short nextPercentage = nextEvent->value;
        short previousPercentage = previousEvent->value;

        // Calculate how far we are between waypoints (0.0 to 1.0)
        qreal totalInterval = nextEventTimestamp - previousEventTimestamp;
        qreal progress = 0;

        if (totalInterval > 0)
        {
            progress = static_cast<qreal>(sample.totalTimestamp() - previousEventTimestamp) / totalInterval;
        }

        // Convert percentages to phase differences (0-1 range representing 0-360 degrees)
        // 100% = 0.0 (straight up)
        // 0% = 0.5 (straight down)
        // 50% = 0.25 or 0.75 (sides)
        qreal prevPhaseDiff = convertPercentageToPhase(previousPercentage);
        qreal nextPhaseDiff = convertPercentageToPhase(nextPercentage);

        // Determine the shortest rotation path between the two positions
        qreal rotationDistance = calculateShortestRotation(prevPhaseDiff, nextPhaseDiff);

        // Calculate the current phase difference by interpolating
        phaseDifference = prevPhaseDiff + (rotationDistance * progress);

        // Ensure phase difference is within [0, 1)
        phaseDifference = normalisePhase(phaseDifference);
    }
    else if (nextEvent != nullptr)
    {
        // Only next waypoint available, use its percentage
        phaseDifference = convertPercentageToPhase(nextEvent->value);
    }
    else if (previousEvent != nullptr)
    {
        // Only previous waypoint available, use its percentage
        phaseDifference = convertPercentageToPhase(previousEvent->value);
    }

    // Apply the phase difference to the sample
    sample.setPhase(1, sample.getPrimaryPhase() + phaseDifference);
}

// Helper function to convert percentage (1-100) to phase difference (0-1)
qreal TriphaseVirtualStrokerPositionModifier::convertPercentageToPhase(short percentage)
{
    // Convert from percentage (1-100) to normalised value (0-1)
    qreal normalisedPercentage = percentage / 100.0;

    // Map from percentage to phase:
    // 100% (straight up) = 0.0
    // 0% (straight down) = 0.5
    // Linear mapping between these points
    return 0.5 - (normalisedPercentage * 0.5);
}

// Helper function to calculate the shortest rotation between two phases
qreal TriphaseVirtualStrokerPositionModifier::calculateShortestRotation(qreal fromPhase, qreal toPhase)
{
    // Special case: if we're at straight up (0.0) and moving away, always go clockwise
    if (std::abs(fromPhase) < 0.001 && toPhase > 0)
    {
        return toPhase;
    }

    // Special case: if we're at straight down (0.5) and moving away, always go clockwise
    if (std::abs(fromPhase - 0.5) < 0.001 && toPhase != 0.5)
    {
        // If target is less than 0.5, we need to go through 1.0 to maintain clockwise direction
        if (toPhase < 0.5)
        {
            return (1.0 - fromPhase) + toPhase;
        }
        else
        {
            return toPhase - fromPhase;
        }
    }

    // Calculate direct distance
    qreal directDistance = toPhase - fromPhase;

    // Normalise to [-0.5, 0.5] range for shortest path
    if (directDistance > 0.5)
    {
        directDistance -= 1.0;
    }
    else if (directDistance < -0.5)
    {
        directDistance += 1.0;
    }

    return directDistance;
}

// Helper function to ensure phase is in [0, 1) range
qreal TriphaseVirtualStrokerPositionModifier::normalisePhase(qreal phase)
{
    // Ensure phase is in [0, 1) range
    phase = fmod(phase, 1.0);
    if (phase < 0)
    {
        phase += 1.0;
    }
    return phase;
}
