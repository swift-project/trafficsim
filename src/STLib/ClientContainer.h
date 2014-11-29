/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CLIENT_CONTAINER_H_
#define CLIENT_CONTAINER_H_

#include "Client.h"

class ClientContainer : public QList<pClient>
{
public:
    ClientContainer();
    ClientContainer(QString Filename);

    pClient SearchClient(QString Callsign, eClientType Type);
    bool WriteToXMLFile(QString Filename);

    void SetStartTime(int StartTime);
    int GetStartTime() const;

private:
    void CalculateTimes();

    int mStartTime;
};

#endif
