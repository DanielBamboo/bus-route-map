#ifndef STRINGOP_H
#define STRINGOP_H

#include <string>
#include <QString>

class StringOp
{
public:
    StringOp();
    static std::string qstr2str(const QString);
    static QString str2qstr(const std::string);
};

#endif // STRINGOP_H
