/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//#define VATSIM_GERMANY_TEST

// QtArg include.
#include <QtArg/Arg>
#include <QtArg/XorArg>
#include <QtArg/CmdLine>
#include <QtArg/Help>

#include "STLib/ClientContainer.h"
#include "ClientProcess.h"
#include "AirplaneClientProcess.h"
#include "ControllerClientProcess.h"
#include "helper.h"

#ifdef VATSIM_GERMANY_TEST
    #define SERVER_ADDR	"vatsim-germany.org"
    #define SERVER_PORT	6809
    #define USER_ID		"111111"
    #define USER_PASS	"111111"
#else
    #define SERVER_ADDR	"192.168.222.128"
    #define SERVER_PORT	6809
    #define USER_ID		"sup"
    #define USER_PASS	"sup"
#endif


QString ClientProcess::Server = SERVER_ADDR;
qint16 ClientProcess::Port = SERVER_PORT;
QString ClientProcess::Username = USER_ID;
QString ClientProcess::Password = USER_PASS;

int main(int argc, char *argv[])
{
    QString FileName = "";
    try
    {
        QtArgCmdLine cmd(argc, argv);

        QtArg xmlFile(QLatin1String("xml"), QLatin1String("XML Logfile"), true, true);
        QtArg server(QLatin1String("server"), QLatin1String("FSD Serveraddress"), false, true);
        QtArg user(QLatin1String("user"), QLatin1String("FSD Username"), false, true);
        QtArg password(QLatin1String("password"), QLatin1String("FSD Password"), false, true);
        QtArg port(QLatin1String("port"), QLatin1String("FSD Port"), false, true);

        QtArgHelp help(&cmd);
        help.printer()->setProgramDescription(QLatin1String("Testing help printing."));
        help.printer()->setExecutableName(QLatin1String(argv[0]));

        cmd.addArg(xmlFile);
        cmd.addArg(server);
        cmd.addArg(port);
        cmd.addArg(user);
        cmd.addArg(password);
        cmd.addArg(help);

        cmd.parse();

        FileName = xmlFile.value().toString();
        if(server.isDefined())
        {
            ClientProcess::Server = server.value().toString();
        }
        if(port.isDefined())
        {
            ClientProcess::Port = port.value().toInt();
        }
        if(user.isDefined())
        {
            ClientProcess::Username = user.value().toString();
        }
        if(password.isDefined())
        {
            ClientProcess::Password = password.value().toString();
        }
    }
    catch(const QtArgHelpHasPrintedEx & x)
    {
        qDebug() << x.what();
        return -1;
    }
    catch(const QtArgBaseException & x)
    {
        qDebug() << x.what();
        return -1;
    }
    //return 0;

    qDebug() << "XML Filename:      " << FileName;
    qDebug() << "FSD Serveraddress: " << ClientProcess::Server;
    qDebug() << "FSD Port:          " << ClientProcess::Port;
    qDebug() << "FSD Username:      " << ClientProcess::Username;
    qDebug() << "FSD Password:      " << ClientProcess::Password;

	QCoreApplication a(argc, argv);
    qDebug() << "Loading Logfile!";
	ClientContainer Cont(FileName);

	QList<QThread*> Threads;
	ThreadHelper * closer = new ThreadHelper(&Threads);

    qDebug() << "Create and Start Threads";
	for(ClientContainer::iterator iter = Cont.begin(); iter != Cont.end(); iter++)
	{
		ClientProcess * process;
		if((*iter)->GetType() == AirplaneType)
		{
			process = new AirplaneClientProcess(*iter);
		}
		else if((*iter)->GetType() == ControllerType)
		{
			process = new ControllerClientProcess(*iter);
		}
		if(process != 0)
        {
            QThread * thread = new QThread();
            QThread::connect(thread, &QThread::started, process, &ClientProcess::Run);
            QThread::connect(thread, &QThread::finished, closer, &ThreadHelper::AllThreadsClosed);
            QThread::connect(process, &ClientProcess::ClientFinished, thread, &QThread::quit);

			process->moveToThread(thread);
			thread->start();

			Threads.append(thread);
		}
	}
    if(Cont.size() == 0)
    {
        qDebug() << "No Data!";
        return 0;
    }
	return a.exec();
}
