#include "cmdlineparser.h"

CmdLineParser::CmdLineParser(const QStringList &arguments)
    : _args(arguments)
{
}

QVariant CmdLineParser::get(const QByteArray &key, const QVariant &defaultValue)
{
    for (int i = 0; i < _args.size(); ++i) {
        if (_args[i].startsWith("--"+key+"=")) {
            QString strValue = _args[i].section('=', 1, 1);
            QVariant val(strValue);
            bool ok;

            switch (defaultValue.type()) {
            case QVariant::Double:
                val = val.toDouble(&ok);
                break;
            case QVariant::Int:
                val = val.toInt(&ok);
                break;
            case QVariant::UInt:
                val = val.toUInt(&ok);
                break;
            default:
                ok = true;
                break;
            }

            if (ok == true)
                return val;
        }
    }

    return defaultValue;
}

bool CmdLineParser::isSet(const QByteArray &key)
{
    for (int i = 0; i < _args.size(); ++i)
        if (_args[i] == ("--"+key) || _args[i].startsWith("--"+key+"="))
            return true;

    return false;
}
