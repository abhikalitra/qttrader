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

#include "TypeCandle.h"

#include <QtDebug>

TypeCandle::TypeCandle ()
{
  _list << "2CROWS";
  _list << "3BLACKCROWS";
  _list << "3INSIDE";
  _list << "3LINESTRIKE";
  _list << "3OUTSIDE";
  _list << "3STARSINSOUTH";
  _list << "3WHITESOLDIERS";
  _list << "ABANDONEDBABY";
  _list << "ADVANCEBLOCK";
  _list << "BELTHOLD";
  _list << "BREAKAWAY";
  _list << "CLOSINGMARUBOZU";
  _list << "CONCEALBABYSWALL";
  _list << "COUNTERATTACK";
  _list << "DARKCLOUDCOVER";
  _list << "DOJI";
  _list << "DOJISTAR";
  _list << "DRAGONFLYDOJI";
  _list << "ENGULFING";
  _list << "EVENINGDOJISTAR";
  _list << "EVENINGSTAR";
  _list << "GAPSIDESIDEWHITE";
  _list << "GRAVESTONEDOJI";
  _list << "HAMMER";
  _list << "HANGINGMAN";
  _list << "HARAMI";
  _list << "HARAMICROSS";
  _list << "HIGHWAVE";
  _list << "HIKKAKE";
  _list << "HIKKAKEMOD";
  _list << "HOMINGPIGEON";
  _list << "IDENTICAL3CROWS";
  _list << "INNECK";
  _list << "INVERTEDHAMMER";
  _list << "KICKING";
  _list << "KICKINGBYLENGTH";
  _list << "LADDERBOTTOM";
  _list << "LONGLEGGEDDOJI";
  _list << "LONGLINE";
  _list << "MARUBOZU";
  _list << "MATCHINGLOW";
  _list << "MATHOLD";
  _list << "MORNINGDOJISTAR";
  _list << "MORNINGSTAR";
  _list << "ONNECK";
  _list << "PIERCING";
  _list << "RICKSHAWMAN";
  _list << "RISEFALL3METHODS";
  _list << "SEPARATINGLINES";
  _list << "SHOOTINGSTAR";
  _list << "SHORTLINE";
  _list << "SPINNINGTOP";
  _list << "STALLEDPATTERN";
  _list << "STICKSANDWICH";
  _list << "TAKURI";
  _list << "TASUKIGAP";
  _list << "THRUSTING";
  _list << "TRISTAR";
  _list << "UNIQUE3RIVER";
  _list << "UPSIDEGAP2CROWS";
  _list << "XSIDEGAP3METHODS";
}

QStringList
TypeCandle::list ()
{
  return _list;
}

int
TypeCandle::stringToIndex (QString d)
{
  return _list.indexOf(d);
}

QString
TypeCandle::indexToString (int d)
{
  return _list.at(d);
}
