#include "messenger.h"
#include "ui_messenger.h"
//#include "clickablelabel.h"

Messenger::Messenger(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Messenger)
{
    ui->setupUi(this);
    setMinimumSize(800,800);
    setMaximumSize(800,800);
    infoLayout = new QGridLayout;

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(infoLayout);


    openAct = new QAction(tr("&New"), this);
    openAct->setStatusTip(tr("Create a new file"));
    connect(openAct, &QAction::triggered, this, &Messenger::newFile);
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);

    tcpSocket = new QTcpSocket(this);
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_5_9);
    connect(tcpSocket, &QIODevice::readyRead, this, &Messenger::readNetwork);
    tcpSocket->abort();
    tcpSocket->connectToHost("127.0.0.1", 54321);

    pollingTimer = new QTimer(this);
    pollingTimer->setInterval(3000);
    if (pollingTimer->isActive())
        pollingTimer->stop();
    connect(pollingTimer, SIGNAL(timeout()), this, SLOT(pollingServer()));

    loginPage();
}

void Messenger::recvCmdRegister(QString recvData)
{
    QString uid = recvData.split("|").at(0);
    QString message = recvData.split("|").at(1);
    statusLabel->setText(message);
}

void Messenger::recvCmdLogin(QString recvData)
{
    QString auth = recvData.split(" ").at(0);
    QString uid = recvData.split(" ").at(1);
    int ret = auth.compare(QString("yes"));
    if (!ret) {
        clientUid = uid.toInt();
        authUser(true);
    }
    else {
        authUser(false);
    }
}

void Messenger::recvCmdFriendList(QString recvData)
{
    QString num = recvData.split(" ").at(0);
    friendVectorNew.clear();
    for (int i = 0; i < num.toInt(); ++i) {
        QString name = recvData.split(" ").at(2 * i + 1);
        QString uid = recvData.split(" ").at(2 * (i + 1));
        if (!checkfriendVectorExist(name)) {
            struct friendInfo tempInfo;
            tempInfo.name = name;
            tempInfo.uid = uid.toInt();
            tempInfo.hasClientIcon = false;
            tempInfo.friendLabel = new ClickableLabel;

            friendVector.push_back(tempInfo);
            friendVectorNew.push_back(tempInfo);
        }
    }

    addFriendList();
}

void Messenger::recvCmdTalkRecv(QString recvData)
{
    qDebug() << "cmdid 5 " << recvData;
    QString num = recvData.split("\n").at(0);
    for (int i = 0; i < num.toInt(); ++i) {
        QString uid = recvData.split("\n").at(2 * i + 1);
        QString text = recvData.split("\n").at(2 * (i + 1));

        QString name = findNameByUid(uid.toInt());
        bool findFriendOnTab = false;
        int tabIndex, j;
        for (j = 0; j < friendTabs->count(); ++j) {
            if (name == friendTabs->tabText(j)) {
                findFriendOnTab = true;
                break;
            }
        }

        if (findFriendOnTab) {
            putMsgOnTab(j, true, text, true, nullptr);
        }
        else {
            tabIndex = callFriend(name);
            putMsgOnTab(tabIndex, true, text, true, nullptr);
        }
    }
}

void Messenger::recvCmdPicMeta(QString recvData)
{
    qDebug() << "cmdid 7 " << recvData;
    QString num = recvData.split(" ").at(0);
    qDebug() << "num" << num;
    for (int i = 0; i < num.toInt(); i++) {
        QString uid = recvData.split(" ").at(i + 1);
        picUidVector.push_back(uid.toInt());
    }
}

void Messenger::recvCmdPicRecv()
{
    QByteArray nextByte;
    in >> nextByte;

    QString clientFolder = picFolder + QString::number(clientUid);
    qDebug() << "server 8 " << clientFolder;
    QDir dir(clientFolder);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString clientFile = clientFolder + "/" + QString::number((int)friendUid) + ".jpg";
    qDebug() << "client file " << clientFile;
    QFile file(clientFile);
    file.open(QIODevice::WriteOnly);
    file.write(nextByte);
    file.close();

    for (int i = 0; i < friendVector.count(); ++i) {
        if (friendVector[i].uid == ((int)friendUid)) {
            friendVector[i].friendLabel->updateLabelPixmap(clientFile, friendVector[i].name);
        }
    }

    friendUid = 0;
}

