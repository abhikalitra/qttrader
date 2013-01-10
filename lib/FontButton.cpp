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

#include "FontButton.h"

#include <QFontDialog>

FontButton::FontButton (QWidget *w, QFont f) : QPushButton (w)
{
  _font = f;
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(fontDialog()));
  setMaximumHeight(25);
  setFontButton();
  setText(tr("Font"));
  _changed = FALSE;
}

void FontButton::setFontButton ()
{
  setFont(_font);
}

QFont & FontButton::font ()
{
  return _font;
}

void FontButton::fontDialog ()
{
  QFontDialog *dialog = new QFontDialog(_font, this);
  connect(dialog, SIGNAL(fontSelected(const QFont &)), this, SLOT(fontDialog2(QFont)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void FontButton::fontDialog2 (QFont font)
{
  _font = font;
  setFontButton();
  _changed = TRUE;
  emit valueChanged();
}

int FontButton::isChanged()
{
  return _changed;
}
