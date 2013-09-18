/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#define _CRT_SECURE_NO_WARNINGS

#include "ClientProcess.h"

QString ConvertConnStatusToQString(Cvatlib_Network::connStatus Status)
{
	switch(Status)
	{
	case Cvatlib_Network::connStatus_Idle:
		return "Idle";
		break;
	case Cvatlib_Network::connStatus_Connecting:
		return "Connecting";
		break;
	case Cvatlib_Network::connStatus_Connected:
		return "Connected";
		break;
	case Cvatlib_Network::connStatus_Disconnected:
		return "Disconnected";
		break;
	case Cvatlib_Network::connStatus_Error:
		return "Error";
		break;
	}
	return "Unknown";
}

ClientProcess::ClientProcess(pClient client)
	: mClient(client), mNetwork(0), isConnected(false)
{
	mNetwork = Create_Cvatlib_Network();
	if(mNetwork != 0)
	{
		char result[50];
		try
		{
			sprintf(result, "%s=1", mNetwork->capability_ModelDesc);
			mNetwork->CreateNetworkSession("SimTest 1.0", 1, 0, "MSFS", 0, "", result);
			mNetwork->InstallOnConnectionStatusChangedEvent(&ClientProcess::ConnectionStatusChanged, this);
			mNetwork->InstallOnErrorReceivedEvent(&ClientProcess::ErrorReceived, this);
			mNetwork->InstallOnPilotInfoRequestReceivedEvent(&ClientProcess::PilotInfoRequest, this);
			mNetwork->InstallOnTextMessageReceivedEvent(&ClientProcess::TextMessageReceived, this);
			PushNextUpdate();
		}
        catch (InvalidObjectException e)
		{
            qDebug() << "ShimException: " << e.what();
			mNetwork = 0;
		}
        catch (NetworkSessionExistsException e)
		{
            qDebug() << "ShimException: " << e.what();
			mNetwork = 0;
		}
        catch(std::exception e)
		{
            qDebug() << "StdException: " << e.what();
		}
    }
}

bool ClientProcess::LoginToServer()
{
	if(mNetwork != 0)
	{
		if(isConnected)
		{
			return true;
		}
        this->SetLoginInformation();
		bool retVal = true;
		try
		{
			retVal = mNetwork->ConnectAndLogon();
		}
        catch (InvalidObjectException e)
		{
            qDebug() << mClient->GetCallsign() << ": " << e.what();
		}
        catch (InvalidNetworkSessionException e)
		{
            qDebug() << mClient->GetCallsign() << ": " << e.what();
		}
		if(!retVal)
		{
			qDebug() << qPrintable(mClient->GetCallsign()) << " error in connecting!";
			return false;
		}
        //qDebug() << qPrintable(mClient->GetCallsign()) << " connected!";
        //isConnected = true;
        return true;
	}
	return false;
}

