#include "mainApp.h"

#include <QApplication>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDateTime>
#include <QTimeZone>
#include <QStyle>
#include <QScreen>
#include <QFontDatabase>
#include <QFile>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QtMath>

/*
 Retro Theme (Ocean Professional):
 - Primary: #2563EB (blue)
 - Secondary: #F59E0B (amber)
 - Background: #f9fafb
 - Surface: #ffffff
 - Text: #111827
*/

// Simple widget that draws an analog clock with smooth seconds.
class AnalogClock : public QWidget
{
    Q_OBJECT
public:
    explicit AnalogClock(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setMinimumSize(220, 220);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setAttribute(Qt::WA_TranslucentBackground, false);
    }

    // PUBLIC_INTERFACE
    void setDateTime(const QDateTime& dt) {
        m_dt = dt;
        update();
    }

    // PUBLIC_INTERFACE
    void setColors(const QColor& face, const QColor& ticks, const QColor& hourHand,
                   const QColor& minuteHand, const QColor& secondHand, const QColor& glow)
    {
        faceColor = face;
        tickColor = ticks;
        hourHandColor = hourHand;
        minuteHandColor = minuteHand;
        secondHandColor = secondHand;
        glowColor = glow;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        const int side = qMin(width(), height());
        p.translate(width() / 2.0, height() / 2.0);
        p.scale(side / 240.0, side / 240.0);

        // Face with subtle gradient and glow
        QRadialGradient grad(0, 0, 120);
        grad.setColorAt(0.0, faceColor.lighter(108));
        grad.setColorAt(1.0, faceColor.darker(102));
        p.setBrush(grad);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(0, 0), 110, 110);

        // Rim stroke
        QPen rim(QColor("#e5e7eb")); // light gray
        rim.setWidth(3);
        p.setPen(rim);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(0, 0), 112, 112);

        // ticks
        QPen tickPen(tickColor);
        tickPen.setWidth(2);
        p.setPen(tickPen);
        for (int i = 0; i < 60; ++i) {
            const int len = (i % 5 == 0) ? 10 : 5;
            p.drawLine(0, -98, 0, -98 + len);
            p.rotate(6.0);
        }

        // numerals (12, 3, 6, 9) for retro gauge feel
        QFont f = font();
        f.setBold(true);
        f.setPointSize(10);
        p.setFont(f);
        p.setPen(QPen(QColor("#111827")));
        drawCenteredText(p, QPointF(0, -72), "12");
        drawCenteredText(p, QPointF(72, 0), "3");
        drawCenteredText(p, QPointF(0, 72), "6");
        drawCenteredText(p, QPointF(-72, 0), "9");

        // compute hands
        const QTime t = m_dt.time();
        const double hour = (t.hour() % 12) + t.minute() / 60.0 + t.second() / 3600.0;
        const double minute = t.minute() + t.second() / 60.0;
        const double second = t.second() + t.msec() / 1000.0;

        // hour hand
        p.save();
        p.rotate(30.0 * hour);
        QPen h(hourHandColor);
        h.setCapStyle(Qt::RoundCap);
        h.setWidth(5);
        p.setPen(h);
        p.drawLine(QPointF(0, 8), QPointF(0, -50));
        p.restore();

        // minute hand
        p.save();
        p.rotate(6.0 * minute);
        QPen m(minuteHandColor);
        m.setCapStyle(Qt::RoundCap);
        m.setWidth(4);
        p.setPen(m);
        p.drawLine(QPointF(0, 12), QPointF(0, -75));
        p.restore();

        // second hand (smooth)
        p.save();
        p.rotate(6.0 * second);
        // subtle glow
        QPen sg(glowColor);
        sg.setWidth(8);
        sg.setCapStyle(Qt::RoundCap);
        sg.setColor(glowColor);
        sg.setJoinStyle(Qt::RoundJoin);
        p.setPen(sg);
        p.drawLine(QPointF(0, 16), QPointF(0, -84));
        // main second hand
        QPen s(secondHandColor);
        s.setCapStyle(Qt::RoundCap);
        s.setWidth(2);
        p.setPen(s);
        p.drawLine(QPointF(0, 16), QPointF(0, -86));
        p.restore();

        // center cap
        p.setBrush(QBrush(secondHandColor));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(0, 0), 4, 4);
    }

