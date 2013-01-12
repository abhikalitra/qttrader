/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "CSVThread.h"
#include "CSVFunction.h"

#include <QDebug>

CSVThread::CSVThread (QObject *p, QStringList files, QString format, QString dateFormat,
                      QString delimiter, QString type, QString exchange, bool useFilename) : QThread (p)
{
  _files = files;
  _format = format;
  _dateFormat = dateFormat;
  _delimiter = delimiter;
  _type = type;
  _exchange = exchange;
  _useFilename = useFilename;
  
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void CSVThread::run ()
{
  CSVFunction csv(this);
  connect(&csv, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  csv.import(_files,
             _format,
             _dateFormat,
             _delimiter,
             _type,
             _exchange,
             _useFilename);
}
