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

// *************************************************************************************************
// *************************************************************************************************

#ifndef DATE_RANGE_HPP
#define DATE_RANGE_HPP

#include <QStringList>
#include <QDateTime>

class DateRange
{
  public:
    enum Key
    {
      _DAY,
      _WEEK,
      _MONTH,
      _MONTH3,
      _MONTH6,
      _YEAR,
      _YEAR2,
      _YEAR5,
      _YEAR10,
      _YEAR25,
      _YEAR50,
      _ALL
    };

    DateRange ();
    QStringList list ();
    int stringToIndex (QString);
    QString indexToString (int);
    QDateTime interval (QDateTime, int);
    QString rangeKey (QDateTime sd, QDateTime ed);
    
  private:
    QStringList _list;
};

#endif
