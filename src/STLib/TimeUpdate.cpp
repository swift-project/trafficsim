/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "TimeUpdate.h"
#include "exporter.h"

QString UpdateReasonToString(UpdateReason reason)
{
	switch(reason)
	{
	case NotInitReason:
		return "NotInitReason";
	case AddAirplaneReason:
		return "AddAirplaneReason";
	case RemoveAirplaneReason:
		return "RemoveAirplaneReason";
	case AddATCReason:
		return "AddATCReason";
	case RemoveATCReason:
		return "RemoveATCReason";
	case TextMsg:
		return "TextMsg";
	case PositionAirplaneReason:
		return "PositionAirplaneReason";
	case PositionATCReason:
		return "PositionATCReason";
	case SBInfoReason:
		return "SBInfoReason";
	}
	return "something-else";
}

TimeUpdate::TimeUpdate(UpdateReason Reason, int TimeDiff)
{
	mUpdateReason = Reason;
	mTimeDiff = TimeDiff;
}

TimeUpdate::TimeUpdate(UpdateReason Reason, QXmlStreamReader * xmlReader)
{
	mUpdateReason = Reason;
	mTimeDiff = xmlReader->attributes().value("Time").toString().toInt();
}

void TimeUpdate::SetTimeDiff(int TimeDiff)
{
	mTimeDiff = TimeDiff;
}

int TimeUpdate::GetTimeDiff() const
{
	return mTimeDiff;
}

void TimeUpdate::SetUpdateReason(UpdateReason Reason)
{
	mUpdateReason = Reason;
}

UpdateReason TimeUpdate::GetUpdateReason() const
{
	return mUpdateReason;
}

void TimeUpdate::SerializeTime(QXmlStreamWriter * xmlWriter) const
{
	xmlWriter->writeAttribute("Time", QString::number(mTimeDiff));
}

void TimeUpdate::Serialize(QXmlStreamWriter * xmlWriter) const
{
	switch(mUpdateReason)
	{
	case AddAirplaneReason:
		xmlWriter->writeStartElement("AddAirplane");
		break;
	case RemoveAirplaneReason:
		xmlWriter->writeStartElement("RemoveAirplane");
		break;
	case AddATCReason:
		xmlWriter->writeStartElement("AddController");
		break;
	case RemoveATCReason:
		xmlWriter->writeStartElement("RemoveController");
		break;
    case NotInitReason:
    case TextMsg:
    case PositionAirplaneReason:
    case PositionATCReason:
    case SBInfoReason:
    default:
        break;
	}
	this->SerializeTime(xmlWriter);
	xmlWriter->writeEndElement();
}


AirplanePositionUpdate::AirplanePositionUpdate(int TimeDiff, QString Line)
	: TimeUpdate(PositionAirplaneReason, TimeDiff)
{
	QList<QString> List = Seperate(Line, ':');
	mSquawkMode = List[0][0];
	mSquawk = List[2].toInt();
	mRating = List[3].toInt();
	mLat = List[4].toDouble();
	mLong = List[5].toDouble();
	mAlt = (int)List[6].toDouble();
	mSpeed = List[7].toInt();

	ConvertPBHToDoubles(List[8].toULong(), mPitch, mBank, mHeading);

	mPressureDelta = List[9].toInt();
}

AirplanePositionUpdate::AirplanePositionUpdate(QXmlStreamReader * xmlReader)
	: TimeUpdate(PositionAirplaneReason, xmlReader)
{
	mSquawkMode = xmlReader->attributes().value("SquawkMode").toString()[0];
	mSquawk = xmlReader->attributes().value("Squawk").toString().toInt();
	mRating = xmlReader->attributes().value("Rating").toString().toInt();
	mLat = xmlReader->attributes().value("Lat").toString().toDouble();
	mLong = xmlReader->attributes().value("Long").toString().toDouble();
	mAlt = xmlReader->attributes().value("Alt").toString().toInt();
	mSpeed = xmlReader->attributes().value("Speed").toString().toInt();
	mPitch = xmlReader->attributes().value("Pitch").toString().toDouble();
	mBank = xmlReader->attributes().value("Bank").toString().toDouble();
	mHeading = xmlReader->attributes().value("Heading").toString().toDouble();
	mPressureDelta = xmlReader->attributes().value("PressureDelta").toString().toInt();
}

void AirplanePositionUpdate::Serialize(QXmlStreamWriter * xmlWriter) const
{
	xmlWriter->writeStartElement("AirplanePosition");
	SerializeTime(xmlWriter);
	xmlWriter->writeAttribute("SquawkMode", mSquawkMode);
	xmlWriter->writeAttribute("Squawk", QString::number(mSquawk));
	xmlWriter->writeAttribute("Rating", QString::number(mRating));
	xmlWriter->writeAttribute("Lat", QString::number(mLat, 'f', 6));
	xmlWriter->writeAttribute("Long", QString::number(mLong, 'f', 6));
	xmlWriter->writeAttribute("Alt", QString::number(mAlt));
	xmlWriter->writeAttribute("Speed", QString::number(mSpeed));
	xmlWriter->writeAttribute("Pitch", QString::number(mPitch));
	xmlWriter->writeAttribute("Bank", QString::number(mBank));
	xmlWriter->writeAttribute("Heading", QString::number(mHeading));
	xmlWriter->writeAttribute("PressureDelta", QString::number(mPressureDelta));
	xmlWriter->writeEndElement();
}

QChar AirplanePositionUpdate::GetSquawkMode() const
{
	return mSquawkMode;
}