void Messenger::recvCmdGifRecv(QString recvData)
{
    qDebug() << "cmdid 5 " << recvData;
    QString num = recvData.split("\n").at(0);
    for (int i = 0; i < num.toInt(); ++i) {
        QString uid = recvData.split("\n").at(2 * i + 1);
        QString gifNum = recvData.split("\n").at(2 * (i + 1));

        QString name = findNameByUid(uid.toInt());
        bool findFriendOnTab = false;
        int tabIndex, j;
        for (j = 0; j < friendTabs->count(); ++j) {
            if (name == friendTabs->tabText(j)) {
                findFriendOnTab = true;
                break;
            }
        }

        if (findFriendOnTab) {
            putMsgOnTab(j, false, nullptr, true, gifFilePathName(gifNum.toInt()));
        }
        else {
            tabIndex = callFriend(name);
            putMsgOnTab(tabIndex, false, nullptr, true, gifFilePathName(gifNum.toInt()));
        }
    }
}

void Messenger::readNetwork()
{
    if (cmdID == None) {
        if (tcpSocket->bytesAvailable() < (qint64)sizeof(quint64))
            return;
        in >> cmdID;
        qDebug() << "cmd Id " << cmdID;
    }

    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (qint64)sizeof(quint64))
            return;
        in >> blockSize;
    }

    if (cmdID == PicRecv) {

        qDebug() << "cmdid 8";
        qDebug() << "size " << blockSize;
        if (friendUid == 0) {
            if (tcpSocket->bytesAvailable() < (qint64)sizeof(quint64))
                return;
            in >> friendUid;
            qDebug() << friendUid;
        }

        if (tcpSocket->bytesAvailable() < (qint64)blockSize)
            return;

        recvCmdPicRecv();
    }
    else {

        if (tcpSocket->bytesAvailable() < (qint64)blockSize)
            return;

        QString recvData;
        in >> recvData;

        if (cmdID == Register) {
            recvCmdRegister(recvData);
        }
        if (cmdID == Login) {
            recvCmdLogin(recvData);
        }

        if (cmdID == FriendList) {
            recvCmdFriendList(recvData);
        }

        if (cmdID == TalkRecv) {
            recvCmdTalkRecv(recvData);
        }

        if (cmdID == PicMeta) {
            recvCmdPicMeta(recvData);
        }

        if (cmdID == GifRecv) {
            recvCmdGifRecv(recvData);
        }

    }

    cmdID = None;
    blockSize = 0;
}

void Messenger::addFriendList()
{
    for (int i = 0; i < friendVectorNew.count(); ++i) {

        friendVectorNew[i].friendLabel->updateLabelPixmap(":/list/login.jpg", friendVectorNew[i].name);
        connect(friendVectorNew[i].friendLabel, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(friendVectorNew[i].friendLabel, friendVectorNew[i].uid);
        friendListVBLayout->addWidget(friendVectorNew[i].friendLabel);
    }
}

bool Messenger::checkfriendVectorExist(QString name)
{
    for (int i = 0; i < friendVector.count(); ++i) {
        if (name == friendVector[i].name)
            return true;
    }
    return false;
}

void MessengerTab::tabChange(int index)
{
    qDebug() << index;
}

bool Messenger::fileExists(QString path)
{
    QFileInfo check_file(path);
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

void Messenger::newFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("開啟檔案"),"/","Head files(*.jpg)");

    QPixmap pixmap;
    pixmap.load(filePath);
    QString clientFolder = picFolder + QString::number(clientUid);
    QString clientFile = clientFolder + "/" + QString::number(clientUid) + ".jpg";
    QFile fileSelf(clientFile);
    fileSelf.open(QIODevice::WriteOnly);
    pixmap.save(&fileSelf, "JPG");
    pixmap = pixmap.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    clientIcon->setPixmap(pixmap);


    sendNetworkfile(filePath);
}

