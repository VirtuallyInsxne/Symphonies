
#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "agumon.h"
#include "projectile.h"


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
    ent->health = 100.0;
    ent->think = agumon_think;
    ent->update = agumon_update;
    ent->onDamage = agumon_damage;
    ent->bounds.x = -10;
    ent->bounds.y = -10;
    ent->bounds.z = -10;
    ent->bounds.w = 20;
    ent->bounds.h = 20;
    ent->bounds.d = 20;
    ent->projectileCooldown = 0;
    ent->punchCooldown = 0;
    ent->jumped = 0;
    ent->blocking = 0;
    vector3d_copy(ent->position,position);
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
    Vector3D backward = {0};
    Vector3D shieldPosition;
    Vector2D w;
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z);
    backward.x = -w.x;
    backward.y = -w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);

    shieldPosition = self->position;
    if (self->player1)shieldPosition.x = self->position.x - 15;
    if (!self->player1)shieldPosition.x = self->position.x + 15;

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
        if (keys[SDL_SCANCODE_TAB]) {
            self->blocking = 1;
            if (self->blocking)
            {
                shield_new(self, shieldPosition, backward);
                self->blocking = 0;
            }

        }
        if (self->projectileCooldown <= 0) {
            if (keys[SDL_SCANCODE_Q]) {
                projectile_new(self, self->position, backward, 1, 10);
                self->projectileCooldown = 100;
            }
        }
        if (self->punchCooldown <= 0) {
            if (keys[SDL_SCANCODE_E]) {
                short_projectile_new(self, self->position, backward, 2, 10);
                self->punchCooldown = 50;
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
        if (keys[SDL_SCANCODE_P]) {
            self->blocking = 1;
            if (self->blocking)
            {
                shield_new(self, shieldPosition, backward);
                self->blocking = 0;
            }

        }
        if (self->projectileCooldown <= 0) {
            if (keys[SDL_SCANCODE_RCTRL]) {
                projectile_new(self, self->position, backward, 1, 10);
                self->projectileCooldown = 100;
            }
        }
        if (self->punchCooldown <= 0) {
            if (keys[SDL_SCANCODE_RSHIFT]) {
                short_projectile_new(self, self->position, backward, 2, 10);
                self->punchCooldown = 50;
            }
        }
    }

}

/*eol@eof*/
