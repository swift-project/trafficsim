/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "exporter.h"

QList<QString> Seperate(QString Str, QChar seperator)
{
	QList<QString> List;
	QString Temp = "";
	for(int i = 0; i < Str.size(); i++)
	{
		if(Str[i] == seperator)
		{
			List.append(Temp);
			Temp = "";
		}
		else
		{
			Temp += Str[i];
		}
	}
	if(Temp != "")
	{
		List.append(Temp);
	}
	return List;
}
