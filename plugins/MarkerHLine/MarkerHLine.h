/*
 *  Qtstalker stock charter
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


#ifndef PLUGIN_MARKER_HLINE_H
#define PLUGIN_MARKER_HLINE_H

#include <QObject>

#include "Plugin.h"

class MarkerHLine : public QObject, public Plugin
{
  Q_OBJECT
  Q_INTERFACES(Plugin)
  
  signals:
    void signalMessage (QString);
  
  public:
    int draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *);
    int command (PluginData *);    
    int info (PluginData *);
    int highLow (PluginData *);
    int move (PluginData *);
    int click (PluginData *);
    int create (PluginData *);
    int settings (PluginData *);
};

#endif
