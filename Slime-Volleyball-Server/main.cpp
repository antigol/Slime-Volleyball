#include <QtCore/QCoreApplication>
#include <QTextStream>
#include "cmdlineparser.h"
#include "server.h"

QTextStream cout(stdout);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // todo: lire les arguments
    QStringList args = a.arguments();
    CmdLineParser parser(args);

    if (parser.isSet("help")) {
        cout << "Slime Volleyball Server" << endl;
        cout << "--port=PORT_NUMBER" << endl;
        cout << "--gval=ACCELERATION_VALUE" << endl;
        return 0;
    }

    // todo: créer la classe serveur
    Server server;

    // todo: donner les arguments
    server.setPort(parser.get("port", 2222).toInt());
    server.world()->setgValue(10);
    server.world()->setheight(400);
    server.world()->setwidth(600);
    server.world()->setnetHeight(30);
    server.world()->setplayerSpeed(5);

    // todo: lancer le serveur
    server.play();

    return a.exec();
}
