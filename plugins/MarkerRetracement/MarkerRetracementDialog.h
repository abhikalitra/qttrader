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

#ifndef PLUGIN_MARKER_RETRACEMENT_DIALOG_HPP
#define PLUGIN_MARKER_RETRACEMENT_DIALOG_HPP

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QStringList>
#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>

#include "Entity.h"
#include "ColorButton.h"

class MarkerRetracementDialog : public QDialog
{
  Q_OBJECT

  public:
    MarkerRetracementDialog (QWidget *);
    void setGUI (Entity *);

  public slots:
    void done ();
    void cancel ();
    void help ();
    void loadSettings ();
    void saveSettings ();
    void valueChanged ();
    void save ();

  protected:
    Entity *_settings;
    QTabWidget *_tabs;
    QDialogButtonBox *_buttonBox;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QString _helpFile;
    int _modified;
    ColorButton *_color;
    QDoubleSpinBox *_price;
    QDoubleSpinBox *_price2;
    QDateTimeEdit *_date;
    QDateTimeEdit *_date2;
    QDoubleSpinBox *_level1;
    QDoubleSpinBox *_level2;
    QDoubleSpinBox *_level3;
    QDoubleSpinBox *_level4;
    QDoubleSpinBox *_level5;
    QDoubleSpinBox *_level6;
};

#endif
