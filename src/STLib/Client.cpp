/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Client.h"
#include "exporter.h"
#include <iterator>

Client::Client(QString Callsign, eClientType Type)
{
	mCallsign = Callsign;
	mType = Type;
	mRating = -1;
}

Client::Client(QXmlStreamReader * xmlReader)
{
	mCallsign = xmlReader->attributes().value("Callsign").toString();
	mRating = xmlReader->attributes().value("Rating").toString().toInt();
	if(xmlReader->name() == "Airplane")
	{
		mType = AirplaneType;
	}
	else if(xmlReader->name() == "Controller")
	{
		mType = ControllerType;
	}
}

void Client::ReadInnerElements(QXmlStreamReader * xmlReader)
{
	while(!(xmlReader->isEndElement() && (xmlReader->name() == "Airplane" || xmlReader->name() == "Controller")) || xmlReader->atEnd())
	{
		xmlReader->readNext();
		if(xmlReader->isStartElement() && xmlReader->name() == "AddAirplane")
		{
			AddTimeUpdate(pTimeUpdate(new TimeUpdate(AddAirplaneReason, xmlReader)));
		}
		else if(xmlReader->isStartElement() && xmlReader->name() == "RemoveAirplane")
		{
			AddTimeUpdate(pTimeUpdate(new TimeUpdate(RemoveAirplaneReason, xmlReader)));
		}
		else if(xmlReader->isStartElement() && xmlReader->name() == "AddController")
		{
			AddTimeUpdate(pTimeUpdate(new TimeUpdate(AddATCReason, xmlReader)));
		}
		else if(xmlReader->isStartElement() && xmlReader->name() == "RemoveController")
		{
			AddTimeUpdate(pTimeUpdate(new TimeUpdate(RemoveATCReason, xmlReader)));
		}
		else if(xmlReader->isStartElement() && xmlReader->name() == "AirplanePosition")
		{
			AddTimeUpdate(pTimeUpdate(new AirplanePositionUpdate(xmlReader)));
		}
		else if(xmlReader->isStartElement() && xmlReader->name() == "ControllerPosition")
		{
			AddTimeUpdate(pTimeUpdate(new ControllerPositionUpdate(xmlReader)));
		}
		else if(xmlReader->isStartElement() && xmlReader->name() == "TextMsg")
		{
			AddTimeUpdate(pTimeUpdate(new TextMessageUpdate(xmlReader)));
		}
	}

	if(mTimeUpdate.begin() != mTimeUpdate.end() && (*mTimeUpdate.begin())->GetUpdateReason() != AddAirplaneReason)
	{
		mIsOnline = true;
	}
	else
	{
		mIsOnline = false;
	}
}

QString Client::GetCallsign() const
{
	return mCallsign;
}

eClientType Client::GetType() const
{
	return mType;
}

int Client::GetRating() const
{
	return mRating;
}

void Client::SetOnline()
{
	mIsOnline = true;
}

void Client::SetOffline()
{
	mIsOnline = false;
}

bool Client::IsOnline() const
{
	return mIsOnline;
}

void Client::SerializeClient(QXmlStreamWriter * xmlWriter)
{
    // https://dev.vatsim-germany.org/issues/340
    // Delete all TimeUpdates after the last position update, because
    // without further position updates the airplane is considered out
    // of range.
    if (mType == AirplaneType)
    {
        using RevIter = std::reverse_iterator<TimeUpdateContainer::Iterator>;

        RevIter rbegin(mTimeUpdate.end());
        RevIter rend(mTimeUpdate.begin());

        RevIter lastPositionUpdate = std::find_if(rbegin, rend, [](pTimeUpdate timeUpdate)
        {
            return timeUpdate->GetUpdateReason() == PositionAirplaneReason;
        });

        // lastPositionUpdate points now logically to the last position update,
        // but lastPositionUpdate.base() does return a forward iterator
        // pointing already to the next item in the list. Hence we can start
        // erasing from there.
        mTimeUpdate.erase(lastPositionUpdate.base(), mTimeUpdate.end());
    }

    xmlWriter->writeAttribute("Callsign", mCallsign);
    xmlWriter->writeAttribute("Rating", QString::number(mRating));

	for(TimeUpdateContainer::const_iterator iter = mTimeUpdate.begin(); iter != mTimeUpdate.end(); iter++)
	{
		(*iter)->Serialize(xmlWriter);
	}
}

