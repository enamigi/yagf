/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009 Andrei Borovsky <anb@symmetrica.net>

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

#include "FileToolBar.h"
#include <QSize>
#include <QAction>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QMatrix>
#include "utils.h"

FileToolBar::FileToolBar(QWidget * parent):QToolBar(trUtf8("Loaded Images"), parent)
{
    buttonsAdded = false;
    this->setIconSize(QSize(64, 96));
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    filesMap = new StringMap();
    rotMap = new IntMap();
    scaleMap = new FloatMap();
    saveButton = NULL;
    clearButton = NULL;
    removeButton = NULL;
    currentImage = "";
}

void FileToolBar::addFile(const QPixmap & pixmap, const QString & name)
{
    QAction * action;
    if (filesMap->values().contains(name))
        return;
    if (!buttonsAdded) {
        clearButton = new QPushButton(QIcon(":/clear.png"), trUtf8("Clear"), this);
        clearButton->setIconSize(QSize(24,24));
        connect(clearButton, SIGNAL(clicked()), this, SLOT(clearAll()));
        action = insertWidget(0, clearButton);
        saveButton = new QPushButton(QIcon(":/save_all.png"), trUtf8("Save..."), this);
        saveButton->setIconSize(QSize(24,24));
        connect(saveButton, SIGNAL(clicked()), this, SLOT(saveAll()));
        insertWidget(action, saveButton);
        removeButton = new QPushButton(QIcon(":/remove.png"), trUtf8("Remove"), this);
        removeButton->setIconSize(QSize(24,24));
        connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
        insertWidget(action, removeButton);
        buttonsAdded = true;
    }
    QPixmap pm = pixmap.scaledToHeight(96, Qt::FastTransformation);
    QString fn = extractFileName(name);
    if (filesMap->contains(fn))
        fn = fn.replace(".", "-1.");
    filesMap->insert(fn, name);
    rotMap->insert(fn, 0);
    action = addAction(QIcon(pm), fn);
    connect(action, SIGNAL(triggered()), this, SLOT(selected()));
    currentImage = fn;
}

void FileToolBar::selected()
{
    QString key = ((QAction *) sender())->text();
    currentImage = key;
    QString path = filesMap->value(key);
    emit fileSelected(path);
}

void FileToolBar::saveAll()
{
    QFileDialog fd(this, trUtf8("Select a directory"), QDir::home().path());
    fd.setFileMode(QFileDialog::DirectoryOnly);
    int rot = 0;
    QPixmap pm;
    if (fd.exec()) {
        QFile file;
        QMapIterator<QString, QString> i(*filesMap);
        while (i.hasNext()) {
             i.next();
             file.setFileName(i.value());
             rot = getRotation(i.value());
             if (rot) {
                pm.load(i.value());
                if (!pm.isNull()) {
                    QMatrix matrix;
                    matrix.rotate(rot);
                    pm = pm.transformed(matrix, Qt::SmoothTransformation);
                }
             }
             QString newName = fd.selectedFiles().at(0) + '/' + extractFileName(i.value());
             if (file.exists(newName)) {
                    QPixmap icon;
                    icon.load(":/warning.png");
                    QMessageBox messageBox(QMessageBox::NoIcon, "YAGF", trUtf8("File %1 already exists. Do you want to replace it?").arg(newName),
                        QMessageBox::Yes|QMessageBox::No, this);
                    messageBox.setIconPixmap(icon);
                    int result = messageBox.exec();
                    if (result == QMessageBox::No)
                        continue;
                    else
                        file.remove(newName);
             }
             if (rot && (!pm.isNull()))
                 pm.save(newName);
             else
                file.copy(newName);
         }

    }

}

void FileToolBar::clearAll()
{
    delete saveButton;
    delete clearButton;
    clear();
    buttonsAdded = false;
    filesMap->clear();
    rotMap->clear();
    scaleMap->clear();
}

QStringList FileToolBar::getFileNames()
{
    QStringList S;
    QMapIterator<QString, QString> i(*filesMap);
    while (i.hasNext()) {
        i.next();
        S << i.value();
    }
    return S;
}

void FileToolBar::remove()
{
    if (currentImage == "")
        return;
    for (int i = 0; i < actions().count(); i++)
        if (actions().at(i)->text() == currentImage) {
            this->removeAction(actions().at(i));
            filesMap->remove(currentImage);
            rotMap->remove(currentImage);
            scaleMap->remove(currentImage);
            currentImage = "";
            if (filesMap->count() > 0) {
                currentImage = filesMap->keys().at(0);
                QString path = filesMap->value(currentImage);
                emit fileSelected(path);
            }
            break;
        }
}

void FileToolBar::setRotation(int r)
{
    if (currentImage != "") {
        rotMap->remove(currentImage);
        rotMap->insert(currentImage, r % 360);
    }
}

void FileToolBar::setScale(float s)
{
    if (currentImage != "") {
        scaleMap->remove(currentImage);
        scaleMap->insert(currentImage, s);
    }
}

int FileToolBar::getRotation()
{
    if (currentImage == "")
        return 0;
    return rotMap->value(currentImage);
}

float FileToolBar::getScale()
{
    if (currentImage == "")
        return 0;
    return scaleMap->value(currentImage, float(0));
}

int FileToolBar::getRotation(const QString &name)
{
    if (name == "")
        return 0;
    QString internal = filesMap->keys(name).first();
    return rotMap->value(internal);
}

bool FileToolBar::fileLoaded(const QString &name)
{
    return filesMap->values().contains(name);
}

void FileToolBar::select(const QString &name)
{
    currentImage = filesMap->keys(name).first();
}