private:
    void drawCenteredText(QPainter& p, const QPointF& pos, const QString& text) {
        QRectF r(pos.x() - 14, pos.y() - 10, 28, 20);
        p.drawText(r, Qt::AlignCenter, text);
    }

    QDateTime m_dt = QDateTime::currentDateTime();
    QColor faceColor = QColor("#ffffff");
    QColor tickColor = QColor("#6b7280");       // gray-500
    QColor hourHandColor = QColor("#111827");   // text
    QColor minuteHandColor = QColor("#2563EB"); // primary
    QColor secondHandColor = QColor("#F59E0B"); // secondary
    QColor glowColor = QColor(245, 158, 11, 50);
};

// Helper to load embedded font if any; here we keep default but set a segmented-like fallback using styles.
static void applyAppStyleSheet(QApplication& app)
{
    const QString primary = "#2563EB";
    const QString secondary = "#F59E0B";
    const QString background = "#f9fafb";
    const QString surface = "#ffffff";
    const QString text = "#111827";

    const QString style = QString(R"(
        QWidget {
            background: %1;
            color: %5;
            font-size: 14px;
        }
        QMainWindow {
            background: %1;
        }
        QLabel#Digital {
            background: %4;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            padding: 16px 22px;
            font-weight: 700;
            letter-spacing: 2px;
            /* Segmented digital look approximation */
            font-family: "Courier New", monospace;
            color: %5;
            box-shadow: 0 6px 16px rgba(0,0,0,0.06);
        }
        QLabel#Date {
            color: #374151;
        }
        QWidget#AnalogContainer {
            background: qlineargradient(x1:0,y1:0, x2:0,y2:1,
                        stop:0 rgba(255,255,255,1), stop:1 rgba(249,250,251,1));
            border: 1px solid #e5e7eb;
            border-radius: 16px;
        }
        QComboBox {
            background: %4;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            padding: 6px 10px;
        }
        QCheckBox::indicator {
            width: 18px; height: 18px;
        }
        QPushButton {
            background: %2;
            border: none;
            border-radius: 10px;
            padding: 8px 14px;
            color: #111827;
            font-weight: 600;
        }
        QPushButton#Start {
            background: %3;
            color: white;
        }
        QPushButton:hover {
            filter: brightness(1.05);
        }
    )").arg(background, secondary, primary, surface, text);

    app.setStyleSheet(style);
}

// MainWindow implementation

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    setupMenuBar();
    setupStatusBar();
    setupConnections();
    applyTheme();

    // Timer at ~30fps for smooth seconds animation
    tickTimer.setInterval(33);
    tickTimer.start();

    // initial paint
    cached = QDateTime::currentDateTime();
    updateAnalogAndDigital();

    // center and show reasonable size
    resize(720, 480);
    setWindowTitle("Ocean Retro Clock");
}

void MainWindow::setupUi()
{
    central = new QWidget(this);
    setCentralWidget(central);

    auto mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Top area: Digital + Date
    auto topLayout = new QVBoxLayout();
    digitalLabel = new QLabel("--:--:--", this);
    digitalLabel->setObjectName("Digital");
    digitalLabel->setAlignment(Qt::AlignCenter);

    dateLabel = new QLabel("", this);
    dateLabel->setObjectName("Date");
    dateLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(digitalLabel);
    topLayout->addWidget(dateLabel);

    // Middle area: Analog inside a rounded surface
    auto analogContainer = new QWidget(this);
    analogContainer->setObjectName("AnalogContainer");
    auto analogLayout = new QVBoxLayout(analogContainer);
    analogLayout->setContentsMargins(12, 12, 12, 12);

    analogWidget = new AnalogClock(analogContainer);
    analogLayout->addWidget(analogWidget);

    // Controls: timezone, 12/24h, start/stop
    auto controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(10);

    tzCombo = new QComboBox(this);
    populateTimezones();

    format24hCheck = new QCheckBox("24-hour", this);
    format24hCheck->setChecked(true);
    use24h = true;

    startStopBtn = new QPushButton("Pause", this);
    startStopBtn->setObjectName("Start");

    controlsLayout->addWidget(new QLabel("Timezone:", this));
    controlsLayout->addWidget(tzCombo, 1);
    controlsLayout->addSpacing(10);
    controlsLayout->addWidget(format24hCheck);
    controlsLayout->addSpacing(10);
    controlsLayout->addWidget(startStopBtn);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(analogContainer, 1);
    mainLayout->addLayout(controlsLayout);
}

