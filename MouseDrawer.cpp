#include "MouseDrawer.h"
#include "ui_MouseDrawer.h"

MouseDrawer::MouseDrawer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MouseDrawer),
    serial(new QSerialPort),
    statusLabel(new QLabel),
    timer(new QTimer),
    centerX(0.0),
    centerY(0.0)
{
    ui->setupUi(this);

#if defined(Q_OS_LINUX)
    ui->portNameEdit->setText("/dev/ttyACM0");
#elif defined(Q_OS_WIN)
    ui->portNameEdit->setText("COM4");
#endif

    // Set up plot properties
    //ui->customPlot->addGraph();
    //ui->customPlot->graph(0)->setPen(QPen(Qt::red));
    //ui->customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    //ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    //ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    //ui->customPlot->graph(0)->setData(pointsX, pointsY);

    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");

    ui->customPlot->xAxis->setRange(-1, 1);
    ui->customPlot->yAxis->setRange(-1, 1);
    ui->customPlot->replot();

    trajectory = new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis);
    trajectory->setPen(QPen(Qt::red));

    // Add status label
    ui->statusBar->addWidget(statusLabel);
    updateStatus(tr("Ready"));

    connect(ui->openPortButton, &QPushButton::clicked, this, &MouseDrawer::openPort);
    connect(ui->closePortButton, &QPushButton::clicked, this, &MouseDrawer::closePort);
    connect(ui->actionExit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
}

void MouseDrawer::openPort() {

    QString portName = ui->portNameEdit->text();
    int portBaudrate = ui->portBaudRateSelector->currentText().toInt();

    serial->setPortName(portName);
    serial->setBaudRate(portBaudrate);

    if (serial->open(QIODevice::ReadOnly)) {
        // Lock port parameters
        ui->openPortButton->setEnabled(false);
        ui->portNameEdit->setEnabled(false);
        ui->portBaudRateSelector->setEnabled(false);

        // Unlock close port button
        ui->closePortButton->setEnabled(true);

        qDebug() << "Open port " << portName << ", baudrate " << portBaudrate;
        updateStatus(tr("Connected to %1").arg(serial->portName()));

        centerX = 0.0;
        centerY = 0.0;
        pointsX.clear();
        pointsY.clear();

        connect(timer, &QTimer::timeout, this, &MouseDrawer::updatePlot);
        timer->start(1);

    } else {
        QMessageBox::critical(this, tr("Open port error"), serial->errorString());
        updateStatus(tr("Open Error"));
    }

}

void MouseDrawer::closePort() {

    serial->close();
    timer->stop();

    // Unlock port parameters
    ui->openPortButton->setEnabled(true);
    ui->portNameEdit->setEnabled(true);
    ui->portBaudRateSelector->setEnabled(true);
    ui->openPortButton->setEnabled(true);

    // Lock close port button
    ui->closePortButton->setEnabled(false);

    updateStatus(tr("Ready"));

    qDebug() << "Close port ";
}

void MouseDrawer::updatePlot() {
    if (serial->canReadLine()) {
        QString readBuffer;
        readBuffer = QString(serial->readAll());
        QStringList tokens = readBuffer.split("\t");
        if (tokens.size() == 4) {
            centerX += tokens[0].toDouble();
            centerY += tokens[2].toDouble();

            pointsX.push_back(centerX);
            pointsY.push_back(centerY);

            trajectory->setData(pointsX, pointsY);
            ui->customPlot->replot();
            ui->customPlot->rescaleAxes();

        }
    }
}

void MouseDrawer::updateStatus(const QString &message) {
    statusLabel->setText("Status: " + message);
}

MouseDrawer::~MouseDrawer()
{
    qDebug() << "App done";
    delete ui;
}