void Client::AddTimeUpdate(pTimeUpdate NextUpdate)
{
	if(mRating == -1)
	{
		if(NextUpdate->GetUpdateReason() == PositionAirplaneReason)
		{
			mRating = ((AirplanePositionUpdate*)NextUpdate.get())->GetRating();
		}
		else if(NextUpdate->GetUpdateReason() == PositionATCReason)
		{
			mRating = ((ControllerPositionUpdate*)NextUpdate.get())->GetRating();
		}
	}
	mTimeUpdate.append(NextUpdate);
}

TimeUpdateContainer * Client::GetTimeUpdateContainer()
{
	return &mTimeUpdate;
}


Airplane::Airplane(QString Callsign)
	: Client(Callsign, AirplaneType)
{
	mIsAirplaneInfoSet = false;
	mAircraftClientType = AircraftTypeNotSet;
	mAircraftType = "";
	mAircraftAirline = "";
	mAircraftLivery = "";
	mX = 0.0;
	mY = 0.0;
	mZ = 0.0;
	mUniqId = "";
	mFSAircraftName = "";
}

Airplane::Airplane(QXmlStreamReader * xmlReader)
	: Client(xmlReader)
{
	mAircraftClientType = (AircraftClientType)xmlReader->attributes().value("ClientType").toString().toInt();
	if(mAircraftClientType == FSInnType)
	{
		mAircraftType = xmlReader->attributes().value("AircraftType").toString();
		mX = xmlReader->attributes().value("X").toString().toDouble();
		mY = xmlReader->attributes().value("Y").toString().toDouble();
		mZ = xmlReader->attributes().value("Z").toString().toDouble();
		mUniqId = xmlReader->attributes().value("UniqId").toString();
		mFSAircraftName = xmlReader->attributes().value("FSAircraftName").toString();
	}
	else if(mAircraftClientType == SBType)
	{
		mAircraftType = xmlReader->attributes().value("AircraftType").toString();
		mAircraftAirline = xmlReader->attributes().value("Airline").toString();
		mAircraftLivery = xmlReader->attributes().value("Livery").toString();
	}
	mIsAirplaneInfoSet = true;
	ReadInnerElements(xmlReader);
}

void Airplane::Serialize(QXmlStreamWriter * xmlWriter)
{
	if(mAircraftClientType == AircraftTypeNotSet)
	{
		return;
	}
	xmlWriter->writeStartElement("Airplane");
	xmlWriter->writeAttribute("ClientType", QString::number((int)mAircraftClientType));
	if(mAircraftClientType == FSInnType)
	{
		xmlWriter->writeAttribute("AircraftType", mAircraftType);
		xmlWriter->writeAttribute("X", QString::number(mX));
		xmlWriter->writeAttribute("Y", QString::number(mY));
		xmlWriter->writeAttribute("Z", QString::number(mZ));
		xmlWriter->writeAttribute("UniqId", mUniqId);
		xmlWriter->writeAttribute("FSAircraftName", mFSAircraftName);
	}
	else if(mAircraftClientType == SBType)
	{
		xmlWriter->writeAttribute("AircraftType", mAircraftType);
		xmlWriter->writeAttribute("Airline", mAircraftAirline);
		xmlWriter->writeAttribute("Livery", mAircraftLivery);
	}
	this->SerializeClient(xmlWriter);
	xmlWriter->writeEndElement();
}

