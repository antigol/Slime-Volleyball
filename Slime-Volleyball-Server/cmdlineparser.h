#ifndef CMDLINEPARSER_H
#define CMDLINEPARSER_H

#include <QVariant>
#include <QStringList>

class CmdLineParser
{
public:
    CmdLineParser(const QStringList &arguments);
    QVariant get(const QByteArray &key, const QVariant &defaultValue);

private:
    QStringList _args;
};

#endif // CMDLINEPARSER_H
