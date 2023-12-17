#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    World *w;
    Entity *player1;
    Entity *player2;
    Matrix4 skyMat;
    Model *sky;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
    
    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    
    slog_sync();
    
    entity_system_init(1024);
    
    
    player1 = agumon_new(vector3d(35,5,0), vector3d(0,0,-GFC_HALF_PI), 1);
    player2 = agumon_new(vector3d(-35,5,0), vector3d(0,0,GFC_HALF_PI), 2);
    if (player1)player1->selected = 1;
    if (player2)player2->selected = 1;
    Entity *players[] = { player1, player2 };
    w = world_load("config/testworld.json");
    
    slog_sync();
    gf3d_camera_set_position(vector3d(0,90,40));
    gf3d_camera_set_rotation(vector3d(-GFC_PI,0,-GFC_PI));
    gf3d_camera_set_scale(vector3d(1,1,1));

    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        int i;
        gfc_input_update();
        gf2d_font_update();
        
        world_run_updates(w);
        entity_think_all();
        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
                entity_draw_all();
                
            //2D draws
                gf2d_draw_rect_filled(gfc_rect(10 ,10,1000,32),gfc_color8(128,128,128,255));
                gf2d_font_draw_line_tag("Press ALT+F4 to exit",FT_H1,gfc_color(1,1,1,1), vector2d(10,10));

                char player1HealthString[50];
                char player2HealthString[50];

                sprintf(player1HealthString, "%d", (int)player1->health);
                strcat(player1HealthString, "/100");
                gf2d_font_draw_line_tag(player1HealthString,FT_H6,gfc_color(1,1,1,1), vector2d(50,100));

                sprintf(player2HealthString, "%d", (int)player2->health);
                strcat(player2HealthString, "/100");
                gf2d_font_draw_line_tag(player2HealthString,FT_H6,gfc_color(1,1,1,1), vector2d(1050,100));

                for (i = 0; i < 2; i++)
                {
                    if (players[i]->health <= 0)
                    {
                        if (players[i]->player1 == 1)
                        {
                            gf2d_font_draw_line_tag("Player 2 Wins!",FT_H6,gfc_color(1,1,1,1), vector2d(550,100));
                        }
                        else
                        {
                            gf2d_font_draw_line_tag("Player 1 Wins!",FT_H6,gfc_color(1,1,1,1), vector2d(550,100));
                        }
                    }
                }
                
                gf2d_draw_rect(gfc_rect(10 ,10,1000,32),gfc_color8(255,255,255,255));
                
        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }    
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
