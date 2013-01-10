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

#ifndef BAR_HPP
#define BAR_HPP

#include <QDateTime>
#include <QColor>
#include <QString>

class Bar
{
  public:
    Bar ();
    virtual void setDate (QDateTime);
    virtual QDateTime date ();
    virtual void setColor (QColor);
    virtual QColor color ();
    virtual void set (QString, double);
    virtual int get (QString, double &);
    virtual int highLow (double &h, double &l);
    virtual void setOpen (double);
    virtual double open ();
    virtual void setHigh (double);
    virtual double high ();
    virtual void setLow (double);
    virtual double low ();
    virtual void setClose (double);
    virtual double close ();
    virtual void setValue (double);
    virtual double value ();
    virtual void setBase (double);
    virtual double base ();
};

#endif
