#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Benchmark Viewer");

    MainWindow w;

    if (argc > 1)
        QMetaObject::invokeMethod(&w, [&w, argv]{
            w.show();
        }, Qt::QueuedConnection);

    w.show();
    return app.exec();
}

#include "main.moc"    // ← add this line at the very end