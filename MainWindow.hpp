#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollArea>
#include <QPainter>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFrame>
#include <QTimer>
#include <QDateTime>
#include <functional>

namespace T {
    constexpr auto BG       = "#000000";
    constexpr auto GREEN    = "#00ff00";
    constexpr auto YELLOW   = "#ffff00";
    constexpr auto CYAN     = "#00ffff";
    constexpr auto RED      = "#ff4444";
    constexpr auto DIMGREEN = "#00aa00";
    constexpr auto WHITE    = "#e0e0e0";
    constexpr auto FONT     = "Courier New";
    constexpr int  FS       = 12;
}

static QString styleLabel(const char* color = T::GREEN) {
    return QString("color:%1; font-family:'%2'; font-size:%3px; background:transparent;")
           .arg(color).arg(T::FONT).arg(T::FS);
}

// Fixed-width QString column using leftJustified — replaces printf %-Ns
static QString col(const QString& s, int width) {
    return s.leftJustified(width, ' ', true);
}

// ─────────────────────────────────────────────────────────────
// TermBar — bracket progress bar  [ |||      ]
// ─────────────────────────────────────────────────────────────
class TermBar : public QWidget {
    Q_OBJECT
public:
    explicit TermBar(float fraction = 0.f,
                     const char* fillColor = T::GREEN,
                     QWidget* parent = nullptr)
        : QWidget(parent), mFraction(fraction), mColor(fillColor)
    {
        setFixedHeight(16);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        QFont f(T::FONT, T::FS);
        p.setFont(f);
        QFontMetrics fm(f);
        int charW = fm.horizontalAdvance('|');
        if (charW < 1) charW = 8;
        int inner = (width() - 2 * charW) / charW;
        int filled = static_cast<int>(mFraction * inner);
        filled = std::clamp(filled, 0, inner);

        p.setPen(QColor(T::DIMGREEN));
        p.drawText(0, 0, charW, height(), Qt::AlignLeft | Qt::AlignVCenter, "[");
        p.drawText(width() - charW, 0, charW, height(),
                   Qt::AlignLeft | Qt::AlignVCenter, "]");

        p.setPen(QColor(mColor));
        int x = charW;
        for (int i = 0; i < filled; ++i, x += charW)
            p.drawText(x, 0, charW, height(), Qt::AlignLeft | Qt::AlignVCenter, "|");

        p.setPen(QColor(T::DIMGREEN));
        for (int i = filled; i < inner; ++i, x += charW)
            p.drawText(x, 0, charW, height(), Qt::AlignLeft | Qt::AlignVCenter, " ");
    }

private:
    float       mFraction;
    const char* mColor;
};

// ─────────────────────────────────────────────────────────────
// TermLine — horizontal dash rule
// ─────────────────────────────────────────────────────────────
class TermLine : public QLabel {
public:
    explicit TermLine(QWidget* parent = nullptr) : QLabel(parent) {
        setStyleSheet(styleLabel(T::DIMGREEN));
        setText(QString(80, '-'));
        setFixedHeight(16);
    }
};

// ─────────────────────────────────────────────────────────────
// MetricTable — one benchmark section rendered as terminal rows
// ─────────────────────────────────────────────────────────────
class MetricTable : public QWidget {
public:
    MetricTable(const QString& title,
                const QString& colA,
                const QString& colB,
                QWidget* parent = nullptr)
        : QWidget(parent)
    {
        mLayout = new QVBoxLayout(this);
        mLayout->setContentsMargins(0, 0, 0, 0);
        mLayout->setSpacing(0);

        auto* t = new QLabel(title);
        t->setStyleSheet(styleLabel(T::YELLOW));
        mLayout->addWidget(t);

        // Column header — fixed-width columns via leftJustified
        QString hdrText = "  " + col("Metric", 36)
                        + " " + col(colA, 22)
                        + " " + col(colB, 22);
        auto* hdr = new QLabel(hdrText);
        hdr->setStyleSheet(styleLabel(T::CYAN));
        mLayout->addWidget(hdr);

        auto* div = new QLabel(QString(82, '-'));
        div->setStyleSheet(styleLabel(T::DIMGREEN));
        mLayout->addWidget(div);
    }

