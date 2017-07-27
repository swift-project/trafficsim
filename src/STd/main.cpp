/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//#define VATSIM_GERMANY_TEST

#include <QCommandLineParser>

#include "STLib/ClientContainer.h"
#include "ClientProcess.h"
#include "AirplaneClientProcess.h"
#include "ControllerClientProcess.h"
#include "helper.h"

#ifdef VATSIM_GERMANY_TEST
#define SERVER_ADDR "vatsim-germany.org"
#define SERVER_PORT 6809
#define USER_ID     "sup"
#define USER_PASS   "sup"
#else
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 6809
#define USER_ID     "sup"
#define USER_PASS   "sup"
#endif
#define DEFAULT_FILENAME "../Logs/Onlineday_LOWW.xml"


QString ClientProcess::Server = SERVER_ADDR;
qint16 ClientProcess::Port = SERVER_PORT;
QString ClientProcess::Username = USER_ID;
QString ClientProcess::Password = USER_PASS;

int main(int argc, char *argv[])
{
    qDebug() << "Servus!";
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("trafficsim");
    QCommandLineParser parser;
    parser.setApplicationDescription("traffic simulator is a tool to replay recorded traffic to an FSD server.");
    parser.addHelpOption();
    parser.addOption({{"x", "xml"},
                      QCoreApplication::translate("main", "Scenario <scenariofile> in xml format"),
                      QCoreApplication::translate("main", "scenariofile"),
                      DEFAULT_FILENAME
                     });
    parser.addOption({{"s", "server"},
                      QCoreApplication::translate("main", "FSD server <url>"),
                      QCoreApplication::translate("main", "url"),
                      SERVER_ADDR
                     });
    parser.addOption({{"P", "port"},
                      QCoreApplication::translate("main", "FSD server <port>"),
                      QCoreApplication::translate("main", "port"),
                      QString::number(SERVER_PORT)
                     });
    parser.addOption({{"u", "user"},
                      QCoreApplication::translate("main", "FSD <user>"),
                      QCoreApplication::translate("main", "user"),
                      USER_ID
                     });
    parser.addOption({{"p", "password"},
                      QCoreApplication::translate("main", "FSD user <password>"),
                      QCoreApplication::translate("main", "password"),
                      USER_PASS
                     });

    // Process the actual command line arguments given by the user
    parser.process(a);

    QString FileName = parser.value("xml");
    ClientProcess::Server = parser.value("server");
    ClientProcess::Port = parser.value("port").toInt();
    ClientProcess::Username = parser.value("user");
    ClientProcess::Password = parser.value("password");

    qDebug() << "XML Filename:      " << FileName;
    qDebug() << "FSD Serveraddress: " << ClientProcess::Server;
    qDebug() << "FSD Port:          " << ClientProcess::Port;
    qDebug() << "FSD Username:      " << ClientProcess::Username;
    qDebug() << "FSD Password:      " << ClientProcess::Password;

    qDebug() << "Loading Logfile!";
    ClientContainer Cont(FileName);

    QList<QThread *> Threads;
    ThreadHelper *closer = new ThreadHelper(&Threads);

    qDebug() << "Create and Start Threads";
    for (ClientContainer::iterator iter = Cont.begin(); iter != Cont.end(); iter++)
    {
        ClientProcess *process = 0;
        if ((*iter)->GetType() == AirplaneType)
        {
            process = new AirplaneClientProcess(*iter);
        }
        else if ((*iter)->GetType() == ControllerType)
        {
            process = new ControllerClientProcess(*iter);
        }
        if (process != 0)
        {
            QThread *thread = new QThread();
            QThread::connect(thread, &QThread::started, process, &ClientProcess::Run);
            QThread::connect(thread, &QThread::finished, closer, &ThreadHelper::AllThreadsClosed);
            QThread::connect(process, &ClientProcess::ClientFinished, thread, &QThread::quit);

            process->moveToThread(thread);
            thread->start();

            Threads.append(thread);
        }
    }
    if (Cont.size() == 0)
    {
        qDebug() << "No Data!";
        return 0;
    }
    return a.exec();
}
