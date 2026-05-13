#include "game_main_window.h"
#include "badminton_game.h"
#include "poetry_game.h"
#include <QPixmap>
#include <QHBoxLayout>
#include <QMessageBox>

// ==================== 构造/析构 ====================

GameMainWindow::GameMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , rng(std::random_device{}())
{
    initUI();
    resetGame();
}

GameMainWindow::~GameMainWindow()
{
}

// ==================== UI初始化 ====================

void GameMainWindow::initUI()
{
    setWindowTitle("小猫校园模拟器");
    setFixedSize(1200, 800);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ========== 属性栏（常置在顶部） ==========
    attributeLabel = new QLabel();
    attributeLabel->setFixedHeight(45);
    attributeLabel->setStyleSheet(
        "QLabel{"
        "   background-color: rgba(0, 0, 0, 0.7);"
        "   color: #ffd700;"
        "   font-size: 15px;"
        "   font-weight: bold;"
        "   border-bottom: 2px solid #ffd700;"
        "}"
        );
    attributeLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(attributeLabel);

    // 背景区域
    backgroundLabel = new QLabel();
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setMinimumSize(1200, 800);
    backgroundLabel->setStyleSheet("background-color: #2c3e50;");

    QVBoxLayout* bgLayout = new QVBoxLayout(backgroundLabel);
    bgLayout->setContentsMargins(50, 80, 50, 50);

    storyText = new QTextBrowser();
    storyText->setFixedHeight(220);
    storyText->setStyleSheet(
        "QTextBrowser{"
        "   background-color:rgba(0,0,0,0.65);"
        "   color:white;"
        "   font-size:16px;"
        "   border:none;"
        "   border-radius:15px;"
        "   padding:15px;"
        "}"
        );

    optionsWidget = new QWidget();
    optionsWidget->setFixedHeight(160);
    optionsWidget->setStyleSheet("background-color:rgba(0,0,0,0.55);border-radius:15px;");
    QVBoxLayout* optLayout = new QVBoxLayout(optionsWidget);

    for(int i = 0; i < 4; i++) {
        optBtn[i] = new QPushButton();
        optBtn[i]->setFixedHeight(38);
        optBtn[i]->setStyleSheet(
            "QPushButton{"
            "   background-color:rgba(52,152,219,0.85);"
            "   color:white;"
            "   font-size:14px;"
            "   font-weight:bold;"
            "   border:none;"
            "   border-radius:8px;"
            "}"
            "QPushButton:hover{"
            "   background-color:rgba(41,128,185,0.95);"
            "}"
            );
        optLayout->addWidget(optBtn[i]);
        optBtn[i]->hide();
    }

    connect(optBtn[0], &QPushButton::clicked, this, &GameMainWindow::onOption1);
    connect(optBtn[1], &QPushButton::clicked, this, &GameMainWindow::onOption2);
    connect(optBtn[2], &QPushButton::clicked, this, &GameMainWindow::onOption3);

    nextBtn = new QPushButton("✨ 点击继续 ✨");
    nextBtn->setFixedHeight(45);
    nextBtn->setStyleSheet(
        "QPushButton{"
        "   background-color:rgba(231,76,60,0.85);"
        "   color:white;"
        "   font-size:16px;"
        "   font-weight:bold;"
        "   border:none;"
        "   border-radius:10px;"
        "}"
        "QPushButton:hover{"
        "   background-color:rgba(192,57,43,0.95);"
        "}"
        );
    connect(nextBtn, &QPushButton::clicked, this, &GameMainWindow::onNext);

    bgLayout->addWidget(storyText);
    bgLayout->addWidget(optionsWidget);
    bgLayout->addWidget(nextBtn);
    mainLayout->addWidget(backgroundLabel);

    optionsWidget->hide();
    updateAttr();
}

// ==================== 辅助函数 ====================

void GameMainWindow::clearOptions()
{
    for(int i = 0; i < 4; i++) optBtn[i]->hide();
    optionsWidget->hide();
}

void GameMainWindow::showOptions(QStringList texts)
{
    clearOptions();
    for(int i = 0; i < texts.size(); i++) {
        optBtn[i]->setText(texts[i]);
        optBtn[i]->show();
    }
    optionsWidget->show();
    nextBtn->hide();
}

