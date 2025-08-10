#ifndef TRIPHASEVIRTUALSTROKERPOSITIONMODIFIER_H
#define TRIPHASEVIRTUALSTROKERPOSITIONMODIFIER_H

#include "../stimsignalmodifier.h"

class TriphaseVirtualStrokerPositionModifier : public StimSignalModifier
{
public:
    TriphaseVirtualStrokerPositionModifier();
    void modify(StimSignalSample &sample) override;
private:
    qreal convertPercentageToPhase(short percentage);
    qreal calculateShortestRotation(qreal fromPhase, qreal toPhase);
    qreal normalisePhase(qreal phase);
};

#endif // TRIPHASEVIRTUALSTROKERPOSITIONMODIFIER_H
