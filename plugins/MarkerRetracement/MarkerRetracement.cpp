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


#include "MarkerRetracement.h"
#include "Strip.h"
#include "Marker.h"
#include "Plot.h"
#include "PlotStatus.h"
#include "PlotDateScaleDraw.h"
#include "Global.h"

#include <QtGui>


int
MarkerRetracement::command (PluginData *pd)
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
MarkerRetracement::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &, void *m)
{
  Marker *ret = (Marker *) m;
  Entity *e = ret->settings();
  
  QVariant *date = e->get(QString("date"));
  if (! date)
    return 0;
  
  QVariant *date2 = e->get(QString("date2"));
  if (! date2)
    return 0;

  QVariant *high = e->get(QString("high"));
  if (! high)
    return 0;

  QVariant *low = e->get(QString("low"));
  if (! low)
    return 0;

  QVariant *tset = e->get(QString("color"));
  if (! tset)
    return 0;
  QColor color(tset->toString());

  QVariant *extend = e->get(QString("extend"));
  if (! extend)
    return 0;

  QVariant *l1 = e->get(QString("level1"));
  if (! l1)
    return 0;

  QVariant *l2 = e->get(QString("level2"));
  if (! l2)
    return 0;

  QVariant *l3 = e->get(QString("level3"));
  if (! l3)
    return 0;

  QVariant *l4 = e->get(QString("level4"));
  if (! l4)
    return 0;

  QVariant *l5 = e->get(QString("level5"));
  if (! l5)
    return 0;

  QVariant *l6 = e->get(QString("level6"));
  if (! l6)
    return 0;
  
  PlotDateScaleDraw *dsd = (PlotDateScaleDraw *) ret->plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->dateToX(date->toDateTime()));

  QDateTime dt = date2->toDateTime();
  if (extend->toBool())
  {
    int s, e;
    g_symbol->startEndRange(s, e);
    Bar *bar = g_symbol->bar(e);
    if (! bar)
      return 0;
    
    dt = bar->date();
  }

  int x2 = xMap.transform(dsd->dateToX(dt));

  p->setPen(color);

  ret->clearSelectionArea();

  QList<double> lineList;
  lineList.append(l1->toDouble());
  lineList.append(l2->toDouble());
  lineList.append(l3->toDouble());
  lineList.append(l4->toDouble());
  lineList.append(l5->toDouble());
  lineList.append(l6->toDouble());

  for (int loop = 0; loop < lineList.size(); loop++)
  {
    double td = lineList.at(loop);
    if (td != 0)
    {
      double range = high->toDouble() - low->toDouble();
      double r = 0;
      if (td < 0)
        r = low->toDouble() + (range * td);
      else
      {
        if (td > 0)
          r = low->toDouble() + (range * td);
        else
        {
          if (td < 0)
            r = high->toDouble();
          else
            r = low->toDouble();
        }
      }

      int y = yMap.transform(r);
      p->drawLine (x, y, x2, y);
      p->drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      QPolygon array;
      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      ret->appendSelectionArea(QRegion(array));
    }
  }

  // draw the low line
  int y = yMap.transform(low->toDouble());
  p->drawLine (x, y, x2, y);
  
  Strip strip;
  QString ts;
  strip.strip(low->toDouble(), 4, ts);
  p->drawText(x, y - 1, "0% - " + ts);

  // store the selectable area the low line occupies
  QPolygon array;
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  ret->appendSelectionArea(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(high->toDouble());
  p->drawLine (x, y2, x2, y2);
  
  strip.strip(high->toDouble(), 4, ts);
  p->drawText(x, y2 - 1, "100% - " + ts);

  // store the selectable area the high line occupies
  array.clear();
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  ret->appendSelectionArea(QRegion(array));

  if (ret->selected())
  {
    int handleWidth = ret->handleWidth();
    
    ret->clearGrabHandles();

    //top left corner
    ret->appendGrabHandle(QRegion(x, y2 - (handleWidth / 2),
                          handleWidth,
                          handleWidth,
                          QRegion::Rectangle));

    p->fillRect(x,
                y2 - (handleWidth / 2),
                handleWidth,
                handleWidth,
                color);

    //bottom right corner
    ret->appendGrabHandle(QRegion(x2, y - (handleWidth / 2),
                          handleWidth,
                          handleWidth,
                          QRegion::Rectangle));

    p->fillRect(x2,
                y - (handleWidth / 2),
                handleWidth,
                handleWidth,
                color);
  }
  
  return 1;
}

