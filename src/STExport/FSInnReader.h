/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FS_INN_READER_H_
#define FS_INN_READER_H_

#include "STLib/ClientContainer.h"

enum Direction
{
	eDNotInit,
	eSent,
	eRecv,
};

class FSInnReader
{
public:
	FSInnReader(QString Filename);
	bool ReadFile(ClientContainer & Cont);

private:
	QFile mFile;

	bool ExportLine(QString & Line, int & time, Direction & direction, UpdateReason & command, QString & Argument);
	int ConvertTimeStr(QString & TimeStr);
};

bool ReadLine(QFile & File, QString & String);

#endif
