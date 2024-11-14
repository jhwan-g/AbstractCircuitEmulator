#ifndef ASSEMBLECLICKED_H
#define ASSEMBLECLICKED_H

#include <QObject>
#include <QDebug>

class assembleClicked : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE void Run();

    assembleClicked();
    virtual ~assembleClicked();  // Declare a virtual destructor
    virtual void someFunction(); // Declare a virtual function
};

#endif // ASSEMBLECLICKED_H
