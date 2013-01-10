/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2011 Stefan S. Stratigakos
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

#include "DataBase.h"

#include <QtDebug>
#include <QHash>


DataBase::DataBase (QString table)
{
  _table = table;
  open();
  init();
}

int
DataBase::init ()
{
  // EAV structure for data
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("a INTEGER PRIMARY KEY"); // auto increment index
  s.append(", name TEXT"); // entity name
  s.append(", key TEXT"); // attribute
  s.append(", value TEXT"); // attribute value
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::init: " << q.lastError().text();
    return 0;
  }
  
  return 1;
}

int
DataBase::open ()
{
  _db = QSqlDatabase::database("data");
  if (! _db.isOpen())
  {
    QString s = QDir::homePath() + "/QtTrader/db/";
    QDir dir;
    if (! dir.mkpath(s))
    {
      qDebug() << "DataBase::open: error creating database" << s;
      return 0;
    }
    
    s.append("data");
    
    _db = QSqlDatabase::addDatabase("QSQLITE", "data");
    _db.setHostName("QtTrader");
    _db.setDatabaseName(s);
    _db.setUserName("QtTrader");
    _db.setPassword("QtTrader");
    if (! _db.open())
    {
      qDebug() << "DataBase::open:" << _db.lastError().text();
      return 0;
    }
  }
  
  return 1;
}

void
DataBase::transaction ()
{
  _db.transaction();
}

void
DataBase::commit ()
{
  _db.commit();
}

int
DataBase::get (Entity *data)
{
  QString name = data->name();
  if (name.isEmpty())
  {
    qDebug() << "DataBase::get: invalid name" << name;
    return 0;
  }

  QSqlQuery q(_db);
  QString s = "SELECT key,value FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::get: " << q.lastError().text();
    return 0;
  }

  int flag = 0;
  while (q.next())
  {
    flag++;

    QString key = q.value(0).toString();
    QVariant *td = data->get(key);
    if (! td)
      continue;

    if (td->type() == QVariant::StringList)
    {
      QStringList l = q.value(1).toString().split(",");
      td->setValue(l);
    }
    else
      td->setValue(q.value(1).toString());
  }
  
  if (! flag)
    return 0;

  return 1;
}

int
DataBase::getTypes (QString type, QStringList &l)
{
  l.clear();
  
  if (type.isEmpty())
  {
    qDebug() << "DataBase::getTypes: invalid type" << type;
    return 0;
  }

  QSqlQuery q(_db);
  QString s = "SELECT name FROM " + _table + " WHERE key='type' AND value='" + type + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getTypes: " << q.lastError().text();
    return 0;
  }

  while (q.next())
    l << q.value(0).toString();
  
  return 1;
}

int
DataBase::set (Entity *d)
{
  QString name = d->name();
  if (name.isEmpty())
  {
    qDebug() << "DataBase::set: invalid name" << name;
    return 0;
  }
  
  if (! remove(QStringList() << name))
  {
    qDebug() << "DataBase::set: error during remove";
    return 0;
  }

  QSqlQuery q(_db);

  // save all the settings
  QHash<QString, QVariant *> data = d->settings();
  QHashIterator<QString, QVariant *> it(data);
  while (it.hasNext())
  {
    it.next();
    QVariant *td = it.value();
    
    QString s = "INSERT INTO " + _table + " VALUES (";
    s.append("NULL"); // auto increment
    s.append(",'" + name + "'");
    s.append(",'" + it.key() + "'");
    
    if (td->type() == QVariant::StringList)
      s.append(",'" + td->toStringList().join(",") + "'");
    else
      s.append(",'" + td->toString() + "'");
    
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::set: " << q.lastError().text();
      qDebug() << s;
      return 0;
    }
  }

  return 1;
}

int
DataBase::remove (QStringList l)
{
  QSqlQuery q(_db);
  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    QString s = "DELETE FROM " + _table + " WHERE name='" + l.at(loop) + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::remove: " << q.lastError().text();
      qDebug() << s;
      return 0;
    }
  }

  return 1;
}

int
DataBase::names (QStringList &rl)
{
  rl.clear();

  QSqlQuery q(_db);

  QString s = "SELECT DISTINCT name FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::names: " << q.lastError().text();
    return 0;
  }

  while (q.next())
    rl << q.value(0).toString();

  rl.sort();

  return 1;
}

/*
int
DataBase::search (EAVSearch &req, QStringList &names)
{
  names.clear();

  QSqlQuery q(_db);

  QHash<QString, int> tnames;
  int loop = 0;
  for (; loop < req.size(); loop++)
  {
    QString key, op, value;
    req.get(loop, key, op, value);
    
    QString s = "SELECT name FROM " + _table + " WHERE";
    s.append(" key='" + key + "' AND value" + op + "'" + value + "'");
    
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::search: " << q.lastError().text();
      qDebug() << s;
      return 1;
    }
    
    while (q.next())
    {
      int count = 1;
      if (tnames.contains(q.value(0).toString()))
      {
        count = tnames.value(q.value(0).toString());
        count++;
      }
      tnames.insert(q.value(0).toString(), count);
    }
  }
  
  QHashIterator<QString, int> it(tnames);
  while (it.hasNext())
  {
    it.next();
    if (it.value() == req.size())
      names << it.key();
  }
  
  names.sort();

  return 0;
}
*/

/*
int
DataBase::keys (QStringList &d)
{
  d.clear();

  QSqlQuery q(_db);

  QString s = "SELECT key FROM " + _table + " GROUP BY key";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::names: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    d << q.value(0).toString();
  
  return 0;
}
*/

/*
int
DataBase::attributeValues (QString field, QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);

  QString s = "SELECT value FROM " + _table + " WHERE key='" + field + "' GROUP BY value";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::attributeValues: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();
  
  return 0;
}
*/
