#ifndef MESSENGER_H
#define MESSENGER_H

#include "messengertab.h"


#include <QMainWindow>
#include <QTextEdit>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QVector>

#include <QDebug>

namespace Ui {
class Messenger;
}

class Messenger : public QMainWindow
{
    Q_OBJECT

public:
    explicit Messenger(QWidget *parent = 0);
    ~Messenger();

private slots:
    void clickSendMsg();
    void clickLogin();
    void clickClear();

    void newFile();

private:
    Ui::Messenger *ui;
    QGridLayout *infoLayout;

    // Message information
    QTextEdit *inputArea;

    //QVBoxLayout *tabelLayout;
    //QScrollArea *messageArea;
    //QFrame *scrollFrame;
    QPushButton *sendMsgBtn;

    MessengerTab *firendTabs;
    QWidget *messageArea;
    QVector<QVBoxLayout *> friendList;

    // Login information
    QLabel *nameLabel;
    QLabel *passwdLabel;
    QLineEdit *nameLine;
    QLineEdit *passwdLine;
    QPushButton *loginBtn;
    QPushButton *clearBtn;

    // Function
    void loginPage();
    void messagePage();
    void clearLoginPage();
    void callFriend(QString name);
    int dialog_num = 0;

    QMenu *fileMenu;
    QAction *openAct;
    QLabel *test;
};

#endif // MESSENGER_H
