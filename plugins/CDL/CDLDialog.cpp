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

#include "CDLDialog.h"
#include "Doc.h"
#include "Global.h"
#include "CurveOHLCType.h"
#include "TypeCandle.h"

#include <QtDebug>
#include <QVariant>
#include <QLayout>
#include <QFormLayout>


CDLDialog::CDLDialog (QWidget *p) : QDialog (p)
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

void CDLDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void CDLDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void CDLDialog::cancel ()
{
  saveSettings();
  reject();
}

void CDLDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("cdl_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("cdl_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void CDLDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("cdl_dialog_size", size());
  settings.setValue("cdl_dialog_pos", pos());
}

void CDLDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // CDL page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // color
  QVariant *set = _settings->get(QString("color"));
  
  _color = new ColorButton(this, QColor(set->toString()));
  connect(_color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _color);
  
  // pen
  set = _settings->get(QString("pen"));

  _pen = new QDoubleSpinBox;
  _pen->setRange(0.0, 100.0);
  _pen->setValue(set->toDouble());
  connect(_pen, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Penetration"), _pen);

  _tabs->addTab(w, QString("Candles"));

  // method 1 page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // method
  set = _settings->get(QString("method1"));

  TypeCandle ct;
  _method1 = new QComboBox;
  _method1->addItems(ct.list());
  _method1->setCurrentIndex(_method1->findText(set->toString(), Qt::MatchExactly));
  connect(_method1, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Method"), _method1);
  
  // bull color
  set = _settings->get(QString("bullColor1"));
  
  _bullColor1 = new ColorButton(this, QColor(set->toString()));
  connect(_bullColor1, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Bullish Color"), _bullColor1);
  
  // bear color
  set = _settings->get(QString("bearColor1"));
  
  _bearColor1 = new ColorButton(this, QColor(set->toString()));
  connect(_bearColor1, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Bearish Color"), _bearColor1);
  
  // show
  set = _settings->get(QString("show1"));

  _show1 = new QCheckBox;
  _show1->setChecked(set->toBool());
  connect(_show1, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _show1);

  _tabs->addTab(w, tr("Method 1"));

  
  // method 2 page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // method
  set = _settings->get(QString("method2"));

  _method2 = new QComboBox;
  _method2->addItems(ct.list());
  _method2->setCurrentIndex(_method2->findText(set->toString(), Qt::MatchExactly));
  connect(_method2, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Method"), _method2);
  
  // bull color
  set = _settings->get(QString("bullColor2"));
  
  _bullColor2 = new ColorButton(this, QColor(set->toString()));
  connect(_bullColor2, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Bullish Color"), _bullColor2);
  
  // bear color
  set = _settings->get(QString("bearColor2"));
  
  _bearColor2 = new ColorButton(this, QColor(set->toString()));
  connect(_bearColor2, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Bearish Color"), _bearColor2);
  
  // show
  set = _settings->get(QString("show2"));

  _show2 = new QCheckBox;
  _show2->setChecked(set->toBool());
  connect(_show2, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _show2);

  _tabs->addTab(w, tr("Method 2"));
}

void CDLDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("color"));
  set->setValue(_color->color().name());

  set = _settings->get(QString("pen"));
  set->setValue(_pen->value());

  set = _settings->get(QString("method1"));
  set->setValue(_method1->currentText());

  set = _settings->get(QString("bullColor1"));
  set->setValue(_bullColor1->color().name());

  set = _settings->get(QString("bearColor1"));
  set->setValue(_bearColor1->color().name());

  set = _settings->get(QString("show1"));
  set->setValue(_show1->isChecked());

  set = _settings->get(QString("method2"));
  set->setValue(_method2->currentText());

  set = _settings->get(QString("bullColor2"));
  set->setValue(_bullColor2->color().name());

  set = _settings->get(QString("bearColor2"));
  set->setValue(_bearColor2->color().name());

  set = _settings->get(QString("show2"));
  set->setValue(_show2->isChecked());
}

void CDLDialog::modified ()
{
  _modified = 1;
}
