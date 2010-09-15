#include "qargumentparser.h"
#include "qswitchargument.h"
#include <limits.h>
#include <QLabel>
#include <QTextDocument>
#include <QWidget>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QString>
#include <QObject>
#include <QRegExp>
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTime>
#include <QFrame>
#include <QPalette>
#include <QColorGroup>
#include <QFont>
#include <QRect>
#include <QApplication>
#include <QTimer>
#include <QDesktopWidget>

class Label : public QLabel
{
public:
    Label(QWidget *parent, Qt::WFlags f = 0)
	: QLabel(parent, f | Qt::X11BypassWindowManagerHint), showingExtra(false), closeTimer(-1)
    {
    }

    void start(int s)
    {
	closeTimer = startTimer(s);
    }
    void timerEvent(QTimerEvent *e)
    {
	if (e->timerId() == closeTimer) {
	    close();
	}
    }
    void mousePressEvent(QMouseEvent *e)
    {
	if (e->button() == Qt::RightButton) {
	    if (closeTimer != -1) {
		killTimer(closeTimer);
		closeTimer = -1;
	    }
	    if (!extra.isEmpty()) {
		setText(showingExtra ? normal : normal + (textFormat() == Qt::RichText ? "<br/>" : "\n") + extra);
		showingExtra = !showingExtra;
                if (showingExtra) {
                    showFullScreen();
                } else {
                    showNormal();
                }
	    } else {
                adjustSize();
            }
	} else {
	    close();
	}
    }
    bool showingExtra;
    int closeTimer;
    QString normal, extra;
};


class QMsgArgParser : public QObject, public QArgumentsParser
{
    Q_OBJECT
public:
    QMsgArgParser(int argc, char** argv)
	: QArgumentsParser(argc, argv), runProgram(true), html(false), wordWrap(true), sleep(0), wait(0)
    {
	x = y = w = h = INT_MAX;
    }
    bool runProgram, html, wordWrap;
    QString message, extra;
    int sleep;
    quint64 wait;
    int x, y, w, h;

    int digit(const QString &str, bool *ok)
    {
	if (!*ok)
	    return -1;

	return (str[0] == 'x' ? str.mid(1).toInt(ok) : str.toInt(ok));
    }

    void parseGeometry(const QString &str)
    {
	QRegExp rx("([\\+\\-x])");
	QString string = str.trimmed();
	string.replace(rx, " \\1");
	QStringList stringList = string.split(" ");
	bool ok;
	if (stringList.count() == 4) {
	    w = digit(stringList[0], &ok);
	    h = digit(stringList[1], &ok);
	    x = digit(stringList[2], &ok);
	    y = digit(stringList[3], &ok);
	} else if (stringList.count() == 2) {
	    if (stringList[1][0] == 'x') {
		w = digit(stringList[0], &ok);
		h = digit(stringList[1], &ok);
	    } else {
		x = digit(stringList[0], &ok);
		y = digit(stringList[1], &ok);
	    }
	} else {
            ok = false;
	}
	if (!ok)
	    x = y = w = h = INT_MAX;
    }

    QString recent(const QString &str) const
    {
        static const int kvarter = 900;
        QString date = str.left(24);
        if (date.length() == 24) {
            QDateTime dt = QDateTime::fromString(date);
            if (dt.isValid() && qAbs(dt.secsTo(QDateTime::currentDateTime())) < kvarter) {
                return "recent " + str;
            }
        }
        return str;
    }


