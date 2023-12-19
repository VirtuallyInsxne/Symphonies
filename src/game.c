#include <SDL.h>
#include <SDL_mixer.h>

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"

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
#include "fighter2.h"
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

    Bool mainMenu = true;
    Bool edit = false;
    Bool custom = false;


    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
        if (strcmp(argv[a],"--edit") == 0)
        {
            edit = true;
            custom = true;
        }
        if (strcmp(argv[a],"--custom") == 0)
        {
            custom = true;
        }
    }

    if(edit)
    {
        FILE *custom;
        custom = fopen("config/custom.json","w");

        Uint32 customPlayer1Health;
        Uint32 customPlayer2Health;

        fprintf(custom,"{\n\t\"world\":\n\t{\n\t\t");
        fprintf(custom,"\"model\":\"models/antioch.model\",\n\t\t");
        fprintf(custom,"\"position\":[0,0,-50],\n\t\t");
        fprintf(custom,"\"scale\":[25,25,25],\n\t\t");
        fprintf(custom,"\"rotation\":[0,0,0]\n\t},");
        fprintf(custom,"\n\n\t\"spawns\":\n\t[\n\t\t{");
        fprintf(custom,"\n\t\t\t\"name\":\"player1\",\n\t\t\t");

        fprintf(custom,"\"position\":[5,5,0],\n\t\t\t");
        printf("How much health would you like player 1 to have? Enter an integer. \n");
        scanf("%u", &customPlayer1Health);
        fprintf(custom,"\"health\":%d\n\t\t}",customPlayer1Health);

        fprintf(custom,"\n\t\t{");

        fprintf(custom,"\n\t\t\t\"name\":\"player2\",\n\t\t\t");
        fprintf(custom,"\"position\":[-5,5,0],\n\t\t\t");

        printf("How much health would you like player 2 to have? Enter an integer. \n");
        scanf("%u", &customPlayer2Health);
        fprintf(custom,"\"health\":%d\n\t\t}",customPlayer2Health);

        fprintf(custom,"\n\t]\n}");
        fclose(custom);
    }


    init_logger("gf3d.log",0);
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    gfc_audio_init(100,32,4,10,1,0);

    slog_sync();

    Mix_Music *backgroundMusic = gfc_sound_load_music("audio/backgroundMusic.wav");
    Mix_PlayMusic(backgroundMusic, -1);
    Mix_VolumeMusic(10);

    entity_system_init(1024);

    if (custom) {
        w = world_load("config/custom.json");
    }
    else {
        w = world_load("config/testworld.json");
    }

    player1 = gfc_list_get_nth(w->entityList, 0);
    player2 = gfc_list_get_nth(w->entityList, 1);
    Entity *players[] = { player1, player2 };

    slog_sync();
    gf3d_camera_set_position(vector3d(0,20,30));
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
        if (!mainMenu) {
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

                    char player1CurrHealthString[50];
                    char player1MaxHealthString[50];

                    char player2CurrHealthString[50];
                    char player2MaxHealthString[50];

                    char player1FullHealthString[50];
                    char player2FullHealthString[50];

                    sprintf(player1CurrHealthString, "%d", (int)player1->currHealth);
                    sprintf(player2CurrHealthString, "%d", (int)player2->currHealth);

                    sprintf(player1MaxHealthString, "%d", (int)player1->maxHealth);
                    sprintf(player2MaxHealthString, "%d", (int)player2->maxHealth);

                    strcpy(player1FullHealthString, strcat(strcat(player1CurrHealthString, "/"), player1MaxHealthString));
                    strcpy(player2FullHealthString, strcat(strcat(player2CurrHealthString, "/"), player2MaxHealthString));

                    gf2d_font_draw_line_tag(player1FullHealthString,FT_H6,gfc_color(1,1,1,1), vector2d(50,100));
                    gf2d_font_draw_line_tag(player2FullHealthString,FT_H6,gfc_color(1,1,1,1), vector2d(1000,100));

                    for (i = 0; i < 2; i++)
                    {
                        if (players[i]->currHealth <= 0)
                        {
                            if (players[i]->player1 == 1)
                            {
                                gf2d_font_draw_line_tag("Player 2 Wins!",FT_H6,gfc_color(1,1,1,1), vector2d(450,100));
                            }
                            else
                            {
                                gf2d_font_draw_line_tag("Player 1 Wins!",FT_H6,gfc_color(1,1,1,1), vector2d(450,100));
                            }

                            gf2d_font_draw_line_tag("Press Escape to go back to Main Menu",FT_H4,gfc_color(1,1,1,1), vector2d(450,150));
                            if(gfc_input_command_pressed("cancel")) mainMenu = 1;
                        }
                    }

                    gf2d_draw_rect(gfc_rect(10 ,10,1000,32),gfc_color8(255,255,255,255));

            gf3d_vgraphics_render_end();

            if (gfc_input_command_down("exit"))done = 1; // exit condition
        }
        else
        {
            gfc_input_update();
            gf2d_font_update();
            gf3d_camera_update_view();
            gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

            gf3d_vgraphics_render_start();

                //gf2d_draw_rect_filled(gfc_rect(10 ,10,1000,32),gfc_color8(128,128,128,255));
                gf2d_font_draw_line_tag("Symphonies",FT_H6,gfc_color(1,1,1,1), vector2d(480,80));
                gf2d_font_draw_line_tag("Press space to begin",FT_H2,gfc_color(1,1,1,1), vector2d(420,580));
                gf2d_font_draw_line_tag("Press escape to exit",FT_H2,gfc_color(1,1,1,1), vector2d(420,600));

            if(gfc_input_command_pressed("jump"))
            {
                slog("Jump Button Pressed");
                mainMenu = false;
            }
            else if(gfc_input_command_pressed("cancel")) done = 1;

            gf3d_vgraphics_render_end();

            if (gfc_input_command_down("exit"))done = 1; // exit condition
        }
    }

    world_delete(w);

    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
