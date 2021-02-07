#ifndef STRINGOP_H
#define STRINGOP_H

#include <string>
#include <QString>

//由于windows的默认编码问题使得string 和 QString 不兼容，需要使用这个类进行转换
//QString 是 qt里面的string类型

class StringOp
{
public:
    StringOp();
    static std::string qstr2str(const QString);	// QString	->	string
    static QString str2qstr(const std::string);	// string	->	QString
};

#endif // STRINGOP_H