void GameMainWindow::updateAttr()
{
    attributeLabel->setText(QString("🧠 智慧: %1      💗 暖心: %2      😴 睡眠: %3")
                                .arg(data.wisdom)
                                .arg(data.warmth)
                                .arg(data.sleep));
}

void GameMainWindow::addAttr(QString type, int val)
{
    if(type == "wisdom") data.wisdom += val;
    else if(type == "warmth") data.warmth += val;
    else if(type == "sleep") data.sleep += val;
    updateAttr();

    QString name = (type == "wisdom") ? "智慧" : ((type == "warmth") ? "暖心" : "睡眠");
    storyText->append(QString("<p style='color:#ffd700;'>✨ %1 %2%3 ✨</p>")
                          .arg(name)
                          .arg(val >= 0 ? "+" : "")
                          .arg(val));
}

bool GameMainWindow::randChance(int p)
{
    return (rng() % 100) < p;
}

QString GameMainWindow::bgPath(int scene)
{
    QString base = ":/images/";
    if(scene == 0) return base + "campus.jpg";
    if(scene == 1) return base + "bedroom.jpg";
    if(scene == 2) return base + "living_room.jpg";
    if(scene == 3 || scene == 4 || scene == 7 || scene == 8 || scene == 10) return base + "classroom.jpg";
    if(scene == 5) return base + "canteen.jpg";
    if(scene == 6) return base + "free_time.jpg";
    if(scene == 9) return base + "sunset.jpg";
    if(scene == 11) return base + "night_classroom.jpg";
    if(scene == 12 || scene == 13) return base + "bedroom_night.jpg";
    return base + "bedroom_night.jpg";
}

