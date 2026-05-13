#include "badminton_game.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>

BadmintonGame::BadmintonGame(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("🏸 羽毛球对战 - 3分一局 🏸");
    setFixedSize(700, 500);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("background-color: #000000;");

    initGame();
}

BadmintonGame::~BadmintonGame()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
}

void BadmintonGame::initGame()
{
    m_playerScore = 0;
    m_aiScore = 0;
    m_isGameOver = false;
    m_playerX = width() / 2 - PADDLE_WIDTH / 2;
    m_aiX = width() / 2 - PADDLE_WIDTH / 2;
    m_shuttleX = width() / 2;
    m_shuttleY = PLAYER_Y - 30;
    m_velocityX = (QRandomGenerator::global()->bounded(2) == 0) ? 3 : -3;
    m_velocityY = -4;
    m_isServing = true;
    m_serveCounter = 0;
    m_moveLeft = false;
    m_moveRight = false;

    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BadmintonGame::updateGame);
    m_timer->start(16);
}

void BadmintonGame::serve()
{
    m_isServing = true;
    m_serveCounter = 0;
    m_shuttleX = m_playerX + PADDLE_WIDTH / 2;
    m_shuttleY = PLAYER_Y - 20;
    m_velocityX = QRandomGenerator::global()->bounded(-4, 5);
    if (m_velocityX == 0) m_velocityX = (QRandomGenerator::global()->bounded(2) == 0) ? 2 : -2;
    m_velocityY = -5;
}

void BadmintonGame::updateGame()
{
    if (m_isGameOver) return;

    if (m_isServing) {
        m_serveCounter++;
        if (m_serveCounter > 25) {
            m_isServing = false;
        }
        update();
        return;
    }

    if (m_moveLeft && m_playerX > 10) {
        m_playerX -= 9;
    }
    if (m_moveRight && m_playerX < width() - PADDLE_WIDTH - 10) {
        m_playerX += 9;
    }

    int aiTarget = m_shuttleX - PADDLE_WIDTH / 2;
    aiTarget = qBound(10, aiTarget, width() - PADDLE_WIDTH - 10);
    if (m_aiX < aiTarget) m_aiX += 4;
    if (m_aiX > aiTarget) m_aiX -= 4;

    m_shuttleX += m_velocityX;
    m_shuttleY += m_velocityY;

    if (m_shuttleX <= 5 || m_shuttleX >= width() - SHUTTLE_SIZE - 5) {
        m_velocityX = -m_velocityX;
    }

    QRect aiRect(m_aiX, AI_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
    QRect shuttleRect(m_shuttleX, m_shuttleY, SHUTTLE_SIZE, SHUTTLE_SIZE);

    if (shuttleRect.intersects(aiRect) && m_velocityY < 0) {
        m_velocityY = -m_velocityY;
        int offset = (m_shuttleX - (m_aiX + PADDLE_WIDTH/2)) / 5;
        m_velocityX += offset;
        m_velocityX = qBound(-7, m_velocityX, 7);
        m_shuttleY = AI_Y + PADDLE_HEIGHT;
    }

    QRect playerRect(m_playerX, PLAYER_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
    if (shuttleRect.intersects(playerRect) && m_velocityY > 0) {
        m_velocityY = -m_velocityY;
        int offset = (m_shuttleX - (m_playerX + PADDLE_WIDTH/2)) / 5;
        m_velocityX += offset;
        m_velocityX = qBound(-8, m_velocityX, 8);
        m_shuttleY = PLAYER_Y - SHUTTLE_SIZE;
    }

    if (m_shuttleY <= 0) {
        m_playerScore++;
        checkScore();
        serve();
    } else if (m_shuttleY >= height() - SHUTTLE_SIZE - 20) {
        m_aiScore++;
        checkScore();
        serve();
    }

    update();
}

void BadmintonGame::checkScore()
{
    if (m_playerScore >= 3 || m_aiScore >= 3) {
        endGame();
    }
}

void BadmintonGame::endGame()
{
    m_isGameOver = true;
    m_timer->stop();

    bool isVictory = (m_playerScore > m_aiScore);
    QString resultText = isVictory ?
                             "🎉 恭喜获胜！配合默契，赢得比赛！" :
                             "😔 遗憾落败...再接再厉！";

    QMessageBox::information(this, "比赛结束",
                             QString("最终比分：%1 : %2\n\n%3")
                                 .arg(m_playerScore).arg(m_aiScore).arg(resultText));

    emit gameFinished(isVictory);
    QTimer::singleShot(500, this, &QWidget::close);
}

void BadmintonGame::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 黑色背景
    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0));

    // 球场边界
    painter.setPen(QPen(Qt::white, 3));
    painter.drawRect(10, 10, width() - 20, height() - 20);
    painter.drawLine(0, height() / 2, width(), height() / 2);

    // 中线
    painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    painter.drawLine(width() / 2, 10, width() / 2, height() - 10);

    // 网
    painter.setPen(QPen(Qt::white, 2));
    for (int i = 0; i < width(); i += 20) {
        painter.drawLine(i, height() / 2, i + 10, height() / 2);
    }

    // 玩家球拍（下方 - 小猫）
    painter.setBrush(QBrush(QColor(255, 200, 0)));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(m_playerX, PLAYER_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
    painter.drawRect(m_playerX + PADDLE_WIDTH / 2 - 3, PLAYER_Y + PADDLE_HEIGHT, 6, 15);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(m_playerX + PADDLE_WIDTH / 2 - 8, PLAYER_Y - 5, "🐱");

    // AI球拍（上方 - 机器人）
    painter.setBrush(QBrush(QColor(0, 200, 255)));
    painter.drawRect(m_aiX, AI_Y, PADDLE_WIDTH, PADDLE_HEIGHT);
    painter.drawRect(m_aiX + PADDLE_WIDTH / 2 - 3, AI_Y - 15, 6, 15);
    painter.drawText(m_aiX + PADDLE_WIDTH / 2 - 8, AI_Y + 25, "🤖");

    // 羽毛球
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(m_shuttleX, m_shuttleY, SHUTTLE_SIZE, SHUTTLE_SIZE);
    painter.drawLine(m_shuttleX + SHUTTLE_SIZE / 2, m_shuttleY + SHUTTLE_SIZE,
                     m_shuttleX + SHUTTLE_SIZE / 2, m_shuttleY + SHUTTLE_SIZE + 8);

    // 比分显示
    painter.setPen(QPen(Qt::white, 3));
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    painter.drawText(width() / 2 - 80, 60, QString("🐱 %1").arg(m_playerScore));
    painter.drawText(width() / 2 + 30, 60, QString("🤖 %1").arg(m_aiScore));

    // 目标分数提示
    painter.setFont(QFont("Arial", 12));
    painter.setPen(QPen(Qt::yellow, 1));
    painter.drawText(width() / 2 - 40, 95, "先得3分获胜");

    // 操作提示
    painter.setFont(QFont("Arial", 12));
    painter.setPen(QPen(Qt::white, 1));
    painter.drawText(20, height() - 30, "← → 移动球拍");

    // 发球提示
    if (m_isServing) {
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.setPen(QPen(Qt::yellow, 3));
        painter.drawText(width() / 2 - 50, height() / 2 - 60, "发球中...");
    }
}

void BadmintonGame::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Left) {
        m_moveLeft = true;
    } else if (event->key() == Qt::Key_Right) {
        m_moveRight = true;
    }
}

void BadmintonGame::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Left) {
        m_moveLeft = false;
    } else if (event->key() == Qt::Key_Right) {
        m_moveRight = false;
    }
}