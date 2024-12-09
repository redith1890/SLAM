#include<raylib.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include"array.h"

typedef struct Wall
{
    Vector2 position;
    Vector2 size;
}Wall;


Wall wallCreate(Vector2 position, Vector2 size){
    Wall wall;
    wall.position = position;
    wall.size = size;
    return wall;
}

typedef struct Robot
{
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
}Robot;

Robot robotCreate(Vector2 position, Vector2 size, Vector2 velocity){
    Robot robot;
    robot.position = position;
    robot.size = size;
    robot.velocity = velocity;
    return robot;
}

void robotUpdate(Robot *robot){
    robot->position.x += robot->velocity.x;
    robot->position.y += robot->velocity.y;

    robot->velocity.x = 0;
    robot->velocity.y = 0;

}

void robotCollide(Robot *robot, Wall *wall) {
    bool collisionX = robot->position.x + robot->size.x > wall->position.x &&
                      wall->position.x + wall->size.x > robot->position.x;
    
    bool collisionY = robot->position.y + robot->size.y > wall->position.y &&
                      wall->position.y + wall->size.y > robot->position.y;

    if (collisionX && collisionY) {
        float penetrationX = fmin(
            (robot->position.x + robot->size.x) - wall->position.x,
            (wall->position.x + wall->size.x) - robot->position.x
        );
        
        float penetrationY = fmin(
            (robot->position.y + robot->size.y) - wall->position.y,
            (wall->position.y + wall->size.y) - robot->position.y
        );

        if (penetrationX < penetrationY) {
            if (robot->position.x < wall->position.x) {
                robot->position.x = wall->position.x - robot->size.x;
            } else {
                robot->position.x = wall->position.x + wall->size.x;
            }
        } else {
            if (robot->position.y < wall->position.y) {
                robot->position.y = wall->position.y - robot->size.y;
            } else {
                robot->position.y = wall->position.y + wall->size.y;
            }
        }
    }
}

typedef struct Laser
{
    Vector2 position;
    Vector2 velocity;
}Laser;

void laserUpdate(Laser *laser){
    laser->position.x += laser->velocity.x;
    laser->position.y += laser->velocity.y;
}

