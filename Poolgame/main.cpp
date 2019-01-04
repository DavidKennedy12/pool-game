#include "dialog.h"
#include "game.h"
#include "utils.h"
#include "gamebuilder.h"
#include "stagetwobuilder.h"
#include <QApplication>
#include <QFile>
#include <iostream>
#include <QString>
#include <QJsonObject>
#include <ctime>
#include <QJsonDocument>
#include "strategyone.h"
#include "strategytwo.h"

QJsonObject loadConfig() {
    // load json from config file
    QFile conf_file(config_path);
    conf_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = conf_file.readAll();
    conf_file.close();
    QJsonObject config = QJsonDocument::fromJson(content.toUtf8()).object();
    return config;
}

int main(int argc, char *argv[])
{
    QJsonObject conf = loadConfig();
    // create our game based on our config
    GameDirector director(&conf);
    // use builder2 if we're stage two (defaults to false), otherwise no
    if (conf.value("stage2").toBool(false) == true  ||  conf.value("stage3").toBool(false) == true) {
       director.setBuilder(new StageTwoBuilder());
    } else {
        // set and transfer ownership of this builder to the director
        director.setBuilder(new StageOneBuilder());
    }

    Game* game = director.createGame();

    // display the dialog that contains the game and run
    QApplication a(argc, argv);
    Dialog w(game, nullptr);

    if (conf.value("stage3").toBool(false) == true)
    {
        Opponent* opp = new Opponent(game->clone(), new StrategyTwo);
        w.setOpponent(opp);
    }
    else
    {
        Opponent* opp = new Opponent(nullptr, new StrategyOne);
        w.setOpponent(opp);
    }
    w.show();

    return a.exec();
}
