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


#include "MarkerBuy.h"
#include "Strip.h"
#include "Marker.h"
#include "Plot.h"
#include "PlotStatus.h"
#include "PlotDateScaleDraw.h"
#include "Global.h"
#include "MarkerBuyDialog.h"

#include <QtGui>


int
MarkerBuy::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "info" << "highLow" << "move" << "click" << "create" << "settings" << "dialog";
  
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
    case 7: // dialog
      rc = dialog(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
MarkerBuy::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &, void *m)
{
  Marker *buy = (Marker *) m;
  Entity *e = buy->settings();
  
  PlotDateScaleDraw *dsd = (PlotDateScaleDraw *) buy->plot()->axisScaleDraw(QwtPlot::xBottom);

  QVariant *date = e->get(QString("date"));
  if (! date)
  {
    qDebug() << "MarkerBuy::draw: date missing";
    return 0;
  }
  
  int x = xMap.transform(dsd->dateToX(date->toDateTime()));
  if (x == -1)
    return 1;

  QVariant *price = e->get(QString("price"));
  if (! price)
  {
    qDebug() << "MarkerBuy::draw: price missing";
    return 0;
  }
  
  int y = yMap.transform(price->toDouble());

  QVariant *tset = e->get(QString("color"));
  if (! tset)
  {
    qDebug() << "MarkerBuy::draw: color missing";
    return 0;
  }
  QColor color(tset->toString());
  
  p->setBrush(color);

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y + 5,
                  x + 2, y + 5,
                  x + 2, y + 11,
                  x - 2, y + 11,
                  x - 2, y + 5,
                  x - 5, y + 5);

  p->drawPolygon(arrow, Qt::OddEvenFill);

  buy->clearSelectionArea();

  buy->appendSelectionArea(QRegion(arrow));
  
  int handleWidth = buy->handleWidth();

  if (buy->selected())
  {
    buy->clearGrabHandles();

    buy->appendGrabHandle(QRegion(x - (handleWidth / 2),
                          y - handleWidth,
                          handleWidth,
                          handleWidth,
                          QRegion::Rectangle));

    p->fillRect(x - (handleWidth / 2),
                y - handleWidth,
                handleWidth,
                handleWidth,
                color);
  }
  
  return 1;
}

int
MarkerBuy::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *buy = (Marker *) pd->data;
  
  Entity *e = buy->settings();
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
  pd->info << QString("Buy=") + ts;
  
  return 1;
}

int
MarkerBuy::move (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *buy = (Marker *) pd->data;
  
  switch (pd->status)
  {
    case PlotStatus::_MOVE:
    {
      Entity *e = buy->settings();
      
      QVariant *price = e->get(QString("price"));
      if (! price)
        return 0;

      QVariant *date = e->get(QString("date"));
      if (! date)
        return 0;
      
      QwtScaleMap map = buy->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) pd->point.x());

      if (! g_symbol)
        return 0;
      Bar *tbar = g_symbol->bar(x);
      if (! tbar)
        return 0;
      date->setValue(tbar->date());

      map = buy->plot()->canvasMap(QwtPlot::yRight);
      price->setValue(map.invTransform((double) pd->point.y()));
      
      buy->setModified(TRUE);

      buy->plot()->replot();
      break;
    }
    default:
      break;
  }
  
  return 1;
}

int
MarkerBuy::highLow (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *buy = (Marker *) pd->data;
  
  Entity *e = buy->settings();
  QVariant *price = e->get(QString("price"));
  if (! price)
    return 0;

  pd->high = price->toDouble();
  pd->low = price->toDouble();

  return 1;
}

int
MarkerBuy::click (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *buy = (Marker *) pd->data;
  
  switch ((PlotStatus::Key) pd->status)
  {
    case PlotStatus::_SELECTED:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          if (buy->isGrabSelected(pd->point))
          {
            pd->status = PlotStatus::_MOVE;
            return 1;
          }

          if (! buy->isSelected(pd->point))
          {
            pd->status = PlotStatus::_NONE;
            buy->setSelected(FALSE);

            Plot *tplot = (Plot *) buy->plot();
            tplot->unselectMarker();

            buy->plot()->replot();
            return 1;
          }
          break;
        case Qt::RightButton:
        {
          Plot *p = (Plot *) buy->plot();
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
          if (buy->isSelected(pd->point))
          {
            pd->status = PlotStatus::_SELECTED;
            buy->setSelected(TRUE);

            Plot *tplot = (Plot *) buy->plot();
            tplot->selectMarker(buy->ID());

            buy->plot()->replot();
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
MarkerBuy::create (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *buy = (Marker *) pd->data;
  
  pd->status = PlotStatus::_MOVE;
  buy->setSelected(TRUE);
  emit signalMessage(QObject::tr("Place Buy marker"));
  return 1;
}

int
MarkerBuy::settings (PluginData *pd)
{
  Entity *e = new Entity;
  e->set(QString("plot"), new QVariant(QString()));
  e->set(QString("type"), new QVariant(QString("marker")));
  e->set(QString("symbol"), new QVariant(QString()));
  e->set(QString("plugin"), new QVariant(QString("MarkerBuy")));
  e->set(QString("date"), new QVariant(QDateTime::currentDateTime()));
  e->set(QString("price"), new QVariant(0.0));
  e->set(QString("color"), new QVariant(QString("green")));
  pd->settings = e;
  
  return 1;
}

int
MarkerBuy::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "MarkerBuy::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "MarkerBuy::dialog: invalid settings";
    return 0;
  }
  
  MarkerBuyDialog *dialog = new MarkerBuyDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

// do not remove
Q_EXPORT_PLUGIN2(markerbuy, MarkerBuy);
