#include <QtCore/QCoreApplication>
#include <QTextStream>
#include "cmdlineparser.h"

QTextStream cout(stdout);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // todo: lire les arguments
    QStringList args = a.arguments();
    CmdLineParser parser(args);

    // todo: créer la classe serveur

    // todo: donner les arguments
    cout << parser.get("port", 2222).toInt() << endl;

    // todo: lancer le serveur

    return a.exec();
}