int
MarkerRetracement::info (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *m = (Marker *) pd->data;
  
  Entity *e = m->settings();

  QVariant *date = e->get(QString("date"));
  if (! date)
    return 0;
  
  QVariant *date2 = e->get(QString("date2"));
  if (! date2)
    return 0;

  QVariant *high = e->get(QString("high"));
  if (! high)
    return 0;

  QVariant *low = e->get(QString("low"));
  if (! low)
    return 0;

  QVariant *l1 = e->get(QString("level1"));
  if (! l1)
    return 0;

  QVariant *l2 = e->get(QString("level2"));
  if (! l2)
    return 0;

  QVariant *l3 = e->get(QString("level3"));
  if (! l3)
    return 0;

  QVariant *l4 = e->get(QString("level4"));
  if (! l4)
    return 0;

  QVariant *l5 = e->get(QString("level5"));
  if (! l5)
    return 0;

  QVariant *l6 = e->get(QString("level6"));
  if (! l6)
    return 0;
  
  pd->info << tr("Retracement");

  QDateTime dt = date->toDateTime();
  pd->info << tr("Start Date") + "=" + dt.toString("yyyy-MM-dd");
  pd->info << tr("Start Time") + "=" + dt.toString("HH:mm:ss");

  dt = date2->toDateTime();
  pd->info << tr("End Date") + "=" + dt.toString("yyyy-MM-dd");
  pd->info << tr("End Time") + "=" + dt.toString("HH:mm:ss");

  Strip strip;
  QString ts;
  strip.strip(high->toDouble(), 4, ts);
  pd->info << tr("High") + "=" + ts;
  
  strip.strip(low->toDouble(), 4, ts);
  pd->info << tr("Low") + "=" + ts;
  
  strip.strip(l1->toDouble(), 4, ts);
  pd->info << tr("Level 1") + "=" + ts;
  
  strip.strip(l2->toDouble(), 4, ts);
  pd->info << tr("Level 2") + "=" + ts;
  
  strip.strip(l3->toDouble(), 4, ts);
  pd->info << tr("Level 3") + "=" + ts;
  
  strip.strip(l4->toDouble(), 4, ts);
  pd->info << tr("Level 4") + "=" + ts;
  
  strip.strip(l5->toDouble(), 4, ts);
  pd->info << tr("Level 5") + "=" + ts;
  
  strip.strip(l6->toDouble(), 4, ts);
  pd->info << tr("Level 6") + "=" + ts;
  
  return 1;
}

int
MarkerRetracement::move (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *m = (Marker *) pd->data;
  
  Entity *e = m->settings();
  
  switch (pd->status)
  {
    case PlotStatus::_MOVE:
    case PlotStatus::_CREATE_MOVE:
    {
      QVariant *date = e->get(QString("date"));
      if (! date)
        return 0;
  
      QVariant *date2 = e->get(QString("date2"));
      if (! date2)
        return 0;

      QVariant *high = e->get(QString("high"));
      if (! high)
        return 0;

      QVariant *low = e->get(QString("low"));
      if (! low)
        return 0;
      
      QwtScaleMap map = m->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) pd->point.x());

      Bar *bar = g_symbol->bar(x);
      if (! bar)
        return 0;
      date->setValue(bar->date());

      map = m->plot()->canvasMap(QwtPlot::yRight);
      high->setValue(map.invTransform((double) pd->point.y()));

      if (pd->status == PlotStatus::_CREATE_MOVE)
      {
        date2->setValue(bar->date());
        low->setValue(high->toDouble());
      }

      m->plot()->replot();

      m->setModified(TRUE);
      break;
    }
    case PlotStatus::_MOVE2:
    {
      QVariant *date2 = e->get(QString("date2"));
      if (! date2)
        return 0;

      QVariant *low = e->get(QString("low"));
      if (! low)
        return 0;

      QwtScaleMap map = m->plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) pd->point.x());

      Bar *bar = g_symbol->bar(x);
      if (! bar)
        return 0;
      date2->setValue(bar->date());

      map = m->plot()->canvasMap(QwtPlot::yRight);
      low->setValue(map.invTransform((double) pd->point.y()));

      m->plot()->replot();

      m->setModified(TRUE);
      break;
    }
    default:
      break;
  }
  
  return 1;
}