void Airplane::SetAirplaneInfo(QString Line)
{
	//   0       1      2   3 4  5      6        7        8               9        10              11
	// AUA417C:LHA449:FSIPI:1::B738:7.89949:-0.59355:1348.00000:4.6DD02851.A18EC2E8::Boeing 737-8Z9NGX Austrian Airlines Winglets
	//   0       1    2  3    4
	// AUA642:LHA449:PI:GEN:EQUIPMENT=B737
	QList<QString> List = Seperate(Line, ':');
	if(List[2] == "FSIPI" && List.size() == 12)
	{
		mAircraftClientType = FSInnType;
		mAircraftType = List[5];
		mX = List[6].toDouble();
		mY = List[7].toDouble();
		mZ = List[8].toDouble();
		mUniqId = List[9];
		mFSAircraftName = List[11];
	}
	else if(List[2] == "PI" && List[3] == "GEN" && List.size() > 4)
	{
		mAircraftClientType = SBType;
		for(QList<QString>::const_iterator iter = List.begin()+4; iter != List.end(); iter++)
		{
			QList<QString> comval = Seperate(*iter, '=');
			if(comval[0] == "EQUIPMENT")
			{
				mAircraftType = comval[1];
			}
			else if(comval[0] == "AIRLINE")
			{
				mAircraftAirline = comval[1];
			}
			else if(comval[0] == "LIVERY")
			{
				mAircraftLivery = comval[1];
			}
		}
	}
	else
	{
		qDebug() << "error: " << qPrintable(Line);
		return;
	}
	mIsAirplaneInfoSet = true;
}

bool Airplane::IsAirplaneInfoSet() const
{
	return mIsAirplaneInfoSet;
}

AircraftClientType Airplane::GetAircraftClientType() const
{
	return mAircraftClientType;
}

QString Airplane::GetAircraftType() const
{
	return mAircraftType;
}

QString Airplane::GetAircraftAirline() const
{
	return mAircraftAirline;
}

QString Airplane::GetAircraftLivery() const
{
	return mAircraftLivery;
}

double Airplane::GetX() const
{
	return mX;
}

double Airplane::GetY() const
{
	return mY;
}

double Airplane::GetZ() const
{
	return mZ;
}

QString Airplane::GetUniqId() const
{
	return mUniqId;
}

QString Airplane::GetFSAircraftName() const
{
	return mFSAircraftName;
}

Cvatlib_Network::PilotConnectionInfo Airplane::GetConnectionInfo() const
{
	Cvatlib_Network::PilotConnectionInfo info;
	info.callsign = mCallsign.toStdString().c_str();
	info.name = "PilotName";
	info.rating = mRating;
	info.sim = Cvatlib_Network::simType_Unknown;
	return info;
}


Controller::Controller(QString Callsign)
	: Client(Callsign, ControllerType)
{
}

Controller::Controller(QXmlStreamReader * xmlReader)
	: Client(xmlReader)
{
	ReadInnerElements(xmlReader);
}

void Controller::Serialize(QXmlStreamWriter * xmlWriter)
{
	if(CountPositionUpdates() > 0)
	{
		xmlWriter->writeStartElement("Controller");
		this->SerializeClient(xmlWriter);
		xmlWriter->writeEndElement();
	}
}

Cvatlib_Network::ATCConnectionInfo Controller::GetConnectionInfo() const
{
	Cvatlib_Network::ATCConnectionInfo info;
	info.callsign = mCallsign.toStdString().c_str();
	info.name = "ControllerName";
	info.rating = mRating;
	return info;
}

int Controller::CountPositionUpdates() const
{
	int counter = 0;
	for(TimeUpdateContainer::const_iterator iter = mTimeUpdate.begin(); iter != mTimeUpdate.end(); iter++)
	{
		if((*iter)->GetUpdateReason() == PositionATCReason)
		{
			counter++;
		}
	}
	return counter;
}