void Messenger::loginPage()
{
    comboboxLabel = new QLabel("register/Login");
    nameLabel = new QLabel("Enter name: ");
    passwdLabel = new QLabel("Enter password: ");
    statusLabel = new QLabel;
    registerLoginComboBox =new QComboBox;
    registerLoginComboBox->setEditable(false);
    registerLoginComboBox->insertItem(0, "Register");
    registerLoginComboBox->insertItem(1, "Login");
    nameLine = new QLineEdit;
    passwdLine = new QLineEdit;
    enterBtn = new QPushButton("Enter");
    clearBtn = new QPushButton("clear input");

    infoLayout->addWidget(comboboxLabel, 1, 0);
    infoLayout->addWidget(registerLoginComboBox, 1, 1);
    infoLayout->addWidget(nameLabel, 2, 0);
    infoLayout->addWidget(nameLine, 2, 1);
    infoLayout->addWidget(passwdLabel, 3, 0);
    infoLayout->addWidget(passwdLine, 3, 1);
    infoLayout->addWidget(enterBtn, 4, 0);
    infoLayout->addWidget(clearBtn, 4, 1);

    infoLayout->addWidget(statusLabel, 5, 1);

    userIcon = new QLabel();
    QPixmap *p = new QPixmap(":/list/login.jpg");
    QPixmap p1(p->scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    userIcon->setPixmap(p1);
    infoLayout->addWidget(userIcon, 0, 1); 

    QObject::connect(enterBtn, SIGNAL(clicked()), this, SLOT(clickLogin()));
    QObject::connect(clearBtn, SIGNAL(clicked()), this, SLOT(clickClear()));

}

void Messenger::authUser(bool isAuth)
{
    if (isAuth == true)
    {
        clearLoginPage();
        messagePage();
    }
    else
    {
        passwdLine->setText("");
        QMessageBox::warning(this,"System information", "Invalid User or Password");
    }
}

void Messenger::clearLoginPage()
{
    infoLayout->removeWidget(comboboxLabel);
    infoLayout->removeWidget(registerLoginComboBox);
    infoLayout->removeWidget(userIcon);
    infoLayout->removeWidget(nameLabel);
    infoLayout->removeWidget(nameLine);
    infoLayout->removeWidget(passwdLabel);
    infoLayout->removeWidget(passwdLine);
    infoLayout->removeWidget(enterBtn);
    infoLayout->removeWidget(clearBtn);
    infoLayout->removeWidget(statusLabel);

    comboboxLabel->hide();
    registerLoginComboBox->hide();
    userIcon->hide();
    nameLabel->hide();
    nameLine->hide();
    passwdLabel->hide();
    passwdLine->hide();
    enterBtn->hide();
    clearBtn->hide();
    statusLabel->hide();
}

void Messenger::sendNetworkCmd(quint64 cmdID, QString message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);

    out << (quint64)0;
    out << (quint64)0;
    out << message;
    out.device()->seek(0);
    out << cmdID;
    out.device()->seek(sizeof(quint64));
    out << (quint64)(block.size() - (2 * sizeof(quint64)));

    tcpSocket->write(block);
    tcpSocket->waitForBytesWritten();
}

void Messenger::sendNetworkfile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "open file path " + filePath + "failed";
        return;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);

    out << (quint64)0;
    out << (quint64)0;
    out << (quint64)0;
    out << file.readAll();
    out.device()->seek(0);
    out << (quint64)PicSend;
    out.device()->seek(sizeof(quint64));
    out << (quint64)(block.size() - (3 * sizeof(quint64)));
    out.device()->seek(2 * sizeof(quint64));
    out << (quint64)clientUid;

    tcpSocket->write(block);
    tcpSocket->waitForBytesWritten();
}

void Messenger::clickLogin()
{
    QString name = nameLine->text();
    QString passwd = passwdLine->text();
    if (registerLoginComboBox->currentIndex() == 0)
    {
        sendNetworkCmd(Register, name + " " + passwd + " ");
        passwdLine->setText("");
    }
    else {
        sendNetworkCmd(Login, name + " " + passwd + " ");
    }
}

void Messenger::clickClear()
{
    nameLine->clear();
    passwdLine->clear();
}

int Messenger::callFriend(QString name)
{
    QScrollArea *myScroll = new QScrollArea;
    QFrame *scrollFrame = new QFrame;
    scrollFrame->setStyleSheet("background-color:white;");
    QVBoxLayout *tabelLayout = new QVBoxLayout(scrollFrame);
    friendList.push_back(tabelLayout);

    myScroll->setWidget(scrollFrame);
    myScroll->setWidgetResizable(true);

    return friendTabs->addTab(myScroll, name);
}

