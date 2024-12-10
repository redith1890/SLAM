#include<raylib.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include"array.h"

typedef struct Wall{
    Vector2 position;
    Vector2 size;
    Color color;
}Wall;

typedef struct Laser{
    Vector2 position;
    Vector2 velocity;
}Laser;

typedef struct Robot{
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
}Robot;

typedef Vector2 LaserPoint;

Wall wallCreate(Vector2 position, Vector2 size, Color color){
    Wall wall;
    wall.position = position;
    wall.size = size;
    wall.color = color;
    return wall;
}
Robot robotCreate(Vector2 position, Vector2 size, Vector2 velocity){
    Robot robot;
    robot.position = position;
    robot.size = size;
    robot.velocity = velocity;
    return robot;
}

void robotUpdate(Robot* robot){
    robot->position.x += robot->velocity.x;
    robot->position.y += robot->velocity.y;

    robot->velocity.x = 0;
    robot->velocity.y = 0;

}

void robotCollide(Robot* robot, Wall *wall) {
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


void laserUpdate(Laser* laser){
    laser->position.x += laser->velocity.x;
    laser->position.y += laser->velocity.y;
}

Laser laserCreate(Robot* robot){
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

bool laserCollide(Laser* laser, Wall* wall, Array* point_arr){
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
        LaserPoint point = {laser->position.x, laser->position.y};
        addElement(point_arr, &point);
        return true;
    }
    return false;
}



int main(){
    const int screenWidth = 1920;
    const int screenHeight = 1040; // 1080 - i3 borders
    InitWindow(screenWidth, screenHeight, "Slam simulation");
    SetTargetFPS(60);
    bool paused = false;

    Wall walls[5];
    walls[0] = wallCreate((Vector2){100, 800}, (Vector2){1650, 100}, DARKGRAY);    
    walls[1] = wallCreate((Vector2){1650, 100}, (Vector2){100, 800}, DARKGRAY);
    walls[2] = wallCreate((Vector2){100, 100}, (Vector2){1650, 100}, DARKGRAY);
    walls[3] = wallCreate((Vector2){100, 100}, (Vector2){100, 600}, DARKGRAY);
    walls[4] = wallCreate((Vector2){400, 400}, (Vector2){100, 100}, DARKGRAY);
    Robot robot = robotCreate((Vector2){100, 700}, (Vector2){100, 100}, (Vector2){10, 0});

    Array lasers_arr;
    initArray(&lasers_arr, 10, sizeof(Laser));
    Array point_arr;
    initArray(&point_arr, 10, sizeof(LaserPoint));
    
    for (int i = 0; i < 2; i++){
        Laser laser = laserCreate(&robot);
        addElement(&lasers_arr, &laser);
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
            
            for (size_t i = 0; i < sizeof(walls)/sizeof(walls[0]); i++) {
                Laser* lasers = (Laser *)lasers_arr.data;
                DrawRectangle(walls[i].position.x, walls[i].position.y, walls[i].size.x, walls[i].size.y, walls[i].color);
                robotCollide(&robot, &walls[i]);
                for (size_t j = 0; j < lasers_arr.size; j++){
                    if(laserCollide(&lasers[j], &walls[i], &point_arr)) {
                        removeElement(&lasers_arr, j);
                        j--;
                    }
                }
            }

            DrawRectangle(robot.position.x, robot.position.y, robot.size.x, robot.size.y, BLUE);
            if(!paused) robotUpdate(&robot);
            
            // Change LaserPoints
            for(size_t i = 0; i < lasers_arr.size; i++){
            Laser* lasers = (Laser *)lasers_arr.data;
                
                if(lasers[i].position.x < 0 || lasers[i].position.x > screenWidth || lasers[i].position.y < 0 || lasers[i].position.y > screenHeight){
                    removeElement(&lasers_arr, i);
                    i--;
                    continue;                   
                }
                // if(lasers[i].velocity.x == 0 && lasers[i].velocity.y == 0){
                //     DrawCircle(lasers[i].position.x, lasers[i].position.y, 2, RED);

                //     for (size_t j = 0; j < lasers_arr.size; j++)
                //     {   
                //         if(i != j){
                //             float diff_x = lasers[i].position.x - lasers[j].position.x;
                //             float diff_y = lasers[i].position.y - lasers[j].position.y;
                //             if(abs(diff_x) < 1.5 && abs(diff_y) < 1.5) removeElement(&lasers_arr, i);
                //         }
                //     }
                    
                // }
                
                // else{
                DrawLineBezier((Vector2){lasers[i].position.x, lasers[i].position.y}, (Vector2){lasers[i].position.x - lasers[i].velocity.x * 2, lasers[i].position.y - lasers[i].velocity.y * 2}, 2, RED);
                // }
                if(!paused) laserUpdate(&lasers[i]);
            }
            LaserPoint* points = (LaserPoint *)point_arr.data;
            for (size_t i = 0; i < point_arr.size; i++){
                DrawCircle(points[i].x, points[i].y, 2, RED);
                for (size_t j = i + 1; j < point_arr.size; j++) {
                    float diff_x = points[i].x - points[j].x;
                    float diff_y = points[i].y - points[j].y;
                    if(abs(diff_x) < 1.25 && abs(diff_y) < 1.25) {
                        removeElement(&point_arr, j);
                        j--;
                    }
                }
                
            }
            


            // Temporal controls
            
            if (IsKeyDown(KEY_RIGHT)) robot.velocity.x = 10;
            if (IsKeyDown(KEY_LEFT)) robot.velocity.x = -10;
            if (IsKeyDown(KEY_UP)) robot.velocity.y = -10;
            if (IsKeyDown(KEY_DOWN)) robot.velocity.y = 10;
            if (IsKeyDown(KEY_SPACE)) {
                for (size_t i = 0; i < 20; i++){                
                    Laser laser = laserCreate(&robot);
                    addElement(&lasers_arr, &laser);
                }
            
            }
            if (IsKeyPressed(KEY_Q)) SetTargetFPS(0);
            if (IsKeyPressed(KEY_E)) SetTargetFPS(60);
            if (IsKeyPressed(KEY_ENTER)) if(paused) paused = false; else paused = true;


        EndDrawing();
    }
    CloseWindow();

    freeArray(&lasers_arr);



    return 0;
}
