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

#include <QtGui>

#include "DBStock.h"
#include "DateRange.h"
#include "BarLength.h"
#include "BarType.h"

int
DBStock::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "init" << "getBars" << "setBars" << "newTable" << "transaction" << "commit";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("db");
      rc = 1;
      break;
    case 1: // init
      rc = init();
      break;
    case 2: // getBars
      rc = getBars(pd);
      break;
    case 3: // setBars
      rc = setBars(pd);
      break;
    case 4: // newTable
      rc = newTable(pd);
      break;
    case 5: // transaction
      _db.transaction();
      rc = 1;
      break;
    case 6: // commit
      _db.commit();
      rc = 1;
      break;
    default:
      break;
  }
  
  return rc;
}

int
DBStock::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}

int
DBStock::init ()
{
  _db = QSqlDatabase::database("quote");
  if (! _db.isOpen())
  {
    QString s = QDir::homePath() + "/QtTrader/db/";
    QDir dir;
    if (! dir.mkpath(s))
    {
      qDebug() << "DBSymbol::init: error creating database" << s;
      return 0;
    }
    
    s.append("quote");
    
    _db = QSqlDatabase::addDatabase("QSQLITE", "quote");
    _db.setHostName("QtTrader");
    _db.setDatabaseName(s);
    _db.setUserName("QtTrader");
    _db.setPassword("QtTrader");
    if (! _db.open())
    {
      qDebug() << "DBSymbol::init:" << _db.lastError().text();
      return 0;
    }
  }
  
  return 1;
}

