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
#include <QComboBox>

#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>
#include <QPainter>
#include <QSignalMapper>

#include <QDebug>

class QTcpSocket;
class QNetworkSession;

namespace Ui {
class Messenger;
}

struct friendInfo
{
    QString name;
    int uid;
};

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
    void readNetwork();
    void pollingServer();
    void handleCallFriend(int param);

private:
    Ui::Messenger *ui;
    QGridLayout *infoLayout;

    // Message information
    QTextEdit *inputArea;

    //QVBoxLayout *tabelLayout;
    //QScrollArea *messageArea;
    //QFrame *scrollFrame;
    QPushButton *sendMsgBtn;

    MessengerTab *friendTabs = nullptr;
    QVector<QVBoxLayout *> friendList;

    // Login information
    QLabel *userIcon;
    QLabel *comboboxLabel;
    QLabel *nameLabel;
    QLabel *passwdLabel;
    QLineEdit *nameLine;
    QLineEdit *passwdLine;
    QPushButton *enterBtn;
    QPushButton *clearBtn;
    QComboBox *registerLoginComboBox;
    QLabel *statusLabel;

    QScrollArea *friendInfoList;
    QVBoxLayout *friendListVBLayout;

    // Function
    void loginPage();
    void messagePage();
    void clearLoginPage();
    void callFriend(QString name);
    void authUser(bool isAuth);
    void sendNetworkCmd(quint64 cmdID, QString message);
    void sendNetworkfile(QString filePath);
    void sendGetFriendList();
    bool checkfriendVectorExist(QString name);
    void addFriendList();

    int dialog_num = 0;

    QMenu *fileMenu;
    QAction *openAct;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;

    quint64 cmdID = 0;
    quint64 blockSize = 0;
    int clientUid = 0;

    //QString picFolder = "C:/Users/A60013/Pictures/temp/";
    QString picFolder = "L:/Users/admin/Pictures/temp/";
    QVector<struct friendInfo> friendVector;
    QVector<struct friendInfo> friendVectorNew;
    QTimer *pollingTimer;
    QSignalMapper *signalMapper;
};

#endif // MESSENGER_H