int AirplanePositionUpdate::GetSquawk() const
{
	return mSquawk;
}

int AirplanePositionUpdate::GetRating() const
{
	return mRating;
}

double AirplanePositionUpdate::GetLat() const
{
	return mLat;
}

double AirplanePositionUpdate::GetLong() const
{
	return mLong;
}

int AirplanePositionUpdate::GetAlt() const
{
	return mAlt;
}

int AirplanePositionUpdate::GetSpeed() const
{
	return mSpeed;
}

double AirplanePositionUpdate::GetPitch() const
{
	return mPitch;
}

double AirplanePositionUpdate::GetBank() const
{
	return mBank;
}

double AirplanePositionUpdate::GetHeading() const
{
	return mHeading;
}

int AirplanePositionUpdate::GetPressureDelta() const
{
	return mPressureDelta;
}

Cvatlib_Network::PilotPosUpdate AirplanePositionUpdate::GetPosUpdate() const
{
	Cvatlib_Network::PilotPosUpdate pos;
	pos.altAdj = mPressureDelta;
	pos.altTrue = mAlt;
	pos.bank = mBank;
	pos.groundSpeed = mSpeed;
	pos.heading = mHeading;
	pos.lat = mLat;
	pos.lon = mLong;
	pos.pitch = mPitch;
	pos.rating = mRating;
	pos.xpdrCode = mSquawk;
    pos.xpdrMode = mSquawkMode.toLatin1();
	return pos;
}


ControllerPositionUpdate::ControllerPositionUpdate(int TimeDiff, QString Line)
	: TimeUpdate(PositionATCReason, TimeDiff)
{
	QList<QString> List = Seperate(Line, ':');
	mFrequency = 100000 + List[1].toInt();
	mFacilityType = List[2].toInt();
	mVisRange = List[3].toInt();
	mRating = List[4].toInt();
	mLat = List[5].toDouble();
	mLong = List[6].toDouble();
	mAlt = List[7].toInt();
}

ControllerPositionUpdate::ControllerPositionUpdate(QXmlStreamReader * xmlReader)
	: TimeUpdate(PositionATCReason, xmlReader)
{
	mFrequency = xmlReader->attributes().value("Frequency").toString().toInt();
	mFacilityType = xmlReader->attributes().value("FacilityType").toString().toInt();
	mVisRange = xmlReader->attributes().value("VisRange").toString().toInt();
	mRating = xmlReader->attributes().value("Rating").toString().toInt();
	mLat = xmlReader->attributes().value("Lat").toString().toDouble();
	mLong = xmlReader->attributes().value("Long").toString().toDouble();
	mAlt = xmlReader->attributes().value("Alt").toString().toInt();
}

void ControllerPositionUpdate::Serialize(QXmlStreamWriter * xmlWriter) const
{
	xmlWriter->writeStartElement("ControllerPosition");
	SerializeTime(xmlWriter);
	xmlWriter->writeAttribute("Frequency", QString::number(mFrequency));
	xmlWriter->writeAttribute("FacilityType", QString::number(mFacilityType));
	xmlWriter->writeAttribute("VisRange", QString::number(mVisRange));
	xmlWriter->writeAttribute("Rating", QString::number(mRating));
	xmlWriter->writeAttribute("Lat", QString::number(mLat, 'f', 6));
	xmlWriter->writeAttribute("Long", QString::number(mLong, 'f', 6));
	xmlWriter->writeAttribute("Alt", QString::number(mAlt));
	xmlWriter->writeEndElement();
}

int ControllerPositionUpdate::GetFrequency() const
{
	return mFrequency;
}

int ControllerPositionUpdate::GetFacilityType() const
{
	return mFacilityType;
}

int ControllerPositionUpdate::GetVisRange() const
{
	return mVisRange;
}

int ControllerPositionUpdate::GetRating() const
{
	return mRating;
}

double ControllerPositionUpdate::GetLat() const
{
	return mLat;
}

double ControllerPositionUpdate::GetLong() const
{
	return mLong;
}

int ControllerPositionUpdate::GetAlt() const
{
	return mAlt;
}

Cvatlib_Network::ATCPosUpdate ControllerPositionUpdate::GetPosUpdate() const
{
	Cvatlib_Network::ATCPosUpdate pos;
	pos.elevation = mAlt;
	pos.facility = mFacilityType;
	pos.frequency = mFrequency;
	pos.lat = mLat;
	pos.lon = mLong;
	pos.rating = mRating;
	pos.visibleRange = mVisRange;
	return pos;
}


TextMessageUpdate::TextMessageUpdate(int TimeDiff, QString Line)
	: TimeUpdate(TextMsg, TimeDiff)
{
	QList<QString> List = Seperate(Line, ':');
	mMessage = List[2];
	mReceiver += List[1];
}

TextMessageUpdate::TextMessageUpdate(QXmlStreamReader * xmlReader)
	: TimeUpdate(TextMsg, xmlReader)
{
	mMessage = xmlReader->attributes().value("Message").toString();
	mReceiver = xmlReader->attributes().value("Receiver").toString();
}

void TextMessageUpdate::Serialize(QXmlStreamWriter * xmlWriter) const
{
	xmlWriter->writeStartElement("TextMsg");
	SerializeTime(xmlWriter);
	xmlWriter->writeAttribute("Message", mMessage);
	xmlWriter->writeAttribute("Receiver", mReceiver);
	xmlWriter->writeEndElement();
}

QString TextMessageUpdate::GetMessage() const
{
	return mMessage;
}

QString TextMessageUpdate::GetReceiver() const
{
	return mReceiver;
}
