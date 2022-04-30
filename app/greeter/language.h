//
// Created by dingjing on 4/5/22.
//

#ifndef GRACEFUL_GREETER_LANGUAGE_H
#define GRACEFUL_GREETER_LANGUAGE_H

#include <QVector>

struct Language
{
    QString code;                       // 语言码，例如zh_CN, en, en_US
    QString name;                       // 语言码对应的名称，中文, 英语
    QString territory;                  // 地区
};

typedef QVector<Language> LanguagesVector;

LanguagesVector &getLanguages();
Language getLanguage(const QString &lang);


#endif //GRACEFUL_GREETER_LANGUAGE_H
