#ifndef CHART_H
#define CHART_H

#include "Bars.h"

#include <QDateTime>

class Chart : public Bars {

public:
    QDateTime getLastDate();

};
#endif // CHART_H
