#include "triphasevirtualstrokersignalgenerator.h"
#include "modifiers/phasesettermodifier.h"
#include "modifiers/triphasevirtualstrokerpositionmodifier.h"
#include "modifiers/triphasevirtualstrokerspeedmodifier.h"
#include "modifiers/waypointfollowermodifier.h"
#include "modifiers/progressincreasemodifier.h"
#include "modifiers/phaseinvertermodifier.h"
#include "modifiers/fadefromcoldmodifier.h"
#include "modifiers/breaksoftenermodifier.h"
#include "modifiers/channelbalancemodifier.h"
#include "mainwindow.h"
#include "optionsdialog.h"
#include "stereostimsignalsample.h"


TriphaseVirtualStrokerSignalGenerator::TriphaseVirtualStrokerSignalGenerator(QAudioFormat audioFormat, QObject *parent)
    :
      StimSignalGenerator(audioFormat, parent)
{
}

void TriphaseVirtualStrokerSignalGenerator::setModifiers()
{
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimTriphaseStrokerStartingFrequency(), OptionsDialog::getEstimTriphaseStrokerEndingFrequency()));
    modifiers.append(new TriphaseVirtualStrokerPositionModifier());
    QList<unsigned char> strokerStrokes{e_eventType::EVENT_STROKER_WAYPOINT};
    QVector<Event> eventsToUse = filteredEvents(strokerStrokes);
    auto waypoints = TriphaseVirtualStrokerSpeedModifier::generateWaypoints(eventsToUse);
    if (!waypoints->isEmpty())
        modifiers.append(new WaypointFollowerModifier(waypoints));
    else
        delete waypoints;
    modifiers.append(new ProgressIncreaseModifier());
    if (OptionsDialog::getEstimStrokerInvertStrokes())
        modifiers.append(new PhaseInverterModifier());
    modifiers.append(new FadeFromColdModifier());
    modifiers.append(new BreakSoftenerModifier());
    if (OptionsDialog::getEstimSignalPan())
        modifiers.append(new ChannelBalanceModifier());
}

long TriphaseVirtualStrokerSignalGenerator::getStopTimestamp()
{
    return mainWindow->totalPlayTime() + OptionsDialog::getEstimStrokerFadeOutTime();
}

StimSignalSample *TriphaseVirtualStrokerSignalGenerator::createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp)
{
    return new StereoStimSignalSample(wholeTimestamp, fractionalTimestamp);
}
