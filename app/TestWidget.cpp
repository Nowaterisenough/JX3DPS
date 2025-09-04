#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    spdlog::info("Starting simple test widget");
    
    QApplication app(argc, argv);
    spdlog::info("QApplication created");
    
    QWidget widget;
    QLabel *label = new QLabel("Hello World", &widget);
    widget.setWindowTitle("Test Widget");
    widget.resize(300, 200);
    
    spdlog::info("Widget created, showing...");
    widget.show();
    
    spdlog::info("Starting event loop...");
    return app.exec();
}