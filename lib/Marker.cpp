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

#include "Marker.h"
#include "PluginFactory.h"
#include "PluginData.h"

#include <QDebug>
#include <QUuid>

Marker::Marker (QString plugin)
{
  _pluginString = plugin;
  _selected = FALSE;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);
  _modified = FALSE;
  _settings = 0;
  _ro = FALSE;
  
  if (! plugin.isEmpty())
  {
    PluginFactory fac;
    Plugin *plug = fac.load(plugin);
    if (plug)
    {
      PluginData pd;
      pd.command = QString("settings");
      if (plug->command(&pd))
        _settings = pd.settings;
    }
  }
}

Marker::~Marker ()
{
  detach();
  if (_settings)
    delete _settings;
}

void
Marker::draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &rect) const
{
//  qDebug() <<"Marker::draw";
  PluginFactory fac;
  Plugin *plug = fac.load(_pluginString);
  if (! plug)
    return;

  plug->draw(painter, xMap, yMap, rect, (void *) this);
}

int
Marker::rtti () const
{
  return Rtti_PlotUserItem;
}

int
Marker::isSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _selectionArea.count(); loop++)
  {
    QRegion r = _selectionArea.at(loop);
    if (r.contains(p))
      return 1;
  }

  return 0;
}

void
Marker::clearSelectionArea ()
{
  _selectionArea.clear();
}

void
Marker::appendSelectionArea (QRegion r)
{
  _selectionArea.append(r);
}

int
Marker::isGrabSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _grabHandles.count(); loop++)
  {
    QRegion r = _grabHandles.at(loop);
    if (r.contains(p))
      return loop + 1;
  }

  return 0;
}

void
Marker::clearGrabHandles ()
{
  _grabHandles.clear();
}

void
Marker::appendGrabHandle (QRegion r)
{
  _grabHandles.append(r);
}

void
Marker::setSelected (bool d)
{
  _selected = d;
}

bool
Marker::selected ()
{
  return _selected;
}

void
Marker::setSettings (Entity *d)
{
  if (_settings)
    delete _settings;
  _settings = d;
}

Entity *
Marker::settings ()
{
  return _settings;
}

void
Marker::setModified (bool d)
{
  _modified = d;
}

bool
Marker::modified ()
{
  return _modified;
}

void
Marker::setReadOnly (bool d)
{
  _ro = d;
}

bool
Marker::readOnly ()
{
  return _ro;
}

void
Marker::setID (QString d)
{
  _id = d;
}

QString
Marker::ID ()
{
  return _id;
}

QString
Marker::plugin ()
{
  return _pluginString;
}

int
Marker::handleWidth ()
{
  return _handleWidth;
}

int
Marker::info (QStringList &l)
{
  PluginFactory fac;
  Plugin *plug = fac.load(_pluginString);
  if (! plug)
    return 0;

  PluginData pd;
  pd.command = QString("info");
  pd.data = (void *) this;

  int rc = plug->command(&pd);
  if (rc)
    l = pd.info;
  
  return rc;
}

int
Marker::highLow (int start, int end, double &high, double &low)
{
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
    high = pd.high;
    low = pd.low;
  }
  
  return rc;
}

int
Marker::move (int &status, QPoint p)
{
  PluginFactory fac;
  Plugin *plug = fac.load(_pluginString);
  if (! plug)
    return 0;

  PluginData pd;
  pd.command = QString("move");
  pd.point = p;
  pd.data = (void *) this;
  pd.status = status;

  int rc = plug->command(&pd);
  if (rc)
    status = pd.status;
  
  return rc;
}

int
Marker::click (int &status, int button, QPoint p)
{
  PluginFactory fac;
  Plugin *plug = fac.load(_pluginString);
  if (! plug)
    return 0;

  PluginData pd;
  pd.command = QString("click");
  pd.button = button;
  pd.point = p;
  pd.data = (void *) this;
  pd.status = status;

  int rc = plug->command(&pd);
  if (rc)
    status = pd.status;
  
  return rc;
}

int
Marker::create (int &status)
{
  PluginFactory fac;
  Plugin *plug = fac.load(_pluginString);
  if (! plug)
    return 0;

  PluginData pd;
  pd.command = QString("create");
  pd.data = (void *) this;
  pd.status = status;

  int rc = plug->command(&pd);
  if (rc)
    status = pd.status;
  
  return rc;
}
