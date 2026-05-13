#ifndef POETRY_GAME_H
#define POETRY_GAME_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

class PoetryGame : public QWidget
{
    Q_OBJECT

public:
    explicit PoetryGame(QWidget *parent = nullptr);
    ~PoetryGame();

signals:
    void gameFinished(int correctCount);  // 返回答对数量

private slots:
    void onAnswerClicked(int idx);

private:
    void initUI();
    void loadQuestions();
    void showQuestion();
    void endGame();

    struct Question {
        QString text;
        QString options[4];
        int answer;
    };

    QVector<Question> questions;
    int currentIndex;
    int correctCount;
    QTextBrowser* questionLabel;
    QPushButton* answerBtns[4];
    QLabel* scoreLabel;
};

#endif