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

#include "PlotDateScaleDraw.h"
#include "BarLength.h"
#include "DateRange.h"
#include "Global.h"

#include <QString>
#include <QDebug>

#define STEPS 150

PlotDateScaleDraw::PlotDateScaleDraw ()
{
}

void
PlotDateScaleDraw::clear ()
{
  _dates.clear();
}

void
PlotDateScaleDraw::setDates ()
{
  if (! g_symbol)
    return;
  
  _dates.clear();
  
  DateRange dr;
  for (int pos = 0; pos < getPlotEndValue(); pos++)
  {
    Bar *b = g_symbol->bar(pos);
    if(!b){
      b = g_symbol->bar(g_symbol->bars()-1);
      QDateTime SistaDatum = b->date();
      QDateTime datum = SistaDatum.addDays(pos-g_symbol->bars()+1);
      QDateTime ted = dr.interval(datum, g_symbol->barLength());
      _dates.insert(ted.toString(Qt::ISODate), pos);
    }else{
      QDateTime ted = dr.interval(b->date(), g_symbol->barLength());
      _dates.insert(ted.toString(Qt::ISODate), pos);
    }
  }
}

QwtText
PlotDateScaleDraw::label (double v) const
{
  if (! g_symbol)
    return QString();
  
  int t = (int) v;
  if (t < 1 || t >= getPlotEndValue())
    return QString();

  Bar *bar = g_symbol->bar(t);
  QDateTime dt;
  if (! bar)
  {
    bar = g_symbol->bar(g_symbol->bars()-1);
    if(!bar)
        return QString();
    QDateTime SistaDatum = bar->date();
    dt = SistaDatum.addDays(t-g_symbol->bars()+1);
  }else{
    dt = bar->date();
  }
  QwtText date;

  switch ((BarLength::Key) g_symbol->barLength())
  {
    case BarLength::_MINUTE1:
    case BarLength::_MINUTE5:
    case BarLength::_MINUTE10:
    case BarLength::_MINUTE15:
    case BarLength::_MINUTE30:
    case BarLength::_MINUTE60:
      date = dt.toString("d h:m");
      break;
    case BarLength::_DAILY:
      if (dt.date().month() == 1)
        date = dt.toString("yyyy");
      else
        date = dt.toString("MMM");
      break;
    case BarLength::_WEEKLY:
      if (dt.date().month() == 1)
        date = dt.toString("yyyy");
      else
      {
        QString s = dt.toString("MMM");
        s.chop(2);
        date = s;
      }
      break;
    case BarLength::_MONTHLY:
      date = dt.toString("yyyy");
      break;
    default:
      break;
  }

  return date;
}

void
PlotDateScaleDraw::info (int index, QStringList &l)
{
  if (! g_symbol)
    return;
  
  Bar *bar = g_symbol->bar(index);
  if (! bar)
    return;
  
  QDateTime dt = bar->date();
  l << "D=" + dt.toString("yyyy-MM-dd");
  l << "T=" + dt.toString("HH:mm:ss");
}

