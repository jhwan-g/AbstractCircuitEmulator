#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>


#include "./Controller/globalfilereader.h"
#include "./Controller/assembleclicked.h"
#include "./ModelView/consoleglobal.h"
#include "./Controller/runclicked.h"

// #include "./Controller/runclicked.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    assembleClicked assemble_click_handler{};
    runClicked run_click_handler{};

    // runClicked run_click_handler;

    engine.rootContext()->setContextProperty("file_reader", &file_reader);
    engine.rootContext()->setContextProperty("assemble_clicked", &assemble_click_handler);
    engine.rootContext()->setContextProperty("run_clicked", &run_click_handler);

    engine.rootContext()->setContextProperty("console_view", &debug_console);
    engine.rootContext()->setContextProperty("stat_view", &stat_console);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("AbstractCircuitEmulator", "Main");

    return app.exec();
}
