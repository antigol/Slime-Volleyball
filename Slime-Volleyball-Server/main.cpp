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
        cout << "--width=WIDTH_PX" << endl;
        cout << "--height=HEIGHT_PX" << endl;
        cout << "--netheight=NET_HEIGHT_PX" << endl;
        cout << "--playerSpeed=PLAYER_SPEED" << endl;
        return 0;
    }

    // todo: créer la classe serveur
    Server server;

    // todo: donner les arguments
    server.setPort(parser.get("port", 2424).toInt());
    server.world()->_gValue = parser.get("gval", 150.0).toDouble();
    server.world()->_height = parser.get("height", 400).toInt();
    server.world()->_width = parser.get("width", 600).toInt();
    server.world()->_netHeight = parser.get("netheight", 50).toInt();
    server.world()->_playerSpeed = parser.get("playerSpeed", 170.0).toDouble();
    server.world()->_enTransFactor = parser.get("entrans", 0.15).toDouble();
    server.world()->_ballRadius = parser.get("rball", 7.0).toDouble();
    server.world()->_slimeRadius = parser.get("rslime", 30.0).toDouble();
    server.world()->_ballMaximumSpeed = parser.get("ballMax", 400.0).toDouble();
    server.world()->_ballMinimumSpeed = parser.get("ballMin", 100.0).toDouble();

    // todo: lancer le serveur
    if (!server.startServer())
        return -1;

    return a.exec();
}
