/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <QList>
#include "TimeUpdate.h"

enum eClientType
{
	AirplaneType,
	ControllerType,
	NotDefinedType,
};

class Client
{
public:
	Client(QString Callsign, eClientType Type);
	Client(QXmlStreamReader * xmlReader);
	void ReadInnerElements(QXmlStreamReader * xmlReader);

	QString GetCallsign() const;
	eClientType GetType() const;
	int GetRating() const;

	void SetOnline();
	void SetOffline();
	bool IsOnline() const;

	virtual void Serialize(QXmlStreamWriter * xmlWriter) = 0;

    void AddTimeUpdate(pTimeUpdate NextUpdate);
	TimeUpdateContainer * GetTimeUpdateContainer();

protected:
	void SerializeClient(QXmlStreamWriter * xmlWriter);
	TimeUpdateContainer mTimeUpdate;
	QString mCallsign;
	int mRating;

private:
	eClientType mType;
	bool mIsOnline;
};

typedef std::shared_ptr<Client> pClient;


enum AircraftClientType
{
	AircraftTypeNotSet = 0,
	FSInnType = 1,
	SBType = 2,
};

class Airplane : public Client
{
public:
	Airplane(QString Callsign);
	Airplane(QXmlStreamReader * xmlReader);

	virtual void Serialize(QXmlStreamWriter * xmlWriter);
	void SetAirplaneInfo(QString Line);
	bool IsAirplaneInfoSet() const;

	AircraftClientType GetAircraftClientType() const;
	QString GetAircraftType() const;
	QString GetAircraftAirline() const;
	QString GetAircraftLivery() const;
	double GetX() const;
	double GetY() const;
	double GetZ() const;
	QString GetUniqId() const;
	QString GetFSAircraftName() const;

    VatPilotConnection GetConnectionInfo() const;

private:
	bool mIsAirplaneInfoSet;

	AircraftClientType mAircraftClientType;
	QString mAircraftType;
	QString mAircraftAirline;
	QString mAircraftLivery;
	double mX;
	double mY;
	double mZ;
	QString mUniqId;
	QString mFSAircraftName;
};

typedef std::shared_ptr<Airplane> pAirplane;


class Controller : public Client
{
public:
	Controller(QString Callsign);
	Controller(QXmlStreamReader * xmlReader);

	virtual void Serialize(QXmlStreamWriter * xmlWriter);

    VatAtcConnection GetConnectionInfo() const;
private:
	int CountPositionUpdates() const;
};

typedef std::shared_ptr<Controller> pController;

#endif
