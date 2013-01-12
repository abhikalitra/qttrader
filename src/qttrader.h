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

#ifndef QTTRADER_H
#define QTTRADER_H

#include <QMainWindow>
#include <QStringList>
#include <QLayout>

class QtTrader : public QMainWindow
{
  Q_OBJECT

  signals:
    void signalLoadSettings ();
    void signalShutDown ();

  public:
    QtTrader (QString session, QString plugin);
    void createGUI ();
    void loadSettings ();

  public slots:
    void save ();
    void statusMessage (QString);
    void wakeup ();
    void shutDown ();
    int loadPlugin (QString);
    void pluginDialog ();
    void pluginDialog2 (QString);
    void newPluginDialog ();
    void help ();
    void runSession ();
    void runSession2 (QString);
    
  private:
    QString _helpFile;
};

#endif  //QTTRADER_H
