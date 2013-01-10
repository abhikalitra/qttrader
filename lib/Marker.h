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

#ifndef MARKER_HPP
#define MARKER_HPP

#include <QList>
#include <QRegion>
#include <QPainter>
#include <qwt_plot_item.h>
#include <qwt_scale_map.h>

#include "Entity.h"


class Marker : public QwtPlotItem
{
  public:
    Marker (QString);
    ~Marker ();
    virtual void draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &) const;  // via plugin
    int info (QStringList &); // via plugin
    int highLow (int start, int end, double &high, double &low); // via plugin
    int move (int &status, QPoint); // via plugin
    int click (int &status, int button, QPoint); // via plugin
    int create (int &status); // via plugin
    
    int rtti () const;
    void clearSelectionArea ();
    void appendSelectionArea (QRegion);
    int isSelected (QPoint);
    int isGrabSelected (QPoint);
    void clearGrabHandles ();
    void appendGrabHandle (QRegion);
    void setSelected (bool);
    bool selected ();
    void setModified (bool);
    bool modified ();
    Entity * settings ();
    void setSettings (Entity *);
    int handleWidth ();
    void setReadOnly (bool);
    bool readOnly ();
    void setID (QString);
    QString ID ();
    QString plugin ();

  protected:
    Entity *_settings;
    mutable QList<QRegion> _selectionArea;
    mutable QList<QRegion> _grabHandles;
    bool _selected;
    int _handleWidth;
    bool _modified;
    bool _ro;
    QString _id;
    QString _pluginString;
};

#endif
