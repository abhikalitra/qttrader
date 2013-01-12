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


#ifndef DBSTOCK_H
#define DBSTOCK_H

#include <QObject>
#include <QtSql>
#include <QStringList>

#include "Plugin.h"

class DBStock : public QObject, Plugin
{
  Q_OBJECT
  Q_INTERFACES(Plugin)

  public:
    int command (PluginData *);    
    int draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *);
    
    int init ();
    int setBars (PluginData *);
    int getBars (PluginData *);
    int newTable (PluginData *);
    QDateTime getMaxDate (Bars *);

  private:
    QSqlDatabase _db;
};

#endif
