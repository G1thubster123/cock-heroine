#ifndef TRIPHASEMODIFIER_H
#define TRIPHASEMODIFIER_H

#include "../stimsignalmodifier.h"

enum StrokeStyle {
    UP_DOWN_BEAT,
    DOWN_BEAT_UP
};

class TriphaseBeatStrokesModifier : public StimSignalModifier
{
public:
    TriphaseBeatStrokesModifier();
    void modify(StimSignalSample &sample) override;
private:
    int strokeLength;
    int getMaxTriphaseStrokeLength();
    StrokeStyle style;
};

#endif // TRIPHASEMODIFIER_H
