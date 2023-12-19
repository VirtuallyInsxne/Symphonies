
#include "simple_logger.h"
#include "fighter2.h"

// Modify these values as needed
#define CHARACTER_SIZE_X 0.0005
#define CHARACTER_SIZE_Y 0.0005
#define CHARACTER_SIZE_Z 0.001

// Bounding box sizes for low, mid, and high attacks
#define LOW_BOX_SIZE_X 8.0
#define LOW_BOX_SIZE_Y 8.0
#define LOW_BOX_SIZE_Z 4.0

#define MID_BOX_SIZE_X 8.0
#define MID_BOX_SIZE_Y 8.0
#define MID_BOX_SIZE_Z 4.0

#define HIGH_BOX_SIZE_X 8.0
#define HIGH_BOX_SIZE_Y 8.0
#define HIGH_BOX_SIZE_Z 4.0

// Add these constants for hitbox sizes
#define LOW_HITBOX_SIZE_X 8.0
#define LOW_HITBOX_SIZE_Y 8.0
#define LOW_HITBOX_SIZE_Z 4.0

#define MID_HITBOX_SIZE_X 8.0
#define MID_HITBOX_SIZE_Y 8.0
#define MID_HITBOX_SIZE_Z 4.0

#define HIGH_HITBOX_SIZE_X 8.0
#define HIGH_HITBOX_SIZE_Y 8.0
#define HIGH_HITBOX_SIZE_Z 4.0

void fighter2_update(Entity *self);

void fighter2_think(Entity *self);

Entity *fighter2_new(Vector3D position, Vector3D rotation, int playerNum)
{
    Entity *ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no second fighter for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0,0,0,1);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/fighter2.model");
    ent->think = fighter2_think;
    ent->update = fighter2_update;
    ent->type = ENT_P2;
    ent->state = ES_stand;

    vector3d_copy(ent->position, position);
    vector3d_copy(ent->rotation, rotation);

    // Larger overall bounding box for the character model
    Box characterBounds = gfc_box(position.x, position.y, position.z, CHARACTER_SIZE_X, CHARACTER_SIZE_Y, CHARACTER_SIZE_Z);
    ent->bounds = characterBounds;

    // Larger bounding boxes
    Box lowBounds = gfc_box(position.x, position.y, position.z, -LOW_BOX_SIZE_X, -LOW_BOX_SIZE_Y, -LOW_BOX_SIZE_Z);
    Box midBounds = gfc_box(position.x, position.y, position.z + LOW_BOX_SIZE_Z, -MID_BOX_SIZE_X, -MID_BOX_SIZE_Y, -MID_BOX_SIZE_Z);
    Box highBounds = gfc_box(position.x, position.y, position.z + 2 * LOW_BOX_SIZE_Z, -HIGH_BOX_SIZE_X, -HIGH_BOX_SIZE_Y, -HIGH_BOX_SIZE_Z);

     // Assign bounding boxes to the entity
    ent->lowBounds = lowBounds;
    ent->midBounds = midBounds;
    ent->highBounds = highBounds;

    Box lowAttackBounds = gfc_box(position.x, position.y, position.z, -LOW_HITBOX_SIZE_X, -LOW_HITBOX_SIZE_Y, -LOW_HITBOX_SIZE_Z);
    Box midAttackBounds = gfc_box(position.x, position.y, position.z, -MID_HITBOX_SIZE_X, -MID_HITBOX_SIZE_Y, -MID_HITBOX_SIZE_Z);
    Box highAttackBounds = gfc_box(position.x, position.y, position.z, -HIGH_HITBOX_SIZE_X, -HIGH_HITBOX_SIZE_Y, -HIGH_HITBOX_SIZE_Z);

    // Assign bounding boxes to the entity
    ent->lowAttackBounds = lowAttackBounds;
    ent->midAttackBounds = midAttackBounds;
    ent->highAttackBounds = highAttackBounds;

    ent->projectileCooldown = 0;
    ent->punchCooldown = 0;
    ent->jumped = 0;
    ent->blocking = 0;
    return ent;
}

void fighter2_update(Entity *self)
{
    float height = 28.0;
    float limit = 5.0;

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

void fighter2_think(Entity *self)
{
    Vector3D forward = {0};
    Vector2D w;
    Entity *other;

    const Uint8 *keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    SDL_Joystick *joystick;
    joystick = SDL_JoystickOpen(0);

    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);

    other = entity_get_collision(self);

    if(other != NULL && other->type == ENT_P1 && self->state == ES_attack)
    {
        slog("Collision between players occured");
        other->currHealth-=10;
    }
    else if (other != NULL && other->type == ENT_P1) {
        slog("Players bumped into each other");
        self->position.x -= 10;
        other->position.x += 10;
    }

    if (!self)
        return;

    /*if (SDL_JoystickGetAxis(joystick, 0) > 32700)
    {
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
    if (keys[SDL_SCANCODE_D] || SDL_JoystickGetAxis(joystick, 0) < -32700)
    {
        vector3d_add(self->position, self->position, forward);
    }
    if (keys[SDL_SCANCODE_A] || SDL_JoystickGetAxis(joystick, 0) > 32700)
    {
        vector3d_add(self->position, self->position, -forward);
    }
    if (self->atkCooldown <= 0)
    {
        if (SDL_JoystickGetButton(joystick, 0) == 1)
        {
            slog("Cross");
            gf3d_model_free(self->model);
            self->model = gf3d_model_load("models/kiryuRP.model");
            self->state = ES_attack_low; // For example, this move always hits low
            self->atkCooldown = 400;
        }
        if (SDL_JoystickGetButton(joystick, 1) == 1)
        {
            slog("Circle");
            gf3d_model_free(self->model);
            self->model = gf3d_model_load("models/kiryuRK.model");
            self->state = ES_attack_high; // For example, this move always hits high
            self->atkCooldown = 600;
        }
        if (SDL_JoystickGetButton(joystick, 3) == 1)
        {
            slog("Triangle");
            gf3d_model_free(self->model);
            self->model = gf3d_model_load("models/kiryuLK.model");
            self->state = ES_attack;
            self->atkCooldown = 200;
        }
        if (SDL_JoystickGetButton(joystick, 2) == 1)
        {
            slog("Square");
            gf3d_model_free(self->model);
            self->model = gf3d_model_load("models/kiryuLP.model");
            self->state = ES_attack;
            self->atkCooldown = 100;
        }
        if (SDL_JoystickGetButton(joystick, 5) == 1)
        {
            slog("R1");
            gf3d_model_free(self->model);
            self->model = gf3d_model_load("models/kiryuForRK.model");
            self->state = ES_attack;
            self->atkCooldown = 1000;
        }
    }
    else if (self->atkCooldown == 75)
    {
        gf3d_model_free(self->model);
        self->model = gf3d_model_load("models/kiryu.model");
        self->state = ES_stand;
        self->atkCooldown--;
    }
    else
    {
        self->atkCooldown--;
        self->state = ES_stand;
    }*/

    switch (self->state)
    {
    case ES_stand:
        // stand
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
        break;
    case ES_attack_low:
        // low attack frame
        break;
    case ES_attack_mid:
        break;
    case ES_attack_high:
        // high attack frame
        break;
    }
}

/*eol@eof*/
