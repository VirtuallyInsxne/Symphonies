
#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "agumon.h"
#include "projectile.h"
#include "world.h"


void agumon_update(Entity *self);

void agumon_think(Entity *self);

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
    ent->health = 100.0;
    ent->think = agumon_think;
    ent->update = agumon_update;
    ent->bounds.x = -2;
    ent->bounds.y = -2;
    ent->bounds.z = -2;
    ent->bounds.w = 4;
    ent->bounds.h = 4;
    ent->bounds.d = 4;
    ent->cooldown = 0;
    ent->jumped = 0;
    vector3d_copy(ent->position,position);
    vector3d_copy(ent->rotation, rotation);
    return ent;
}

void agumon_update(Entity *self)
{
    float height = 35.0;
    float limit = 50.0;

    self->cooldown -= 1;
    self->blocking = 0;

    if (!self->blocking) entity_free(self->shield);

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
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);

    if (self->player1)
    {
        if (keys[SDL_SCANCODE_W] && (self->jumped==0))
        {
            self->jumped = 1;
            self->acceleration.z += 1;
            self->velocity.z += 2;
        }
        if (keys[SDL_SCANCODE_S])
        {
            self->position.z -= 1;
        }
        if (keys[SDL_SCANCODE_D])
        {
            vector3d_add(self->position,self->position,-forward);
        }
        if (keys[SDL_SCANCODE_A])
        {
            vector3d_add(self->position,self->position,forward);
        }
        if (self->cooldown <= 0) {
            if (keys[SDL_SCANCODE_Q]) {
                projectile_new(self, self->target, self->position, forward, 10, 10);
                self->cooldown = 100;
            }
        }
    }
    else
    {
        if (keys[SDL_SCANCODE_UP] && (self->jumped==0))
        {
            self->jumped = 1;
            self->acceleration.z += 1;
            self->velocity.z += 2;
        }
        if (keys[SDL_SCANCODE_DOWN])
        {
            self->position.z -= 1;
        }
        if (keys[SDL_SCANCODE_RIGHT])
        {
            vector3d_add(self->position,self->position,forward);
        }
        if (keys[SDL_SCANCODE_LEFT])
        {
            vector3d_add(self->position,self->position,-forward);
        }
        if (keys[SDL_SCANCODE_RSHIFT]) {
            self->blocking = 1;
            if (self->blocking)
            {
                Entity *shield = NULL;
                shield = entity_new();
                shield->model = gf3d_model_load("models/dino.model");

                shield->position = self->position;
                shield->position.x = shield->position.x + 10;
            }

        }
        if (self->cooldown <= 0) {
            if (keys[SDL_SCANCODE_RCTRL]) {
                projectile_new(self, self->target, self->position, forward, 10, 10);
                self->cooldown = 100;
            }
        }
    }

}

/*eol@eof*/