    void addRow(const QString& label,
                const QString& valA, const QString& valB,
                bool passA, bool passB)
    {
        // Lambda assigned to auto — fixes MSVC C2440
        auto passStr = [](bool p) -> QString {
            return p ? " [PASS]" : " [FAIL]";
        };

        const char* colorA = passA ? T::GREEN : T::RED;
        const char* colorB = passB ? T::GREEN : T::RED;

        auto* row = new QWidget;
        auto* lay = new QHBoxLayout(row);
        lay->setContentsMargins(2, 0, 0, 0);
        lay->setSpacing(0);

        // Label column
        auto* lbl = new QLabel("  " + col(label, 36));
        lbl->setStyleSheet(styleLabel(T::WHITE));
        lbl->setFixedWidth(320);
        lay->addWidget(lbl);

        // Value A + pass/fail
        auto* vA = new QLabel(col(valA, 14) + passStr(passA));
        vA->setStyleSheet(styleLabel(colorA));
        vA->setFixedWidth(220);
        lay->addWidget(vA);

        // Value B + pass/fail
        auto* vB = new QLabel(col(valB, 14) + passStr(passB));
        vB->setStyleSheet(styleLabel(colorB));
        lay->addWidget(vB);

        lay->addStretch();
        mLayout->addWidget(row);
    }

    void addInfoRow(const QString& label,
                    const QString& valA,
                    const QString& valB)
    {
        auto* row = new QWidget;
        auto* lay = new QHBoxLayout(row);
        lay->setContentsMargins(2, 0, 0, 0);
        lay->setSpacing(0);

        auto* lbl = new QLabel("  " + col(label, 36));
        lbl->setStyleSheet(styleLabel(T::WHITE));
        lbl->setFixedWidth(320);
        lay->addWidget(lbl);

        auto* vA = new QLabel(col(valA, 22));
        vA->setStyleSheet(styleLabel(T::GREEN));
        vA->setFixedWidth(220);
        lay->addWidget(vA);

        auto* vB = new QLabel(col(valB, 22));
        vB->setStyleSheet(styleLabel(T::GREEN));
        lay->addWidget(vB);

        lay->addStretch();
        mLayout->addWidget(row);
    }

    void addSpacer() { mLayout->addSpacing(6); }

private:
    QVBoxLayout* mLayout;
};

// ─────────────────────────────────────────────────────────────
// MainWindow
// ─────────────────────────────────────────────────────────────
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("BenchmarkViewer");
        setMinimumSize(900, 640);
        resize(980, 720);
        setAcceptDrops(true);

        setStyleSheet(QString(
            "QMainWindow, QWidget { background:%1; }"
            "QScrollArea { border:none; background:%1; }"
            "QScrollBar:vertical { background:%1; width:8px; }"
            "QScrollBar::handle:vertical { background:%2; min-height:20px; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }"
            "QPushButton { color:%3; background:%1; border:1px solid %2;"
            "font-family:'Courier New'; font-size:12px; padding:2px 10px; }"
            "QPushButton:hover { background:%2; }")
            .arg(T::BG).arg(T::DIMGREEN).arg(T::GREEN));

        buildUI();
        showDropHint();

        mClock = new QTimer(this);
        connect(mClock, &QTimer::timeout, this, &MainWindow::updateClock);
        mClock->start(1000);
        updateClock();
    }

protected:
    void dragEnterEvent(QDragEnterEvent* e) override {
        if (e->mimeData()->hasUrls()) e->acceptProposedAction();
    }
    void dropEvent(QDropEvent* e) override {
        auto urls = e->mimeData()->urls();
        if (!urls.isEmpty()) loadJson(urls.first().toLocalFile());
    }

private slots:
    void onOpenFile() {
        QString path = QFileDialog::getOpenFileName(
            this, "Open results.json", QString(), "JSON Files (*.json)");
        if (!path.isEmpty()) loadJson(path);
    }

    void updateClock() {
        if (mClockLabel)
            mClockLabel->setText(
                QDateTime::currentDateTime()
                .toString("  yyyy-MM-dd  hh:mm:ss  "));
    }

