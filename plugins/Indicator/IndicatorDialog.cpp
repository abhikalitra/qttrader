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

#include "IndicatorDialog.h"
#include "Global.h"

#include <QtDebug>
#include <QSettings>

IndicatorDialog::IndicatorDialog (QWidget *p) : Dialog (p)
{
  _keySize = "indicator_dialog_size";
  _keyPos = "indicator_dialog_pos";;

  // indicators
  QSettings settings(g_settings);
  settings.beginGroup("plugins");
  
  _indicator = new QComboBox;
  _indicator->addItems(settings.value("indicator").toStringList());
  _form->addRow(tr("Indicator"), _indicator);
  
  // row
  _row = new QSpinBox;
  _row->setRange(0, 5);
  _row->setValue(0);
  _form->addRow(tr("Row"), _row);

  // name
  _name = new QLineEdit(tr("New Indicator"));
  _form->addRow(tr("Name"), _name);
  
  QStringList tl;
  tl << g_title << tr("Add Indicator");
  setWindowTitle(tl.join(" "));
  
  Dialog::loadSettings();
}

void IndicatorDialog::done ()
{
  emit signalDone(_indicator->currentText(),
                  _row->value(),
                  _name->text());
  
  accept();
}
