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


#include "MarkerSell.h"
#include "Strip.h"
#include "Marker.h"
#include "Plot.h"
#include "PlotStatus.h"
#include "PlotDateScaleDraw.h"
#include "Global.h"

#include <QtGui>


int
MarkerSell::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "info" << "highLow" << "move" << "click" << "create" << "settings";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("marker");
      rc = 1;
      break;
    case 1: // info
      rc = info(pd);
      break;
    case 2: // highLow
      rc = highLow(pd);
      break;
    case 3: // move
      rc = move(pd);
      break;
    case 4: // click
      rc = click(pd);
      break;
    case 5: // create
      rc = create(pd);
      break;
    case 6: // settings
      rc = settings(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
MarkerSell::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &, void *m)
{
  Marker *sell = (Marker *) m;
  Entity *e = sell->settings();
  
  PlotDateScaleDraw *dsd = (PlotDateScaleDraw *) sell->plot()->axisScaleDraw(QwtPlot::xBottom);

  QVariant *date = e->get(QString("date"));
  if (! date)
  {
    qDebug() << "MarkerSell::draw: date missing";
    return 0;
  }
  
  int x = xMap.transform(dsd->dateToX(date->toDateTime()));
  if (x == -1)
    return 1;

  QVariant *price = e->get(QString("price"));
  if (! price)
  {
    qDebug() << "MarkerSell::draw: price missing";
    return 0;
  }
  
  int y = yMap.transform(price->toDouble());

  QVariant *tset = e->get(QString("color"));
  if (! tset)
  {
    qDebug() << "MarkerSell::draw: color missing";
    return 0;
  }
  QColor color(tset->toString());
  
  p->setBrush(color);

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
                  x + 2, y - 5,
                  x + 2, y - 11,
                  x - 2, y - 11,
                  x - 2, y - 5,
                  x - 5, y - 5);

  p->drawPolygon(arrow, Qt::OddEvenFill);

  sell->clearSelectionArea();

  sell->appendSelectionArea(QRegion(arrow));
  
  int handleWidth = sell->handleWidth();

  if (sell->selected())
  {
    sell->clearGrabHandles();

    sell->appendGrabHandle(QRegion(x - (handleWidth / 2),
                           y - handleWidth,
                           handleWidth,
                           handleWidth,
                           QRegion::Rectangle));

    p->fillRect(x - (handleWidth / 2),
                y + (handleWidth / 2),
                handleWidth,
                handleWidth,
                color);
  }
  
  return 1;
}

int
MarkerSell::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *sell = (Marker *) pd->data;
  
  Entity *e = sell->settings();
  QVariant *price = e->get(QString("price"));
  if (! price)
    return 0;

  QVariant *date = e->get(QString("date"));
  if (! date)
    return 0;

  QDateTime dt = date->toDateTime();
  pd->info << "D=" + dt.toString("yyyy-MM-dd");
  pd->info << "T=" + dt.toString("HH:mm:ss");
  
  Strip strip;
  QString ts;
  strip.strip(price->toDouble(), 4, ts);
  pd->info << QString("Sell=") + ts;
  
  return 1;
}

int
MarkerSell::move (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *sell = (Marker *) pd->data;
  
  switch (pd->status)
  {
    case PlotStatus::_MOVE:
    {
      Entity *e = sell->settings();
      
      QVariant *price = e->get(QString("price"));
      if (! price)
        return 0;

      QVariant *date = e->get(QString("date"));
      if (! date)
        return 0;
      
      QwtScaleMap map = sell->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) pd->point.x());

      if (! g_symbol)
        return 0;
      Bar *tbar = g_symbol->bar(x);
      if (! tbar)
        return 0;
      date->setValue(tbar->date());

      map = sell->plot()->canvasMap(QwtPlot::yRight);
      price->setValue(map.invTransform((double) pd->point.y()));
      
      sell->setModified(TRUE);

      sell->plot()->replot();
      break;
    }
    default:
      break;
  }
  
  return 1;
}

int
MarkerSell::highLow (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *sell = (Marker *) pd->data;
  
  Entity *e = sell->settings();
  QVariant *price = e->get(QString("price"));
  if (! price)
    return 0;

  pd->high = price->toDouble();
  pd->low = price->toDouble();

  return 1;
}

int
MarkerSell::click (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *sell = (Marker *) pd->data;
  
  switch ((PlotStatus::Key) pd->status)
  {
    case PlotStatus::_SELECTED:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          if (sell->isGrabSelected(pd->point))
          {
            pd->status = PlotStatus::_MOVE;
            return 1;
          }

          if (! sell->isSelected(pd->point))
          {
            pd->status = PlotStatus::_NONE;
            sell->setSelected(FALSE);

            Plot *tplot = (Plot *) sell->plot();
            tplot->unselectMarker();

            sell->plot()->replot();
            return 1;
          }
          break;
        case Qt::RightButton:
        {
          Plot *p = (Plot *) sell->plot();
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
          if (sell->isSelected(pd->point))
          {
            pd->status = PlotStatus::_SELECTED;
            sell->setSelected(TRUE);

            Plot *tplot = (Plot *) sell->plot();
            tplot->selectMarker(sell->ID());

            sell->plot()->replot();
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
MarkerSell::create (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *sell = (Marker *) pd->data;
  
  pd->status = PlotStatus::_MOVE;
  sell->setSelected(TRUE);
  emit signalMessage(QObject::tr("Place Sell marker"));
  return 1;
}

int
MarkerSell::settings (PluginData *pd)
{
  Entity *e = new Entity;
  e->set(QString("plot"), new QVariant(QString()));
  e->set(QString("type"), new QVariant(QString("marker")));
  e->set(QString("symbol"), new QVariant(QString()));
  e->set(QString("plugin"), new QVariant(QString("MarkerSell")));
  e->set(QString("date"), new QVariant(QDateTime::currentDateTime()));
  e->set(QString("price"), new QVariant(0.0));
  e->set(QString("color"), new QVariant(QString("red")));
  pd->settings = e;
  
  return 1;
}

// do not remove
Q_EXPORT_PLUGIN2(markersell, MarkerSell);
