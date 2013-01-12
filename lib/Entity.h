/*
 *  QtTrader stock charter
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

// ******************************************************
// ******************************************************

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <QStringList>
#include <QHash>
#include <QSettings>
#include <QVariant>

//#include "Setting.h"

class Entity
{
  public:
    Entity ();
    ~Entity ();
    void clear ();
    void set (QString k, QVariant *);
    QVariant * get (QString k);
    void remove (QString);
    QList<QString> keys ();
    int size ();
    int saveSettings (QSettings &);
    int loadSettings (QSettings &);
//    void order (QStringList &);
    void setName (QString);
    QString name ();
    QHash<QString, QVariant *> settings ();
    
  protected:
    QString _name;
    QHash<QString, QVariant *> _settings;
};

#endif
