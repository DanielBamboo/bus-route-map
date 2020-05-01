/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [1]
#include "googlesuggest.h"
#include "matrixop.h"
#include <map>
#include <vector>
#include <string>
#include "search_box/pinyin.h"
using namespace std;

const QString gsuggestUrl(QStringLiteral("http://google.com/complete/search?output=toolbar&q=%1"));
//! [1]

//! [2]
GSuggestCompletion::GSuggestCompletion(QLineEdit *parent): QObject(parent), editor(parent)
{
    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(1);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->header()->hide();

    popup->installEventFilter(this);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            SLOT(doneCompletion()));

    timer.setSingleShot(true);
    timer.setInterval(500);
    connect(&timer, SIGNAL(timeout()), SLOT(autoSuggest()));
    connect(editor, SIGNAL(textEdited(QString)), &timer, SLOT(start()));

    /*
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleNetworkData(QNetworkReply*)));
    */

    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("127.0.0.1");
    proxy.setPort(10809);
    QNetworkProxy::setApplicationProxy(proxy);
}
//! [2]

//! [3]
GSuggestCompletion::~GSuggestCompletion()
{
    delete popup;
}
//! [3]

//! [4]
bool GSuggestCompletion::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != popup)
        return false;

    if (ev->type() == QEvent::MouseButtonPress) {
        popup->hide();
        editor->setFocus();
        return true;
    }

    if (ev->type() == QEvent::KeyPress) {
        bool consumed = false;
        int key = static_cast<QKeyEvent*>(ev)->key();
        switch (key) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            doneCompletion();
            consumed = true;
            break;

        case Qt::Key_Escape:
            editor->setFocus();
            popup->hide();
            consumed = true;
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            break;

        default:
            editor->setFocus();
            editor->event(ev);
            popup->hide();
            break;
        }

        return consumed;
    }

    return false;
}
//! [4]

//! [5]
void GSuggestCompletion::showCompletion(const QVector<QString> &choices)
{
    if (choices.isEmpty())
        return;

    const QPalette &pal = editor->palette();
    QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);

    popup->setUpdatesEnabled(false);
    popup->clear();

    for (const auto &choice : choices) {
        auto item  = new QTreeWidgetItem(popup);
        item->setText(0, choice);
        item->setForeground(0, color);
    }

    popup->setCurrentItem(popup->topLevelItem(0));
    popup->resizeColumnToContents(0);
    popup->setUpdatesEnabled(true);

    popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
    popup->setFocus();
    popup->show();
}
//! [5]

//! [6]
void GSuggestCompletion::doneCompletion()
{
    timer.stop();
    popup->hide();
    editor->setFocus();
    QTreeWidgetItem *item = popup->currentItem();
    if (item) {
        editor->setText(item->text(0));
        //QMetaObject::invokeMethod(editor, "returnPressed");
    }
}
//! [6]

//! [7]
void GSuggestCompletion::autoSuggest()
{
    QString str = editor->text();
    //QString url = gsuggestUrl.arg(str);
    //networkManager.get(QNetworkRequest(url));
    kmpResult(str);
}
//! [7]

//! [8]
void GSuggestCompletion::preventSuggest()
{
    timer.stop();
}
//! [8]

/*
//! [9]
void GSuggestCompletion::handleNetworkData(QNetworkReply *networkReply)
{
    QUrl url = networkReply->url();
    if (networkReply->error() == QNetworkReply::NoError) {
        qDebug("get the result");
        QVector<QString> choices;

        QByteArray response(networkReply->readAll());
        QXmlStreamReader xml(response);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.tokenType() == QXmlStreamReader::StartElement)
                if (xml.name() == "suggestion") {
                    QStringRef str = xml.attributes().value("data");
                    choices << str.toString();
                }
        }

        showCompletion(choices);
    } else {
        qDebug("error");
    }

    networkReply->deleteLater();
}
//! [9]
*/

void GSuggestCompletion::setMap(MatrixOp *mapOp) {
    this->mapOp = mapOp;
}

//is b match any sequence of a ?
int match_or_not(QString a, QString b) {
    int lena = a.length(), lenb = b.length();
    int **matrix = new int*[lena + 1];
    for(int i = 0; i < lena + 1; i++) {
        matrix[i] = new int[lenb + 1];
        memset(matrix[i], 0, sizeof(int) * lenb);
    }
    //lena * lenb 的矩阵
    for(int i = 1; i < lena+1; i++) {
        for(int j = 1; j < lenb+1; j++) {
            if(a[i-1] != b[j-1]) {
                matrix[i][j] = max(matrix[i-1][j], matrix[i][j-1]);
            } else {
                matrix[i][j] = matrix[i-1][j-1] + 1;
            }
        }
    }
    int res = matrix[lena][lenb] == lenb;
    for(int i = 0; i < lena + 1; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    return res;
}

void GSuggestCompletion::kmpResult(QString str) {
    using namespace std;
    QVector<QString> res;
    vector<string> notQnames = mapOp->getNames();
    QVector<QString> names = mapOp->QgetNames();

    //这个map的定义是这样的，汉字->原汉字，拼音->汉字
    QMap<QString, QString> include_pinyin_map;
    for(auto i : names) {
        include_pinyin_map.insert(i, i);
    }

    //还得将拼音和中文绑定在一起
    for(auto i : notQnames) {
        QString pinyin_name = StringOp::str2qstr(Pinyin::convert_py(i));
        names.push_back(pinyin_name);
        include_pinyin_map.insert(pinyin_name, StringOp::str2qstr(i));
    }

    //改成match_or_not()，以下被注释的是匹配子串
    /*
    //先不写模式匹配，因为我还没有实现过，先实现一下普通的搜索算法
    for(auto name : names) {
        int i = 0, j = 0;
        while(j < name.length() && i < str.length()) {
            if(str[i] == name[j]) {
                i = i+1;
                j = j+1;
            } else {
                j = j - i + 1;
                i = 0;
            }
        }
        if(i == str.length()) {
            res.push_back(include_pinyin_map[name]);
        }
    }
    */
    for(auto name : names) {
        if(match_or_not(name, str)) {
            res.push_back(include_pinyin_map[name]);
        }
    }

    showCompletion(res);
}
