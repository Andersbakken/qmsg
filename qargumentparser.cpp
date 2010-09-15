#include "qargumentparser.h"
#include <qregexp.h>
#include <stdlib.h>


QArgumentsParser::QArgumentsParser(int argc, char** argv)
{
    args << argv[0];
    for (int i=1; i<argc; ++i) {
	QString tmp = argv[i];
        if (!tmp.startsWith("--") && tmp.startsWith("-")) {
	    for (int j=1; j<(int)tmp.length(); ++j) {
		args << QString("-" + tmp[j]);
	    }
	} else {
	    args << tmp;
	}
    }
}

QArgumentsParser::~QArgumentsParser()
{

}

void QArgumentsParser::exec()
{
    setupArguments();

    int argc = args.count();
    appname = args[0];
    apppath = getenv("PWD");
    int slash = appname.lastIndexOf('/');
    if (slash != -1) {
        apppath = appname.left(slash);
	appname = appname.mid(slash + 1);
    }
    if (argc == 1) {
// 	setErrorString(QString("No arguments passed to %1").arg(appname));
// 	return;
    }
    int i;
    for (i=1; i<argc; ++i) {
        if (isArgument(args[i])) {
	    QSwitchArgument arg = getArgumentType(args[i]);
 	    int extra = arg.values().count();
	    if (extra > 0) {
		if (i+extra >= argc) {
		    setErrorString(QString("Not enough arguments passed to %1").arg(args[i]));
		    return;
		}
		QString value = args[++i];
		--extra;
		while (extra > 0) {
		    value += " " + args[++i];
		    --extra;
		}
		if (arg.values().count() > 1)
		    arg.setValues(value.split(' '));
		else
		    arg.setValue(value);
	    }
	    argumentsSet.append(arg);
	} else {
	    setErrorString(QString("Unknown argument %1").arg(args[i]));
	    return;
	}
    }
    if (!checkInvalidCombinations()) {
	return;
    }

    for (i=0; i<(int)argumentsSet.count(); ++i) {
        processArgument(argumentsSet[i]);
    }
}

bool QArgumentsParser::checkInvalidCombinations()
{
    return true;
}

QString QArgumentsParser::usage() const
{
    static const int argMaxlength = 40;
    static const int descriptionMaxLength = 80 - argMaxlength - 10;
    QString usageStr = QString("USAGE: %1 [arguments]...\n\n"
				"Arguments:\n").arg(appname);

    QArgumentList::ConstIterator i = validArgumentTypes.begin();
    while (i!=validArgumentTypes.end()) {
	QString v;
	QString type = (*i).first.types().join(" or ");
	for (int j=0; j<(int)(*i).first.values().count(); ++j) {
	    QString value = (*i).first.values()[j];
	    if ((int)(type.length() + v.length() + value.length()) > argMaxlength)
		v += QString("\n");
	    v += QString(" [%1]").arg(value);
	}

	QString description = (*i).second;
	if ((int)description.length() > descriptionMaxLength) {
	    for (int j=argMaxlength; j<(int)description.length(); j+=descriptionMaxLength) {
		int word = description.lastIndexOf(" ", j);
 		if (description[word].isSpace())
		    description.remove(word, 1);

		description.insert(word, "\n");
	    }

	    description.replace(QRegExp("(\n)"), "\\1" + QString().leftJustified(argMaxlength + 1, ' '));
	}
	if (v.contains('\n')) {
	    int lastNewLine = v.lastIndexOf('\n');
	    v = v.leftJustified(lastNewLine + argMaxlength + 2, ' ');
	}
	usageStr.append(" " + (type + v).
                        leftJustified(argMaxlength) + description + "\n");
	++i;
    }

    return usageStr;
}

bool QArgumentsParser::isArgument(const QString &type) const
{
    QArgumentList::ConstIterator i = validArgumentTypes.begin();

    while (i!=validArgumentTypes.end()) {
	if ((*i).first.is(type))
	    return true;
	++i;
    }

    return false;
}

bool QArgumentsParser::isArgumentSet(const QString &type) const
{
    QSwitchArguments::ConstIterator it;
    for (it = argumentsSet.begin(); it != argumentsSet.end(); ++it) {
	if ((*it).is(type))
	    return true;
    }
    return false;
}

bool QArgumentsParser::isValid() const
{
    return mErrorString.isEmpty();
}

QSwitchArgument QArgumentsParser::getArgument(const QString &type) const
{
    QSwitchArguments list = argumentsSet;
    QSwitchArguments::iterator i=list.begin();
    while (i!=list.end()) {
	if ((*i).is(type))
	    return (*i);
	++i;
    }

    return QSwitchArgument();
}

QSwitchArguments QArgumentsParser::arguments() const
{
    return argumentsSet;
}

QSwitchArgument QArgumentsParser::getArgumentType(const QString &arg) const
{
    QArgumentList::ConstIterator i = validArgumentTypes.begin();
    while (i!=validArgumentTypes.end()) {
	if ((*i).first.is(arg))
	    return (*i).first;
	++i;
    }

    return QSwitchArgument();
}

QString QArgumentsParser::value(const QString &type) const
{
    return values(type).join(" ");
}

QStringList QArgumentsParser::values(const QString &type) const
{
    QSwitchArgument s = getArgument(type);
    return s.values();
}

QString QArgumentsParser::error() const
{
    return mErrorString;
}

QArgumentList QArgumentsParser::validArguments() const
{
    return validArgumentTypes;
}

void QArgumentsParser::addArgument(const QSwitchArgument &arg, const QString &description)
{
    validArgumentTypes.append(qMakePair(arg, description));
}

void QArgumentsParser::setErrorString(const QString &msg)
{
    mErrorString = msg;
}

void QArgumentsParser::processArgument(const QSwitchArgument &/*arg*/)
{
}

void QArgumentsParser::addText(const QString &text)
{
    validArgumentTypes.append(qMakePair(QSwitchArgument(text), QString("")));
}

void QArgumentsParser::setArguments(const QSwitchArguments& args)
{
    argumentsSet = args;
}

bool QArgumentsParser::isEmpty() const
{
    return argumentsSet.isEmpty();
}

int QArgumentsParser::count() const
{
    return argumentsSet.count();
}

QString QArgumentsParser::applicationName() const
{
    return appname;
}

QString QArgumentsParser::applicationPath() const
{
    return apppath;
}
