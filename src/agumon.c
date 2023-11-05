
#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "agumon.h"
#include "world.h"


void agumon_update(Entity *self);

void agumon_think(Entity *self);

Entity *agumon_new(Vector3D position, Vector3D rotation)
{
    Entity *ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/dino.model");
    ent->health = 100.0;
    ent->think = agumon_think;
    ent->update = agumon_update;
    ent->jumped = 0;
    vector3d_copy(ent->position,position);
    vector3d_copy(ent->rotation, rotation);
    return ent;
}

void agumon_update(Entity *self)
{
    float height = 35.0;

    if (!self)
    {
        slog("self pointer not provided");
        return;
    }

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

    if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;
}

/*eol@eof*/
