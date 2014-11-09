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
    VatAtcConnection ControllerInfo;
	std::string callsign = mClient->GetCallsign().toStdString();
	ControllerInfo.callsign = callsign.c_str();
	ControllerInfo.name = "Controller Name";
    ControllerInfo.rating = static_cast<VatAtcRating>(mClient->GetRating());

    Vat_SpecifyATCLogon(mNetwork, Server.toStdString().c_str(), Port, Username.toStdString().c_str(),
                        Password.toStdString().c_str(), &ControllerInfo);
}

void ControllerClientProcess::SendPositionInfo(pTimeUpdate Update)
{
	ControllerPositionUpdate * ATCPos = (ControllerPositionUpdate*)Update.get();
    VatAtcPosition ATCUpdate = ATCPos->GetPosUpdate();
    Vat_SendATCUpdate(mNetwork, &ATCUpdate);
}
