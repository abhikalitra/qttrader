/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
 *  Copyright (C) 2013 Mattias Johansson
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


#include "MarkerVLine.h"
#include "Marker.h"
#include "Plot.h"
#include "PlotStatus.h"
#include "PlotDateScaleDraw.h"
#include "Global.h"

#include <QtGui>


int
MarkerVLine::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "info" << "move" << "click" << "create" << "settings";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("marker");
      rc = 1;
      break;
    case 1: // info
      rc = info(pd);
      break;
    case 2: // move
      rc = move(pd);
      break;
    case 3: // click
      rc = click(pd);
      break;
    case 4: // create
      rc = create(pd);
      break;
    case 5: // settings
      rc = settings(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
MarkerVLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &, const QRect &, void *m)
{
  Marker *vline = (Marker *) m;
  Entity *e = vline->settings();
  
  PlotDateScaleDraw *dsd = (PlotDateScaleDraw *) vline->plot()->axisScaleDraw(QwtPlot::xBottom);

  QVariant *date = e->get(QString("date"));
  if (! date)
  {
    qDebug() << "MarkerVLine::draw: date missing";
    return 0;
  }
  
  int x = xMap.transform(dsd->dateToX(date->toDateTime()));
  if (x == -1)
    return 1;

  QVariant *tset = e->get(QString("color"));
  if (! tset)
  {
    qDebug() << "MarkerVLine::draw: color missing";
    return 0;
  }
  QColor color(tset->toString());
  
  p->setPen(color);

  p->drawLine (x, 0, x, p->window().height());

  vline->clearSelectionArea();

  QPolygon array;
  array.putPoints(0,
                  4,
                  x - 2, 0,
                  x + 2, 0,
                  x + 2, p->window().height(),
                  x - 2, p->window().height());

  vline->appendSelectionArea(QRegion(array));

  if (vline->selected())
  {
    int handleWidth = vline->handleWidth();
    
    vline->clearGrabHandles();

    int t = (int) p->window().height() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      vline->appendGrabHandle(QRegion(x - (handleWidth / 2),
                              t * loop,
                              handleWidth,
                              handleWidth,
                              QRegion::Rectangle));

      p->fillRect(x - (handleWidth / 2),
                  t * loop,
                  handleWidth,
                  handleWidth,
                  color);
    }
  }
  
  return 1;
}

int
MarkerVLine::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *vline = (Marker *) pd->data;
  
  Entity *e = vline->settings();

  QVariant *date = e->get(QString("date"));
  if (! date)
    return 0;

  QDateTime dt = date->toDateTime();
  pd->info << QString("VLine");
  pd->info << "D=" + dt.toString("yyyy-MM-dd");
  pd->info << "T=" + dt.toString("HH:mm:ss");
  
  return 1;
}

int
MarkerVLine::move (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *vline = (Marker *) pd->data;
  
  switch (pd->status)
  {
    case PlotStatus::_MOVE:
    {
      Entity *e = vline->settings();
      
      QVariant *date = e->get(QString("date"));
      if (! date)
        return 0;
      
      QwtScaleMap map = vline->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) pd->point.x());

      if (! g_symbol)
        return 0;

      date->setValue(g_symbol->date(x));

      vline->setModified(TRUE);
      vline->plot()->replot();
      break;
    }
    default:
      break;
  }
  
  return 1;
}

int
MarkerVLine::click (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *vline = (Marker *) pd->data;
  
  switch ((PlotStatus::Key) pd->status)
  {
    case PlotStatus::_SELECTED:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          if (vline->isGrabSelected(pd->point))
          {
            pd->status = PlotStatus::_MOVE;
            return 1;
          }

          if (! vline->isSelected(pd->point))
          {
            pd->status = PlotStatus::_NONE;
            vline->setSelected(FALSE);

            Plot *tplot = (Plot *) vline->plot();
            tplot->unselectMarker();

            vline->plot()->replot();
            return 1;
          }
          break;
        case Qt::RightButton:
        {
          Plot *p = (Plot *) vline->plot();
          p->showMarkerMenu();
          break;
        }
        default:
          break;
      }

      break;
    }
    case PlotStatus::_MOVE:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          pd->status = PlotStatus::_SELECTED;
          return 1;
        default:
          break;
      }

      break;
    }
    default: // _None
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
        {
          if (vline->isSelected(pd->point))
          {
            pd->status = PlotStatus::_SELECTED;
            vline->setSelected(TRUE);

            Plot *tplot = (Plot *) vline->plot();
            tplot->selectMarker(vline->ID());

            vline->plot()->replot();
            return 1;
          }
          break;
        }
        default:
          break;
      }

      break;
    }
  }
  
  return 1;
}

int
MarkerVLine::create (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *vline = (Marker *) pd->data;
  
  pd->status = PlotStatus::_MOVE;
  vline->setSelected(TRUE);
  emit signalMessage(QObject::tr("Place VLine marker"));
  return 1;
}

int
MarkerVLine::settings (PluginData *pd)
{
  Entity *e = new Entity;
  e->set(QString("plot"), new QVariant(QString()));
  e->set(QString("type"), new QVariant(QString("marker")));
  e->set(QString("symbol"), new QVariant(QString()));
  e->set(QString("plugin"), new QVariant(QString("MarkerVLine")));
  e->set(QString("date"), new QVariant(QDateTime::currentDateTime()));
  e->set(QString("color"), new QVariant(QString("red")));
  pd->settings = e;
  
  return 1;
}

// do not remove
Q_EXPORT_PLUGIN2(markervline, MarkerVLine);