void
PlotDateScaleDraw::draw (QPainter *painter, const QPalette &) const
{
//  qDebug() << "PlotDateScaleDraw::draw ";
  if (! g_symbol)
    return;
  
  if (! this->getPlotEndValue())
    return;

  QwtScaleDiv sd = scaleDiv();
  int loop = sd.lowerBound();
  int size = sd.upperBound();

  Bar *bar = g_symbol->bar(loop);
  if (!bar){
      qWarning() << "ERROR: bar == NULL";
      return;
  }
  QDateTime dt = bar->date();
  
  QDate oldDate = dt.date();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
  QDate oldMonth = oldDate;
  QDate oldYear = oldDate;

  QDateTime nextHour = dt;
  QDateTime oldDay = nextHour;
  nextHour.setTime(QTime(nextHour.time().hour(), 0, 0, 0));
  if ((BarLength::Key) g_symbol->barLength() != BarLength::_MINUTE1)
    nextHour = nextHour.addSecs(7200);
  else
    nextHour = nextHour.addSecs(3600);

  //TODO: Draw the scale ten steps a head
  size = size + STEPS;
  for (; loop < (size); loop++)
  {
    bar = g_symbol->bar(loop);
    if (!bar){
      //this only works for day scale
        dt = dt.addDays(1);
        //continue;
    }else{
        dt = bar->date();
    }
    switch ((BarLength::Key) g_symbol->barLength())
    {
      case BarLength::_MINUTE1:
      case BarLength::_MINUTE5:
      case BarLength::_MINUTE10:
      case BarLength::_MINUTE15:
      case BarLength::_MINUTE30:
      case BarLength::_MINUTE60:
      {
        if (dt.date().day() != oldDay.date().day())
        {
          oldDay = dt;

          // big tick
          drawTick(painter, loop, 10);
          drawLabel(painter, loop);
          //QString text = date.date().toString("MMM d");
        }
        else
        {
          if (dt >= nextHour)
          {
            if ((BarLength::Key) g_symbol->barLength() < BarLength::_MINUTE30)
            {
              // draw the short tick
              drawTick(painter, loop, 4);
              //QString text = QString::number(date.time().hour()) + ":00";
            }
          }
        }

        if (dt >= nextHour)
        {
          nextHour = dt;
          nextHour.setTime(QTime(dt.time().hour(), 0, 0, 0));
          if ((BarLength::Key) g_symbol->barLength() != BarLength::_MINUTE1)
            nextHour = nextHour.addSecs(7200);
          else
            nextHour = nextHour.addSecs(3600);
        }
        break;
      }
      case BarLength::_DAILY:
      {
        QDate date = dt.date();
        if (date.month() != oldDate.month())
        {
          oldDate = date;
          oldWeek = date;
          oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

          // draw the long tick
          drawTick(painter, loop, 10);
          drawLabel(painter, loop);
          // QString text = date.toString("MMM-yy");
        }
        else
        {
          // if start of new week make a tick
          if (date > oldWeek)
          {
            oldWeek = date;
            oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

            // draw the short week tick
            drawTick(painter, loop, 4);
//            drawLabel(painter, loop);
            //QString text = date.toString("d");
          }
        }
        break;
      }
      case BarLength::_WEEKLY:
      {
        QDate date = dt.date();
        if (date.month() != oldMonth.month())
        {
          oldMonth = date;
          if (date.month() == 1)
          {
            // draw the long tick
            drawTick(painter, loop, 10);
            drawLabel(painter, loop);
            //QString text = date.toString("yyyy");
          }
          else
          {
            // draw the short tick
            drawTick(painter, loop, 4);
            drawLabel(painter, loop);
            //QString text = date.toString("MMM");
            //text.chop(2);
          }
        }
        break;
      }
      case BarLength::_MONTHLY:
      {
        QDate date = dt.date();
        if (date.year() != oldYear.year())
        {
          oldYear = date;

          // draw the long tick
          drawTick(painter, loop, 10);
          drawLabel(painter, loop);
          //QString text = date.toString("yyyy");
        }
        break;
      }
      default:
        break;
    }
  }
}

int
PlotDateScaleDraw::startEndRange (int &start, int &end)
{
  if (! g_symbol)
    return 0;
  
  g_symbol->startEndRange(start, end);
  return 1;
}

int
PlotDateScaleDraw::dateToX (QDateTime d)
{
  int x = -1;
  DateRange dr;
  QDateTime ted = dr.interval(d, g_symbol->barLength());
  QString s = ted.toString(Qt::ISODate);
  if (_dates.contains(s))
    x = _dates.value(s);

  return x;
}

int PlotDateScaleDraw::getPlotEndValue() const
{
  return g_symbol->bars() + STEPS;
}
