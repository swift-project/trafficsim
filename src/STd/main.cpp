/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "STLib/ClientContainer.h"
#include "ClientProcess.h"
#include "AirplaneClientProcess.h"
#include "ControllerClientProcess.h"
#include "helper.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		qDebug() << "You have to add the filename of the XML-file!";
		qDebug() << "usage:";
		qDebug() << "      " << argv[0] << " <XML-FILE>";
		return 1;
	}
	QString FileName = "";
	if(argc >= 2)
	{
		FileName = argv[1];
	}

	QCoreApplication a(argc, argv);
	ClientContainer Cont(FileName);

	QList<QThread*> Threads;
	ThreadHelper * closer = new ThreadHelper(&Threads);

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
			QThread::connect(thread, SIGNAL(started()), process, SLOT(Run()));
			//QThread::connect(thread, SIGNAL(finished()), process, SLOT(deleteLater()));
			QThread::connect(thread, SIGNAL(finished()), closer, SLOT(AllThreadsClosed()));
			QThread::connect(process, SIGNAL(ClientFinished()), thread, SLOT(quit()));

			process->moveToThread(thread);
			thread->start();

			Threads.append(thread);
		}
	}
	return a.exec();
}
