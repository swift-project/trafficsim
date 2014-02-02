/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#define _CRT_SECURE_NO_WARNINGS

#include "AirplaneClientProcess.h"

AirplaneClientProcess::AirplaneClientProcess(pClient client)
	: ClientProcess(client)
{
	pAirplane = (Airplane*)client.get();
}

void AirplaneClientProcess::SetLoginInformation()
{
	Cvatlib_Network::PilotConnectionInfo PilotInfo;
	std::string callsign = mClient->GetCallsign().toStdString();
	PilotInfo.callsign = callsign.c_str();
	PilotInfo.name = "Test Client";
	PilotInfo.rating = mClient->GetRating();
	PilotInfo.sim = Cvatlib_Network::simType_MSFS95;
	try
	{
        mNetwork->SetPilotLoginInfo(Server.toStdString().c_str(), Port,
                                    Username.toStdString().c_str(), Password.toStdString().c_str(),
                                    PilotInfo);
	}
    catch (InvalidObjectException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
    catch (InvalidNetworkSessionException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
}

void AirplaneClientProcess::SendPositionInfo(pTimeUpdate Update)
{
	AirplanePositionUpdate * AirPos = (AirplanePositionUpdate*)Update.get();
	Cvatlib_Network::PilotPosUpdate Pos;
    const char xMod = AirPos->GetSquawkMode().toLatin1();
	Pos.xpdrMode = (int)xMod;
	Pos.xpdrCode = AirPos->GetSquawk();
	Pos.rating = AirPos->GetRating();
	Pos.lat = AirPos->GetLat();
	Pos.lon = AirPos->GetLong();
	Pos.altTrue = AirPos->GetAlt();
	Pos.groundSpeed = AirPos->GetSpeed();
	Pos.pitch = AirPos->GetPitch();
	Pos.bank = AirPos->GetBank();
	Pos.heading = AirPos->GetHeading();
	Pos.altAdj = AirPos->GetPressureDelta();
	try
	{
		mNetwork->SendPilotUpdate(Pos);
	}
    catch (InvalidObjectException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
    catch (InvalidNetworkSessionException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
    catch (NetworkNotConnectedException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
}

void AirplaneClientProcess::SendPlaneInfoRequest(const char * callsign)
{
#define ARRAY_SIZE 7
	char ** args = new char*[ARRAY_SIZE];
	for (int i = 0; i < ARRAY_SIZE-1; i++)
	{
		args[i] = new char[20];
        args[i] = '\0';
	}
	args[ARRAY_SIZE-1] = 0;
	if(pAirplane->GetAircraftClientType() == FSInnType)
	{
		sprintf(args[0], "%s", mNetwork->acinfo_Equipment);
		sprintf(args[1], "%s", pAirplane->GetAircraftType().toStdString().c_str());
	}
	else if(pAirplane->GetAircraftClientType() == SBType)
	{
		int i = 0;
		if(pAirplane->GetAircraftType() != "")
		{
			sprintf(args[i], "%s", mNetwork->acinfo_Equipment);
			sprintf(args[i+1], "%s", pAirplane->GetAircraftType().toStdString().c_str());
			i = i + 2;
		}
		if(pAirplane->GetAircraftAirline() != "")
		{
			sprintf(args[i], "%s", mNetwork->acinfo_Airline);
			sprintf(args[i+1], "%s", pAirplane->GetAircraftAirline().toStdString().c_str());
			i = i + 2;
		}
		if(pAirplane->GetAircraftLivery() != "")
		{
			sprintf(args[i], "%s", mNetwork->acinfo_Livery);
			sprintf(args[i+1], "%s", pAirplane->GetAircraftLivery().toStdString().c_str());
		}
	}
	try
	{
		mNetwork->SendPlaneInfo(callsign, const_cast<const char**>(args));
	}
    catch (InvalidObjectException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
    catch (InvalidNetworkSessionException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
    catch (NetworkNotConnectedException e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
	}
}
