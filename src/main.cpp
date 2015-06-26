#include <QtGui/QApplication>
#include "rcon_window.hpp"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    RconWindow window;
    window.show();
    return app.exec();
}
