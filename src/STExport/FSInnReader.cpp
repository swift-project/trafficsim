/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FSInnReader.h"
#include "STLib/exporter.h"

FSInnReader::FSInnReader(QString Filename)
	: mFile(Filename)
{
}

bool FSInnReader::ReadFile(ClientContainer & Cont)
{
	if (!mFile.open(QFile::ReadOnly | QFile::Text))
	{
		qDebug() << "Error: Cannot write file "
			<< qPrintable(mFile.errorString());
		return false;
	}
	bool needStartTime = true;
	while(!mFile.atEnd())
	{
		QString Line = "";
		if(!ReadLine(mFile, Line))
		{
			mFile.close();
			return false;
		}
		int Time;
		Direction dir;
		UpdateReason com;
		QString Argument;
		if(ExportLine(Line, Time, dir, com, Argument))
		{
			switch (com)
			{
			case AddAirplaneReason:
				{
					QString Callsign = (Seperate(Argument, ':'))[0];
					pClient client = Cont.SearchClient(Callsign, AirplaneType);
					client->AddTimeUpdate(pTimeUpdate(new TimeUpdate(AddAirplaneReason, Time)));
				}
				break;
			case RemoveAirplaneReason:
				{
					QString Callsign = (Seperate(Argument, ':'))[0];
					pClient client = Cont.SearchClient(Callsign, AirplaneType);
					client->AddTimeUpdate(pTimeUpdate(new TimeUpdate(RemoveAirplaneReason, Time)));
				}
				break;
			case AddATCReason:
				{
					QString Callsign = (Seperate(Argument, ':'))[0];
					pClient client = Cont.SearchClient(Callsign, ControllerType);
					client->AddTimeUpdate(pTimeUpdate(new TimeUpdate(AddATCReason, Time)));
				}
				break;
			case RemoveATCReason:
				{
					QString Callsign = (Seperate(Argument, ':'))[0];
					pClient client = Cont.SearchClient(Callsign, ControllerType);
					client->AddTimeUpdate(pTimeUpdate(new TimeUpdate(RemoveATCReason, Time)));
				}
				break;
			case PositionAirplaneReason:
				{
					QString Callsign = (Seperate(Argument, ':'))[1];
					pClient client = Cont.SearchClient(Callsign, AirplaneType);
					client->AddTimeUpdate(pTimeUpdate(new AirplanePositionUpdate(Time, Argument)));
				}
				break;
			case PositionATCReason:
				{
					QString Callsign = (Seperate(Argument, ':'))[0];
					pClient client = Cont.SearchClient(Callsign, ControllerType);
					client->AddTimeUpdate(pTimeUpdate(new ControllerPositionUpdate(Time, Argument)));
				}
				break;
			case TextMsg:
				{
					if(dir == eRecv)
					{
						QString Callsign = (Seperate(Argument, ':'))[0];
						pClient client = Cont.SearchClient(Callsign, NotDefinedType);
						if(client != 0)
						{
							client->AddTimeUpdate(pTimeUpdate(new TextMessageUpdate(Time, Argument)));
						}
					}
				}
				break;
			case SBInfoReason:
				{
					QList<QString> List = Seperate(Argument, ':');
					if(List[2] == "FSIPI" || List[2] == "PI")
					{
						QString Callsign = List[0];
						Airplane * client = (Airplane*)(Cont.SearchClient(Callsign, AirplaneType).get());
						if(client != 0 && !client->IsAirplaneInfoSet())
						{
							client->SetAirplaneInfo(Argument);
						}
					}
				}
				break;
			}
			if(needStartTime)
			{
				Cont.SetStartTime(Time);
				needStartTime = false;
			}
		}
	}
	mFile.close();

	return true;
}

bool FSInnReader::ExportLine(QString & Line, int & time, Direction & direction, UpdateReason & command, QString & Argument)
{
	time = 0;
	direction = eDNotInit;
	command = NotInitReason;

	bool pars_time = false;
	bool pars_time_was = false;
	bool pars_command = false;
	bool pars_argument = false;
	QString TimeStr = "";
	for (int i = 1; i < Line.size(); i++)
	{
		// Time export:
		if(Line[i] == '[' && !pars_time_was)
		{
			pars_time = true;
			pars_time_was = true;
		}
		else if(pars_time && Line[i] == ' ')
		{
			pars_time = false;
			time = ConvertTimeStr(TimeStr);
		}
		if(pars_time && Line[i] != '[')
		{
			TimeStr += Line[i];
		}

		// argument export:
		if(pars_argument)
		{
			if(Line[i] != '<' && Line.size() - 1 != i)
			{
				Argument += Line[i];
			}
		}
		// command export:
		if(pars_command)
		{
			if(Line[i] == '@')
			{
				command = PositionAirplaneReason;
			}
			else if(Line[i] == '%')
			{
				command = PositionATCReason;
			}
			else if(Line[i] == '#' && Line[i+1] == 'A' && Line[i+2] == 'P')
			{
				command = AddAirplaneReason;
			}
			else if(Line[i] == '#' && Line[i+1] == 'D' && Line[i+2] == 'P')
			{
				command = RemoveAirplaneReason;
			}
			else if(Line[i] == '#' && Line[i+1] == 'A' && Line[i+2] == 'A')
			{
				command = AddATCReason;
			}
			else if(Line[i] == '#' && Line[i+1] == 'D' && Line[i+2] == 'A')
			{
				command = RemoveATCReason;
			}
			else if(Line[i] == '#' && Line[i+1] == 'T' && Line[i+2] == 'M')
			{
				command = TextMsg;
			}
			else if(Line[i] == '#' && Line[i+1] == 'S' && Line[i+2] == 'B')
			{
				command = SBInfoReason;
			}
			else
			{
				return false;
			}
			if(Line[i] == '#')
			{
				i += 2;
			}
			pars_argument = true;
			pars_command = false;
		}

		// direction export:
		if(Line[i] == 'F' && Line[i+1] == 'S' && Line[i+2] == 'D' && Line[i+3] ==  ' ')
		{
			if(Line[i+4] == 'S' && Line[i+5] == 'e' && Line[i+6] == 'n' && Line[i+7] == 't') // Sent
			{
				direction = eSent;
			}
			else if(Line[i+4] == 'R' && Line[i+5] == 'e' && Line[i+6] == 'c' && Line[i+7] == 'v') // Recv
			{
				direction = eRecv;
			}
			i += 9;
			pars_command = true;
		}
	}

	return true;
}

int FSInnReader::ConvertTimeStr(QString & TimeStr)
{
	int h = TimeStr[0].digitValue() * 10 + TimeStr[1].digitValue();
	int min = TimeStr[3].digitValue() * 10 + TimeStr[4].digitValue();
	int sek = TimeStr[6].digitValue() * 10 + TimeStr[7].digitValue();
	int hund = TimeStr[9].digitValue() * 100 + TimeStr[10].digitValue() * 10 + TimeStr[11].digitValue();

	return (((((h * 60) + min) * 60 ) + sek) * 1000) + hund;
}



bool ReadLine(QFile & File, QString & String)
{
	if(!File.isOpen())
	{
		return false;
	}
	char buf[1024];
	qint64 lineLength = File.readLine(buf, sizeof(buf));
	if(lineLength == -1)
	{
		return false;
	}
	String = buf;
	return true;
}


