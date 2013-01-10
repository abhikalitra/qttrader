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

#ifndef BARS_HPP
#define BARS_HPP

#include <QString>
#include <QList>
#include <QMap>

#include "CBar.h"

class Bars
{
  public:
    Bars ();
    ~Bars ();
    void clear ();
    void setPlugin (QString);
    QString plugin ();
    void setTable (QString);
    QString table ();
    void setSymbolType (QString);
    QString symbolType ();
    void setTicker (QString);
    QString ticker ();
    void setExchange (QString);
    QString exchange ();
    void setName (QString);
    QString name ();
    void setBarLength (int);
    int barLength ();
    void setPlotRange (int);
    int plotRange ();
    int setSymbol (QString);
    QString symbol ();
    void setBar (int, CBar *);
    CBar * bar (int);
    int bars ();
    QList<int> keys ();
    void startEndRange (int &, int &);
    
  private:
    QString _plugin;
    QString _table;
    QString _symbolType;
    QString _ticker;
    QString _exchange;
    QString _name;
    int _length;
    int _range;
    QMap<int, CBar *> _bars;
};

#endif
