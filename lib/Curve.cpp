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

#include "Curve.h"
#include "Strip.h"
#include "Bar.h"
#include "BarType.h"
#include "PluginFactory.h"
#include "PluginData.h"

#include <QDebug>

Curve::Curve (QString plugin)
{
  init();
  setPlugin(plugin);
}

Curve::Curve ()
{
  init();
}

Curve::~Curve ()
{
  qDeleteAll(_bars);
}

void
Curve::init ()
{
  setZ(-1);
  _type = -1;
  _pen = 1;
  _style = -1;
  _color = QColor(Qt::red);
  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);
}

void
Curve::setPlugin (QString plugin)
{
  if (!plugin.isEmpty()){
    _pluginString = plugin;
  }
}

void
Curve::draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &rect) const
{
  if (!_pluginString.isEmpty()){
      PluginFactory fac;
      Plugin *plug = fac.load(_pluginString);
      if (! plug)
        return;

      plug->draw(painter,
                xMap,
                yMap,
                rect,
                (void *) this);
  }
}

int
Curve::info (int index, QStringList &l)
{
  if (!_pluginString.isEmpty()){
      PluginFactory fac;
      Plugin *plug = fac.load(_pluginString);
      if (! plug)
        return 0;

      PluginData pd;
      pd.command = QString("info");
      pd.index = index;
      pd.data = (void *) this;

      int rc = plug->command(&pd);
      if (rc)
      {
        for (int pos = 0; pos < pd.info.size(); pos++)
          l << pd.info.at(pos);
      }

      return rc;
  }

  return 0;
}

int
Curve::scalePoint (int i, QColor &color, double &v)
{
  if (!_pluginString.isEmpty()){
      PluginFactory fac;
      Plugin *plug = fac.load(_pluginString);
      if (! plug)
        return 0;

      PluginData pd;
      pd.command = QString("scalePoint");
      pd.index = i;
      pd.data = (void *) this;

      int rc = plug->command(&pd);
      if (rc)
      {
        color = pd.color;
        v = pd.value;
      }

      return rc;
  }
  return 0;
}

int
Curve::highLowRange (int start, int end, double &h, double &l)
{
  if (!_pluginString.isEmpty()){
      PluginFactory fac;
      Plugin *plug = fac.load(_pluginString);
      if (! plug)
        return 0;

      PluginData pd;
      pd.command = QString("highLowRange");
      pd.start = start;
      pd.end = end;
      pd.data = (void *) this;

      int rc = plug->command(&pd);
      if (rc)
      {
        h = pd.high;
        l = pd.low;
      }

      return rc;
    }

  return 0;
}

int
Curve::fill (QString k1, QString k2, QString k3, QString k4, QColor color)
{
  if (!_pluginString.isEmpty()){
      PluginFactory fac;
      Plugin *plug = fac.load(_pluginString);
      if (! plug)
        return 0;

      PluginData pd;
      pd.command = QString("fill");
      pd.key1 = k1;
      pd.key2 = k2;
      pd.key3 = k3;
      pd.key4 = k4;
      pd.color = color;
      pd.data = (void *) this;

      return plug->command(&pd);
  }
  return 0;
}

int
Curve::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

void
Curve::setType (int d)
{
  _type = d;
}

int
Curve::type ()
{
  return _type;
}

void
Curve::setLabel (QString d)
{
  _label = d;
}

QString
Curve::label ()
{
  return _label;
}

void
Curve::setPen (int d)
{
  _pen = d;
}

int
Curve::pen ()
{
  return _pen;
}

void
Curve::setStyle (int d)
{
  _style = d;
}

int
Curve::style ()
{
  return _style;
}

void
Curve::setColor (QColor d)
{
  _color = d;
}

QColor
Curve::color ()
{
  return _color;
}

void
Curve::setBar (int k, Bar *d)
{
  Bar *b = _bars.value(k);
  if (b)
    delete b;
  
  _bars.insert(k, d);
}

Bar *
Curve::bar (int d)
{
  return _bars.value(d);
}

int
Curve::bars ()
{
  return _bars.size();
}
