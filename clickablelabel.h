#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <QT>
#include <QPainter>

class ClickableLabel : public QLabel {
  Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();
    void updateLabelPixmap(QString pixFilePath, QString labelName);
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // CLICKABLELABEL_H
