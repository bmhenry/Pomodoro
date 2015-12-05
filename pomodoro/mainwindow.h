#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSound>
#include <QCloseEvent>

#include "help.h"


namespace Ui {
class MainWindow;
}

typedef enum STATE {NONE, WORK, BREAK, LONG} STATE;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void workChanged(int);
    void breakChanged(int);
    void longChanged(int);
    void settingsChanged(bool);
    void openHelp();

    void timeUpdate();

    void start();
    void stop();
    void pause();

private:
    void closeEvent(QCloseEvent *event);

    // settings
    void get_settings();
    void save_settings();

    Ui::MainWindow *ui;

    Help *helpWindow;

    QTimer *timer;
    QSound *timerSound;

    int pomodoros;
    int session_pomodoros;

    int current_pomodoro;
    int cur_min;
    int cur_sec;

    STATE state;
    int paused;
    bool save;  // should setting be changed (t/f)

    // these times are always in seconds
    int work_time;
    int break_time;
    int long_time;
};

#endif // MAINWINDOW_H
