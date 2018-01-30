#ifndef MESSENGERTAB_H
#define MESSENGERTAB_H

#include <QTabWidget>

class MessengerTab : public QTabWidget
{
    Q_OBJECT
public:
    MessengerTab(QWidget *parent)
    {
        this->setParent(parent);
        connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
        connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChange(int)));
    }
    ~MessengerTab(){}
public slots:
    void closeTab(int index)
    {
        this->removeTab(index);
    }
    void tabChange(int index);
};


#endif // MESSENGERTAB_H
