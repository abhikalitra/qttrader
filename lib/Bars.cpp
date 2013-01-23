/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *  Copyright (C) 2001-2010 Mattias Johansson
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

#include "Bars.h"
#include "BarLength.h"
#include "DateRange.h"

#include <QtDebug>

Bars::Bars ()
{
  _length = BarLength::_DAILY;
  _range = DateRange::_YEAR;
  _exchange = "NONE";
  _symbolType = "Stock";
  _plugin = "DBStock";
}

Bars::~Bars ()
{
  clear();
}

void
Bars::clear ()
{
  qDeleteAll(_bars);
  _bars.clear();
}

void
Bars::setPlugin (QString d)
{
  _plugin = d;
}

QString
Bars::plugin ()
{
  return _plugin;
}

void
Bars::setTable (QString d)
{
  _table = d;
}

QString
Bars::table ()
{
  return _table;
}

void
Bars::setSymbolType (QString d)
{
  _symbolType = d;
}

QString
Bars::symbolType ()
{
  return _symbolType;
}

void
Bars::setTicker (QString d)
{
  _ticker = d;
}

QString
Bars::ticker ()
{
  return _ticker;
}

void
Bars::setExchange (QString d)
{
  _exchange = d;
}

QString
Bars::exchange ()
{
  return _exchange;
}

void
Bars::setName (QString d)
{
  _name = d;
}

QString
Bars::name ()
{
  return _name;
}

void
Bars::setBarLength (int d)
{
  _length = d;
}

int
Bars::barLength ()
{
  return _length;
}

void
Bars::setPlotRange (int d)
{
  _range = d;
}

int
Bars::plotRange ()
{
  return _range;
}

QString
Bars::symbol ()
{
  return QString(_exchange + ":" + _ticker);
}

int
Bars::setSymbol (QString d)
{
  QStringList l = d.split(":", QString::SkipEmptyParts);
  if (l.size() != 2)
    return 0;
  
  _exchange = l.at(0);
  _ticker = l.at(1);
  
  return 1;
}

CBar *
Bars::bar (int k)
{
  return _bars.value(k);
}

void
Bars::setBar (int k, CBar *d)
{
  Bar *b = _bars.value(k);
  if (b)
    delete b;
  
  _bars.insert(k, d);
}

int
Bars::bars ()
{
  return _bars.size();
}

QList<int>
Bars::keys ()
{
  return _bars.keys();
}

void
Bars::startEndRange (int &start, int &end)
{
  start = -1;
  end = -1;
  
  QMapIterator<int, CBar *> it(_bars);
  it.toFront();
  if (it.hasNext())
  {
    it.next();
    start = it.key();
    end = start;
  }
  
  it.toBack();
  if (it.hasPrevious())
  {
    it.previous();
    end = it.key();
  }
}

QDateTime Bars::date (int x){

  QDateTime date;
  Bar *pBar = bar(x);
  if (!pBar){
    pBar = bar(bars()-1);
    QDateTime lastDate= pBar->date();
    QDateTime datum = lastDate.addDays(x-bars()+1);
    date = datum;
  }else{
      date = pBar->date();
  }

  return date;
}

bool Bars::isValid (int x){
  Bar *pBar = bar(x);
  if (!pBar){
    pBar = bar(bars()-1);
    if (!pBar)
        return false;
  }
  return true;
}

// This is very hacky...
QDateTime Bars::week (int x){

  QDateTime date;
  Bar *pBar = bar(x);
  if (!pBar){
    pBar = bar(bars()-1);
    QDateTime lastDate= pBar->date();
    QDateTime datum = lastDate.addDays(((x-bars())*7));
    date = datum;
  }else{
      date = pBar->date();
  }

  return date;
}

// This is very hacky...
QDateTime Bars::month (int x){

  QDateTime date;
  Bar *pBar = bar(x);
  if (!pBar){
    pBar = bar(bars()-1);
    QDateTime lastDate= pBar->date();
    QDateTime datum = lastDate.addMonths(x-bars());
    date = datum;
  }else{
      date = pBar->date();
  }

  return date;
}
