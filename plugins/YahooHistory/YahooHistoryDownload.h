/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef YAHOO_HISTORY_DOWNLOAD_HPP
#define YAHOO_HISTORY_DOWNLOAD_HPP

#include <QObject>
#include <QStringList>
#include <QDateTime>
#include <QByteArray>

class YahooHistoryDownload : public QObject
{
  Q_OBJECT
  
  signals:
    void signalMessage (QString);
  
  public:
    YahooHistoryDownload (QObject *);
    void download (QStringList symbolFiles, QDateTime sd, QDateTime ed);
    void getUrl (QDateTime sd, QDateTime ed, QString symbol, QString &url);
    void parseHistory (QByteArray &ba, QString &symbol, QString &name);
    int downloadName (QString symbol, QString &name);
    void importHistory ();
    void getLatestUrl(QString symbol, QString &url);
    void parseToday (QByteArray &ba, QString &symbol, QString &name);

  public slots:
    void stop ();
    
  private:
    bool _stop;
};

#endif
