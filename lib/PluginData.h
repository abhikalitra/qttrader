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

// ******************************************************
// ******************************************************

#ifndef PLUGIN_DATA_HPP
#define PLUGIN_DATA_HPP

#include "Entity.h"
#include "Bars.h"
#include "Curve.h"
#include "Marker.h"
#include "Widget.h"

#include <QStringList>
#include <QColor>
#include <QPoint>
#include <QDialog>

typedef struct
{
  QString command;          //Command to execute in the plug-in
  Entity *settings;
  QList<Curve *> curves;
  QList<Marker *> markers;
  int index;
  QStringList info;
  QColor color;
  double high;
  double low;
  double value;
  int start;
  int end;
  int status;
  QPoint point;
  int button;
  Bars *bars;
  QString symbol;
  QList<Bars> symbols;
  void *data;
  QString key1;
  QString key2;
  QString key3;
  QString key4;
  QString search;           // The sting to search for in a DB plugin (DBSymbol only)
  QString type;             // The type of plugin
  QDialog *dialog;
  Widget *gui;
  QWidget *dialogParent;
  
} PluginData;


#endif
