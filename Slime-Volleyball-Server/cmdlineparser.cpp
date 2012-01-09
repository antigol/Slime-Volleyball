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

            if (val.canConvert(defaultValue.type()))
                return val;
        }
    }

    return defaultValue;
}