Laser laserCreate(Robot *robot){
    Laser laser;
    Vector2 robot_center = {robot->position.x + robot->size.x/2, robot->position.y + robot->size.y/2}; 
    laser.position = robot_center;
    float a = rand() % 21 - 10; 
    float b = rand() % 21 - 10;
    if(abs(a)>abs(b)){
        if(a<0) a = -10;
        else a = 10;
    }
    else{
        if(b<0) b = -10;
        else b = 10;
    }

    Vector2 velocity = {a, b};
    laser.velocity = velocity;
    return laser;
}
void laserCollide(Laser *laser, Wall *wall){
    bool collisionX = laser->position.x > wall->position.x &&
                      wall->position.x + wall->size.x > laser->position.x;
    
    bool collisionY = laser->position.y > wall->position.y &&
                      wall->position.y + wall->size.y > laser->position.y;
    if(collisionX && collisionY) {
        float penetrationX = fmin(
            (laser->position.x) - wall->position.x,
            (wall->position.x + wall->size.x) - laser->position.x
        );
        
        float penetrationY = fmin(
            (laser->position.y) - wall->position.y,
            (wall->position.y + wall->size.y) - laser->position.y
        );

        if (penetrationX < penetrationY) {
            if (laser->position.x <= wall->position.x + penetrationX) {
                laser->position.x = wall->position.x;
            } else {
                laser->position.x = wall->position.x + wall->size.x;
            }
        } else {
            if (laser->position.y <= wall->position.y + penetrationY) {
                laser->position.y = wall->position.y;
            } else {
                laser->position.y = wall->position.y + wall->size.y;
            }
        }
        laser->velocity.x = 0;
        laser->velocity.y = 0;
    }
}
int main(){
    const int screenWidth = 1920;
    const int screenHeight = 1040; // 1080 - i3 borders
    InitWindow(screenWidth, screenHeight, "Slam simulation");
    SetTargetFPS(60);
    bool paused = false;


    Wall walls[4];
    walls[0] = wallCreate(Vector2{x: 100, y: 800}, Vector2{x: 1650, y: 100});    
    walls[1] = wallCreate(Vector2{x: 1650, y: 100}, Vector2{x: 100, y: 800});
    walls[2] = wallCreate(Vector2{x: 100, y: 100}, Vector2{x: 1650, y: 100});
    walls[3] = wallCreate(Vector2{x: 100, y: 100}, Vector2{x: 100, y: 600});
    Robot robot = robotCreate(Vector2{x: 100, y: 700}, Vector2{x: 100, y: 100}, Vector2{x: 10, y: 0});

    Array lasersarr;
    initArray(&lasersarr, 10, sizeof(Laser));

    for (int i = 0; i < 2; i++){
        Laser laser = laserCreate(&robot);
        addElement(&lasersarr, &laser);
    }
    while (!WindowShouldClose()){
        BeginDrawing();
            ClearBackground(BEIGE);
            
            // Print FPS
            
            int fps = GetFPS();
            char fpstring[3];
            sprintf(fpstring,"%d", fps);
            DrawText(fpstring, 10, 10, 20, RED);
            
            // Game Loop
            
            for (Wall wall : walls)
            {
                DrawRectangle(wall.position.x, wall.position.y, wall.size.x, wall.size.y, DARKGRAY);
                robotCollide(&robot, &wall);
                Laser *lasers = (Laser *)lasersarr.data;
                for (size_t j = 0; j < lasersarr.size; j++){
                    laserCollide(&lasers[j], &wall);
                }
            }
            DrawRectangle(robot.position.x, robot.position.y, robot.size.x, robot.size.y, BLUE);
            if(!paused) robotUpdate(&robot);
            
            for(size_t i = 0; i < lasersarr.size; i++){
                Laser* lasers = (Laser *)lasersarr.data;
                if(lasers[i].position.x < 0 || lasers[i].position.x > screenWidth || lasers[i].position.y < 0 || lasers[i].position.y > screenHeight){
                    removeElement(&lasersarr, i);
                    i--;
                    continue;                   
                }
                if(lasers[i].velocity.x == 0 && lasers[i].velocity.y == 0){
                    DrawCircle(lasers[i].position.x, lasers[i].position.y, 2, RED);

                    for (size_t j = 0; j < lasersarr.size; j++)
                    {   
                        if(i != j){
                            float diff_x = lasers[i].position.x - lasers[j].position.x;
                            float diff_y = lasers[i].position.y - lasers[j].position.y;
                            if(abs(diff_x) < 1.5 && abs(diff_y) < 1.5) removeElement(&lasersarr, i);
                        }
                    }
                    
                }
                else{
                    DrawLineBezier(Vector2{x: lasers[i].position.x,  y:lasers[i].position.y}, Vector2{x: lasers[i].position.x - lasers[i].velocity.x *2, y:lasers[i].position.y - lasers[i].velocity.y *2}, 2, RED);
                }
                if(!paused) laserUpdate(&lasers[i]);
            }
            
            // Temporal controls
            
            if (IsKeyDown(KEY_RIGHT)) robot.velocity.x = 10;
            if (IsKeyDown(KEY_LEFT)) robot.velocity.x = -10;
            if (IsKeyDown(KEY_UP)) robot.velocity.y = -10;
            if (IsKeyDown(KEY_DOWN)) robot.velocity.y = 10;
            if (IsKeyDown(KEY_SPACE)) {
                for (size_t i = 0; i < 20; i++)
                {                
                Laser laser = laserCreate(&robot);
                addElement(&lasersarr, &laser);
                }
            
            }
            if (IsKeyPressed(KEY_Q)) SetTargetFPS(0);
            if (IsKeyPressed(KEY_E)) SetTargetFPS(60);
            if (IsKeyPressed(KEY_ENTER)) if(paused) paused = false; else paused = true;


        EndDrawing();
    }
    CloseWindow();

    freeArray(&lasersarr);



    return 0;
}
