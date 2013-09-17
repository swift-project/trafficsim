/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AIRPLANE_CLIENT_PROCESS_H_
#define AIRPLANE_CLIENT_PROCESS_H_

#include "ClientProcess.h"

class AirplaneClientProcess : public ClientProcess
{
	Q_OBJECT
public:
	AirplaneClientProcess(pClient client);

    virtual void SetLoginInformation();

protected:
	virtual void SendPositionInfo(pTimeUpdate Update);
	virtual void SendPlaneInfoRequest(const char * callsign);

private:
	Airplane * pAirplane;
};

#endif
