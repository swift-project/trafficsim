/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ControllerClientProcess.h"

ControllerClientProcess::ControllerClientProcess(pClient client)
	: ClientProcess(client)
{
	pController = (Controller*)client.get();
}

void ControllerClientProcess::SetLoginInformation()
{
	Cvatlib_Network::ATCConnectionInfo ControllerInfo;
	std::string callsign = mClient->GetCallsign().toStdString();
	ControllerInfo.callsign = callsign.c_str();
	ControllerInfo.name = "Controller Name";
	ControllerInfo.rating = mClient->GetRating();
	try
    {
        mNetwork->SetATCLoginInfo(Server.toStdString().c_str(), Port,
                                    Username.toStdString().c_str(), Password.toStdString().c_str(),
                                    ControllerInfo);
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

void ControllerClientProcess::SendPositionInfo(pTimeUpdate Update)
{
	ControllerPositionUpdate * ATCPos = (ControllerPositionUpdate*)Update.get();
	Cvatlib_Network::ATCPosUpdate ATCUpdate;
	ATCUpdate.frequency = ATCPos->GetFrequency();
	ATCUpdate.facility = ATCPos->GetFacilityType();
	ATCUpdate.visibleRange = ATCPos->GetVisRange();
	ATCUpdate.rating = ATCPos->GetRating();
	ATCUpdate.lat = ATCPos->GetLat();
	ATCUpdate.lon = ATCPos->GetLong();
	ATCUpdate.elevation = ATCPos->GetAlt();
	try
	{
		mNetwork->SendATCUpdate(ATCUpdate);
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
