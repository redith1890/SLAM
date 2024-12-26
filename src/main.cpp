#include<raylib.h>
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<cstdint>

#include"array.h"


#define N_LASER_PER_FRAME 10
#define POINT_THRESHOLD_DISTANCE 20.0

// ====================================================================================================================================
// TYPES
// ====================================================================================================================================


typedef struct
{
    Vector2 position;
    Vector2 size;
}Wall;

typedef struct
{
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
}Robot;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
}Laser;

typedef Vector2 LaserPoint;

typedef enum : int8_t
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE = -1
}Direction;


// ===================================================================================================================================
// PROTOTYPES
// ===================================================================================================================================


Wall wallCreate(Vector2 position, Vector2 size);
Robot robotCreate(Vector2 position, Vector2 size, Vector2 velocity);
void robotUpdate(Robot *robot);
void robotCollide(Robot *robot, const Wall *wall);
void laserUpdate(Laser *laser);
Laser laserCreate(const Robot *robot, Vector2 velocity);
void radiansToVelocities(Vector2 velocities[], unsigned int divisions);
bool pointExists(const Array* pointarr, const LaserPoint* laserpoint);
bool laserCollide(Laser* laser, const Wall* wall, Array* pointarr);


// ===================================================================================================================================
// IMPL
// ===================================================================================================================================


Wall
wallCreate(Vector2 position, Vector2 size){
    Wall wall;
    wall.position = position;
    wall.size = size;
    return wall;
}

Robot
robotCreate(Vector2 position, Vector2 size, Vector2 velocity){
    Robot robot;
    robot.position = position;
    robot.size = size;
    robot.velocity = velocity;
    return robot;
}

void
robotUpdate(Robot *robot){
    robot->position.x += robot->velocity.x;
    robot->position.y += robot->velocity.y;

    robot->velocity.x = 0;
    robot->velocity.y = 0;

}

