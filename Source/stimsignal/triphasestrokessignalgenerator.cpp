#include "triphasestrokessignalgenerator.h"
#include "modifiers/phasesettermodifier.h"
#include "modifiers/triphasebeatstrokesmodifier.h"
#include "modifiers/triphasebeatproximitymodifier.h"
#include "modifiers/progressincreasemodifier.h"
#include "modifiers/boostfaststrokesmodifier.h"
#include "modifiers/phaseinvertermodifier.h"
#include "modifiers/fadefromcoldmodifier.h"
#include "modifiers/breaksoftenermodifier.h"
#include "modifiers/channelbalancemodifier.h"
#include "mainwindow.h"
#include "optionsdialog.h"
#include "stereostimsignalsample.h"


TriphaseStrokesSignalGenerator::TriphaseStrokesSignalGenerator(QAudioFormat audioFormat, QObject *parent)
    :
      StimSignalGenerator(audioFormat, parent)
{
}

void TriphaseStrokesSignalGenerator::setModifiers()
{
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimTriphaseStartingFrequency(), OptionsDialog::getEstimTriphaseEndingFrequency()));
    modifiers.append(new TriphaseBeatStrokesModifier());
    modifiers.append(new TriphaseBeatProximityModifier());
    modifiers.append(new ProgressIncreaseModifier());
    modifiers.append(new BoostFastStrokesModifier());
    if (OptionsDialog::getEstimInvertStrokes())
        modifiers.append(new PhaseInverterModifier());
    modifiers.append(new FadeFromColdModifier());
    modifiers.append(new BreakSoftenerModifier());
    if (OptionsDialog::getEstimSignalPan())
        modifiers.append(new ChannelBalanceModifier());
}

long TriphaseStrokesSignalGenerator::getStopTimestamp()
{
    return mainWindow->totalPlayTime() + OptionsDialog::getEstimBeatFadeOutDelay() + OptionsDialog::getEstimBeatFadeOutTime();
}

StimSignalSample *TriphaseStrokesSignalGenerator::createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp)
{
    return new StereoStimSignalSample(wholeTimestamp, fractionalTimestamp);
}
