#ifndef TRIPHASESTROKESSIGNALGENERATOR_H
#define TRIPHASESTROKESSIGNALGENERATOR_H

#include "stimsignalgenerator.h"

class TriphaseStrokesSignalGenerator : public StimSignalGenerator
{
public:
    explicit TriphaseStrokesSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
protected:
    void setModifiers() override;
    long getStopTimestamp() override;
    StimSignalSample * createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp) override;
};

#endif // TRIPHASESTROKESSIGNALGENERATOR_H
