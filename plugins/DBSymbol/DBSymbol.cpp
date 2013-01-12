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

#include "DBSymbol.h"
#include "PluginFactory.h"

int
DBSymbol::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "init" << "getBars" << "setBars" << "search" << "transaction" << "commit";
  
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
    case 4: // search
      rc = search(pd);
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
DBSymbol::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}

int
DBSymbol::init ()
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

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS symbolIndex (";
  s.append("a INTEGER PRIMARY KEY"); // auto increment index
  s.append(", name TEXT"); // entity name
  s.append(", key TEXT"); // attribute
  s.append(", value TEXT"); // value
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::init:" << _db.lastError().text();
    return 0;
  }
  
  return 1;
}

int
DBSymbol::getBars (PluginData *pd)
{
  if (! pd->bars)
    return 0;
  
  Bars *bd = pd->bars;
  
  int length = bd->barLength();
  if (length == -1)
  {
    qDebug() << "DBSymbol::getBars: invalid length";
    return 0;
  }
  
  if (! getSymbol(bd))
    return 0;

  PluginFactory fac;
  Plugin *plug = fac.load(bd->plugin());
  if (! plug)
  {
    qDebug() << "DBSymbol::getBars: invalid plugin" << bd->plugin();
    return 0;
  }

  PluginData tpd;
  tpd.command = QString("getBars");
  tpd.bars = bd;
  
  return plug->command(&tpd);
}

int
DBSymbol::setBars (PluginData *pd)
{
  if (! pd->bars)
    return 0;
  
  Bars *symbol = pd->bars;

  if (! getSymbol(symbol))
  {
    if (! newSymbol(symbol))
      return 0;
  }
  else
  {
    // check if we need to save the name
    if (! symbol->name().isEmpty())
      setName(symbol);
  }
  
  PluginFactory fac;
  Plugin *plug = fac.load(symbol->plugin());
  if (! plug)
  {
    qDebug() << "DBSymbol::setBars: invalid plugin" << symbol->plugin();
    return 0;
  }
  
  PluginData tpd;
  tpd.command = QString("setBars");
  tpd.bars = symbol;
  
  return plug->command(&tpd);
}

int
DBSymbol::newSymbol (Bars *symbol)
{
  if (symbol->ticker().isEmpty() || symbol->exchange().isEmpty() || symbol->plugin().isEmpty())
  {
    qDebug() << "DBSymbol::newSymbol: invalid SYMBOL" << symbol->symbol();
    return 0;
  }
  
  // set default name to symbol
  QString name = symbol->name();
  if (name.isEmpty())
    symbol->setName(symbol->symbol());
  
  QString table = "Q" + QUuid::createUuid().toString();
  table.remove("{");
  table.remove("}");
  table.remove("-");
  symbol->setTable(table);

  // add new symbol entry into the symbolIndex table

  QSqlQuery q(_db);

  // add table
  QString s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + symbol->symbol() + "'");
  s.append(",'table'");
  s.append(",'" + symbol->table() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  // add SYMBOL
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + symbol->symbol() + "'");
  s.append(",'symbol'");
  s.append(",'" + symbol->ticker() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  // add EXCHANGE
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + symbol->symbol() + "'");
  s.append(",'exchange'");
  s.append(",'" + symbol->exchange() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  // add NAME
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + symbol->symbol() + "'");
  s.append(",'name'");
  s.append(",'" + symbol->name() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  // add TYPE
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + symbol->symbol() + "'");
  s.append(",'type'");
  s.append(",'" + symbol->symbolType() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  // add plugin
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + symbol->symbol() + "'");
  s.append(",'plugin'");
  s.append(",'" + symbol->plugin() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  // create new table
  PluginFactory fac;
  Plugin *plug = fac.load(symbol->plugin());
  if (! plug)
  {
    qDebug() << "DBSymbol::newSymbol: invalid plugin" << symbol->plugin();
    return 0;
  }
  
  PluginData pd;
  pd.command = QString("newTable");
  pd.bars = symbol;
  
  if (! plug->command(&pd))
  {
    qDebug() << "DBSymbol::newSymbol: error creating table";
    return 0;
  }
  
  return 1;
}

int
DBSymbol::getSymbol (Bars *symbol)
{
  QSqlQuery q(_db);
  QString s = "SELECT key,value FROM symbolIndex WHERE name='" + symbol->symbol() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::getSymbol:" + q.lastError().text();
    qDebug() << s;
    return 0;
  }

  QStringList keys;
  keys << "type" << "name" << "table" << "plugin";
  
  int rc = 0;
  while (q.next())
  {
    rc++;
    
    switch (keys.indexOf(q.value(0).toString()))
    {
      case 0: // type
        symbol->setSymbolType(q.value(1).toString());
        break;
      case 1: // name
        symbol->setName(q.value(1).toString());
        break;
      case 2: // table
        symbol->setTable(q.value(1).toString());
        break;
      case 3: // plugin
        symbol->setPlugin(q.value(1).toString());
        break;
      default:
        break;
    }
  }

  return rc;
}

int
DBSymbol::deleteSymbol (Bars *bd)
{
  if (! getSymbol(bd))
  {
    qDebug() << "DBSymbol::deleteSymbol: invalid SYMBOL";
    return 0;
  }

  // drop quote table
  QSqlQuery q(_db);

  QString s = "DROP TABLE " + bd->table();
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::deleteSymbol:" + q.lastError().text();
    return 0;
  }

  // remove index records for symbol
  s = "DELETE FROM symbolIndex WHERE name='" + bd->symbol() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::deleteSymbol:" + q.lastError().text();
    return 0;
  }

  return 1;
}

int
DBSymbol::search (PluginData *pd)
{
  if (! init())
    return 0;
  
  pd->symbols.clear();

  QSqlQuery q(_db);

  // first find matching names
  QString s = "SELECT DISTINCT name FROM symbolIndex";

  if (pd->search.contains("%"))
    s.append(" WHERE name LIKE '" + pd->search + "'");
  else
    s.append(" WHERE name='" + pd->search + "'");

  s.append(" ORDER BY name ASC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::search:" + q.lastError().text();
    return 0;
  }

  QStringList names;
  while (q.next())
    names << q.value(0).toString();

  // load names with data
  int loop = 0;
  for (; loop < names.size(); loop++)
  {
    Bars sym;
    sym.setSymbol(names.at(loop));
    
    if (! getSymbol(&sym))
      continue;
    
    pd->symbols << sym;
  }

  return 1;
}

int
DBSymbol::setName (Bars *symbol)
{
  QString name = symbol->name();
  if (name.isEmpty())
  {
    qDebug() << "DBSymbol::getSymbol: invalid NAME";
    return 0;
  }

  QSqlQuery q(_db);

  QString s = "UPDATE symbolIndex";
  s.append(" SET value='" + name + "'");
  s.append(" WHERE name='" + symbol->symbol() + "'");
  s.append(" AND key='name'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBSymbol::setName:" + q.lastError().text();
    return 0;
  }

  return 1;
}


// do not remove
Q_EXPORT_PLUGIN2(dbsymbol, DBSymbol);
