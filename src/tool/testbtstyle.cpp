#include "testbtstyle.h"
#include "QtGui/qpainter.h"
#include <QHBoxLayout>
#include <QDir>
#include <QDebug>
#include <QFileInfo>

TestBTStyle::TestBTStyle(QWidget *parent)
{

    QString filePath = "/Users/winks/Desktop/projects/PicShot/src/resources/tool";
//    QString filePath = "/Users/winks/Desktop/projects/ShotX/icons/normal";
    QDir *dir=new QDir(filePath);
    QStringList filter;
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
//    for(int i = 0;i<fileInfo->count(); i++)
//    {
//        qDebug()<<fileInfo->at(i).filePath();
//        qDebug()<<fileInfo->at(i).fileName();
//    }


    auto layout = new QHBoxLayout(this);
    int i = 0;

    QStringList listName;
    for(int i = 0;i<fileInfo->count(); i++)
    {
        if (fileInfo->at(i).fileName() == "." || fileInfo->at(i).fileName() == "..")
            continue;

        listName << fileInfo->at(i).fileName();
    }

    for (const auto &it : listName){
        auto tb = new QToolButton();
        tb->setObjectName(it);
        tb->setToolButtonStyle(Qt::ToolButtonIconOnly);
        tb->setAutoRaise(true);
        tb->setIcon(QIcon(":/resources/tool/" + it));
        tb->setIconSize(QSize(14, 14) * 2);
        tb->setContentsMargins(0, 0, 0, 0);
        tb->setFixedSize(QSize(14, 14) * 2);
        tb->setToolTip(it);
        tb->setChecked(false);
        tb->setCheckable(true);
        m_vItem.push_back(tb);

        layout->addWidget(tb);
    }

}

void TestBTStyle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

//    updateToolBtnIcon();
    QPainter pa(this);
    pa.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    pa.setPen(Qt::NoPen);
    pa.setBrush(QColor(255, 255, 255, 0.7 * 255));

    const int round = 4;
    pa.drawRoundedRect(contentsRect().adjusted(1, 1, -1, -1), round, round);
}
