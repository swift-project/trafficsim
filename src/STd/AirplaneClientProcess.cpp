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
    VatPilotConnection PilotInfo;
	std::string callsign = mClient->GetCallsign().toStdString();
	PilotInfo.callsign = callsign.c_str();
	PilotInfo.name = "Test Client";
    PilotInfo.rating = static_cast<VatPilotRating>(mClient->GetRating());
    PilotInfo.simType = vatSimTypeMSFS95;

    Vat_SpecifyPilotLogon(mNetwork, Server.toStdString().c_str(), Port, Username.toStdString().c_str(),
                          Password.toStdString().c_str(), &PilotInfo);
}

void AirplaneClientProcess::SendPositionInfo(pTimeUpdate Update)
{
	AirplanePositionUpdate * AirPos = (AirplanePositionUpdate*)Update.get();
    VatPilotPosition Pos = AirPos->GetPosUpdate();

    Vat_SendPilotUpdate(mNetwork, &Pos);
}

void AirplaneClientProcess::SendPlaneInfoRequest(const char * callsign)
{
    VatAircraftInfo aircraftInfo;
    QByteArray type = pAirplane->GetAircraftType().toLocal8Bit();
    QByteArray airline = pAirplane->GetAircraftAirline().toLocal8Bit();
    QByteArray livery = pAirplane->GetAircraftLivery().toLocal8Bit();

    aircraftInfo.aircraftType = type.constData();
    aircraftInfo.airline = airline.constData();
    aircraftInfo.livery = livery.constData();
    Vat_SendModernPlaneInfo(mNetwork, callsign, &aircraftInfo);
}
