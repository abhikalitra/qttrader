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

#include "LengthButton.h"
#include "BarLength.h"

#include <QtDebug>
#include <QMenu>


LengthButton::LengthButton ()
{
  createGUI();
}

void
LengthButton::createGUI ()
{
  QMenu *menu = new QMenu(this);
  connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(lengthAction(QAction *)));
  
  BarLength bl;
  QStringList tl = bl.list();
  for (int pos = 0; pos < tl.size(); pos++)
  {
    QAction *a = menu->addAction(tl.at(pos));
    a->setData(QVariant(tl.at(pos)));
  }
  
  setPopupMode(QToolButton::InstantPopup);
  setText(bl.indexToString(BarLength::_DAILY));
  setToolTip(tr("Bar Length"));
  setStatusTip(tr("Bar Length"));
  setMenu(menu);
  setMaximumSize(QSize(30, 30));
}

void
LengthButton::lengthAction (QAction *d)
{
  setText(d->data().toString());
  emit signalLength();
}

int
LengthButton::length ()
{
  BarLength bl;
  return bl.stringToIndex(text());
}

QString
LengthButton::lengthText ()
{
  return text();
}

void
LengthButton::setLength (int d)
{
  BarLength bl;
  setText(bl.indexToString(d));
}

int
LengthButton::loadSettings (QSettings &)
{
  return 0;
}

int
LengthButton::saveSettings (QSettings &)
{
  return 0;
}
