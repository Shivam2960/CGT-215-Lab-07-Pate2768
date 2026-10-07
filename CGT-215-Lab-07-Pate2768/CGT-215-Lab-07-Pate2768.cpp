#include <iostream>           // Lets us print messages to the console.
#include <SFML/Graphics.hpp>  // Provides the window, colors, drawing, and timing.
#include <SFPhysics.h>        // Provides physics objects and collision handling.

using namespace std;
using namespace sf;
using namespace sfp;

int main()
{
    // Create an 800-by-600 window with "Bounce" in its title bar.
    RenderWindow window(VideoMode(800, 600), "Bounce");

    // Limit drawing to approximately 60 frames per second.
    window.setFramerateLimit(60);

    // Create the physics world.
    // Positive Y points downward, so this gravity pulls the ball down.
    World world(Vector2f(0, 1));

    // Create the ball and place it above the center target.
    PhysicsCircle ball;
    ball.setRadius(20);
    ball.setCenter(Vector2f(400, 100));

    // Give the ball an initial downward push.
    // The X value is zero, so it starts without horizontal movement.
    ball.applyImpulse(Vector2f(0, 0.5f));

    // Register the ball so the world can move it and detect its collisions.
    world.AddPhysicsBody(ball);

    // Bottom wall: 760 pixels wide and 20 pixels tall.
    // Its placement leaves a 20-pixel gap from the bottom window edge.
    PhysicsRectangle floor;
    floor.setSize(Vector2f(760, 20));
    floor.setCenter(Vector2f(400, 570));
    floor.setFillColor(Color::Cyan);
    floor.setStatic(true);
    world.AddPhysicsBody(floor);

    // Top wall: the same size as the floor, near the top of the window.
    PhysicsRectangle ceiling;
    ceiling.setSize(Vector2f(760, 20));
    ceiling.setCenter(Vector2f(400, 30));
    ceiling.setFillColor(Color::Cyan);
    ceiling.setStatic(true);
    world.AddPhysicsBody(ceiling);

    // Left wall: tall and narrow, fitting between the top and bottom walls.
    PhysicsRectangle leftWall;
    leftWall.setSize(Vector2f(20, 520));
    leftWall.setCenter(Vector2f(30, 300));
    leftWall.setFillColor(Color::Cyan);
    leftWall.setStatic(true);
    world.AddPhysicsBody(leftWall);

    // Right wall: matches the left wall on the opposite side.
    PhysicsRectangle rightWall;
    rightWall.setSize(Vector2f(20, 520));
    rightWall.setCenter(Vector2f(770, 300));
    rightWall.setFillColor(Color::Cyan);
    rightWall.setStatic(true);
    world.AddPhysicsBody(rightWall);

    // Center target: stays in its original position and keeps its original size.
    PhysicsRectangle middle;
    middle.setSize(Vector2f(200, 40));
    middle.setCenter(Vector2f(400, 300));
    middle.setStatic(true);
    world.AddPhysicsBody(middle);

    // Keep separate totals for outer-wall hits and center-target hits.
    int thudCount = 0;
    int bang = 0;

    // End the program after the center target has been hit three times.
    const int hitLimit = 3;

    // Define a lambda that the outer walls will use when they collide.
    // Capturing thudCount by reference lets the lambda update the original counter.
    auto wallCollision = [&thudCount](PhysicsBodyCollisionResult) {
        ++thudCount;
        cout << "thud " << thudCount << endl;
        };

    // Give each outer wall the same collision behavior.
    // These assignments register the callback; they do not run it immediately.
    floor.onCollision = wallCollision;
    ceiling.onCollision = wallCollision;
    leftWall.onCollision = wallCollision;
    rightWall.onCollision = wallCollision;

    // Give the center target its own collision lambda.
    // bang and window are captured by reference.
    // hitLimit is captured by value, so the lambda keeps a copy of its value.
    middle.onCollision =
        [&bang, &window, hitLimit](PhysicsBodyCollisionResult) {
        ++bang;
        cout << "bang " << bang << endl;

        // Closing the window causes the main loop to finish.
        if (bang >= hitLimit) {
            window.close();
        }
        };

    // Start a clock and remember the time of the previous physics update.
    Clock clock;
    Time lastTime = clock.getElapsedTime();

    // Repeat while the window is open.
    while (window.isOpen()) {
        // Process pending window events, such as clicking the close button.
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        // Stop this loop immediately if an event closed the window.
        if (!window.isOpen()) {
            break;
        }

        // Find how much time has passed since the previous physics update.
        Time currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        int deltaTimeMS = deltaTime.asMilliseconds();

        // Update only when at least one whole millisecond has passed.
        if (deltaTimeMS > 0) {
            world.UpdatePhysics(deltaTimeMS);
            lastTime = currentTime;
        }

        // The center-target callback might have closed the window
        // during the physics update, so check again before drawing.
        if (!window.isOpen()) {
            break;
        }

        // Erase the previous frame with a black background.
        window.clear(Color::Black);

        // Draw every object in its current position.
        window.draw(ball);
        window.draw(floor);
        window.draw(ceiling);
        window.draw(leftWall);
        window.draw(rightWall);
        window.draw(middle);

        // Show the completed frame.
        window.display();
    }

    // Report that the program finished successfully.
    return 0;
}