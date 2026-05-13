#ifndef GAMEMAINWINDOW_H
#define GAMEMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <random>

class BadmintonGame;
class PoetryGame;

class GameMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameMainWindow(QWidget *parent = nullptr);
    ~GameMainWindow();
    void setAttributeBarVisible(bool visible);

private slots:
    void onOption1();
    void onOption2();
    void onOption3();
    void onNext();
    void onBadmintonFinished(bool win);
    void onPoetryFinished(int correctCount);

private:
    void initUI();
    void showScene(int idx);
    void updateAttr();
    void addAttr(QString type, int val);
    bool randChance(int p);
    QString bgPath(int scene);
    void setBG(const QString& path);
    void clearOptions();
    void showOptions(QStringList texts);
    void resetGame();

    void scene0();
    void scene1();
    void scene2();
    void scene3();
    void scene4();
    void scene5();
    void scene6();
    void scene7();
    void scene8();
    void scene9();
    void scene10();
    void scene11();
    void scene12();
    void ending();

    void startBadminton();
    void startPoetry();

    QLabel* backgroundLabel;
    QTextBrowser* storyText;
    QWidget* optionsWidget;
    QPushButton* optBtn[4];
    QLabel* attributeLabel;
    QPushButton* nextBtn;

    struct Data {
        int wisdom = 0, warmth = 0, sleep = 0;
        int scene = 0;
        bool hasDiarrehea = false;
        bool savedPerson = false;
        bool calledPolice = false;
        bool pianoClosed = false;
        bool poemWritten = false;
        int firstChoice = 0;
        int secondType = 0;
        bool waitSecond = false;
        bool badmintonWin = false;
        int quizCount = 0;
    } data;

    std::mt19937 rng;
    bool needChangeScene;
    int tempNextScene;
};

#endif