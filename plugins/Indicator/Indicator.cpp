/*
 *  QtTrader stock charter
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

#include <QtGui>

#include "Indicator.h"
#include "PlotWidget.h"


int
Indicator::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "gui";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("gui");
      rc = 1;
      break;
    case 1: // gui
      pd->gui = new PlotWidget;
      rc = 1;
      break;
    default:
      break;
  }
  
  return rc;
}

int
Indicator::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}

    
// do not remove
Q_EXPORT_PLUGIN2(indicator, Indicator);
