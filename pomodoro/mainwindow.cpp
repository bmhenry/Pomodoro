#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    this->setWindowIcon(QIcon(":/timer.ico"));

    timer = new QTimer(this);
    timerSound = new QSound(QString(":/bike_bell.wav"), this);


    state = NONE;
    paused = 0;
    save = false;

    pomodoros = 0;
    session_pomodoros = 0;
    work_time = 25 * 60;
    break_time = 5 * 60;
    long_time = 15 * 60;

    // get settings if they're available
    get_settings();

    ui->workTimeSelect->setValue(work_time / 60);
    ui->breakTimeSelect->setValue(break_time / 60);
    ui->longTimeSelect->setValue(long_time / 60);
    ui->pomodoroCounter->setText(QString::number(pomodoros));
    ui->saveSettings->setChecked(save);

    ui->progressBar->setRange(0, work_time);
    ui->progressBar->setValue(0);

    connect(ui->workTimeSelect, SIGNAL(valueChanged(int)), this, SLOT(workChanged(int)));
    connect(ui->breakTimeSelect, SIGNAL(valueChanged(int)), this, SLOT(breakChanged(int)));
    connect(ui->longTimeSelect, SIGNAL(valueChanged(int)), this, SLOT(longChanged(int)));
    connect(ui->saveSettings, SIGNAL(clicked(bool)), this, SLOT(settingsChanged(bool)));

    connect(ui->startButton, SIGNAL(released()), this, SLOT(start()));
    connect(ui->pauseButton, SIGNAL(released()), this, SLOT(pause()));
    connect(ui->stopButton, SIGNAL(released()), this, SLOT(stop()));
    connect(ui->helpButton, SIGNAL(released()), this, SLOT(openHelp()));
    connect(timer, SIGNAL(timeout()), this, SLOT(timeUpdate()));

    this->setFocus();
}

void MainWindow::openHelp()
{
    helpWindow = new Help(this);
    helpWindow->move(this->pos() + QPoint(40,40));
    helpWindow->show();
}

void MainWindow::timeUpdate()
{
    cur_sec--;

    if (cur_sec < 0)
    {
        cur_sec = 59;
        cur_min--;
    }

    // reaches 0:
    if (cur_min == 0 && cur_sec == 0)
    {
        timer->stop();
        timerSound->play();

        if (state == WORK)
        {
            ui->infoLabel->setText("Pomodoro done.");
            pomodoros++;
            session_pomodoros++;
            ui->pomodoroCounter->setText(QString::number(pomodoros));
            ui->currentCounter->setText(QString::number(session_pomodoros));
        }
        else if (state == BREAK || state == LONG)
            ui->infoLabel->setText("Break done.");
    }

    // update the progress bar
    ui->progressBar->setValue(current_pomodoro - (cur_min * 60 + cur_sec));

    // update the clock label
    ui->countdown->setText(QString::number(cur_min) + QString(":") + QString("%1").arg(cur_sec, 2, 10, QChar('0')));
}

void MainWindow::start()
{
    // if the timer is already running, dont restart
    if (timer->isActive())
        return;

    // if paused, simply resume the timer
    if (paused)
    {
        paused = 0;
        timer->start(1000);
        return;
    }

    if (state != WORK)
    {
        ui->infoLabel->setText("Work");
        if (session_pomodoros > 0 && (session_pomodoros + 1) % 4 == 0)
            ui->nextLabel->setText("(Long break next)");
        else
            ui->nextLabel->setText("(Break next)");
        current_pomodoro = work_time;
        state = WORK;
    }
    else if (state == WORK && session_pomodoros > 0 && session_pomodoros % 4 != 0)
    {
        ui->infoLabel->setText("Break");
        ui->nextLabel->setText("(Work next)");
        current_pomodoro = break_time;
        state = BREAK;
    }
    else if (state == WORK && session_pomodoros > 0 && session_pomodoros % 4 == 0)
    {
        ui->infoLabel->setText("Long break");
        ui->nextLabel->setText("(Work next)");
        current_pomodoro = long_time;
        state = LONG;
    }

    cur_min = current_pomodoro / 60;
    cur_sec = 0;

    // set progress bar max
    ui->progressBar->setMaximum(current_pomodoro);
    ui->progressBar->setValue(0);

    // update the clock label
    ui->countdown->setText(QString::number(cur_min) + QString(":") + QString("%1").arg(cur_sec, 2, 10, QChar('0')));

    // start the timer
    timer->start(1000);
}

void MainWindow::stop()
{
    timer->stop();

    if (state == WORK)
        ui->infoLabel->setText("Work stopped.");
    else if (state == BREAK || state == LONG)
        ui->infoLabel->setText("Break stopped.");

    state = NONE;
    ui->nextLabel->setText("(Work next)");

    cur_min = current_pomodoro / 60;
    cur_sec = 0;

    // update the progress bar
    ui->progressBar->setValue(0);

    // update the clock label
    ui->countdown->setText(QString::number(cur_min) + QString(":") + QString("%1").arg(cur_sec, 2, 10, QChar('0')));
}

void MainWindow::pause()
{
    timer->stop();
    paused = 1;
}

void MainWindow::get_settings()
{
    QFile file("./pomodoro.settings");

    if (!file.exists())
    {
        qDebug() << "No settings file.";
        return;
    }

    file.open(QFile::ReadOnly | QFile::Text);
    QString json = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject object = doc.object();

    work_time = object["worktime"].toInt();
    break_time = object["breaktime"].toInt();
    long_time = object["longtime"].toInt();
    pomodoros = object["pomodoros"].toInt();
    save = true;

    file.close();
}

void MainWindow::save_settings()
{
    QFile file("./pomodoro.settings");
    file.open(QFile::WriteOnly);

    QJsonObject object;
    object["worktime"] = work_time;
    object["breaktime"] = break_time;
    object["longtime"] = long_time;
    object["pomodoros"] = pomodoros;

    QJsonDocument doc(object);
    file.write(doc.toJson());

    file.close();
}

void MainWindow::workChanged(int newtime)
{
    this->work_time = newtime * 60;
}

void MainWindow::breakChanged(int newtime)
{
    this->break_time = newtime * 60;
}

void MainWindow::longChanged(int newtime)
{
    this->long_time = newtime * 60;
}

void MainWindow::settingsChanged(bool s)
{
    save = s;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (save)
        save_settings();
    else
        QFile("./pomodoro.settings").remove();

    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    delete timerSound;
}
