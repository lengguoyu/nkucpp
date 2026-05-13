#ifndef BADMINTON_GAME_H
#define BADMINTON_GAME_H

#include <QWidget>
#include <QTimer>

class BadmintonGame : public QWidget
{
    Q_OBJECT

public:
    explicit BadmintonGame(QWidget *parent = nullptr);
    ~BadmintonGame();

signals:
    void gameFinished(bool isVictory);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void updateGame();

private:
    void initGame();
    void serve();
    void checkScore();
    void endGame();

private:
    QTimer* m_timer = nullptr;

    int m_playerScore = 0;
    int m_aiScore = 0;
    bool m_isGameOver = false;

    int m_playerX = 300;
    int m_aiX = 300;
    int m_shuttleX = 300;
    int m_shuttleY = 350;
    int m_velocityX = 3;
    int m_velocityY = -4;

    bool m_isServing = true;
    int m_serveCounter = 0;

    bool m_moveLeft = false;
    bool m_moveRight = false;

    static const int PLAYER_Y = 420;
    static const int AI_Y = 30;
    static const int PADDLE_WIDTH = 70;
    static const int PADDLE_HEIGHT = 12;
    static const int SHUTTLE_SIZE = 10;
};

#endif