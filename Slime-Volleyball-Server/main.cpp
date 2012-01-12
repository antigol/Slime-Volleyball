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
    server.setPort(parser.get("port", 2222).toInt());
    server.world()->setgValue(parser.get("gval", 150.0).toDouble());
    server.world()->setheight(parser.get("height", 400).toInt());
    server.world()->setwidth(parser.get("width", 600).toInt());
    server.world()->setnetHeight(parser.get("netheight", 50).toInt());
    server.world()->setplayerSpeed(parser.get("playerSpeed", 150.0).toDouble());
    server.world()->setEnTransFacor(parser.get("entrans", 0.15).toDouble());
    server.world()->setBallRadius(parser.get("rball", 5.0).toDouble());
    server.world()->setSlimeRadius(parser.get("rslime", 30.0).toDouble());

    // todo: lancer le serveur
    server.play();

    return a.exec();
}