void GameMainWindow::setBG(const QString& path)
{
    QPixmap pix(path);
    if(!pix.isNull()) {
        backgroundLabel->setPixmap(pix.scaled(1200, 800, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}

void GameMainWindow::resetGame()
{
    // 完全重置所有游戏数据
    data = Data();  // 所有属性归零，所有状态重置

    needChangeScene = false;
    tempNextScene = -1;

    // 重置选项相关状态
    data.waitSecond = false;
    data.secondType = 0;
    data.firstChoice = 0;

    // 重置各种事件触发标记
    data.hasDiarrehea = false;
    data.savedPerson = false;
    data.calledPolice = false;
    data.pianoClosed = false;
    data.poemWritten = false;
    data.badmintonWin = false;
    data.quizCount = 0;

    // 刷新属性显示
    updateAttr();

    // 清空选项和剧情
    clearOptions();
    storyText->clear();
    storyText->hide();  // 开始页面隐藏剧情框

    // 开始页面隐藏属性栏
    attributeLabel->hide();

    // 重置按钮文字
    nextBtn->setText("✨ 点击继续 ✨");

    // 回到开始页面（scene0）
    showScene(0);
}

void GameMainWindow::setAttributeBarVisible(bool visible)
{
    attributeLabel->setVisible(visible);
}

void GameMainWindow::showScene(int idx)
{
    data.scene = idx;
    setBG(bgPath(idx));

    // 开始页面（scene0）隐藏属性栏和剧情框，其他页面显示
    if(idx == 0) {
        attributeLabel->hide();
        storyText->hide();
        // 重置介绍标记
        data.waitSecond = false;
        data.secondType = 0;
    } else {
        attributeLabel->show();
        storyText->show();
    }
    switch(idx) {
    case 0: scene0(); break;
    case 1: scene1(); break;
    case 2: scene2(); break;
    case 3: scene3(); break;
    case 4: scene4(); break;
    case 5: scene5(); break;
    case 6: scene6(); break;
    case 7: scene7(); break;
    case 8: scene8(); break;
    case 9: scene9(); break;
    case 10: scene10(); break;
    case 11: scene11(); break;
    case 12: scene12(); break;
    case 13: ending(); break;
    default: break;
    }
}
// ==================== 场景0 ====================

void GameMainWindow::scene0()
{
    storyText->clear();
    storyText->hide();  // 开始页面隐藏剧情框
    showOptions({"🎮 开始游戏", "📖 游戏介绍"});
}

// ==================== 场景1 ====================

void GameMainWindow::scene1()
{
    storyText->show();
    storyText->clear();
    storyText->append("🌅 朝阳温柔地透过窗户，63在晨曦微光中醒来，崭新的上学日正式开启。");
    storyText->append("门外传来猫妈妈小夏的声音：\"63，快起床啦！上学要迟到了！早餐我给你做好了，我先去上班咯！\"");
    showOptions({"😴 再睡五分钟", "💪 马上就起"});
}
// ==================== 场景2 ====================

void GameMainWindow::scene2()
{
    storyText->clear();
    storyText->append("🍜 来到客厅，温暖的晨光洒在窗边的风信子上，紫色的花朵轻轻摇曳。");
    storyText->append("桌子上摆着猫妈妈小夏做的早餐：一碗热腾腾的清汤面，撒着翠绿葱花，暖心又暖胃。");
    showOptions({"🍜 吃完妈妈做的爱心面条再出发", "🥟 出门买烧麦，带去教室食用"});
}

// ==================== 场景3 ====================

void GameMainWindow::scene3()
{
    storyText->clear();

    if(data.firstChoice == 1) {
        storyText->append("🏫 63一路狂奔，最终还是迟到了。刚推开教室门，班主任小猫福禄抬了抬眼镜：\"63，快点回去坐好，天天瞌睡这么多。\"63腼腆一笑，快步走到座位。");
        addAttr("wisdom", -10);
    }
    else if(data.firstChoice == 2) {
        if(randChance(60)) {
            storyText->append("🏫 63提前到达教室，安安稳稳坐下，还能好好背一会儿单词。同桌小猫17推了推眼镜：\"63，你今天来的好早哦。\"63挠挠头不好意思地笑了笑。");
            addAttr("wisdom", 10);
        } else {
            storyText->append("🏫 同桌小猫17拿出63最爱吃的小饼干：\"63，你今天来的好早哦。我给你带了你最爱吃的小饼干。\"63两眼放光，开心得几乎跳起来！低头偷笑：\"谢谢你！我爱你！\"");
            addAttr("warmth", 15);
        }
    }
    else if(data.hasDiarrehea) {
        storyText->append("🏫 63刚坐下没多久，肚子忽然一阵绞痛。她捂着肚子小声对17说：\"完了...我好像肚子疼...\"17担忧地看着她：\"你快去厕所吧，我帮你看着老师。\"");
        addAttr("wisdom", -10);
    }

    needChangeScene = true;
    tempNextScene = 4;
    nextBtn->show();
}

// ==================== 场景4 ====================

void GameMainWindow::scene4()
{
    storyText->clear();
    storyText->append("📚 上课铃响，语文课开始。语文老师笑着说：\"今天做高考语文古诗词抽背训练，每个同学都要准备好，随时有可能抽到你！\"");

    if(randChance(80)) {
        storyText->append("🎯 老师抽到了63！");
        showOptions({"🎯 开始答题挑战", "🙈 假装肚子疼逃避"});
    } else {
        storyText->append("😌 幸运地没有被抽到，63松了一口气。");
        needChangeScene = true;
        tempNextScene = 5;
        nextBtn->show();
    }
}

// ==================== 场景5 ====================

void GameMainWindow::scene5()
{
    storyText->clear();
    storyText->append("🍽️ 上午的课程结束，午餐时间到。小猫17凑过来：\"63，今天吃什么？我纠结好久了。\"");
    showOptions({"🍜 来一碗热气腾腾的羊肉粉", "🍳 选一份鲜香可口的小炒"});
}

// ==================== 场景6 ====================

void GameMainWindow::scene6()
{
    storyText->clear();
    storyText->append("🎯 好不容易的空闲时间，到底做点什么好呢？");
    showOptions({"😴 好好午睡一下吧", "🚶 不然去逛一下吧"});
}

// ==================== 场景7 ====================

void GameMainWindow::scene7()
{
    storyText->clear();

    if(data.sleep > 50) {
        storyText->append("📐 精力充沛，兴致勃勃上数学课。");
        addAttr("wisdom", 10);
        if(randChance(20)) {
            storyText->append("🎯 老师抽中了63回答问题！");
            showOptions({"圆锥曲线", "立体几何", "导数"});  // 只保留3个选项
            return;
        }
    } else {
        storyText->append("😴 数学课上昏昏欲睡，在梦里和周公讨论圆锥曲线。");
        if(randChance(80)) {
            storyText->append("🎯 老师抽中了63回答问题！");
            showOptions({"圆锥曲线", "立体几何", "导数"});
            return;
        }
    }

    storyText->append("😌 幸运地没有被抽中...");
    needChangeScene = true;
    tempNextScene = 8;
    nextBtn->show();
}

// ==================== 场景8 ====================

void GameMainWindow::scene8()
{
    storyText->clear();
    storyText->append("💬 数学课结束，课间闲来无事，小猫17悄悄凑过来。");
    storyText->append("小猫17：\"给你讲个悄悄话，你肯定不知道！你要听炸裂的还是不炸裂的？\"");
    showOptions({"💥 炸裂的", "🌸 不炸裂的"});
}

// ==================== 场景9 ====================

void GameMainWindow::scene9()
{
    storyText->clear();
    storyText->append("🌅 傍晚吃饭时，天空忽然出现绝美晚霞，染红半边天空。小猫小颜指着窗外：\"我去，这么漂亮！\"");
    showOptions({"📝 拿出本子，悄悄写诗赞美晚霞", "🍚 不管风景，专心干饭最重要", "😲 忍不住大喊：我去，这么漂亮！"});
}

// ==================== 场景10 ====================

void GameMainWindow::scene10()
{
    storyText->clear();
    storyText->append("🏸 回到教室，还有40分钟上晚自习。小猫小Q跑来问：\"63！来打羽毛球吗？！\"");
    showOptions({"🏸 去打球", "📖 留教室自习"});
}

// ==================== 场景11 ====================

void GameMainWindow::scene11()
{
    storyText->clear();
    storyText->append("🌙 终于下了晚自习，脑袋都快学爆了。小猫小彤发来邀请：\"要一起走走吗？\"");
    showOptions({"🍜 拒绝，饿了", "🚶 接受，一起走"});
}

// ==================== 场景12 ====================

void GameMainWindow::scene12()
{
    storyText->clear();
    storyText->append("✨ 夜色慢慢降临，63结束了充实的一天。躺在床上，回想着今天的欢笑与小插曲，安心进入梦乡，又做了一个甜甜的好梦。");
    addAttr("sleep", 10);
    addAttr("warmth", 5);

    needChangeScene = true;
    tempNextScene = 13;
    nextBtn->setText("🎉 查看结局 🎉");
    nextBtn->show();
}

// ==================== 结局 ====================

void GameMainWindow::ending()
{
    attributeLabel->show();  // 结局页面显示属性栏

    storyText->clear();
    storyText->append("<h2 style='color:#ffd700;text-align:center;'>🎉 游戏结局 🎉</h2>");

    if(data.wisdom >= 90) {
        storyText->append("<h3>🏆 学霸金榜结局 🏆</h3><p>奖励：智慧+20｜称号：语文之星</p>");
        addAttr("wisdom", 20);
    }
    else if(data.warmth >= 120) {
        storyText->append("<h3>🌸 温柔小太阳结局 🌸</h3><p>奖励：暖心+30｜称号：校园小天使</p>");
        addAttr("warmth", 30);
    }
    else if(data.sleep >= 70 && data.warmth >= 60 && data.wisdom >= 50) {
        storyText->append("<h3>😊 安稳幸福结局 😊</h3><p>奖励：全属性+10｜称号：平凡小美好</p>");
        addAttr("wisdom", 10);
        addAttr("warmth", 10);
        addAttr("sleep", 10);
    }
    else if(data.wisdom <= 30 && data.sleep >= 60) {
        storyText->append("<h3>😴 迷糊可爱结局 😴</h3><p>奖励：睡眠+20｜称号：瞌睡小猫</p>");
        addAttr("sleep", 20);
    }
    else if(data.warmth >= 100 && data.savedPerson && data.calledPolice) {
        storyText->append("<h3>🦸 勇敢英雄结局 🦸</h3><p>奖励：暖心+100 智慧+30｜称号：正义小猫</p>");
        addAttr("warmth", 100);
        addAttr("wisdom", 30);
    }
    else if(data.pianoClosed && data.poemWritten) {
        storyText->append("<h3>🎨 艺术文艺结局 🎨</h3><p>奖励：暖心+50 智慧+15｜称号：文艺小猫</p>");
        addAttr("warmth", 50);
        addAttr("wisdom", 15);
    }
    else {
        storyText->append("<h3>🌟 普通结局 🌟</h3><p>平淡而真实的一天，期待明天会更好！</p>");
    }

    storyText->append(QString("<br>📊 智慧:%1  暖心:%2  睡眠:%3")
                          .arg(data.wisdom)
                          .arg(data.warmth)
                          .arg(data.sleep));

    nextBtn->setText("🔄 重新开始");
    nextBtn->show();
    clearOptions();
}

// ==================== 小游戏 ====================

void GameMainWindow::startBadminton()
{
    BadmintonGame* game = new BadmintonGame(this);
    game->setGeometry(150, 100, 700, 550);
    connect(game, &BadmintonGame::gameFinished, this, &GameMainWindow::onBadmintonFinished);
    game->show();
    nextBtn->hide();
    optionsWidget->hide();
    storyText->hide();
}

void GameMainWindow::startPoetry()
{
    PoetryGame* game = new PoetryGame(this);
    game->setGeometry(200, 100, 800, 500);
    connect(game, &PoetryGame::gameFinished, this, &GameMainWindow::onPoetryFinished);
    game->show();
    nextBtn->hide();
    optionsWidget->hide();
    storyText->hide();
}

void GameMainWindow::onBadmintonFinished(bool win)
{
    QWidget* gameWidget = qobject_cast<QWidget*>(sender());
    if(gameWidget) {
        gameWidget->deleteLater();
    }

    storyText->show();
    data.badmintonWin = win;

    if(win) {
        storyText->append("🎉 63赢得了羽毛球比赛！小Q竖起大拇指：\"63你太厉害了！\"");
        addAttr("warmth", 10);
    } else {
        storyText->append("😭 63输掉了比赛...小Q安慰道：\"没关系，下次再一起练习！\"");
        addAttr("warmth", 5);
    }

    needChangeScene = true;
    tempNextScene = 11;
    nextBtn->show();
}

void GameMainWindow::onPoetryFinished(int correctCount)
{
    QWidget* gameWidget = qobject_cast<QWidget*>(sender());
    if(gameWidget) {
        gameWidget->deleteLater();
    }

    storyText->show();
    data.quizCount = correctCount;

    storyText->append(QString("📊 答题结束！共答对 %1/5 题").arg(correctCount));

    if(correctCount >= 4) {
        storyText->append("🏆 太厉害了！你是古诗文小天才！额外奖励智慧+20");
        addAttr("wisdom", 20);
    } else if(correctCount >= 3) {
        storyText->append("👍 不错哦！再接再厉！额外奖励智慧+10");
        addAttr("wisdom", 10);
    }

    needChangeScene = true;
    tempNextScene = 5;
    nextBtn->show();
}

// ==================== 选项1 ====================

void GameMainWindow::onOption1()
{
    clearOptions();
    QString choice = optBtn[0]->text();

    if(data.scene == 0) {
        if(choice == "🎮 开始游戏") {
            showScene(1);
        } else {
            // 游戏介绍
            storyText->show();
            storyText->clear();
            storyText->append("🐱 欢迎来到小猫校园模拟器！\n\n");
            storyText->append("你将扮演小猫63度过完整校园日常，不同选择会触发分支剧情与属性变化。\n\n");
            storyText->append("内置古诗答题、羽毛球对战趣味小游戏，积累属性即可解锁多重专属结局。");

            // 隐藏选项框，显示继续按钮
            clearOptions();
            nextBtn->show();

            // 标记等待返回开始界面
            data.waitSecond = true;
            data.secondType = 99;
        }
        return;
    }

    if(data.scene == 1) {
        if(choice == "😴 再睡五分钟") {
            storyText->append("😴 63困意翻涌，忍不住闭上眼睛多躺一会儿……没想到一睡就过了头，只能慌慌张张套上衣服，狼狈地冲出家门！");
            addAttr("sleep", 20);
            data.firstChoice = 1;
        } else {
            storyText->append("💪 63听到妈妈的呼喊，立刻起床整理自己，精神满满，拥有了一段从容的早晨时光。");
            addAttr("wisdom", 10);
            data.firstChoice = 2;
        }
        needChangeScene = true;
        tempNextScene = 2;
        nextBtn->show();
        return;
    }

    if(data.scene == 2) {
        if(choice == "🍜 吃完妈妈做的爱心面条再出发") {
            if(data.firstChoice == 1) {
                storyText->append("🍜 63还是决定把面吃完，面条已经微凉结块，可依旧是熟悉的温暖味道。");
                addAttr("warmth", 10);
            } else {
                storyText->append("🍜 63大口吃完这碗面，热气在心底散开，每一口都格外香甜。");
                addAttr("warmth", 12);
            }
        } else {
            setBG(":/images/shaomai.jpg");
            storyText->append("🥟 63出门买了烧麦和酸奶，准备带到教室慢慢吃。心里默默祈祷：希望今天不要因为喝酸奶拉肚子呀……");
            if(randChance(50)) {
                data.hasDiarrehea = true;
                storyText->append("<span style='color:#ff6666;'>⚠️ 不幸的是，63后来真的拉肚子了...</span>");
            }
        }
        needChangeScene = true;
        tempNextScene = 3;
        nextBtn->show();
        return;
    }

    if(data.scene == 4) {
        if(choice == "🎯 开始答题挑战") startPoetry();
        else {
            storyText->append("🙈 63假装肚子疼逃过了问答...但总觉得有点心虚...");
            addAttr("wisdom", -5);
            needChangeScene = true;
            tempNextScene = 5;
            nextBtn->show();
        }
        return;
    }

    if(data.scene == 5) {
        if(choice == "🍜 来一碗热气腾腾的羊肉粉") {
            setBG(":/images/lamb_noodles.jpg");
            storyText->append("🍜 汤鲜味美，一口下去浑身都暖了，疲惫也消散不少。等待时间短，拥有充足午睡时间。");
            addAttr("warmth", 5);
            addAttr("sleep", 10);
        } else {
            setBG(":/images/stir_fry.jpg");
            storyText->append("🍳 饭菜香气浓郁，非常下饭，吃得格外满足。但排队近四十分钟，睡眠时间大大减少。");
            addAttr("warmth", 10);
            addAttr("sleep", -10);
        }
        needChangeScene = true;
        tempNextScene = 6;
        nextBtn->show();
        return;
    }

    if(data.scene == 6) {
        if(choice == "😴 好好午睡一下吧") {
            storyText->clear();
            storyText->append("😴 午餐过后，63趴在桌上安静午睡。这一觉睡得格外安稳，还做了个甜甜的好梦，醒来时整个人都轻松舒服。");
            addAttr("sleep", 20);
            needChangeScene = true;
            tempNextScene = 7;
            nextBtn->show();
            clearOptions();
        } else if(choice == "🚶 不然去逛一下吧") {
            clearOptions();
            storyText->clear();
            storyText->append("🏃 63决定到处逛逛...去哪里好呢？");
            showOptions({"📚图书馆", "🎹 钢琴房"});
            data.waitSecond = true;
            data.secondType = 1;
        }
        return;
    }

    // 第七幕：数学题
    if(data.scene == 7 && (choice == "圆锥曲线" || choice == "立体几何" || choice == "导数")) {
        int gain = 0;

        if(randChance(60)) {
            gain = 30;
            storyText->append("✅ 回答正确！老师赞赏地点点头。");
        } else {
            gain = -10;
            storyText->append("❌ 回答错误...下次要更加努力！");
        }

        addAttr("wisdom", gain);
        needChangeScene = true;
        tempNextScene = 8;
        nextBtn->show();
        return;
    }

    if(data.scene == 8) {
        if(choice == "💥 炸裂的") {
            if(randChance(99)) {
                storyText->append("💥 小猫17说：\"你知道吗？教导主任其实私下在练习街舞！\"63瞪大眼睛：\"什么？！那个严肃的教导主任？！\"两人捂嘴笑成一团...");
                addAttr("warmth", 10);
                addAttr("wisdom", -5);
            } else {
                storyText->append("🎉 小猫17说：\"校长居然是我们学校的校友，当年还是校草呢！\"63：\"天哪！\"");
                addAttr("warmth", 30);
            }
        } else {
            storyText->append("🌸 小猫17说：\"你知道吗？其实小颜每天早上都会去喂校园里的流浪猫。\"63：\"真的吗？她好善良啊！\"63心里暖暖的。");
            addAttr("warmth", 20);
            addAttr("wisdom", -5);
        }
        needChangeScene = true;
        tempNextScene = 9;
        nextBtn->show();
        return;
    }

    if(data.scene == 9) {
        if(choice == "📝 拿出本子，悄悄写诗赞美晚霞") {
            storyText->append("📝 63拿出本子，写下动人的诗句赞美晚霞。");
            addAttr("wisdom", 10);
            data.poemWritten = true;
        } else if(choice == "🍚 不管风景，专心干饭最重要") {
            storyText->append("🍚 63专心吃饭，把晚霞抛在脑后。");
            addAttr("warmth", 10);
        } else {
            storyText->append("😲 63忍不住大喊：\"我去，这么漂亮！\"");
            addAttr("wisdom", -1);
            addAttr("warmth", 15);
        }
        needChangeScene = true;
        tempNextScene = 10;
        nextBtn->show();
        return;
    }

    if(data.scene == 10) {
        if(choice == "🏸 去打球") startBadminton();
        else {
            storyText->append("📖 63留在教室认真学习。");
            addAttr("wisdom", 5);
            needChangeScene = true;
            tempNextScene = 11;
            nextBtn->show();
        }
        return;
    }

    // 第十一幕：晚自习后
    if(data.scene == 11) {
        if(choice == "🍜 拒绝，饿了") {
            setBG(":/images/night_market.jpg");
            storyText->clear();
            storyText->append("🍢 校门口香气扑鼻，小猫小Q靠在墙边：\"63，一起吃点东西再回家？\"");
            showOptions({"🍢 关东煮", "🧈 铁板豆腐"});
            data.waitSecond = true;
            data.secondType = 5;
        } else {
            // 接受小彤邀请，触发救人事件
            setBG(":/images/night_market.jpg");
            storyText->clear();
            storyText->append("🌙 63和小彤一起走在校园小路上...突然看到一辆黑车正在强拉低年级小猫！情况紧急！63该怎么办？");
            showOptions({"💪 上前帮忙", "👀 原地观望", "📞 立刻报警"});
            data.waitSecond = true;
            data.secondType = 4;  // 救人选项，完成后进入第十二幕
        }
        return;
    }
}

// ==================== 选项2 ====================

void GameMainWindow::onOption2()
{
    if(data.waitSecond) {
        QString choice = optBtn[1]->text();

        if(data.secondType == 1) {

            clearOptions();

            if(choice == "📚 图书馆") {
                setBG(":/images/library.jpg");
                storyText->clear();
                storyText->append("📚 63决定去图书馆...图书馆有沙发，看书看累了可以在上面休息。63拿了一本《猫类简史》，兴致勃勃地看了起来。这时候，图书馆书架处好像有猫出现了摩擦声。");
                showOptions({"🔍 过去看看到底发生了什么事情", "📖 不关我的事，一会就好了"});
                data.waitSecond = true;
                data.secondType = 2;
            }
            else if(choice == "🎹 钢琴房") {
                setBG(":/images/piano_room.jpg");
                storyText->clear();
                storyText->append("🎹 63决定去钢琴房...这时候钢琴房正有悠扬的琴声传来。63想前去一探究竟。身穿校服的学长正坐在钢琴前，优美的琴音从他指尖流出，多么美好的画面呀！");
                showOptions({"💬 上前温柔提醒", "🚪 默默将门带上"});
                data.waitSecond = true;
                data.secondType = 3;
            }
            return;
        }

        if(data.secondType == 2) {
            if(choice == "🔍 过去看看发生了什么事") {
                storyText->clear();
                storyText->append("🔍 63走过去查看情况...原来是一个学妹在找书的时候不小心把一个小书架碰倒了。63沉默地上前去帮助它把书架扶起来。\"你没事吧，没被吓到吧。\" 63压低声音轻声询问。学妹轻轻摇头，脸在尴尬和羞愧中已经涨红了。");
                addAttr("warmth", 50);
                addAttr("wisdom", 5);
            } else {
                storyText->clear();
                storyText->append("📚 63继续津津有味地看书。不知不觉时间已经过去，午休结束的铃声响起。下午是数学课，又要昏睡过去了！不过看了喜欢的书，感觉收获了很多。");
                addAttr("wisdom", 15);
            }
            data.waitSecond = false;
            needChangeScene = true;
            tempNextScene = 7;
            nextBtn->show();
            clearOptions();
            return;
        }

        if(data.secondType == 3) {
            if(choice == "💬 上前温柔提醒") {
                storyText->clear();
                storyText->append("🎵 63：\"同学你好，现在是午休时间。\"学长不好意思地笑了笑：\"抱歉，是我考虑不周全，谢谢你同学。\"");
                addAttr("warmth", 15);
                addAttr("wisdom", 5);
            } else {
                storyText->clear();
                storyText->append("🎹 63默默将门带上，阻断声音，暗暗祈祷隔音效果够好。好在音乐教室的门隔音很好，并没有同学因此被打扰。学长走近63，真诚地道谢。");
                addAttr("warmth", 80);
                data.pianoClosed = true;
            }
            data.waitSecond = false;
            needChangeScene = true;
            tempNextScene = 7;
            nextBtn->show();
            clearOptions();
            return;
        }

        if(data.secondType == 4) {
            if(choice == "💪 上前帮忙") {
                storyText->clear();
                storyText->append("💪 63勇敢地上前制止了坏人，救下了学妹！学妹感激地说：\"谢谢你！你真是个大英雄！\"");
                addAttr("warmth", 60);
                addAttr("wisdom", 10);
                data.savedPerson = true;
            } else if(choice == "👀 原地观望") {
                storyText->clear();
                storyText->append("😰 63害怕地站在原地不敢动...幸好后来有人报警了，学妹最终得救。但63很后悔没有挺身而出。");
                addAttr("warmth", -10);
                addAttr("wisdom", 10);
            } else if(choice == "📞 立刻报警") {
                storyText->clear();
                storyText->append("📞 63机智地立刻报警，警察迅速赶到抓住了坏人！学妹和家人都非常感谢63！学校也表彰了63的机智勇敢！");
                addAttr("wisdom", 20);
                addAttr("warmth", 70);
                data.calledPolice = true;
                data.savedPerson = true;
            }
            data.waitSecond = false;
            needChangeScene = true;
            tempNextScene = 12;
            nextBtn->show();
            clearOptions();
            return;
        }

        if(data.secondType == 5) {
            if(choice == "🍢 关东煮") {
                setBG(":/images/guandongzhu.jpg");
                storyText->clear();
                storyText->append("🍢 热腾腾的关东煮温暖了63的心！和小Q边吃边聊，开心极了！");
                addAttr("warmth", 10);
            } else {
                setBG(":/images/tiebodoufu.jpg");
                storyText->clear();
                storyText->append("🧈 香喷喷的铁板豆腐让63赞不绝口！两人分享美食，友谊更深了！");
                addAttr("warmth", 8);
            }
            data.waitSecond = false;
            needChangeScene = true;
            tempNextScene = 12;
            nextBtn->show();
            clearOptions();
            return;
        }
        return;
    }

    QPushButton* tmp = optBtn[0];
    optBtn[0] = optBtn[1];
    onOption1();
    optBtn[0] = tmp;
}

// ==================== 选项3 ====================

void GameMainWindow::onOption3()
{
    if(data.waitSecond) {
        onOption2();
        return;
    }

    QPushButton* tmp = optBtn[0];
    optBtn[0] = optBtn[2];
    onOption1();
    optBtn[0] = tmp;
}

// ==================== 下一步 ====================

void GameMainWindow::onNext()
{
    // 如果点击的是"重新开始"按钮，重置游戏
    if(nextBtn->text() == "🔄 重新开始") {
        resetGame();
        return;
    }

    // 如果是在游戏介绍页面，点击继续后返回开始界面
    if(data.secondType == 99 && data.waitSecond) {
        data.waitSecond = false;
        data.secondType = 0;
        storyText->hide();
        showScene(0);
        return;
    }

    // 正常场景切换
    if(needChangeScene && tempNextScene != -1) {
        showScene(tempNextScene);
        needChangeScene = false;
        tempNextScene = -1;
    } else if(data.scene == 12) {
        showScene(13);
    } else if(data.scene < 12) {
        showScene(data.scene + 1);
    }

    if(nextBtn->text() != "🔄 重新开始") {
        nextBtn->setText("✨ 点击继续 ✨");
    }
}