/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef TIME_UPDATE_H_
#define TIME_UPDATE_H_

#include <QtXml>
#include <memory>
#include "vatlib.h"

enum UpdateReason
{
    NotInitReason,
    AddAirplaneReason,
    RemoveAirplaneReason,
    AddATCReason,
    RemoveATCReason,
    TextMsg,

    PositionAirplaneReason,
    PositionATCReason,
    SBInfoReason,
};

QString UpdateReasonToString(UpdateReason reason);

class TimeUpdate
{
public:
    TimeUpdate(UpdateReason Reason, int TimeDiff);
    TimeUpdate(UpdateReason Reason, QXmlStreamReader *xmlReader);

    void SetTimeDiff(int TimeDiff);
    int GetTimeDiff() const;

    void SetUpdateReason(UpdateReason Reason);
    UpdateReason GetUpdateReason() const;

    void SerializeTime(QXmlStreamWriter *xmlWriter) const;
    virtual void Serialize(QXmlStreamWriter *xmlWriter) const;

private:
    int mTimeDiff;
    UpdateReason mUpdateReason;
};

typedef std::shared_ptr<TimeUpdate> pTimeUpdate;
typedef QList<pTimeUpdate> TimeUpdateContainer;

// S:OEHAB:2200:1:48.12144:16.54890:603:0:4286578972:10
//  0     1     2    3     4   5    6    7    8    9
// S|N:Callsign:SQ:Rating:Lat:Long:Alt:Speed:pbh:Flags
class AirplanePositionUpdate : public TimeUpdate
{
public:
    AirplanePositionUpdate(int TimeDiff, QString Line);
    AirplanePositionUpdate(QXmlStreamReader *xmlReader);

    QString GetLine() const;

    void Serialize(QXmlStreamWriter *xmlWriter) const;

    QChar GetSquawkMode() const;
    int GetSquawk() const;
    int GetRating() const;
    double GetLat() const;
    double GetLong() const;
    int GetAlt() const;
    int GetSpeed() const;
    double GetPitch() const;
    double GetBank() const;
    double GetHeading() const;
    int GetPressureDelta() const;

    VatPilotPosition GetPosUpdate() const;

private:

    VatTransponderMode convertToTransponderMode(QChar identifier) const;
    void ConvertPBHToDoubles(unsigned int pbh, double &pitch, double &bank, double &heading);

    QChar mSquawkMode;
    int mSquawk;
    int mRating;
    double mLat;
    double mLong;
    int mAlt;
    int mSpeed;
    double mPitch;
    double mBank;
    double mHeading;
    int mPressureDelta;
};

// LOWW_APP:28200:5:145:5:48.11028:16.56972:0
//    0         1         2         3        4     5    6   7
// Callsign:Frequency:FacilityType:VisRange:Rating:Lat:Long:Alt
class ControllerPositionUpdate : public TimeUpdate
{
public:
    ControllerPositionUpdate(int TimeDiff, QString Line);
    ControllerPositionUpdate(QXmlStreamReader *xmlReader);

    QString GetLine() const;

    void Serialize(QXmlStreamWriter *xmlWriter) const;

    int GetFrequency() const;
    int GetFacilityType() const;
    int GetVisRange() const;
    int GetRating() const;
    double GetLat() const;
    double GetLong() const;
    int GetAlt() const;

    VatAtcPosition GetPosUpdate() const;

private:
    int mFrequency;
    VatFacilityType mFacilityType;
    int mVisRange;
    int mRating;
    double mLat;
    double mLong;
    int mAlt;
};

class TextMessageUpdate : public TimeUpdate
{
public:
    TextMessageUpdate(int TimeDiff, QString Line);
    TextMessageUpdate(QXmlStreamReader *xmlReader);

    void Serialize(QXmlStreamWriter *xmlWriter) const;

    QString GetMessage() const;
    QString GetReceiver() const;

private:
    QString mMessage;
    QString mReceiver;
};

#endif