    QString getLog(int msg) // < 0 ==> tail > 0 ==> head 0 ==> all
    {
        QFile file(QDir::homePath() + "/.qmsg.log");
        if (!file.open(QIODevice::ReadOnly)) {
            setErrorString(QString("Could not open file (%1)").arg(file.fileName()));
            return QString();
        }
        QStringList lines = ((QString)file.readAll()).split("\n");
        QString ret;
        if (msg == 0) {
            for (QStringList::const_iterator it = lines.begin(); it != lines.end(); ++it)
                ret += recent(*it) + "\n";
        } else {
            int from = 0;
            int to = lines.count();
            if (msg < 0) {
                from = to - msg;
            } else {
                to = msg;
            }
            while (to < from) {
                ret += recent(lines[to++]) + "\n";
            }
        }
        return ret;
    }

protected:
    void processArgument(const QSwitchArgument &arg)
    {
        if (arg.is("--help")) {
	    printf("%s\n", qPrintable(usage()));
	    runProgram = false;
	} else if (arg.is("--version")) {
	    printf("qmsg version 0.1\n");
	    runProgram = false;
	} else if (arg.is("--geometry")) {
	    parseGeometry(arg.value());
	} else if (arg.is("--sleep")) {
	    sleep = arg.value().toInt();
	} else if (arg.is("--wait")) {
            int multplier;
            QString tmp = arg.value();
            if (tmp.indexOf("d") == (int)tmp.length() - 1) {
                tmp.truncate(tmp.length() - 1);
                multplier = 24 * 60 * 60 * 1000;
            } else if (tmp.indexOf("h") == (int)tmp.length() - 1) {
                tmp.truncate(tmp.length() - 1);
                multplier = 60 * 60 * 1000;
            } else if (tmp.indexOf("s") == (int)tmp.length() - 1) {
                tmp.truncate(tmp.length() - 1);
                multplier = 1000;
            } else {
                multplier = 1000 * 60;
            }

            bool ok;
            wait = tmp.toULongLong(&ok) * multplier;
            if (!ok)
		setErrorString("Could not parse --wait '" + arg.value() + "'");
	} else if (arg.is("--at")) {
            QTime time = QTime::fromString(arg.value());
            static const uint MSECS_PER_DAY = 86400000;
            if (time.isValid()) {
                wait = (QTime::currentTime().msecsTo(time) + MSECS_PER_DAY) % MSECS_PER_DAY;
            } else {
		setErrorString("Could not parse --at '" + arg.value() + "'");
            }
	} else if (arg.is("--tail")) {
            printf("%s\n", qPrintable(getLog(-arg.value().toInt())));
            runProgram = false;
        } else if (arg.is("--head")) {
            printf("%s\n", qPrintable(getLog(arg.value().toInt())));
	    runProgram = false;
	} else if (arg.is("--log")) {
            printf("%s\n", qPrintable(getLog(0)));
	    runProgram = false;
        } else if (arg.is("--extra")) {
	    extra = arg.value();
        } else if (arg.is("--message")) {
	    message = arg.value();
	} else if (arg.is("--html")) {
	    html = true;
	} else if (arg.is("--no-wordwrap")) {
	    wordWrap = false;
	} else if (arg.is("--file")) {
	    QFile f(arg.value());
	    if (!f.open(QIODevice::ReadOnly)) {
		setErrorString("Could not open '" + f.fileName() + "' for reading");
	    } else {
		message = (QString)f.readAll();
	    }
	}
    }

    void setupArguments()
    {
	addArgument(QSwitchArgument("--help|-h"), "Print this screen");
	addArgument(QSwitchArgument("--html"), "Parse message as HTML");
	addArgument(QSwitchArgument("--version|-v"), "Print version of qmsg");
	addArgument(QSwitchArgument("--sleep|-s", "seconds"), "Quit after 'seconds' seconds");
	addArgument(QSwitchArgument("--wait|-w", "minutes"), "Wait 'minutes' minutes before starting");
	addArgument(QSwitchArgument("--at|-a", "time"), "Start at 'time'");
	addArgument(QSwitchArgument("--message|-m", "message"), "Display this message");
	addArgument(QSwitchArgument("--geometry|-g", "geo"), "Lay out window with geo");
	addArgument(QSwitchArgument("--file|-f", "message"), "Display this file");
	addArgument(QSwitchArgument("--tail|-t", "lines"), "Print the last 'lines' lines of logfile");
	addArgument(QSwitchArgument("--head", "lines"), "Print the first 'lines' lines of logfile");
	addArgument(QSwitchArgument("--log|-l"), "Print the the logfile");
	addArgument(QSwitchArgument("--extra|-e", "extra"), "Extra message to be printed if user clicks the label");
	addArgument(QSwitchArgument("--no-wordwrap"), "Disable word wrap");
    }

