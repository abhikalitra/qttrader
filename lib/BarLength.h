/*
 *  Qtstalker stock charter
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

// ******************************************************
// ******************************************************

#ifndef BAR_LENGTH_HPP
#define BAR_LENGTH_HPP

#include <QString>
#include <QStringList>
#include <QDateTime>

class BarLength
{
  public:
    enum Key
    {
      _MINUTE1,
      _MINUTE5,
      _MINUTE10,
      _MINUTE15,
      _MINUTE30,
      _MINUTE60,
      _DAILY,
      _WEEKLY,
      _MONTHLY
    };

    BarLength ();
    QStringList list ();
    int stringToIndex (QString);
    QString indexToString (int);
    void interval (QDateTime dt, int length, QDateTime &startDate, QDateTime &endDate);
    
  private:
    QStringList _list;
};

#endif
