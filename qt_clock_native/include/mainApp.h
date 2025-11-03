#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QTimeZone>

class QLabel;
class QComboBox;
class QCheckBox;
class QPushButton;

/**
 * PUBLIC_INTERFACE
 * MainWindow hosts the analog and digital clock, date display,
 * timezone selector, 12/24h toggle, and a start/stop control.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onTick();
    void onToggleFormat(bool checked);
    void onTimezoneChanged(int index);
    void onStartStop();

private:
    void setupUi();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();
    void applyTheme();
    void populateTimezones();
    void updateDateLabel(const QDate& date);
    void updateDigitalLabel(const QDateTime& dt);
    void updateAnalogAndDigital();

private:
    QWidget* central = nullptr;
    QLabel* digitalLabel = nullptr;
    QLabel* dateLabel = nullptr;
    QWidget* analogWidget = nullptr;
    QComboBox* tzCombo = nullptr;
    QCheckBox* format24hCheck = nullptr;
    QPushButton* startStopBtn = nullptr;

    QTimer tickTimer;
    bool running = true;
    bool use24h = true;
    QTimeZone currentZone = QTimeZone::systemTimeZone();

    // cached time (used while paused)
    QDateTime cached;
};

#endif // MAIN_APP_H