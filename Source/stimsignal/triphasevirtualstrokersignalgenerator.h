#ifndef TRIPHASEVIRTUALSTROKERSIGNALGENERATOR_H
#define TRIPHASEVIRTUALSTROKERSIGNALGENERATOR_H

#include "stimsignalgenerator.h"

class TriphaseVirtualStrokerSignalGenerator : public StimSignalGenerator
{
public:
    explicit TriphaseVirtualStrokerSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
protected:
    void setModifiers() override;
    long getStopTimestamp() override;
    StimSignalSample * createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp) override;
};

#endif // TRIPHASEVIRTUALSTROKERSIGNALGENERATOR_H
