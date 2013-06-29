/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QThread>
#include <QCoreApplication>
#include "helper.h"

ThreadHelper::ThreadHelper(QList<QThread*> * Threads)
{
	mThreads = Threads;
}

void ThreadHelper::AllThreadsClosed()
{
	//cout << "all threads closed?" << endl;
	for(QList<QThread*>::iterator iter = mThreads->begin(); iter != mThreads->end(); iter++)
	{
		(*iter)->wait(10); // give the thread time to finish... in Qt finished means not he is finished completely ;)
		if((*iter)->isRunning())
		{
			return;
		}
	}
	QCoreApplication::exit();
}

