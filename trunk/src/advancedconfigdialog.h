/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009-2012 Andrei Borovsky <anb@symmetrica.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ADVANCEDCONFIGDIALOG_H
#define ADVANCEDCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class AdvancedConfigDialog;
}

class AdvancedConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdvancedConfigDialog(QWidget *parent = 0);
    ~AdvancedConfigDialog();
    bool doCrop1();
    void setCrop1(const bool value);
    bool doDeskew();
    void setDeskew(const bool value);
private:
    Ui::AdvancedConfigDialog *ui;
};

#endif // ADVANCEDCONFIGDIALOG_H
