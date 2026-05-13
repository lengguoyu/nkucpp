#include "poetry_game.h"
#include <QHBoxLayout>
#include <QMessageBox>

PoetryGame::PoetryGame(QWidget *parent)
    : QWidget(parent)
    , currentIndex(0)
    , correctCount(0)
{
    loadQuestions();
    initUI();
    showQuestion();
}

PoetryGame::~PoetryGame()
{
}

void PoetryGame::loadQuestions()
{
    // 高考古诗词真题
    questions = {
        {"\"想当年，金戈铁马，气吞万里如虎\"的作者是谁？",
         {"A. 辛弃疾", "B. 苏轼", "C. 陆游", "D. 岳飞"}, 0},

        {"\"出师未捷身先死，长使英雄泪满襟\"描写的是谁？",
         {"A. 诸葛亮", "B. 岳飞", "C. 文天祥", "D. 屈原"}, 0},

        {"下列哪句诗不是李白的作品？",
         {"A. 床前明月光", "B. 天生我材必有用", "C. 两个黄鹂鸣翠柳", "D. 长风破浪会有时"}, 2},

        {"\"大漠孤烟直，长河落日圆\"描绘的是哪里的景色？",
         {"A. 江南水乡", "B. 塞外大漠", "C. 中原平原", "D. 青藏高原"}, 1},

        {"\"劝君更尽一杯酒，西出阳关无故人\"中的\"阳关\"位于哪个省？",
         {"A. 陕西", "B. 青海", "C. 新疆", "D. 甘肃"}, 3},

        {"\"忽如一夜春风来，千树万树梨花开\"描写的是什么？",
         {"A. 春天梨花", "B. 秋天落叶", "C. 冬天大雪", "D. 夏天暴雨"}, 2},

        {"\"采菊东篱下，悠然见南山\"的作者是？",
         {"A. 陶渊明", "B. 谢灵运", "C. 王维", "D. 孟浩然"}, 0},

        {"下列哪句诗表达了爱国情怀？",
         {"A. 举头望明月", "B. 人生得意须尽欢", "C. 王师北定中原日", "D. 采菊东篱下"}, 2},

        {"\"无边落木萧萧下，不尽长江滚滚来\"是谁的名句？",
         {"A. 李白", "B. 杜甫", "C. 白居易", "D. 王维"}, 1},

        {"\"海上生明月，天涯共此时\"表达了什么情感？",
         {"A. 思乡怀人", "B. 壮志豪情", "C. 悠闲自得", "D. 愤世嫉俗"}, 0}
    };
}

void PoetryGame::initUI()
{
    setFixedSize(800, 500);
    setStyleSheet("background-color: rgba(0, 0, 0, 0.85); border-radius: 20px;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel("📖 古诗文知识竞赛 📖");
    title->setStyleSheet("color: #ffd700; font-size: 22px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    questionLabel = new QTextBrowser();
    questionLabel->setFixedHeight(120);
    questionLabel->setStyleSheet("background-color: rgba(255,255,255,0.1); color: black; font-size: 18px; border-radius: 10px; padding: 15px;");
    mainLayout->addWidget(questionLabel);

    for(int i = 0; i < 4; i++) {
        answerBtns[i] = new QPushButton();
        answerBtns[i]->setFixedHeight(50);
        answerBtns[i]->setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(52, 152, 219, 0.8);"
            "   color: white;"
            "   font-size: 16px;"
            "   border-radius: 8px;"
            "   text-align: left;"
            "   padding-left: 20px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(41, 128, 185, 0.9);"
            "}"
            );
        mainLayout->addWidget(answerBtns[i]);
        connect(answerBtns[i], &QPushButton::clicked, [this, i]() { onAnswerClicked(i); });
    }

    scoreLabel = new QLabel();
    scoreLabel->setStyleSheet("color: #ffd700; font-size: 14px;");
    scoreLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(scoreLabel);
}

void PoetryGame::showQuestion()
{
    if(currentIndex >= 5) {
        endGame();
        return;
    }

    Question& q = questions[currentIndex];
    questionLabel->setText(QString("<b>第 %1 / 5 题</b><br><br>%2")
                               .arg(currentIndex + 1)
                               .arg(q.text));

    for(int i = 0; i < 4; i++) {
        answerBtns[i]->setText(q.options[i]);
    }

    scoreLabel->setText(QString("✓ 已答对 %1 题").arg(correctCount));
}

void PoetryGame::onAnswerClicked(int idx)
{
    Question& q = questions[currentIndex];

    if(idx == q.answer) {
        correctCount++;
        QMessageBox::information(this, "回答正确", "✅ 回答正确！\n\n" + q.options[idx]);
    } else {
        QString correctAnswer = q.options[q.answer];
        QMessageBox::warning(this, "回答错误", "❌ 回答错误！\n\n正确答案是：" + correctAnswer);
    }

    currentIndex++;
    showQuestion();
}

void PoetryGame::endGame()
{
    emit gameFinished(correctCount);
    close();
}