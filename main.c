#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <SDL_mixer.h>
#include <stdbool.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION

#include <nuklear.h>
#include <nuklear_sdl_renderer.h>

#include <direct.h>
#include <windows.h>
#include <shellapi.h>
#include <lame.h>
#include <SDL_filesystem.h>
#include <dirent.h>
#include <string.h>


char DirectorySongList[10][100];
int song_count = 0;
void add_to_list(char* file_name) {
    if (song_count < 10) {
        strcpy(DirectorySongList[song_count++], file_name);
    }
}


int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    SDL_Log("SDL is active!");

    char fname[_MAX_FNAME];
    char dir[_MAX_DIR];
    char drive[_MAX_DRIVE];

    if (argc >= 2)
    {
        


        char ext[_MAX_EXT];

        char* file_path = argv[1];
        _splitpath(file_path, drive, dir, fname, ext);
        printf("The file name is: %s%s\n", fname, ext);

    }
    else
    {
        printf("No file path provided\n");
        return -1;
    }

    char songname[1024] = "None";

    if (argc >= 2)
    {
        sprintf(songname, "%s", fname);
    }

    //AUDIO

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    // Load the music file
    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8* wav_buffer;
    Mix_Music* songmusic = Mix_LoadMUS(argv[1], &wav_spec, &wav_buffer, &wav_length);
    if (!songmusic) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;

    }

    // Play the music
    if (Mix_PlayMusic(songmusic, -1) == -1) {
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }



    //Directory


    //const char* folder_path = "C:/Users/alepr/Downloads";
    const char* allowed_extensions[] = { ".mp3", ".wav", ".mid" };

    DIR* directory = opendir(dir);
    if (directory == NULL) {
        printf("Error opening directory: %s\n", directory);
        return 1;
    }



    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Get the file extension
        char* extension = strrchr(entry->d_name, '.');
        if (extension == NULL) {
            continue;
        }

        // Check if the extension is allowed
        int allowed = 0;
        for (int i = 0; i < 3; i++) {
            if (strcmp(extension, allowed_extensions[i]) == 0) {
                allowed = 1;
                break;
            }
        }

        if (allowed) {
            add_to_list(entry->d_name);
        }
    }

    closedir(directory);

    printf("Directory Song List:\n");
    for (int i = 0; i < song_count; i++) {
        printf("%s\n", DirectorySongList[i]);
    }



    //Video


    SDL_Window* window = SDL_CreateWindow("Ale media player", 100, 100, 300*3, 300, 0);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Nuklear init
    struct nk_context* ctx = nk_sdl_init(window, renderer);
    struct nk_font_atlas* atlas;
    struct nk_font_config config = nk_font_config(0);
    struct nk_font* font;
    nk_sdl_font_stash_begin(&atlas);
    font = nk_font_atlas_add_default(atlas, 13, &config);
    nk_sdl_font_stash_end();
    nk_style_set_font(ctx, &font->handle);

    //Texture
    int cornersize = 300;
    int charactersize = 100;

    int width;
    int heigth;
    int channel;
    unsigned char* pixels = stbi_load("C:/Users/alepr/Downloads/music.png", &width, &heigth, &channel, 4);
    if (!pixels) {
        SDL_Log("Unable to open image: %s", stbi__g_failure_reason);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, heigth);
    if (!texture) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    

    SDL_UpdateTexture(texture, NULL, pixels, width * 4);

    //Top left vertex
    SDL_Vertex vertex[4];
    vertex[0].position.x = 0;
    vertex[0].position.y = 0;
    vertex[0].color.r = 255;
    vertex[0].color.g = 255;
    vertex[0].color.b = 255;
    vertex[0].color.a = 255;
    vertex[0].tex_coord.x = 0;
    vertex[0].tex_coord.y = 0;
    vertex[1].position.x = cornersize;
    vertex[1].position.y = 0;
    vertex[1].color.r = 255;
    vertex[1].color.g = 255;
    vertex[1].color.b = 255;
    vertex[1].color.a = 255;
    vertex[1].tex_coord.x = 1;
    vertex[1].tex_coord.y = 0;
    vertex[2].position.x = 0;
    vertex[2].position.y = cornersize;
    vertex[2].color.r = 255;
    vertex[2].color.g = 255;
    vertex[2].color.b = 255;
    vertex[2].color.a = 255;
    vertex[2].tex_coord.x = 0;
    vertex[2].tex_coord.y = 1;
    vertex[3].position.x = cornersize;
    vertex[3].position.y = cornersize;
    vertex[3].color.r = 255;
    vertex[3].color.g = 255;
    vertex[3].color.b = 255;
    vertex[3].color.a = 255;
    vertex[3].tex_coord.x = 1;
    vertex[3].tex_coord.y = 1;

    int index[6] = { 0, 1, 2, 1, 3, 2 };
    //Ball
    int x = 0, y = 00;
    int xVel = 2, yVel = 2;
    int ballsize = 15;
    float Volume = 1.0f;
    int IntVolume = 0;
    char VolumeShow[20];
    bool IsPlaying = true;

    
    int running = 1;
    while (running) {
        SDL_Event event;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;

            }
            nk_sdl_handle_event(&event);
        }
        nk_input_end(ctx);

        //Texture
        SDL_RenderGeometry(renderer, texture, vertex, 4, index, 6);



        //Ball
        // Update the ball's position
        //x += xVel;
        //y += yVel;

        //// Check for wall collisions
        //if (x < 0 || x > cornersize - ballsize * 2) {
        //    xVel = -xVel;
        //}
        //if (y < 0 || y > cornersize - ballsize) {
        //    yVel = -yVel;
        //}

        //// Clear the screen
        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        ////SDL_RenderClear(renderer);

        //// Draw the ball
        //SDL_Rect ball = { x - ballsize * -0.5, y - ballsize * -0.5, ballsize, ballsize };
        //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        //SDL_RenderFillRect(renderer, &ball);

        // Update the screen
        SDL_RenderPresent(renderer);

        if (nk_begin(ctx, "List of song in the same folder", nk_rect(cornersize * 2 +5, 0, 300-5, 300), NK_WINDOW_BORDER)) {
            /* fixed widget pixel width */


            int i;
            for (i = 0; i < 100; i++) {
                if (i <= song_count-1) {
                
                nk_layout_row_static(ctx, 30, 250, 1);
                if (nk_button_label(ctx, &DirectorySongList[i])) { 

                    char path[_MAX_PATH] = "";
                    strcat(path, drive);
                    strcat(path, dir);
                    strcat(path, DirectorySongList[i]);
                    printf("%s\n", path);
                    sprintf(songname, "%s", DirectorySongList[i]);
                    
                    //printf("Dir: %s\n", path);
                    Mix_Music* songmusic = Mix_LoadMUS(path, &wav_spec, &wav_buffer, &wav_length);
                    if (!songmusic) {
                        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
                        return 1;

                    }
                    

                    // Play the music
                    if (Mix_PlayMusic(songmusic, -1) == -1) {
                        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
                        return 1;
                    }
                    IsPlaying = true;

                }
                }
            }


        }
        nk_end(ctx);

        if (nk_begin(ctx, "Ale Music Player", nk_rect(cornersize+5, 0, 300, 300),
            NK_WINDOW_BORDER)) {
            /* fixed widget pixel width */
            nk_layout_row_static(ctx, 30, 80, 1);
            nk_label(ctx, songname, NK_TEXT_LEFT);
            nk_label(ctx, Mix_GetMusicArtistTag, NK_TEXT_LEFT);
            nk_label(ctx, Mix_GetMusicAlbumTag, NK_TEXT_LEFT);

            //Volume
            nk_layout_row_begin(ctx, NK_STATIC, 30, 5);
            {
                nk_layout_row_push(ctx, 50);
                nk_label(ctx, "Volume:", NK_TEXT_LEFT);
                nk_layout_row_push(ctx, 110);
                nk_slider_float(ctx, 0, &Volume, 1.0f, 0.01f);
                int VolumeShowInt = round(Volume * 100);
                sprintf(VolumeShow, "%i", VolumeShowInt);
                nk_label(ctx, VolumeShow, NK_TEXT_LEFT);
            }
            IntVolume = (int)(MIX_MAX_VOLUME * Volume);
            nk_layout_row_end(ctx);
            Mix_VolumeMusic(IntVolume);




            //Play pause
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "Play/Pause")) {
                /* event handling */
                puts("Button pressed");

                if (IsPlaying == true) {

                    Mix_PauseMusic();
                    IsPlaying = false;
                }
                else
                {
                    Mix_ResumeMusic();
                    IsPlaying = true;
                }


            }

            //File explorer
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "Open file explorer")) {
                /* event handling */
                puts("Button pressed");
                char file_path[MAX_PATH];
                OPENFILENAME ofn;
                memset(&ofn, 0, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.lpstrFile = file_path;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFilter = "Audio Files (*.wav;*.mp3;*.mid)\0*.wav;*.mp3;*.mid\0All Files (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn)) {
                    printf("Selected file: %s\n", file_path);

                }
                else {
                    printf("No file selected.\n");
                }
            }
        }
        nk_end(ctx);




        nk_sdl_render(NK_ANTI_ALIASING_ON);

        SDL_RenderPresent(renderer);


    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}




#ifdef FOOBAR
int WinMain(
    void* hInstance,
    void* hPrevInstance,
    char* lpCmdLine,
    int       nShowCmd
) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("SDL is active!");

    getchar();
    SDL_Quit();
    return 0;
}
#endif // FOOBAR