void Messenger::sendGetFriendList()
{
    sendNetworkCmd(FriendList, QString::number(clientUid));
}

void Messenger::sendGetMessage()
{
    sendNetworkCmd(TalkRecv, QString::number(clientUid));
}

void Messenger::sendGetIconMeta()
{
    sendNetworkCmd(PicMeta, QString::number(clientUid));
}

void Messenger::sendGetIconByUid(int uid)
{
    sendNetworkCmd(PicRecv, QString::number(uid));
}

void Messenger::pollingServer()
{
    sendGetFriendList();
    sendGetMessage();
    sendGetIconMeta();

    qDebug() << "picUidVector " << picUidVector.count();
    for (int i = 0; i < picUidVector.count(); i++) {
        //qDebug() << " check pic uid " << picUidVector[i];
        for (int j = 0; j < friendVector.count(); j++) {
            if (friendVector[j].uid == picUidVector[i] && friendVector[j].hasClientIcon == false) {
            qDebug() << "check uid " << friendVector[j].uid;
            friendVector[j].hasClientIcon = true;
            sendGetIconByUid(picUidVector[i]);
            }
        }
    }
    picUidVector.clear();

}

void Messenger::handleCallFriend(int param)
{
    QString name;
    bool isExist = false;

    for (int i = 0; i < friendVector.count(); ++i) {
        if (param == friendVector[i].uid) {
            name = friendVector[i].name;
        }
    }

    for (int i = 0; i < friendTabs->count(); ++i) {
        if (friendTabs->tabText(i) == name)
            isExist = true;
    }

    if (!isExist)
        callFriend(name);
}

void Messenger::messagePage()
{
    sendMsgBtn = new QPushButton("send");
    selectGifBtn = new QPushButton("GIF");
    inputArea = new QTextEdit(this);
    inputArea->setFixedSize(300, 300);
    inputArea->setFocus();

    friendInfoList = new QScrollArea;
    QFrame *scrollFrame = new QFrame;
    friendListVBLayout = new QVBoxLayout(scrollFrame);
    friendInfoList->setWidget(scrollFrame);
    friendInfoList->setWidgetResizable(true);

    signalMapper = new QSignalMapper(this); // for friend dialog
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(handleCallFriend(int)));
    gifSignalMapper = new QSignalMapper(this);
    connect(gifSignalMapper, SIGNAL(mapped(int)), this, SLOT(handleGifSend(int)));

    QString clientFolder = picFolder + QString::number(clientUid);
    QDir dir(clientFolder);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    picUidVector.clear();

    if (!pollingTimer->isActive())
        pollingTimer->start();


    friendTabs = new MessengerTab(this);

    friendTabs->setStyleSheet("QTabBar::tab { min-width: 300px;}");

    infoLayout->addWidget(friendInfoList, 1, 1);

    // self pic icon
    clientIcon = new QLabel();
    QPixmap pix(":/list/login.jpg");
    pix = pix.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    clientIcon->setPixmap(pix);
    clientIcon->setStyleSheet("border:2px solid grey;border-radius: 10px;background-color: transparent;");
    infoLayout->addWidget(clientIcon, 0, 1);

    infoLayout->addWidget(friendTabs, 0, 0);

    infoLayout->setColumnMinimumWidth(0, 300);
    infoLayout->setRowMinimumHeight(0, 300);

    infoLayout->addWidget(inputArea, 1, 0);
    infoLayout->addWidget(sendMsgBtn, 2, 0);
    infoLayout->addWidget(selectGifBtn, 2, 1);

    QObject::connect(sendMsgBtn, SIGNAL(clicked()),this, SLOT(clickSendMsg()));
    QObject::connect(selectGifBtn, SIGNAL(clicked()),this, SLOT(clickGifPage()));
}

QString Messenger::findNameByUid(int uid)
{
    QString name = nullptr;
    for (int i = 0; i < friendVector.count(); i++)
    {
        if (friendVector[i].uid == uid) {
            name = friendVector[i].name;
            break;
        }
    }

    return name;
}

