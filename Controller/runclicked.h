#ifndef RUNCLICKED_H
#define RUNCLICKED_H

#include <QObject>
#include "Model/simulator.h"

class runClicked : public QObject{
    Q_OBJECT
public:
    runClicked();
    Q_INVOKABLE void Run();

protected:
    sImulator simulator;
    bool is_simulator_made;
};

#endif // RUNCLICKED_H
