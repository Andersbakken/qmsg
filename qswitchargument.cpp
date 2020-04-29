#include "qswitchargument.h"
#include <QDebug>

QSwitchArgument::QSwitchArgument(const QString &type, const QString &value)
{
    mTypes = type.split('|');
    if (!value.isEmpty())
        mValues = value.split('|');
}

QString QSwitchArgument::type(int index) const
{
    return (int)mTypes.count() > index ? mTypes[index] : QString();
}

QString QSwitchArgument::value(int index) const
{
    return (int)mValues.count() > index ? mValues[index] : QString();
}

QStringList QSwitchArgument::types() const
{
    return mTypes;
}

QStringList QSwitchArgument::values() const
{
    return mValues;
}

void QSwitchArgument::setType(const QString& type)
{
    mTypes.clear();
    mTypes.append(type);
}

void QSwitchArgument::setValue(const QString& value)
{
    mValues.clear();
    mValues.append(value);
}

void QSwitchArgument::addType(const QString& type)
{
    mTypes.append(type);
}

void QSwitchArgument::addValue(const QString& value)
{
    mValues.append(value);
}

void QSwitchArgument::setTypes(const QStringList& types)
{
    mTypes = types;
}

void QSwitchArgument::setValues(const QStringList& values)
{
    mValues = values;
}


bool QSwitchArgument::isValid() const
{
    return !mTypes.isEmpty();
}

QSwitchArgument &QSwitchArgument::operator=(const QSwitchArgument & t)
{
    mTypes = t.mTypes;
    mValues = t.mValues;
    return *this;
}

QSwitchArgument &QSwitchArgument::operator=(const QString & t)
{
    setType(t);
    return *this;
}

QSwitchArgument &QSwitchArgument::operator=(const QByteArray & t)
{
    setType(t);
    return *this;
}

QSwitchArgument &QSwitchArgument::operator=(const char * t)
{
    setType(t);
    return *this;
}

bool QSwitchArgument::operator==(const QSwitchArgument &arg) const
{
    for (int i=0; i<(int)arg.types().count(); ++i) {
        if (is(arg.types()[i]))
            return true;
    }
    return false;
}

bool QSwitchArgument::operator!=(const QSwitchArgument &arg) const
{
    return !(*this == arg);
}

bool QSwitchArgument::operator<(const QSwitchArgument &arg) const
{
    return type() < arg.type();
}

bool QSwitchArgument::operator<(const QString &arg) const
{
    return type() < arg;
}

bool QSwitchArgument::operator<(const char *arg) const
{
    return type() < QString(arg);
}

bool QSwitchArgument::is(const QString& type) const
{
    for (int i=0; i<(int)mTypes.count(); ++i) {
        if (type == mTypes[i]) {
            return true;
        }
    }
    return false;
}
