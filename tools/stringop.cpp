#include "stringop.h"

StringOp::StringOp()
{

}

std::string StringOp::qstr2str(const QString qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return std::string(cdata);
}

QString StringOp::str2qstr(const std::string str)
{
    return QString::fromLocal8Bit(str.data());
}

