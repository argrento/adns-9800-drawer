#ifndef MOUSEDRAWER_H
#define MOUSEDRAWER_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>

#include "qcustomplot.h"

namespace Ui {
class MouseDrawer;
}

class MouseDrawer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MouseDrawer(QWidget *parent = 0);
    ~MouseDrawer();

    void updateStatus(const QString &);


public slots:
    void openPort();
    void closePort();
    void updatePlot();


private:
    Ui::MouseDrawer *ui;
    QSerialPort *serial;

    QLabel *statusLabel;
    QTimer *timer;

    QCPCurve *trajectory;

    // Trajectory points
    QVector<double> pointsX;
    QVector<double> pointsY;

    // Center point
    double centerX;
    double centerY;
};

#endif // MOUSEDRAWER_H
