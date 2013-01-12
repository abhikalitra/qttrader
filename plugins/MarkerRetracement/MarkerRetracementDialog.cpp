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

#include "MarkerRetracementDialog.h"
#include "Doc.h"
#include "Global.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


MarkerRetracementDialog::MarkerRetracementDialog (QWidget *p) : QDialog (p)
{
  _settings = 0;
  _modified = 0;
  _helpFile = "main.html";
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  // main vbox
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();

  // help button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void
MarkerRetracementDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void
MarkerRetracementDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void
MarkerRetracementDialog::cancel ()
{
  saveSettings();
  reject();
}

void
MarkerRetracementDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("marker_retracement_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("marker_retracement_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void
MarkerRetracementDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("marker_retracement_dialog_size", size());
  settings.setValue("marker_retracement_dialog_pos", pos());
}

void
MarkerRetracementDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // main page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // color
  QVariant *set = _settings->get(QString("color"));
  if (! set)
  {
    qDebug() << "MarkerRetracementDialog::setGUI: no color setting";
    return;
  }
  
  _color = new ColorButton(this, QColor(set->toString()));
  connect(_color, SIGNAL(valueChanged(QColor)), this, SLOT(valueChanged()));
  form->addRow(tr("Color"), _color);
  
  // date
  set = _settings->get(QString("date"));
  if (! set)
  {
    qDebug() << "MarkerRetracementDialog::setGUI: no date setting";
    return;
  }
  
  _date = new QDateTimeEdit;
  _date->setDateTime(set->toDateTime());
  connect(_date, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(valueChanged()));
  form->addRow(tr("Start Date"), _date);
  
  // date2
  set = _settings->get(QString("date2"));
  if (! set)
  {
    qDebug() << "MarkerRetracementDialog::setGUI: no date2 setting";
    return;
  }
  
  _date2 = new QDateTimeEdit;
  _date2->setDateTime(set->toDateTime());
  connect(_date2, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(valueChanged()));
  form->addRow(tr("End Date"), _date2);
  
  // price
  set = _settings->get(QString("price"));
  if (! set)
  {
    qDebug() << "MarkerRetracementDialog::setGUI: no price setting";
    return;
  }

  _price = new QDoubleSpinBox;
  _price->setRange(-99999999.0, 99999999.0);
  _price->setValue(set->toDouble());
  connect(_price, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("Start Price"), _price);
  
  // price2
  set = _settings->get(QString("price2"));
  if (! set)
  {
    qDebug() << "MarkerRetracementDialog::setGUI: no price2 setting";
    return;
  }

  _price2 = new QDoubleSpinBox;
  _price2->setRange(-99999999.0, 99999999.0);
  _price2->setValue(set->toDouble());
  connect(_price2, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("End Price"), _price2);

  _tabs->addTab(w, tr("Settings"));
  
  // levels page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // level1
  set = _settings->get(QString("level1"));

  _level1 = new QDoubleSpinBox;
  _level1->setRange(-99999999.0, 99999999.0);
  _level1->setValue(set->toDouble());
  connect(_level1, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("Level 1"), _level1);
  
  // level2
  set = _settings->get(QString("level2"));

  _level2 = new QDoubleSpinBox;
  _level2->setRange(-99999999.0, 99999999.0);
  _level2->setValue(set->toDouble());
  connect(_level2, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("Level 2"), _level2);
  
  // level3
  set = _settings->get(QString("level3"));

  _level3 = new QDoubleSpinBox;
  _level3->setRange(-99999999.0, 99999999.0);
  _level3->setValue(set->toDouble());
  connect(_level3, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("Level 3"), _level3);
  
  // level4
  set = _settings->get(QString("level4"));

  _level4 = new QDoubleSpinBox;
  _level4->setRange(-99999999.0, 99999999.0);
  _level4->setValue(set->toDouble());
  connect(_level4, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("Level 4"), _level4);
  
  // level5
  set = _settings->get(QString("level5"));

  _level5 = new QDoubleSpinBox;
  _level5->setRange(-99999999.0, 99999999.0);
  _level5->setValue(set->toDouble());
  connect(_level5, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("Level 5"), _level5);
  
  // level6
  set = _settings->get(QString("level6"));

  _level6 = new QDoubleSpinBox;
  _level6->setRange(-99999999.0, 99999999.0);
  _level6->setValue(set->toDouble());
  connect(_level6, SIGNAL(valueChanged(const QString &)), this, SLOT(valueChanged()));
  form->addRow(tr("Level 6"), _level6);

  _tabs->addTab(w, tr("Levels"));
}

void MarkerRetracementDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("color"));
  set->setValue(_color->color().name());

  set = _settings->get(QString("date"));
  set->setValue(_date->dateTime());

  set = _settings->get(QString("date2"));
  set->setValue(_date2->dateTime());

  set = _settings->get(QString("price"));
  set->setValue(_price->value());

  set = _settings->get(QString("price2"));
  set->setValue(_price2->value());

  set = _settings->get(QString("level1"));
  set->setValue(_level1->value());

  set = _settings->get(QString("level2"));
  set->setValue(_level2->value());

  set = _settings->get(QString("level3"));
  set->setValue(_level3->value());

  set = _settings->get(QString("level4"));
  set->setValue(_level4->value());

  set = _settings->get(QString("level5"));
  set->setValue(_level5->value());

  set = _settings->get(QString("level6"));
  set->setValue(_level6->value());
}

void MarkerRetracementDialog::valueChanged ()
{
  _modified = 1;
}
