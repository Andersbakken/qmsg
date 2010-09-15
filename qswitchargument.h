#ifndef QSWITCHARGUMENT_H
#define QSWITCHARGUMENT_H

#include "qswitchargument.h"
#include <QString>
#include <QStringList>

class QSwitchArgument {
public:
    QSwitchArgument(const QString &type = QString::null, const QString &value = QString::null);

    QString type(int index = 0) const;
    QString value(int index = 0) const;

    QStringList types() const;
    QStringList values() const;

    bool is(const QString& type) const;

    void setType(const QString& type);
    void setValue(const QString& value);

    void addType(const QString& type);
    void addValue(const QString& value);

    void setTypes(const QStringList& types);
    void setValues(const QStringList& values);

    bool isValid() const;

    QStringList mTypes;
    QStringList mValues;
    // make stringlist to allow for more than one value set.

// operators
public:
    QSwitchArgument &operator=(const QSwitchArgument & t);
    QSwitchArgument &operator=(const QString & t);
    QSwitchArgument &operator=(const QByteArray & t);
    QSwitchArgument &operator=(const char * t);

    bool operator==(const QSwitchArgument &arg) const;
    bool operator!=(const QSwitchArgument &arg) const;
    bool operator<(const QSwitchArgument & t) const;
    bool operator<(const QString &) const;
    bool operator<(const char *) const;

// friends
    friend inline bool operator==(const QString &arg1, const QSwitchArgument &arg2);
    friend inline bool operator==(const QSwitchArgument &arg1, const QString &arg2);

    friend inline bool operator==(const QByteArray &arg1, const QSwitchArgument &arg2);
    friend inline bool operator==(const QSwitchArgument &arg1, const QByteArray &arg2);

    friend inline bool operator==(const char *arg1, const QSwitchArgument &arg2);
    friend inline bool operator==(const QSwitchArgument &arg1, const char *arg2);
};


inline bool operator==(const QString &arg1, const QSwitchArgument &arg2)
{
    return arg2.is(arg1);
}

inline bool operator==(const QSwitchArgument &arg1, const QString &arg2)
{
    return arg1.is(arg2);
}

inline bool operator==(const QByteArray &arg1, const QSwitchArgument &arg2)
{
    return arg2.is(arg1);
}


inline bool operator==(const QSwitchArgument &arg1, const QByteArray &arg2)
{
    return arg1.is(arg2);
}

inline bool operator==(const char *arg1, const QSwitchArgument &arg2)
{
    return arg2.is(arg1);
}

inline bool operator==(const QSwitchArgument &arg1, const char *arg2)
{
    return arg1.is(arg2);
}

#endif