private:
    QWidget*     mContent    = nullptr;
    QVBoxLayout* mContentLay = nullptr;
    QLabel*      mClockLabel = nullptr;
    QTimer*      mClock      = nullptr;

    void buildUI() {
        auto* central = new QWidget;
        setCentralWidget(central);
        auto* root = new QVBoxLayout(central);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(0);

        // Top status bar
        auto* statusBar = new QWidget;
        statusBar->setFixedHeight(22);
        statusBar->setStyleSheet(QString("background:%1;").arg(T::DIMGREEN));
        auto* slay = new QHBoxLayout(statusBar);
        slay->setContentsMargins(8, 0, 8, 0);

        auto* sTitle = new QLabel("BenchmarkViewer 1.0");
        sTitle->setStyleSheet(
            "color:#000000; font-family:'Courier New';"
            "font-size:12px; font-weight:bold; background:transparent;");
        slay->addWidget(sTitle);
        slay->addStretch();

        mClockLabel = new QLabel;
        mClockLabel->setStyleSheet(
            "color:#000000; font-family:'Courier New';"
            "font-size:12px; background:transparent;");
        slay->addWidget(mClockLabel);

        auto* openBtn = new QPushButton(" [Open results.json] ");
        openBtn->setStyleSheet(
            "color:#000000; background:transparent; border:none;"
            "font-family:'Courier New'; font-size:12px; font-weight:bold;");
        openBtn->setCursor(Qt::PointingHandCursor);
        connect(openBtn, &QPushButton::clicked, this, &MainWindow::onOpenFile);
        slay->addWidget(openBtn);

        root->addWidget(statusBar);

        // Scrollable content area
        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        mContent = new QWidget;
        mContent->setStyleSheet(QString("background:%1;").arg(T::BG));
        mContentLay = new QVBoxLayout(mContent);
        mContentLay->setContentsMargins(8, 8, 8, 8);
        mContentLay->setSpacing(0);
        scroll->setWidget(mContent);
        root->addWidget(scroll);

        // Bottom hint bar
        auto* hint = new QWidget;
        hint->setFixedHeight(18);
        hint->setStyleSheet(QString("background:%1;").arg(T::DIMGREEN));
        auto* hlay = new QHBoxLayout(hint);
        hlay->setContentsMargins(8, 0, 8, 0);
        auto* hLbl = new QLabel("F1 Open    Drag & drop results.json onto window");
        hLbl->setStyleSheet(
            "color:#000000; font-family:'Courier New';"
            "font-size:11px; background:transparent;");
        hlay->addWidget(hLbl);
        root->addWidget(hint);
    }

    void clearContent() {
        QLayoutItem* item;
        while ((item = mContentLay->takeAt(0))) {
            delete item->widget();
            delete item;
        }
    }

    QLabel* line(const QString& text,
                 const char* color = T::GREEN,
                 bool bold = false) {
        auto* l = new QLabel(text);
        QString s = styleLabel(color);
        if (bold) s += "font-weight:bold;";
        l->setStyleSheet(s);
        return l;
    }

    void showDropHint() {
        clearContent();
        mContentLay->addStretch();
        mContentLay->addWidget(line("  Drop results.json onto this window", T::GREEN));
        mContentLay->addWidget(line("  or press [Open results.json] above", T::DIMGREEN));
        mContentLay->addStretch();
    }

    void loadJson(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Error", "Cannot open:\n" + path);
            return;
        }
        QJsonParseError err;
        auto doc = QJsonDocument::fromJson(f.readAll(), &err);
        if (doc.isNull()) {
            QMessageBox::warning(this, "Parse Error", err.errorString());
            return;
        }
        renderResults(doc.object(), path);
    }

    void renderResults(const QJsonObject& root, const QString& path) {
        clearContent();

        QJsonArray results = root["results"].toArray();
        if (results.size() < 2) {
            mContentLay->addWidget(
                line("  ERROR: need at least 2 results in JSON.", T::RED));
            return;
        }

        QJsonObject a = results[0].toObject();
        QJsonObject b = results[1].toObject();
        QString nameA = a["name"].toString();
        QString nameB = b["name"].toString();

        // Header
        mContentLay->addWidget(line(
            "  Benchmark: " + root["benchmark"].toString(),
            T::YELLOW, true));
        mContentLay->addWidget(line(
            QString("  File: %1   Blocks: %2   Block size: %3 samples   Sample rate: %4 Hz")
            .arg(path.split("/").last().split("\\").last())
            .arg(root["blocks_measured"].toInt())
            .arg(root["block_size"].toInt())
            .arg(root["sample_rate"].toDouble(), 0, 'f', 0),
            T::DIMGREEN));
        mContentLay->addWidget(new TermLine);
        mContentLay->addSpacing(4);

        auto ta  = a["timing"].toObject();
        auto tb  = b["timing"].toObject();
        auto ca  = a["churn"].toObject();
        auto cb  = b["churn"].toObject();
        auto fa  = a["fragmentation"].toObject();
        auto fb  = b["fragmentation"].toObject();
        auto cca = a["concurrency"].toObject();
        auto ccb = b["concurrency"].toObject();

        int scoreA = ta["passed"].toBool() + ca["passed"].toBool()
                   + fa["passed"].toBool() + cca["passed"].toBool();
        int scoreB = tb["passed"].toBool() + cb["passed"].toBool()
                   + fb["passed"].toBool() + ccb["passed"].toBool();

        // Score bars
        auto addBar = [&](const QString& label, int score, int total,
                          const char* color) {
            auto* row = new QWidget;
            auto* lay = new QHBoxLayout(row);
            lay->setContentsMargins(2, 1, 2, 1);
            lay->setSpacing(4);

            auto* lbl = new QLabel("  " + col(label, 30));
            lbl->setStyleSheet(styleLabel(color));
            lbl->setFixedWidth(280);
            lay->addWidget(lbl);

            auto* bar = new TermBar(
                static_cast<float>(score) / total, color);
            lay->addWidget(bar, 1);

            auto* pct = new QLabel(
                QString(" %1/%2").arg(score).arg(total));
            pct->setStyleSheet(styleLabel(color));
            pct->setFixedWidth(40);
            lay->addWidget(pct);

            mContentLay->addWidget(row);
        };

        addBar(nameA, scoreA, 4, scoreA == 4 ? T::GREEN : T::RED);
        addBar(nameB, scoreB, 4, scoreB == 4 ? T::GREEN : T::RED);

        mContentLay->addSpacing(4);
        mContentLay->addWidget(new TermLine);
        mContentLay->addSpacing(6);

        // Column headers
        auto* colHdr = new QLabel(
            "  " + col("Metric", 36) + " " + col(nameA, 22) + " " + col(nameB, 22));
        colHdr->setStyleSheet(styleLabel(T::CYAN));
        mContentLay->addWidget(colHdr);
        mContentLay->addWidget(new TermLine);
        mContentLay->addSpacing(4);

        // Section 1: Timing
        auto* s1 = new MetricTable(
            "  Timing Unpredictability   (0 allocs/block = real-time safe)",
            nameA, nameB);
        s1->addRow("Avg allocs per audio block",
            QString::number(ta["allocs_per_block"].toInt()),
            QString::number(tb["allocs_per_block"].toInt()),
            ta["passed"].toBool(), tb["passed"].toBool());
        s1->addInfoRow("Avg block latency",
            QString("%1 us").arg(ta["avg_latency_us"].toDouble(), 0, 'f', 2),
            QString("%1 us").arg(tb["avg_latency_us"].toDouble(), 0, 'f', 2));
        s1->addSpacer();
        mContentLay->addWidget(s1);

        // Section 2: Churn
        auto* s2 = new MetricTable(
            "  Memory Churn   (lower = more stable runtime landscape)",
            nameA, nameB);
        s2->addRow("Total new() calls (steady-state)",
            QString::number(ca["total_allocs"].toInt()),
            QString::number(cb["total_allocs"].toInt()),
            ca["passed"].toBool(), cb["passed"].toBool());
        s2->addRow("Total delete() calls (steady-state)",
            QString::number(ca["total_frees"].toInt()),
            QString::number(cb["total_frees"].toInt()),
            ca["passed"].toBool(), cb["passed"].toBool());
        s2->addSpacer();
        mContentLay->addWidget(s2);

        // Section 3: Fragmentation
        auto* s3 = new MetricTable(
            "  Memory Fragmentation   (shrinking block = fragmentation growing)",
            nameA, nameB);
        s3->addInfoRow("Largest block before run",
            QString("%1 MB").arg(fa["before_mb"].toDouble(), 0, 'f', 1),
            QString("%1 MB").arg(fb["before_mb"].toDouble(), 0, 'f', 1));
        s3->addInfoRow("Largest block after run",
            QString("%1 MB").arg(fa["after_mb"].toDouble(), 0, 'f', 1),
            QString("%1 MB").arg(fb["after_mb"].toDouble(), 0, 'f', 1));
        s3->addRow("Fragmentation growth",
            QString("%1 MB").arg(fa["growth_mb"].toDouble(), 0, 'f', 1),
            QString("%1 MB").arg(fb["growth_mb"].toDouble(), 0, 'f', 1),
            fa["passed"].toBool(), fb["passed"].toBool());
        s3->addSpacer();
        mContentLay->addWidget(s3);

        // Section 4: Concurrency
        auto* s4 = new MetricTable(
            "  Concurrency Safety   (mismatch = shared state leak)",
            nameA, nameB);
        s4->addRow("Checksum mismatches (50 rounds)",
            QString::number(cca["checksum_mismatches"].toInt()),
            QString::number(ccb["checksum_mismatches"].toInt()),
            cca["passed"].toBool(), ccb["passed"].toBool());
        s4->addRow("Instances independent",
            cca["instances_independent"].toBool() ? "YES" : "NO",
            ccb["instances_independent"].toBool() ? "YES" : "NO",
            cca["passed"].toBool(), ccb["passed"].toBool());
        s4->addSpacer();
        mContentLay->addWidget(s4);

        // Summary
        mContentLay->addWidget(new TermLine);
        mContentLay->addWidget(line("  Summary", T::YELLOW, true));

        auto verdictLine = [&](const QString& name, int score) {
            bool safe = (score == 4);
            mContentLay->addWidget(line(
                QString("  %1 %2/4 concepts passed   %3")
                .arg(col(name, 30))
                .arg(score)
                .arg(safe ? "real-time safe" : "needs attention"),
                safe ? T::GREEN : T::RED));
        };
        verdictLine(nameA, scoreA);
        verdictLine(nameB, scoreB);

        mContentLay->addStretch();
    }
};