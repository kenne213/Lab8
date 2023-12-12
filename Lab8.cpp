// Lab8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const double CB_SPEED = 0.2;

void LoadTexture(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f getTextSize(Text text) {
    FloatRect a = text.getGlobalBounds();
    return Vector2f(a.width, a.height);
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunter Simulation Game");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);
    int fontSize(50);

    PhysicsSprite& crossbow = *new PhysicsSprite();
    Texture crossbowTexture;
    LoadTexture(crossbowTexture, "Images/crossbow.png");
    crossbow.setTexture(crossbowTexture);
    Vector2f sz = crossbow.getSize();
    crossbow.setCenter(Vector2f(400, 600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTexture;
    LoadTexture(arrowTexture, "Images/arrow.png");
    arrow.setTexture(arrowTexture);
    bool drawingArrow(false);

    PhysicsRectangle ceiling;
    ceiling.setSize(Vector2f(800, 10));
    ceiling.setCenter(Vector2f(400, 5));
    ceiling.setStatic(true);
    world.AddPhysicsBody(ceiling);

    Texture duckTexture;
    LoadTexture(duckTexture, "Images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;

    ceiling.onCollision = [&drawingArrow, &world, &arrow](PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
    };

    Font font;
    if (!font.loadFromFile("Font/AARDV.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    Clock newDuck;
    Clock newDuckClock;
    Time lastNewDuckTime = newDuckClock.getElapsedTime();
    const Time newDuckInterval = seconds(0.5);

    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();

        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            //MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) && !drawingArrow) {
                drawingArrow = true;
                arrows = arrows - 1;
                arrow.setCenter(crossbow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
            }

            Time currentNewDuckTime = newDuckClock.getElapsedTime();
            if (currentNewDuckTime - lastNewDuckTime >= newDuckInterval) {
                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(duckTexture);
                Vector2f size = duck.getSize();
                duck.setCenter(Vector2f(50, 20 + (size.y / 2)));
                duck.setVelocity(Vector2f(0.25, 0));
                world.AddPhysicsBody(duck);

                lastNewDuckTime = currentNewDuckTime;
                if (duck.getCenter().x > window.getSize().x) {
                    world.RemovePhysicsBody(duck);
                    ducks.QueueRemove(duck);
                }

                duck.onCollision = [&drawingArrow, &world, &arrow, &duck, &ducks, &score](PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                };
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            ducks.DoRemovals();
            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&)duck);
            }
            window.draw(crossbow);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(font);
            scoreText.setPosition(Vector2f(790 - getTextSize(scoreText).x, 550));
            window.draw(scoreText);

            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(font);
            arrowCountText.setPosition(Vector2f(10, 550));
            window.draw(arrowCountText);

            window.display();
        }
    }

    window.display();
   
    bool exitSimulation(false);
    PhysicsRectangle backboard;
    backboard.setSize(Vector2f(800, 600));
    backboard.setCenter(Vector2f(400, 300));
    backboard.setFillColor(Color(240, 240, 240));
    backboard.setStatic(true);
    
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER!");
    gameOverText.setCharacterSize(fontSize);
    gameOverText.setFillColor(Color(255, 0, 0));
    FloatRect gameOverTextSize = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(400 - (gameOverTextSize.width / 2), 200 - (gameOverTextSize.height)));

    Text finalScoreText;
    finalScoreText.setFont(font);
    finalScoreText.setString("You scored: " + to_string(score) + " points!");
    finalScoreText.setCharacterSize(fontSize);
    finalScoreText.setFillColor(Color(0, 0, 0));
    FloatRect finalSize = finalScoreText.getGlobalBounds();
    finalScoreText.setPosition(Vector2f(400 - (finalSize.width / 2), 300 - (finalSize.height)));

    Text leaveText;
    leaveText.setFont(font);
    leaveText.setString("Press the SPACE key to exit.");
    leaveText.setCharacterSize(fontSize* 0.75);
    leaveText.setFillColor(Color(0, 0, 0));
    FloatRect lSz = leaveText.getGlobalBounds();
    leaveText.setPosition(Vector2f(400 - (lSz.width / 2), 400 - (lSz.height)));

    while (!exitSimulation) {
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            exitSimulation = true;
        }
        window.clear();
        window.draw(backboard);
        window.draw(gameOverText);
        window.draw(finalScoreText);
        window.draw(leaveText);
        window.display();
    }
}
