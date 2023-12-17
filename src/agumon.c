
#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "agumon.h"

void agumon_update(Entity *self);

void agumon_think(Entity *self);

void agumon_damage(Entity *self, float damage, Entity *inflictor)
{
    if (!self)return;
    self->health -= damage;
    if (self->health <= 0)entity_free(self);

}

Entity *agumon_new(Vector3D position, Vector3D rotation, int playerNum)
{
    Entity *ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    if (playerNum == 1) ent->player1 = 1;
    if (playerNum != 1) ent->player1 = 0;

    ent->model = gf3d_model_load("models/dino.model");
    ent->health = 100;
    ent->think = agumon_think;
    ent->update = agumon_update;
    ent->onDamage = agumon_damage;

    Box b = gfc_box(position.x, position.y, position.z, 1, 1, 2);
    ent->bounds = b;

    ent->projectileCooldown = 0;
    ent->punchCooldown = 0;
    ent->jumped = 0;
    ent->blocking = 0;
    vector3d_copy(ent->position, position);
    vector3d_copy(ent->rotation, rotation);
    return ent;
}

void agumon_update(Entity *self)
{
    float height = 35.0;
    float limit = 50.0;

    self->projectileCooldown -= 1;
    self->punchCooldown -= 1;
    self->blocking = 0;

    if (!self)
    {
        slog("self pointer not provided");
        return;
    }

    if(self->position.x > limit)self->position.x = limit;
    if(self->position.x < -limit)self->position.x = -limit;

    if(self->position.z > height)
    {
        self->velocity.z -= 0.98;
        self->acceleration.z -= 0.05;
    }

    if(self->position.z < height)
    {
        self->position.z = height;
        self->jumped = 0;
        if(self->velocity.z < 0)self->velocity.z = 0;
        if(self->acceleration.z < 0)self->acceleration.z = 0;
    }

    if(self->position.z == height)
    {
        self->jumped = 0;
        self->velocity.z = 0;
        self->acceleration.z = 0;
    }

}

void agumon_think(Entity *self)
{
    Vector3D forward = {0};
    Vector2D w;
    Entity *other;

    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    SDL_Joystick *joystick;
    joystick = SDL_JoystickOpen(0);
    slog("Axes detected: %i", SDL_JoystickNumAxes(joystick));

    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);

    other = entity_get_collision(self);

    if(other != NULL && other->type == ENT_P2 && self->state == ES_attack)
    {
            slog("Collision between players occured");
            other->health -= 10;
    }

    if (!self)return;


    if(SDL_JoystickGetAxis(joystick, 0) > 32700){
       slog("axis movement detected");
    }

    if ((keys[SDL_SCANCODE_W] || SDL_JoystickGetAxis(joystick, 1) < -32700) && (self->jumped == 0))
    {
        self->jumped = 1;
        self->acceleration.z += 1;
        self->velocity.z += 2;
    }
    if (keys[SDL_SCANCODE_S] || SDL_JoystickGetAxis(joystick, 1) > 32700)
    {
        self->position.z -= 1;
    }
    if (keys[SDL_SCANCODE_D] || SDL_JoystickGetAxis(joystick, 0) > 32700)
    {
        vector3d_add(self->position,self->position,forward);
    }
    if (keys[SDL_SCANCODE_A] || SDL_JoystickGetAxis(joystick, 0) < -32700)
    {
        vector3d_add(self->position,self->position,-forward);
    }
    /*if(self->atkCooldown <= 0)
    {
        if(SDL_JoystickGetButton(joystick, 0) == 1)
        {
                slog("Cross");
                gf3d_model_free(self->model);
                self->model = gf3d_model_load("models/kiryuRP.model");
                self->state = ES_attack;
                self->atkCooldown = 400;
        }
        if(SDL_JoystickGetButton(joystick, 1) == 1)
        {
                slog("Circle");
                gf3d_model_free(self->model);
                self->model = gf3d_model_load("models/kiryuRK.model");
                self->state = ES_attack;
                self->atkCooldown = 600;
        }
        if(SDL_JoystickGetButton(joystick, 2) == 1)
        {
                slog("Triangle");
                gf3d_model_free(self->model);
                self->model = gf3d_model_load("models/kiryuLK.model");
                self->state = ES_attack;
                self->atkCooldown = 200;
        }
        if(SDL_JoystickGetButton(joystick, 3) == 1)
        {
                slog("Square");
                gf3d_model_free(self->model);
                self->model = gf3d_model_load("models/kiryuLP.model");
                self->state = ES_attack;
                self->atkCooldown = 100;
        }
        if(SDL_JoystickGetButton(joystick, 5) == 1)
        {
                slog("R1");
                gf3d_model_free(self->model);
                self->model = gf3d_model_load("models/kiryuForRK.model");
                self->state = ES_attack;
                self->atkCooldown = 1000;
        }
    }
    else if(self->atkCooldown == 75)
    {
        gf3d_model_free(self->model);
        self->model = gf3d_model_load("models/kiryu.model");
        self->state = ES_attack;
        self->atkCooldown--;
    }
    else
    {
        self->atkCooldown--;
        self->state = ES_stand;
    }*/


    switch(self->state)
    {
        case ES_stand:
            //stand
            break;
        case ES_stBlock:
            // stand block
            break;
        case ES_ch:
            // crouch
            break;
        case ES_chBlock:
            // crouch block
            break;
        case ES_attack:
            //attack frame
            break;
        case ES_knockd:
            //knocked down
            break;
    }
}

/*eol@eof*/