    bool checkInvalidCombinations()
    {
	if (isArgumentSet("--help")) {
	    QSwitchArguments al;
	    al << QSwitchArgument("--help");
	    setArguments(al);
	    return true;
	} else if (isArgumentSet("--version")) {
	    QSwitchArguments al;
	    al << QSwitchArgument("--version");
	    setArguments(al);
	    return true;
	} else if (isArgumentSet("--log")) {
	    QSwitchArguments al;
	    al << QSwitchArgument("--log");
	    setArguments(al);
	    return true;
	} else if (!isArgumentSet("--message") && !isArgumentSet("--file")) {
	    QFile si;
	    if (si.open(stdin, QIODevice::ReadOnly)) {
		message = (QString)si.readAll();
	    } else {
		setErrorString("No message to display");
		return false;
	    }
	}
	return true;
    }

public slots:
    void createWidget()
    {
	Label *label = new Label(0, 0);
	if (sleep > 0) {
	    label->start(sleep * 1000);
	}
        if (html) {
            QStringList supported;
            supported << "b" << "font" << "i" << "br" << "p";
            label->setTextFormat(Qt::RichText);
            int index = 0;
            while ((index = message.indexOf('<', index)) != -1) {
                ++index;
                if (message[index] == '/')
                    continue;
                int end = message.indexOf(' ', index);
                int endtag = message.indexOf('>', index);
                if (endtag < end || end == -1)
                    end = endtag;
                QString tag = message.mid(index, end - index);
                if (!supported.contains(tag)) {
                    message.replace(endtag, 1, "&gt;");
                    message.replace(index - 1, 1, "&lt;");
                }
            }
        } else {
            label->setTextFormat(Qt::PlainText);
        }

        label->setText(message);
	label->normal = message;
	if ((html && Qt::mightBeRichText(extra)) || !html) {
	    label->extra = extra;
	} else {
	    label->extra = Qt::convertFromPlainText(extra);
	}
        label->setWordWrap(wordWrap);
	label->setMargin(20);
	label->setFrameStyle(QFrame::Panel | QFrame::Raised);
	label->setLineWidth(2);
	QPalette pal = label->palette();
	pal.setBrush(QPalette::Active, QPalette::Foreground, label->palette().background());
	pal.setBrush(QPalette::Active, QPalette::Background, label->palette().foreground());
	pal.setBrush(QPalette::Disabled, QPalette::Foreground, label->palette().background());
	pal.setBrush(QPalette::Disabled, QPalette::Background, label->palette().foreground());
	pal.setBrush(QPalette::Inactive, QPalette::Foreground, label->palette().background());
	pal.setBrush(QPalette::Inactive, QPalette::Background, label->palette().foreground());
	label->setPalette(pal);
	QFont f = label->font();
	f.setPointSize(qMax(20, f.pointSize()));
	label->setFont(f);
	QRect r = qApp->desktop()->availableGeometry(label);

	if (w == INT_MAX) {
	    w = h = 200;
	} else {
	    w = qMax(200, qMin(r.width(), w));
	    h = qMax(200, qMin(r.height(), h));
	}
	label->setMinimumSize(qMin(r.width(), w), qMin(r.height(), h));
	label->adjustSize();
        w = label->width();
        h = label->height();
	//label->setMaximumSize(r.size());
	if (x == INT_MAX) {
	    x = (int)((r.width() - w) / 2);
	    y = (int)((r.height() - h) / 2);
	} else {
	    x = qMin(r.width() - w, x);
	    y = qMin(r.height() - h, y);
	}
	label->move(x, y);
        QFile file(QDir::homePath() + "/.qmsg.log");
        file.open(QIODevice::WriteOnly | QIODevice::Append);
        QString write = QDateTime::currentDateTime().toString() + ": " + message + "\n";
        file.write(write.toLocal8Bit(), write.length());
        file.close();
	label->show();
	//qDebug("x %d y %d w %d h %d", x, y, w, h);
    }
};

#include "main.moc"

int main(int argc, char **argv)
{
    QMsgArgParser ap(argc, argv);
    ap.exec();
    if (!ap.isValid()) {
	fprintf(stderr, "%s\n", qPrintable(ap.error()));
	return 1;
    } else if (!ap.runProgram) {
	return 0;
    }
    QByteArray argv0 = argv[0];
    if (QDir::isRelativePath(argv0)) {
        QDir dir(argv0);
        argv0 = dir.absolutePath().toLocal8Bit();
        argv[0] = argv0.data();
    }
    QApplication a(argc, argv);
    QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    QTimer::singleShot(ap.wait, &ap, SLOT(createWidget()));
    return a.exec();
}
