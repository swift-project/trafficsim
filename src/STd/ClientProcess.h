/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CLIENT_PROCESS_H_
#define CLIENT_PROCESS_H_

#include "STLib/Client.h"

class ClientProcess : public QObject
{
	Q_OBJECT
public:
	ClientProcess(pClient client);
    virtual void SetLoginInformation() = 0;

    static QString Server;
    static qint16 Port;
    static QString Username;
    static QString Password;
	
signals:
	void ClientFinished();

public slots:
	void Run();

protected:
	virtual void SendPositionInfo(pTimeUpdate Update) = 0;
	virtual void SendPlaneInfoRequest(const char * callsign);
	void SendTextMsg(pTimeUpdate Update);

	pClient mClient;
	Cvatlib_Network * mNetwork;

private slots:
	void DoNextEvent();
	void ProcessShimLib();

private:
	bool LoginToServer();
    void Disconnect();
    void DisconnectAndDestroy();
	void PushNextUpdate();

	static void ConnectionStatusChanged(Cvatlib_Network * obj, Cvatlib_Network::connStatus oldStatus, Cvatlib_Network::connStatus newStatus, void * cbVar);
	static void ErrorReceived(Cvatlib_Network * obj, Cvatlib_Network::error type, const char * message, const char * errorData, void * cbVar);
	static void PilotInfoRequest(Cvatlib_Network * obj, const char * callsign, void * cbVar);
	static void TextMessageReceived(Cvatlib_Network* obj, const char* from, const char* to, const char* message, void* cbVar);

	pTimeUpdate mNextUpdate;
	QTimer * mTimer;
    bool isConnected;
    QMetaObject::Connection mProcessShimLibConnection;
};

#endif