int
DBStock::getBars (PluginData *pd)
{
  if (! init())
    return 0;
  
  if (! pd->bars)
    return 0;
  
  Bars *bd = pd->bars;
  
  // get last date in db
  QDateTime endDate = getMaxDate(bd);
  if (! endDate.isValid())
    return 0;
  
  DateRange dr;
  //Don't bother to load less the all values into memory
  //All computers are bound to be able handle this in year 2012
  //If ever porting to a mobile device, change at that point
  //bd->plotRange()
  QDateTime startDate = dr.interval(endDate, DateRange::_ALL);
  if (! startDate.isValid())
  {
    qDebug() << "DBStock::getBars: invalid range";
    return 0;
  }

  QSqlQuery q(_db);
  QString s = "SELECT date,open,high,low,close,volume";
  s.append(" FROM " + bd->table());
  s.append(" WHERE date >=" + startDate.toString("yyyyMMddHHmmss"));
  s.append(" AND date <=" + endDate.toString("yyyyMMddHHmmss"));
  s.append(" ORDER BY date ASC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBStock::getBars:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  BarType bt;
  BarLength bl;
  QDateTime isDate, ieDate;
  CBar *bar = 0;
  while (q.next())
  {
    QDateTime lastDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");
    
    // is date greater than current bar range?
    if (lastDate >= ieDate || ! bar)
    {
      // save old bar
      if (bar)
        bd->setBar(bd->bars(), bar);
      
      // create new bar
      bl.interval(lastDate, bd->barLength(), isDate, ieDate);
      
      bar = new CBar;
      bar->setDate(lastDate);
      bar->set(bt.indexToString(BarType::_OPEN), q.value(1).toDouble());
      bar->set(bt.indexToString(BarType::_HIGH), q.value(2).toDouble());
      bar->set(bt.indexToString(BarType::_LOW), q.value(3).toDouble());
      bar->set(bt.indexToString(BarType::_CLOSE), q.value(4).toDouble());
      bar->set(bt.indexToString(BarType::_VOLUME), q.value(5).toDouble());
    }
    else
    {
      double v = q.value(2).toDouble();
      double v2;
      bar->get(bt.indexToString(BarType::_HIGH), v2);
      if (v > v2)
        bar->set(bt.indexToString(BarType::_HIGH), v);

      v = q.value(3).toDouble();
      bar->get(bt.indexToString(BarType::_LOW), v2);
      if (v < v2)
        bar->set(bt.indexToString(BarType::_LOW), v);

      bar->set(bt.indexToString(BarType::_CLOSE), q.value(4).toDouble());
      
      v = q.value(5).toDouble();
      bar->get(bt.indexToString(BarType::_VOLUME), v2);
      v += v2;
      bar->set(bt.indexToString(BarType::_VOLUME), v);
    }
  }

  // save any left over bar
  if (bar)
    bd->setBar(bd->bars(), bar);
  
  return 1;
}

int
DBStock::setBars (PluginData *pd)
{
  if (! init())
    return 0;

  if (! pd->bars)
    return 0;
  
  Bars *symbol = pd->bars;

  QSqlQuery q(_db);

  QList<int> keys = symbol->keys();

  BarType bt;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    CBar *bar = symbol->bar(keys.at(loop));

    QDateTime dt = bar->date();
    
    QString date = dt.toString("yyyyMMddHHmmss");

    // first check if record exists so we know to do an update or insert
    QString s = "SELECT date FROM " + symbol->table() + " WHERE date =" + date;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DBStock::setBars:" << q.lastError().text();
      qDebug() << s;
      continue;
    }

    if (q.next()) // record exists, use update
    {
      s = "UPDATE " + symbol->table() + " SET ";

      QStringList tl;
      double v = 0;
      if (bar->get(bt.indexToString(BarType::_OPEN), v))
        tl << "open=" + QString::number(v);
      
      v = 0;
      if (bar->get(bt.indexToString(BarType::_HIGH), v))
        tl << "high=" + QString::number(v);
      
      v = 0;
      if (bar->get(bt.indexToString(BarType::_LOW), v))
        tl << "low=" + QString::number(v);
      
      v = 0;
      if (bar->get(bt.indexToString(BarType::_CLOSE), v))
        tl << "close=" + QString::number(v);
      
      v = 0;
      if (bar->get(bt.indexToString(BarType::_VOLUME), v))
        tl << "volume=" + QString::number(v);
      
      s.append(tl.join(","));
      s.append(" WHERE date=" + date);
    }
    else // new record, use insert
    {
      QStringList tl;
      s = "INSERT INTO " + symbol->table() + " (date,open,high,low,close,volume) VALUES(";

      tl << date;
      
      double v = 0;
      bar->get(bt.indexToString(BarType::_OPEN), v);
      tl << QString::number(v);
      
      v = 0;
      bar->get(bt.indexToString(BarType::_HIGH), v);
      tl << QString::number(v);

      v = 0;
      bar->get(bt.indexToString(BarType::_LOW), v);
      tl << QString::number(v);

      v = 0;
      bar->get(bt.indexToString(BarType::_CLOSE), v);
      tl << QString::number(v);

      v = 0;
      bar->get(bt.indexToString(BarType::_VOLUME), v);
      tl << QString::number(v);

      s.append(tl.join(","));
      s.append(")");
    }

    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DBStock::setBars:" << q.lastError().text();
      qDebug() << s;
      continue;
    }
  }
  
  return 1;
}

int
DBStock::newTable (PluginData *pd)
{
  if (! init())
    return 0;

  if (! pd->bars)
    return 0;
  
  Bars *symbol = pd->bars;

  if (symbol->table().isEmpty())
  {
    qDebug() << "DBStock::newTable: invalid table";
    return 0;
  }
  
  QSqlQuery q(_db);

  // create new table
  QString s = "CREATE TABLE IF NOT EXISTS " + symbol->table() + " (";
  s.append("date INT PRIMARY KEY UNIQUE");
  s.append(", open REAL");
  s.append(", high REAL");
  s.append(", low REAL");
  s.append(", close REAL");
  s.append(", volume INT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBStock::newTable:" + q.lastError().text();
    return 0;
  }

  return 1;
}

QDateTime
DBStock::getMaxDate (Bars *symbol)
{
  QSqlQuery q(_db);
  
  QDateTime dt;
  QString s = "SELECT max(date) FROM " + symbol->table();
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBStock::getMaxDate:" + q.lastError().text();
    qDebug() << s;
    return dt;
  }

  if (q.next())
    dt = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");

  return dt;
}


// do not remove
Q_EXPORT_PLUGIN2(dbstock, DBStock);