int
MarkerRetracement::highLow (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *m = (Marker *) pd->data;
  
  Entity *e = m->settings();
  
  QVariant *h = e->get(QString("high"));
  if (! h)
    return 0;

  QVariant *l = e->get(QString("low"));
  if (! l)
    return 0;

  pd->high = h->toDouble();
  pd->low = l->toDouble();
  
  return 1;
}

int
MarkerRetracement::click (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *m = (Marker *) pd->data;
  
  switch (pd->status)
  {
    case PlotStatus::_SELECTED:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
        {
          int grab = m->isGrabSelected(pd->point);
          if (grab)
          {
            pd->status = PlotStatus::_MOVE;
            if (grab == 2)
              pd->status = PlotStatus::_MOVE2;
            return 1;
          }

          if (! m->isSelected(pd->point))
          {
            pd->status = PlotStatus::_NONE;
            m->setSelected(FALSE);
            Plot *tplot = (Plot *) m->plot();
            tplot->unselectMarker();
            m->plot()->replot();
            return 1;
          }
          break;
        }
        case Qt::RightButton:
        {
          Plot *p = (Plot *) m->plot();
          p->showMarkerMenu();
          break;
        }
        default:
          break;
      }

      break;
    }
    case PlotStatus::_MOVE:
    case PlotStatus::_CREATE_MOVE:
    {
      switch (pd->button)
      {
        case Qt::LeftButton:
          if (pd->status == PlotStatus::_CREATE_MOVE)
          {
            pd->status = PlotStatus::_MOVE2;
            emit signalMessage(tr("Select lowest ending point..."));
            return 1;
          }

          pd->status = PlotStatus::_SELECTED;
          return 1;
        default:
          break;
      }

      break;
    }
    case PlotStatus::_MOVE2:
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
          if (m->isSelected(pd->point))
          {
            pd->status = PlotStatus::_SELECTED;
            m->setSelected(TRUE);

            Plot *tplot = (Plot *) m->plot();
            
            tplot->selectMarker(m->ID());

            m->plot()->replot();
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
MarkerRetracement::create (PluginData *pd)
{
  if (! pd->data)
    return 0;
  
  Marker *m = (Marker *) pd->data;
  
  pd->status = PlotStatus::_CREATE_MOVE;
  m->setSelected(TRUE);
  emit signalMessage(tr("Select highest starting point..."));
  return 1;
}

int
MarkerRetracement::settings (PluginData *pd)
{
  Entity *e = new Entity;
  e->set(QString("plot"), new QVariant(QString()));
  e->set(QString("type"), new QVariant(QString("marker")));
  e->set(QString("symbol"), new QVariant(QString()));
  e->set(QString("plugin"), new QVariant(QString("MarkerRetracement")));
  e->set(QString("date"), new QVariant(QDateTime::currentDateTime()));
  e->set(QString("date2"), new QVariant(QDateTime::currentDateTime()));
  e->set(QString("color"), new QVariant(QString("red")));
  e->set(QString("high"), new QVariant(0.0));
  e->set(QString("low"), new QVariant(0.0));
  e->set(QString("extend"), new QVariant(FALSE));
  e->set(QString("level1"), new QVariant(0.382));
  e->set(QString("level2"), new QVariant(0.5));
  e->set(QString("level3"), new QVariant(0.618));
  e->set(QString("level4"), new QVariant(0.0));
  e->set(QString("level5"), new QVariant(0.0));
  e->set(QString("level6"), new QVariant(0.0));
  pd->settings = e;
  
  return 1;
}

// do not remove
Q_EXPORT_PLUGIN2(markerretracement, MarkerRetracement);
