/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CONTROLLER_CLIENT_PROCESS_H_
#define CONTROLLER_CLIENT_PROCESS_H_

#include "ClientProcess.h"

class ControllerClientProcess : public ClientProcess
{
	Q_OBJECT
public:
	ControllerClientProcess(pClient client);

	virtual void SetLoginInformation();

protected:
	virtual void SendPositionInfo(pTimeUpdate Update);

private:
	Controller * pController;

};

#endif
