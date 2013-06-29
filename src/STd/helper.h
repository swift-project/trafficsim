/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HELPER_H_
#define HELPER_H_

#include <QObject>

class ThreadHelper : public QObject
{
	Q_OBJECT
public:
	ThreadHelper(QList<QThread*> * Threads);

public slots:
	void AllThreadsClosed();

private:
	QList<QThread*> * mThreads;
};

#endif
