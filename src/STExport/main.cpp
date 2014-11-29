/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FSInnReader.h"

std::string removeExtension(const std::string filename)
{
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos)
    {
        return filename;
    }
    return filename.substr(0, lastdot);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        qDebug() << "You have to add the filename of the log-file!";
        qDebug() << "usage:";
        qDebug() << "      " << argv[0] << " <LOG-FILE> { <LOG-FILE> }";
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        qDebug() << "---------------------------------------------------------";
        qDebug() << "-- Next Log-File: " << argv[i];
        QString FileName = argv[i];
        FSInnReader Reader(FileName);
        ClientContainer cont;
        Reader.ReadFile(cont);
        FileName = QString::fromStdString(removeExtension(FileName.toStdString()) + ".xml");
        cont.WriteToXMLFile(FileName);
        qDebug() << "-- exported to xml-File: " << FileName;
        qDebug() << "---------------------------------------------------------";
        qDebug() << "";
    }
    return 0;
}