void MainWindow::setupMenuBar()
{
    auto fileMenu = menuBar()->addMenu("&File");
    auto actQuit = new QAction("Quit", this);
    connect(actQuit, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(actQuit);

    auto viewMenu = menuBar()->addMenu("&View");
    auto actToggleFormat = new QAction("Toggle 12/24h", this);
    actToggleFormat->setCheckable(true);
    actToggleFormat->setChecked(use24h);
    connect(actToggleFormat, &QAction::toggled, this, [this](bool checked) {
        format24hCheck->setChecked(checked);
        onToggleFormat(checked);
    });
    viewMenu->addAction(actToggleFormat);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ocean Professional • Retro Clock");
}

void MainWindow::setupConnections()
{
    connect(&tickTimer, &QTimer::timeout, this, &MainWindow::onTick);
    connect(format24hCheck, &QCheckBox::toggled, this, &MainWindow::onToggleFormat);
    connect(tzCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onTimezoneChanged);
    connect(startStopBtn, &QPushButton::clicked, this, &MainWindow::onStartStop);
}

void MainWindow::applyTheme()
{
    // Configure analog colors
    auto analog = qobject_cast<AnalogClock*>(analogWidget);
    if (analog) {
        analog->setColors(
            QColor("#ffffff"),   // face
            QColor("#6b7280"),   // ticks
            QColor("#111827"),   // hour hand
            QColor("#2563EB"),   // minute hand primary
            QColor("#F59E0B"),   // second hand secondary
            QColor(37, 99, 235, 60) // subtle blue glow
        );
    }
}

void MainWindow::populateTimezones()
{
    tzCombo->clear();
    QList<QByteArray> ids = QTimeZone::availableTimeZoneIds();
    // sort them by name
    std::sort(ids.begin(), ids.end(), [](const QByteArray& a, const QByteArray& b) {
        return QString::fromUtf8(a).toLower() < QString::fromUtf8(b).toLower();
    });

    int systemIndex = -1;
    const QByteArray sysId = QTimeZone::systemTimeZoneId();
    for (int i = 0; i < ids.size(); ++i) {
        const QString name = QString::fromUtf8(ids[i]);
        tzCombo->addItem(name, ids[i]);
        if (ids[i] == sysId) systemIndex = i;
    }
    if (systemIndex >= 0) {
        tzCombo->setCurrentIndex(systemIndex);
        currentZone = QTimeZone(sysId);
    }
}

void MainWindow::updateDateLabel(const QDate& date)
{
    dateLabel->setText(date.toString(Qt::DefaultLocaleLongDate));
}

void MainWindow::updateDigitalLabel(const QDateTime& dt)
{
    const QTime t = dt.time();
    QString text;
    if (use24h) {
        text = t.toString("HH:mm:ss");
    } else {
        text = t.toString("hh:mm:ss ap");
    }
    digitalLabel->setText(text.toUpper());
}

void MainWindow::updateAnalogAndDigital()
{
    const QDateTime now = running ? QDateTime::currentDateTime() : cached;
    const QDateTime localized = now.toTimeZone(currentZone);

    // date updates when the day changes
    updateDateLabel(localized.date());
    updateDigitalLabel(localized);

    // analog update
    if (auto analog = qobject_cast<AnalogClock*>(analogWidget)) {
        analog->setDateTime(localized);
    }
}

void MainWindow::onTick()
{
    updateAnalogAndDigital();
    if (!running) {
        // keep cached steady when paused
        // nothing else needed
    } else {
        cached = QDateTime::currentDateTime();
    }
}

void MainWindow::onToggleFormat(bool checked)
{
    use24h = checked;
    updateAnalogAndDigital();
}

void MainWindow::onTimezoneChanged(int index)
{
    const QByteArray id = tzCombo->itemData(index).toByteArray();
    if (!id.isEmpty() && QTimeZone::isTimeZoneIdAvailable(id)) {
        currentZone = QTimeZone(id);
        updateAnalogAndDigital();
        statusBar()->showMessage(QString("Timezone set to %1").arg(QString::fromUtf8(id)), 2000);
    }
}

void MainWindow::onStartStop()
{
    running = !running;
    if (running) {
        tickTimer.start();
        startStopBtn->setText("Pause");
        startStopBtn->setObjectName("Start");
        startStopBtn->style()->unpolish(startStopBtn);
        startStopBtn->style()->polish(startStopBtn);
        cached = QDateTime::currentDateTime();
        statusBar()->showMessage("Resumed", 1500);
    } else {
        tickTimer.stop();
        startStopBtn->setText("Start");
        startStopBtn->setObjectName("");
        startStopBtn->style()->unpolish(startStopBtn);
        startStopBtn->style()->polish(startStopBtn);
        // freeze current time in cache
        cached = QDateTime::currentDateTime();
        updateAnalogAndDigital();
        statusBar()->showMessage("Paused", 1500);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    applyAppStyleSheet(app);

    MainWindow w;
    w.show();

    return app.exec();
}

#include "mainApp.moc"
