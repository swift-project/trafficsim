/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ClientContainer.h"

ClientContainer::ClientContainer()
{
}

ClientContainer::ClientContainer(QString Filename)
{
	QFile file(Filename);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		qDebug() << "Error: Cannot read file "
			<< qPrintable(Filename) << ": "
			<< qPrintable(file.errorString());
		return;
	}

	QXmlStreamReader xmlReader(&file);
	while(!xmlReader.atEnd())
	{
		xmlReader.readNext();
		if(xmlReader.isStartElement() && xmlReader.name() == "ClientContainer")
		{
			mStartTime = xmlReader.attributes().value("StartTime").toString().toInt();
		}
		else if(xmlReader.isStartElement() && xmlReader.name() == "Airplane")
		{
			this->append(pClient(new Airplane(&xmlReader)));
		}
		else if(xmlReader.isStartElement() && xmlReader.name() == "Controller")
		{
			this->append(pClient(new Controller(&xmlReader)));
		}
	}
	if(xmlReader.hasError())
	{
		qDebug() << qPrintable(xmlReader.errorString());
		return;
	}
	file.close();
}

pClient ClientContainer::SearchClient(QString Callsign, eClientType Type)
{
	for(QList<pClient>::iterator iter = this->begin(); iter != this->end(); iter++)
	{
		if((*iter)->GetCallsign() == Callsign)
		{
			return (*iter);
		}
	}
	if(Type == AirplaneType)
	{
		this->append(pClient(new Airplane(Callsign)));
	}
	else if(Type == ControllerType)
	{
		this->append(pClient(new Controller(Callsign)));
	}
	else
	{
		return pClient(0);
	}
	return this->SearchClient(Callsign, Type);
}

bool ClientContainer::WriteToXMLFile(QString Filename)
{
	CalculateTimes();

	QFile file(Filename);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		qDebug() << "Error: Cannot write file "
			<< qPrintable(Filename) << ": "
			<< qPrintable(file.errorString());
		return false;
	}

	QXmlStreamWriter xmlWriter(&file);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("ClientContainer");
	xmlWriter.writeAttribute("StartTime", QString::number(mStartTime));

	for(ClientContainer::const_iterator iter = this->begin(); iter != this->end(); iter++)
	{
		(*iter)->Serialize(&xmlWriter);
	}

	xmlWriter.writeEndElement();
	xmlWriter.writeEndDocument();
	if(xmlWriter.hasError())
	{
		qDebug() << "Error in xml-write";
		return false;
	}
	file.close();
	if (file.error())
	{
		qDebug() << "Error: Cannot write file "
			<< qPrintable(Filename) << ": "
			<< qPrintable(file.errorString());
		return false;
	}
	return true;
}

void ClientContainer::SetStartTime(int StartTime)
{
	mStartTime = StartTime;
}

int ClientContainer::GetStartTime() const
{
	return mStartTime;
}

void ClientContainer::CalculateTimes()
{
	for(ClientContainer::iterator ClientInter = this->begin(); ClientInter != this->end(); ClientInter++)
	{
		int Time = mStartTime;
		for(TimeUpdateContainer::iterator UpdateIter = (*ClientInter)->GetTimeUpdateContainer()->begin(); UpdateIter != (*ClientInter)->GetTimeUpdateContainer()->end(); UpdateIter++)
		{
			int TimeBuff = (*UpdateIter)->GetTimeDiff();
			(*UpdateIter)->SetTimeDiff(TimeBuff - Time);
			Time = TimeBuff;
		}
	}
}
