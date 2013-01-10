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

#include "Entity.h"
#include "Global.h"
//#include "SettingType.h"

#include <QtDebug>

Entity::Entity ()
{
}

Entity::~Entity ()
{
  qDeleteAll(_settings);
}

void
Entity::clear ()
{
  qDeleteAll(_settings);
  _settings.clear();
}

void
Entity::set (QString k, QVariant *d)
{
  QVariant *set = _settings.value(k);
  if (set)
    delete set;
  
  _settings.insert(k, d);
}

QVariant *
Entity::get (QString k)
{
  if (_settings.contains(k))
    return _settings.value(k);
  
  return 0;
}

void
Entity::remove (QString d)
{
  QVariant *set = _settings.value(d);
  if (! set)
    return;
  
  _settings.remove(d);
  
  delete set;
}

QList<QString>
Entity::keys ()
{
  return _settings.keys();
}

int
Entity::size ()
{
  return _settings.size();
}

int
Entity::saveSettings (QSettings &settings)
{
  QHashIterator<QString, QVariant *> it(_settings);
  while (it.hasNext())
  {
    it.next();
    QVariant *tset = it.value();

    switch ((QVariant::Type) tset->type())
    {
      case QVariant::String:
        settings.setValue(it.key(), tset->toString());
        break;
      case QVariant::StringList:
        settings.setValue(it.key(), tset->toStringList());
        break;
      case QVariant::Int:
        settings.setValue(it.key(), tset->toInt());
        break;
      case QVariant::Double:
        settings.setValue(it.key(), tset->toDouble());
        break;
      case QVariant::Bool:
        settings.setValue(it.key(), tset->toBool());
        break;
      case QVariant::DateTime:
        settings.setValue(it.key(), tset->toDateTime());
        break;
      default:
        return 0;
        break;
    }
  }
  
  return 1;
}

int
Entity::loadSettings (QSettings &settings)
{
  QHashIterator<QString, QVariant *> it(_settings);
  while (it.hasNext())
  {
    it.next();
    QVariant *tset = it.value();

    switch ((QVariant::Type) tset->type())
    {
      case QVariant::String:
        tset->setValue(settings.value(it.key()).toString());
        break;
      case QVariant::StringList:
        tset->setValue(settings.value(it.key()).toStringList());
        break;
      case QVariant::Int:
        tset->setValue(settings.value(it.key()).toInt());
        break;
      case QVariant::Double:
        tset->setValue(settings.value(it.key()).toDouble());
        break;
      case QVariant::Bool:
        tset->setValue(settings.value(it.key()).toBool());
        break;
      case QVariant::DateTime:
        tset->setValue(settings.value(it.key()).toDateTime());
        break;
      default:
        return 0;
        break;
    }
  }
  
  return 1;
}

/*
void
Entity::order (QStringList &d)
{
  d.clear();
  
  QMap<int, QString> order;
  QHashIterator<QString, Setting *> it(_settings);
  while (it.hasNext())
  {
    it.next();
    Setting *td = it.value();
    
    if (td->order() < 0)
      continue;
    
    order.insert(td->order(), it.key());
  }
  
  QMapIterator<int, QString> it2(order);
  while (it2.hasNext())
  {
    it2.next();
    d << it2.value();
  }
}
*/

void
Entity::setName (QString d)
{
  _name = d;
}

QString
Entity::name ()
{
  return _name;
}

QHash<QString, QVariant *>
Entity::settings ()
{
  return _settings;
}
