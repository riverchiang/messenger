#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f) :QLabel(parent) {}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
}

void ClickableLabel::updateLabelPixmap(QString pixFilePath, QString labelName)
{
    QPixmap pixmap;
    pixmap.load(pixFilePath);
    QPixmap *pixmapTarget = new QPixmap(100, 50);
    pixmapTarget->fill(Qt::transparent);
    QPainter *painter = new QPainter(pixmapTarget);
    painter->drawPixmap(0, 0, 50, 50, pixmap);
    painter->drawText(50, 0, 50, 50, Qt::AlignCenter, labelName);
    painter->end();
    setPixmap(*pixmapTarget);
}

void ClickableLabel::setGif(QString gifFilePath)
{
    QMovie *movie = new QMovie(gifFilePath);
    movie->setScaledSize(QSize(50, 50));
    setMovie(movie);
    movie->start();
}