void
robotCollide(Robot *robot, const Wall *wall) {
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


void
laserUpdate(Laser *laser){
    laser->position.x += laser->velocity.x;
    laser->position.y += laser->velocity.y;
}

Laser
laserCreate(const Robot *robot, Vector2 velocity){
    Laser laser;
    Vector2 robot_center = {robot->position.x + robot->size.x/2, robot->position.y + robot->size.y/2};
    laser.position = robot_center;

    laser.velocity = velocity;
    return laser;
}

void
radiansToVelocities(Vector2 velocities[], unsigned int divisions)
{
    float radians_div = 2*M_PI/divisions;
    float r = 10;
    for (unsigned int i = 0; i < divisions; i++) {
        float theta = radians_div * i;

        velocities[i].x = cos(theta) * r;
        velocities[i].y = sin(theta) * r;
    }
}

bool
pointExistsOrNear(const Array* pointarr, const LaserPoint* laserpoint)
{
    LaserPoint* points = (LaserPoint *)pointarr->data;

    for(size_t i = 0; i < pointarr->size; i++)
    {
        if (fabs(points[i].x - laserpoint->x) < 1e-6 &&
            fabs(points[i].y - laserpoint->y) < 1e-6)
        {
            return true;
        }

        double dx = points[i].x - laserpoint->x;
        double dy = points[i].y - laserpoint->y;
        if (sqrt(dx * dx + dy * dy) < POINT_THRESHOLD_DISTANCE)
        {
            return true;
        }
    }
    return false;
}

bool
laserCollide(Laser* laser, const Wall* wall, Array* pointarr)
{
    Direction direction = NONE;
    float laser_end_x = laser->position.x + laser->velocity.x;
    float laser_end_y = laser->position.y + laser->velocity.y;
    float wall_end_x = wall->position.x + wall->size.x;
    float wall_end_y = wall->position.y + wall->size.y;

    bool x_colision_right = laser_end_x < wall_end_x;
    bool x_colision_left = laser_end_x > wall->position.x;
    bool y_colision_down = laser_end_y < wall_end_y;
    bool y_colision_up = laser_end_y > wall->position.y;
    float dy;
    float dx;
    if(x_colision_right && x_colision_left && y_colision_down && y_colision_up)
    {
        dx = laser_end_x - wall_end_x;
        dy = laser_end_y - wall_end_y;


        if (fabs(dx) < fabs(dy))
        {
            if(laser_end_x > wall_end_x-(wall->size.x/2)) direction = RIGHT; else direction = LEFT;
        }
        else
        {
            if(laser_end_y > wall_end_y-(wall->size.y/2)) direction = DOWN; else direction = UP;
        }


    }

    switch(direction)
    {
        case UP:
            {
                LaserPoint point;
                point.x = laser_end_x;
                point.y = laser_end_y - wall->size.y - dy;
                if(!pointExistsOrNear(pointarr, &point)) addElement(pointarr, &point);
            }
            return true;
        case DOWN:
            {
                LaserPoint point;
                point.x = laser_end_x;
                point.y = laser_end_y - dy;
                if(!pointExistsOrNear(pointarr, &point)) addElement(pointarr, &point);
            }
            return true;
        case RIGHT:
            {
                LaserPoint point;
                point.x = laser_end_x - dx;
                point.y = laser_end_y;
                if(!pointExistsOrNear(pointarr, &point)) addElement(pointarr, &point);
            }
            return true;
        case LEFT:
            {
                LaserPoint point;
                point.x = laser_end_x - wall->size.x - dx;
                point.y = laser_end_y;
                if(!pointExistsOrNear(pointarr, &point)) addElement(pointarr, &point);
            }
            return true;
        default:
            return false;
    }
    return false;

}

void
laserOutOfScreen(Array* laserarr)
{
    Laser* lasers = (Laser*)laserarr->data;
    for(size_t i = 0; i < laserarr->size; i++)
    {
        if(lasers[i].position.x < 0 || lasers[i].position.x > 1920 || lasers[i].position.y < 0 || lasers[i].position.y > 1080) removeElement(laserarr, i);
    }
}


// ===================================================================================================================================
// MAIN
// ===================================================================================================================================



int
main(){
    const int screenWidth = 1920;
    const int screenHeight = 1080; // 1080 - i3 borders

    bool paused = false;

    Wall walls[4];
    walls[0] = wallCreate((Vector2){100, 800}, (Vector2){1650, 100});
    walls[1] = wallCreate((Vector2){1650, 100}, (Vector2){100, 800});
    walls[2] = wallCreate((Vector2){100, 100}, (Vector2){1650, 100});
    walls[3] = wallCreate((Vector2){100, 100}, (Vector2){100, 600});
    Robot robot = robotCreate((Vector2){100, 700}, (Vector2){100, 100}, (Vector2){10, 0});
    static Array laserarr;
    initArray(&laserarr, 10, sizeof(Laser));

    static Array pointarr;
    initArray(&pointarr, 10, sizeof(LaserPoint));

    InitWindow(screenWidth, screenHeight, "Slam simulation");
    SetTargetFPS(60);

    while (!WindowShouldClose()){
        BeginDrawing();
            ClearBackground(BEIGE);

            // Print FPS
            int fps = GetFPS();


            if (IsKeyDown(KEY_RIGHT)) robot.velocity.x = 10;
            if (IsKeyDown(KEY_LEFT)) robot.velocity.x = -10;
            if (IsKeyDown(KEY_UP)) robot.velocity.y = -10;
            if (IsKeyDown(KEY_DOWN)) robot.velocity.y = 10;
            if (IsKeyDown(KEY_SPACE)) {
                Vector2 velocities[N_LASER_PER_FRAME];
                radiansToVelocities(velocities, N_LASER_PER_FRAME);
                for (size_t i = 0; i < N_LASER_PER_FRAME; i++)
                {
                    Laser laser = laserCreate(&robot, velocities[i]);
                    addElement(&laserarr, &laser);

                }

            }
            if (IsKeyPressed(KEY_Q)) SetTargetFPS(0);
            if (IsKeyPressed(KEY_E)) SetTargetFPS(60);
            if (IsKeyPressed(KEY_ENTER)) {
                if(paused) paused = false;
                else paused = true;
            }


            // Game Loop

            Laser* lasers = (Laser *)laserarr.data;
            LaserPoint* points = (LaserPoint *)pointarr.data;

            if(!paused)
            {
                robotUpdate(&robot);

                for(size_t i = 0; i < laserarr.size; i++)
                {
                    laserUpdate(&lasers[i]);
                    laserOutOfScreen(&laserarr);
                }

                for (size_t i = 0; i < 4; i++)
                {
                    robotCollide(&robot, &walls[i]);
                    for (size_t j = 0; j < laserarr.size; j++)
                    {
                        if(laserCollide(&lasers[j], &walls[i], &pointarr)) removeElement(&laserarr, j);
                    }
                }
            }




            // Rendering

            char fpstring[40];
            sprintf(fpstring,"FPS %d", fps);
            DrawText(fpstring, 10, 10, 20, RED);

            char n_points[40];
            sprintf(n_points,"Number of points: %d", pointarr.size);
            DrawText(n_points, 10, 40, 20, RED);

            for(int i = 0; i < 4; i++)
            {
                DrawRectangle(walls[i].position.x, walls[i].position.y, walls[i].size.x, walls[i].size.y, DARKGRAY);
            }
            for(size_t i = 0; i < laserarr.size; i++)
            {
                Vector2 endPosition = { lasers[i].position.x + lasers[i].velocity.x, lasers[i].position.y + lasers[i].velocity.y };
                DrawLineEx(lasers[i].position, endPosition, 2.0f, RED);
            }
            for(size_t i = 0; i < pointarr.size; i++)
            {
                DrawCircle(points[i].x, points[i].y, 2, RED);
            }
            DrawRectangle(robot.position.x, robot.position.y, robot.size.x, robot.size.y, BLUE);


        EndDrawing();
    }
    CloseWindow();

    freeArray(&laserarr);
    freeArray(&pointarr);


    return 0;
}
