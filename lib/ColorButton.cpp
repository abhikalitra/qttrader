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

#include "ColorButton.h"

#include <QDebug>
#include <QColorDialog>

ColorButton::ColorButton (QWidget *w, QColor c) : QPushButton (w)
{
  _color = c;

  connect(this, SIGNAL(clicked()), this, SLOT(colorDialog()));

  _changed = FALSE;

  setColorButton();
}

void ColorButton::setColorButton ()
{
  QString s = "* { background-color: rgb(";
  s.append(QString::number(_color.red()));
  s.append("," + QString::number(_color.green()));
  s.append("," + QString::number(_color.blue()));
  s.append(") }");
  this->setStyleSheet(s);
}

QColor & ColorButton::color ()
{
  return _color;
}

void ColorButton::colorDialog ()
{
//  QColorDialog *dialog = new QColorDialog(_color, this);
  QColorDialog *dialog = new QColorDialog(_color, 0);
  connect(dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(colorDialog2(QColor)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->setOption(QColorDialog::ShowAlphaChannel, TRUE);
  dialog->show();
}

void ColorButton::colorDialog2 (QColor color)
{
  if (! color.isValid())
    return;

  _color = color;

  setColorButton();

  _changed = TRUE;

  emit valueChanged(_color);
}

void ColorButton::setColor (QColor c)
{
  _color = c;
  setColorButton();
}

int ColorButton::isChanged()
{
  return _changed;
}
