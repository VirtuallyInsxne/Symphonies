#include "projectile.h"

Entity *projectile_new(Entity *parent, Entity *target, Vector3D position, Vector3D dir, float speed, float damage)
{
    Entity *self;

    self = entity_new();
    if (!self)
    {
        slog("failed to make a new projectile");
        return NULL;
    }

    self->parent = parent;
    self->target = target;
    self->model = gf3d_model_load("models/dino.model");
    self->bounds.x = -16;
    self->bounds.y = -16;
    self->bounds.z = -16;
    self->bounds.w = 16;
    self->bounds.h = 16;
    self->bounds.d = 16;
    vector3d_copy(self->position, position);
    self->rotation = dir;
    if (self->parent->player1)
    {
        self->velocity.x = -1;
    }
    else
    {
        self->velocity.x = 1;
    }
    self->damage = damage;

    return self;
}

void projectile_think(Entity *self)
{
    if (!self) return;
}

void projectile_update(Entity *self)
{
    if (!self) return;

    if (entity_check_collision(self, self->target))
    {
        self->target->health -= self->damage;
        entity_free(self);
    }
}

/*eol@eof*/