int Messenger::findUidByName(QString name)
{
    int uid = 0;
    for (int i = 0; i < friendVector.count(); i++)
    {
        if (friendVector[i].name == name) {
            uid = friendVector[i].uid;
            break;
        }
    }
    return uid;
}

void Messenger::putMsgOnTab(int tabId, bool isText,QString text, bool isFriend, QString gifFilePath)
{
    QLabel *new_label1 = new QLabel;
    QLabel *new_label2 = new QLabel;

    new_label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    new_label2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    if (isFriend) {
        if (isText) {
            new_label1->setText(text);
            new_label1->setStyleSheet("color: black;background-color:#1aed4a;font: bold 14px;border-style: outset;border-width: 2px;border-radius: 10px;border-color: beige;min-width: 10em;");
        } else {
            QMovie *movie = new QMovie(gifFilePath);
            movie->setScaledSize(QSize(100, 100));
            new_label1->setMovie(movie);
            movie->start();
        }
        //new_label1->setStyleSheet("color: black;background-color:#1aed4a;font: bold 14px;border-style: outset;border-width: 2px;border-radius: 10px;border-color: beige;min-width: 10em;");
        new_label2->setStyleSheet("min-width: 10em;");
    }
    else {
        if (isText) {
            new_label2->setText(text);
            new_label2->setStyleSheet("color: black;background-color:#eff0f4;font: bold 14px;border-style: outset;border-width: 2px;border-radius: 10px;border-color: beige;min-width: 10em;");
        } else {
            QMovie *movie = new QMovie(gifFilePath);
            movie->setScaledSize(QSize(100, 100));
            new_label1->setMovie(movie);
            movie->start();
        }
        //new_label2->setStyleSheet("color: black;background-color:#eff0f4;font: bold 14px;border-style: outset;border-width: 2px;border-radius: 10px;border-color: beige;min-width: 10em;");
        new_label1->setStyleSheet("min-width: 10em;");
    }
    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addWidget(new_label1);
    labelLayout->addWidget(new_label2);

    friendList.at(tabId)->addLayout(labelLayout);
}

void Messenger::handleGifSend(int gifNum)
{
    QString friendName;
    int uid;
    //struct friendMessage tempMessage;

    friendName = friendTabs->tabText(friendTabs->currentIndex());
    uid = findUidByName(friendName);

    if (uid > 0) {
        sendNetworkCmd(GifSend, QString::number(clientUid) + " " + QString::number(uid) +
                                     " \n" + QString::number(gifNum) + "\n");
        putMsgOnTab(friendTabs->currentIndex(), false, nullptr, false, gifFilePathName(gifNum));
    }
}

QString Messenger::gifFilePathName(int id)
{
    switch (id) {
    case 0:
        return ":/list/giphy.gif";
        break;
    case 1:
        return ":/list/tenor.gif";
        break;
    case 2:
        return ":/list/cat.gif";
        break;
    default:
        return nullptr;
        break;
    }
}

void Messenger::clickGifPage()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle(tr("Select GIF"));
    dialog->setMaximumSize(300, 300);
    dialog->setMinimumSize(300, 300);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *newLayout = new QGridLayout;

    ClickableLabel *gifLabel[3];
    for (int i = 0 ; i < 3; ++i) {
        gifLabel[i] = new ClickableLabel;
        gifLabel[i]->setGif(gifFilePathName(i));
    }

    for (int i = 0; i < 3; ++i)
    {
        connect(gifLabel[i], SIGNAL(clicked()), gifSignalMapper, SLOT(map()));
        gifSignalMapper->setMapping(gifLabel[i], i);
        newLayout->addWidget(gifLabel[i], 0, i);
    }

    dialog->setLayout(newLayout);
    dialog->show();
}

void Messenger::clickSendMsg()
{

    QString friendName;
    QString inputText;

    inputText = inputArea->toPlainText();
    friendName = friendTabs->tabText(friendTabs->currentIndex());

    int uid = findUidByName(friendName);
    if (uid > 0) {
        sendNetworkCmd(TalkSend, QString::number(clientUid) + " " + QString::number(uid) +
                                     " \n" + inputText + "\n");
        putMsgOnTab(friendTabs->currentIndex(), true, inputText, false, nullptr);
    }
}

Messenger::~Messenger()
{
    delete ui;
}
