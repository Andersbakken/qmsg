#ifndef QARGUMENTSPARSER_H
#define QARGUMENTSPARSER_H

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>
#include "qswitchargument.h"

typedef QList<QSwitchArgument> QSwitchArguments;
typedef QList< QPair< QSwitchArgument,QString> > QArgumentList;

class QArgumentsParser
{
public:
    QArgumentsParser(int argc, char** argv);
    virtual ~QArgumentsParser();

    virtual void exec();
    QSwitchArguments arguments() const;
    bool isArgumentSet(const QString &type) const;
    QString value(const QString &type) const;
    QStringList values(const QString &type) const;

    virtual QString usage() const;
    virtual bool isValid() const;
    bool isEmpty() const;
    int count() const;

    void setArguments(const QSwitchArguments &args);
    virtual void setErrorString(const QString &msg);
    QString error() const;
    QString applicationName() const;
    QString applicationPath() const;
protected:
    virtual void processArgument(const QSwitchArgument &arg);
    virtual void setupArguments() = 0;
    void addArgument(const QSwitchArgument &arg, const QString &description = QString::null);
    void addText(const QString &text = QString::null);
    virtual bool checkInvalidCombinations();

    QArgumentList validArguments() const;
    bool isArgument(const QString &type) const;
    QSwitchArgument getArgumentType(const QString &arg) const;
    QSwitchArgument getArgument(const QString &type) const;

private:
    QString appname, apppath;
    QString mErrorString;

    QStringList args;

    QSwitchArguments argumentsSet;
    QArgumentList validArgumentTypes;
};



#endif
