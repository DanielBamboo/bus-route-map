#ifndef STRINGOP_H
#define STRINGOP_H

#include <string>
#include <QString>

class StringOp
{
public:
    StringOp();
    std::string qstr2str(const QString);
    QString str2qstr(const std::string);
};

#endif // STRINGOP_H
