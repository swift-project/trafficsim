/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#define _CRT_SECURE_NO_WARNINGS

#include "ClientProcess.h"

QString ConvertConnStatusToQString(VatConnectionStatus Status)
{
	switch(Status)
	{
    case vatStatusIdle:
		return "Idle";
		break;
    case vatStatusConnecting:
		return "Connecting";
		break;
    case vatStatusConnected:
		return "Connected";
		break;
    case vatStatusDisconnecting:
        return "Disconnecting";
        break;
    case vatStatusDisconnected:
		return "Disconnected";
		break;
    case vatStatusError:
		return "Error";
		break;
    default:
        return "NOT DEFINED";
        break;
	}
	return "Unknown";
}

ClientProcess::ClientProcess(pClient client)
    : mClient(client), mNetwork(0), mTimer(this), m_connectionStatus(vatStatusIdle)

{
    mNetwork = Vat_CreateNetworkSession(vatServerVatsim, "SimTest 1.0", 1, 0, "MSFS", 0xb9ba,
                                        "727d1efd5cb9f8d2c28372469d922bb4",
                                        vatCapsModelDesc | vatCapsInterminPos | vatCapsAircraftConfig);
    if (mNetwork != 0)
    {
        Vat_SetStateChangeHandler(mNetwork, &ClientProcess::ConnectionStatusChanged, this);
        Vat_SetServerErrorHandler(mNetwork, &ClientProcess::ErrorReceived, this);
        Vat_SetAircraftInfoRequestHandler(mNetwork, &ClientProcess::PilotInfoRequest, this);
        Vat_SetTextMessageHandler(mNetwork, &ClientProcess::TextMessageReceived, this);
        PushNextUpdate();
    }
}

bool ClientProcess::LoginToServer()
{
    if (mNetwork != 0)
    {
        if(m_connectionStatus == vatStatusConnecting || m_connectionStatus == vatStatusConnected)
        {
            return true;
        }
        this->SetLoginInformation();
        Vat_Logon(mNetwork);
        return true;
	}
	return false;
}

void ClientProcess::Disconnect()
{
    Vat_Logoff(mNetwork);
    m_connectionStatus = vatStatusDisconnecting;
}

void ClientProcess::DisconnectAndDestroy()
{
    QObject::disconnect(mProcessShimLibConnection);
    Disconnect();
    Vat_DestroyNetworkSession(mNetwork);
    mNetwork = nullptr;
    emit ClientFinished();
}

void ClientProcess::Run()
{
	if(mNetwork == 0)
	{
        emit ClientFinished();
		return;
    }
    //qDebug() << "Next Update in " << mNextUpdate->GetTimeDiff();
    QTimer::singleShot(mNextUpdate->GetTimeDiff(), this, SLOT(DoNextEvent()));
    mProcessShimLibConnection = QObject::connect(&mTimer, &QTimer::timeout, this, &ClientProcess::ProcessShimLib);
    mTimer.start(100);
}

void ClientProcess::SendPlaneInfoRequest(const char * /* callsign */ )
{
	// do nothing ;)
}

void ClientProcess::SendTextMsg(pTimeUpdate Update)
{
	TextMessageUpdate * text = (TextMessageUpdate*)Update.get();
    Vat_SendTextMessage(mNetwork, text->GetReceiver().toStdString().c_str(), text->GetMessage().toStdString().c_str());
}

void ClientProcess::DoNextEvent()
{
    pTimeUpdate UpdateTask = mNextUpdate;

    // do stuff with UpdateTask:
    qDebug() << qPrintable(mClient->GetCallsign()) << ": " << qPrintable(UpdateReasonToString(UpdateTask->GetUpdateReason()));
    if(m_connectionStatus != vatStatusConnecting && m_connectionStatus != vatStatusConnected)
    {
        if (!LoginToServer())
        {
            DisconnectAndDestroy();
            qDebug() << "closing because of error on connecting";
        }
        return;
    }
    if(UpdateTask->GetUpdateReason() == PositionAirplaneReason || UpdateTask->GetUpdateReason() == PositionATCReason)
    {
        SendPositionInfo(UpdateTask);
    }
    else if(UpdateTask->GetUpdateReason() == TextMsg)
    {
        SendTextMsg(UpdateTask);
    }
    else if (UpdateTask->GetUpdateReason() == RemoveAirplaneReason || UpdateTask->GetUpdateReason() == RemoveATCReason)
    {
        Disconnect();
    }

    PushNextUpdate();
	if(mNextUpdate == 0)
    {
        DisconnectAndDestroy();
		qDebug() << "closing";
	}
    else
    {
        // load Timer for next shot:
        int delta = mNextUpdate->GetTimeDiff();
        QTimer::singleShot(delta, this, SLOT(DoNextEvent()));
    }
}

void ClientProcess::ProcessShimLib()
{
    Vat_ExecuteNetworkTasks(mNetwork);
}

void ClientProcess::PushNextUpdate()
{
	QList<pTimeUpdate> * List = mClient->GetTimeUpdateContainer();
	if(List->size() == 0)
	{
		mNextUpdate = 0;
	}
	else
	{
		mNextUpdate = List->first();
		List->pop_front();
    }
}

void ClientProcess::ConnectionStatusChanged(VatSessionID /* obj */ , VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void * cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	qDebug() << "ConnectionStatusChanged: (" << qPrintable(client->mClient->GetCallsign()) << ")";
	qDebug() <<	"    old: " << ConvertConnStatusToQString(oldStatus);
	qDebug() <<	"    new: " << ConvertConnStatusToQString(newStatus);
    if(newStatus == vatStatusConnected)
	{
        if(client->mNextUpdate == 0)
        {
            // there is no next Event, so disconnect:
            client->DisconnectAndDestroy();
            qDebug() << "closing";
            return;
        }
        QTimer::singleShot(client->mNextUpdate->GetTimeDiff(), client, SLOT(DoNextEvent()));
    }
    if (newStatus == vatStatusDisconnected)
    {
        // close it
    }
    client->m_connectionStatus = newStatus;
}

void ClientProcess::ErrorReceived(VatSessionID /* obj */ , VatServerError errorType, const char *message, const char *errorData, void *cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	qDebug() << "ErrorReceived: (" << qPrintable(client->mClient->GetCallsign()) << ")";
    qDebug() <<	"    type:      " << errorType;
	qDebug() << "    message:   " << message;
	qDebug() <<	"    errorData: " << errorData;
}

void ClientProcess::PilotInfoRequest(VatSessionID /* obj */ , const char *callsign, void *cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	qDebug() << "PilotInfoRequest: (" << qPrintable(client->mClient->GetCallsign()) << ")";
	qDebug() <<	"    from:      " << callsign;
	client->SendPlaneInfoRequest(callsign);
}

void ClientProcess::TextMessageReceived(VatSessionID session, const char * from, const char * to, const char * message, void * cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	if(to == client->mClient->GetCallsign())
	{
        QString returnMessage = "I got this Message from you: ";
        returnMessage += message;
        Vat_SendTextMessage(session, from, qPrintable(returnMessage));
	}
}