void ClientProcess::Disconnect()
{
    try
    {
        mNetwork->LogoffAndDisconnect(-1);
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
    isConnected = false;
}

void ClientProcess::DisconnectAndDestroy()
{
    QObject::disconnect(mProcessShimLibConnection);
    try
    {
        mNetwork->LogoffAndDisconnect(-1);
        mNetwork->DestroyNetworkSession();
        mNetwork = 0;
    }
    catch (InvalidObjectException e)
    {
        qDebug() << e.what();
    }
    catch (InvalidNetworkSessionException e)
    {
        qDebug() << e.what();
    }
    catch (NetworkNotConnectedException e)
    {
        qDebug() << mClient->GetCallsign() << ": " << e.what();
    }
    ClientFinished();
}

void ClientProcess::Run()
{
	if(mNetwork == 0)
	{
		ClientFinished();
		return;
    }
    QTimer::singleShot(mNextUpdate->GetTimeDiff(), this, SLOT(DoNextEvent()));

    mTimer = new QTimer(this);
    mProcessShimLibConnection = QObject::connect(mTimer, &QTimer::timeout, this, &ClientProcess::ProcessShimLib);
	mTimer->start(100);
}

void ClientProcess::SendPlaneInfoRequest(const char * callsign)
{
	// do nothing ;)
}

void ClientProcess::SendTextMsg(pTimeUpdate Update)
{
	TextMessageUpdate * text = (TextMessageUpdate*)Update.get();
	try
	{
		mNetwork->SendPrivateTextMessage(text->GetReceiver().toStdString().c_str(), text->GetMessage().toStdString().c_str());
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

void ClientProcess::DoNextEvent()
{
    pTimeUpdate UpdateTask = mNextUpdate;

	// do stuff with UpdateTask:
	qDebug() << qPrintable(mClient->GetCallsign()) << ": " << qPrintable(UpdateReasonToString(UpdateTask->GetUpdateReason()));
    if(!isConnected)
	{
		if(!LoginToServer())
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
        QTimer::singleShot(mNextUpdate->GetTimeDiff(), this, SLOT(DoNextEvent()));
    }
}

void ClientProcess::ProcessShimLib()
{
	try
	{
		mNetwork->DoProcessing();
	}
    catch (InvalidObjectException e)
	{
        qDebug() << e.what();
	}
    catch (InvalidNetworkSessionException e)
	{
        qDebug() << e.what();
	}
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

void ClientProcess::ConnectionStatusChanged(Cvatlib_Network * obj, Cvatlib_Network::connStatus oldStatus, Cvatlib_Network::connStatus newStatus, void * cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	qDebug() << "ConnectionStatusChanged: (" << qPrintable(client->mClient->GetCallsign()) << ")";
	qDebug() <<	"    old: " << ConvertConnStatusToQString(oldStatus);
	qDebug() <<	"    new: " << ConvertConnStatusToQString(newStatus);
	if(oldStatus == Cvatlib_Network::connStatus_Connecting && newStatus == Cvatlib_Network::connStatus_Connected)
	{
        if(client->mNextUpdate == 0)
        {
            // there is no next Event, so disconnect:
            client->DisconnectAndDestroy();
            qDebug() << "closing";
            return;
        }
        client->isConnected = true;
		QTimer::singleShot(client->mNextUpdate->GetTimeDiff(), client, SLOT(DoNextEvent()));
	}
	if(newStatus == Cvatlib_Network::connStatus_Disconnected)
	{
		// close it
        client->isConnected = false;
	}
}

void ClientProcess::ErrorReceived(Cvatlib_Network * obj, Cvatlib_Network::error type, const char * message, const char * errorData, void * cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	qDebug() << "ErrorReceived: (" << qPrintable(client->mClient->GetCallsign()) << ")";
	qDebug() <<	"    type:      " << type;
	qDebug() << "    message:   " << message;
	qDebug() <<	"    errorData: " << errorData;
}

void ClientProcess::PilotInfoRequest(Cvatlib_Network * obj, const char * callsign, void * cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	qDebug() << "PilotInfoRequest: (" << qPrintable(client->mClient->GetCallsign()) << ")";
	qDebug() <<	"    from:      " << callsign;
	client->SendPlaneInfoRequest(callsign);
}

void ClientProcess::TextMessageReceived(Cvatlib_Network * obj, const char * from, const char * to, const char * message, void * cbVar)
{
	ClientProcess * client = static_cast<ClientProcess*>(cbVar);
	if(to == client->mClient->GetCallsign())
	{
		const char * appendMessage = "I got this Message from you: ";
		char * newMessage = new char[strlen(message) + strlen(appendMessage) + 5];
		sprintf(newMessage, "%s%s", appendMessage, message);
		try
		{
			obj->SendPrivateTextMessage(from, newMessage);
		}
        catch (InvalidObjectException e)
		{
            qDebug() << client->mClient->GetCallsign() << ": " << e.what();
		}
        catch (InvalidNetworkSessionException e)
		{
            qDebug() << client->mClient->GetCallsign() << ": " << e.what();
		}
        catch (NetworkNotConnectedException e)
		{
            qDebug() << client->mClient->GetCallsign() << ": " << e.what();
		}
	}
}
