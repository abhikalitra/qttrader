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

#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFormLayout>

#include "Entity.h"

class Dialog : public QDialog
{
  Q_OBJECT

  public:
    Dialog (QWidget *);
    ~Dialog ();

  public slots:
    virtual void done ();
    virtual void cancel ();
    virtual void help ();
    virtual void loadSettings ();
    virtual void saveSettings ();
    void setWidget (QWidget *);

  protected:
    QDialogButtonBox *_buttonBox;
    QVBoxLayout *_vbox;
    QFormLayout *_form;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QPushButton *_helpButton;
    QString _helpFile;
    QLabel *_message;
    QString _keySize;
    QString _keyPos;
};

#endif
