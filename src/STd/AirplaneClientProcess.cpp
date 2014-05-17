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
#define ARRAY_SIZE 20
    /*char ** args = new char*[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE-1; i++)
	{
        args[i] = new char[50];
        args[i][0] = '\0';
    }*/
    char ** args;
    int created = 0;
	if(pAirplane->GetAircraftClientType() == FSInnType)
	{
        created = 2;
        args = new char*[created + 1];
        for (int i = 0; i < created; i++)
        {
            args[i] = new char[50];
            args[i][0] = '\0';
        }
		sprintf(args[0], "EQUIPMENT");
		sprintf(args[1], "%s", pAirplane->GetAircraftType().toStdString().c_str());
        args[2] = 0;
	}
	else if(pAirplane->GetAircraftClientType() == SBType)
	{
        if(pAirplane->GetAircraftType() != "")
        {
            created += 2;
        }
        if(pAirplane->GetAircraftAirline() != "")
        {
            created += 2;
        }
        if(pAirplane->GetAircraftLivery() != "")
        {
            created += 2;
        }

        args = new char*[created + 1];
        for (int i = 0; i < created; i++)
        {
            args[i] = new char[50];
            args[i][0] = '\0';
        }

		int i = 0;
		if(pAirplane->GetAircraftType() != "")
		{
			sprintf(args[i], "EQUIPMENT");
			sprintf(args[i+1], "%s", pAirplane->GetAircraftType().toStdString().c_str());
            i += 2;
		}
		if(pAirplane->GetAircraftAirline() != "")
		{
			sprintf(args[i], "AIRLINE");
			sprintf(args[i+1], "%s", pAirplane->GetAircraftAirline().toStdString().c_str());
            i += 2;
		}
		if(pAirplane->GetAircraftLivery() != "")
		{
			sprintf(args[i], "LIVERY");
			sprintf(args[i+1], "%s", pAirplane->GetAircraftLivery().toStdString().c_str());
            i += 2;
		}
        args[i] = 0;
	}
	try
	{
        mNetwork->SendPlaneInfo(callsign, const_cast<const char**>(args));
	}
    catch (const std::exception &e)
	{
        qDebug() << mClient->GetCallsign() << ": " << e.what();
    }
    for (int i = 0; i < created; i++)
    {
        delete args[i];
    }
    delete args;
}
