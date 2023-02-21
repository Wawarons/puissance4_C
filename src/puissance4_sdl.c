#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
//====================DÉCLARATIONS==============================

#define SIZE 100

#define NB_CASE_L 7
#define NB_CASE_H 6

#define L (NB_CASE_L * SIZE)
#define H (NB_CASE_H * SIZE)

static char grille[NB_CASE_L][NB_CASE_H];
char *gagnant = NULL;
int ia = 0;
int c1 = 500, c2 = 500, c3 = 500, c4 = 500, c5 = 500, c6 = 500, c7 = 500;
int *pc1 = &c1, *pc2 = &c2, *pc3 = &c3, *pc4 = &c4, *pc5 = &c5, *pc6 = &c6, *pc7 = &c7;

//================ENTÊTES DE FONCTIONS==================================

void SDL_ExitWithError(const char *message);
void dessiner_grille(SDL_Renderer *renderer);
void dessiner_jeton(SDL_Renderer *renderer, int x, int y, int w, int h, int tour);
int verification_partie_gagne();
void ajouter_grille(int r, int *pc, int tour);
void who_win(int tour);
void ia_play(SDL_Renderer *renderer, int r, int *pc, int tour);
void ia_easy(SDL_Renderer *renderer, int r, int tour);
void mySleep(int sleepMs);
int getRand(int *(val[7]));
void create_background(SDL_Renderer *r);

//================FONCTION MAIN=========================================

int main(int argc, char *argv[])
{
    int tour = 1;
    int boucle_jeu = 0, boucle_menu = 1;

    //---------------------------------------------------------------------------------------------------------------------------------------------------
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Lancement SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        SDL_ExitWithError("Initialisation SDL");

    // Création fenêtre
    window = SDL_CreateWindow("Puissance 4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, L + 400, H + 300, 0); // 0 pour mettre par défaut
    if (window == NULL)
        SDL_ExitWithError("Création fenetre echouee");

    // Création renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
        SDL_ExitWithError("Création rendu echouee");

    // BACKGROUND
    create_background(renderer);

    //----------------------------------------------------------------------

    // CRÉATION SURFACES ET TEXTURES
    SDL_Surface *surface_jcj = NULL;
    SDL_Surface *surface_jcia = NULL;
    SDL_Surface *surface_iacia = NULL;
    SDL_Texture *menu_jcj = NULL;
    SDL_Texture *menu_jcia = NULL;
    SDL_Texture *menu_iacia = NULL;

    // CHARGEMENT DES IMAGES
    surface_jcj = SDL_LoadBMP("src/images/menu_jcj.bmp");
    surface_jcia = SDL_LoadBMP("src/images/menu_jcia.bmp");
    surface_iacia = SDL_LoadBMP("src/images/menu_iacia.bmp");

    if (surface_jcj == NULL || surface_jcia == NULL || surface_iacia == NULL)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de charger l'une des images");
    }

    // CRÉATION DES TEXTURES
    menu_jcj = SDL_CreateTextureFromSurface(renderer, surface_jcj);
    menu_jcia = SDL_CreateTextureFromSurface(renderer, surface_jcia);
    menu_iacia = SDL_CreateTextureFromSurface(renderer, surface_iacia);

    // ON LIBÈRE LES SURFACES
    SDL_FreeSurface(surface_jcj);
    SDL_FreeSurface(surface_jcia);
    SDL_FreeSurface(surface_iacia);

    if (menu_jcj == NULL || menu_jcia == NULL || menu_iacia == NULL)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de créer l'une des images");
    }

    // CRÉATION DES RECTANGLES POUR LES IMAGES
    SDL_Rect rectangle_jcj;
    SDL_Rect rectangle_jcia;
    SDL_Rect rectangle_iacia;

    if (SDL_QueryTexture(menu_jcj, NULL, NULL, &rectangle_jcj.w, &rectangle_jcj.h) != 0)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de charger la texture menu_jcj");
    }

    if (SDL_QueryTexture(menu_jcia, NULL, NULL, &rectangle_jcia.w, &rectangle_jcia.h) != 0)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de charger la texture menu_jcia");
    }

    if (SDL_QueryTexture(menu_iacia, NULL, NULL, &rectangle_iacia.w, &rectangle_iacia.h) != 0)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible de charger la texture menu_iacia");
    }

    // POSITIONS DES RECTANGLES
    rectangle_jcj.x = ((L + 400) - rectangle_jcj.w) / 2;
    rectangle_jcj.y = (((H + 300) - rectangle_jcj.h) / 6) * 1;

    rectangle_jcia.x = ((L + 400) - rectangle_jcia.w) / 2;
    rectangle_jcia.y = (((H + 300) - rectangle_jcia.h) / 6) * 3;

    rectangle_iacia.x = ((L + 400) - rectangle_iacia.w) / 2;
    rectangle_iacia.y = (((H + 300) - rectangle_iacia.h) / 6) * 5;

    // COPIE LES TEXTURES DANS LES RECTANGLES
    if (SDL_RenderCopy(renderer, menu_jcj, NULL, &rectangle_jcj) != 0)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible d'afficher la texture menu_jcj");
    }

    if (SDL_RenderCopy(renderer, menu_jcia, NULL, &rectangle_jcia) != 0)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible d'afficher la texture menu_jcia");
    }

    if (SDL_RenderCopy(renderer, menu_iacia, NULL, &rectangle_iacia) != 0)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_ExitWithError("Impossible d'afficher la texture menu_iacia");
    }

    SDL_RenderPresent(renderer);

    //----------------------------------------------------------------------

    SDL_bool program_launched = SDL_TRUE;

    while (program_launched)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:

                // MENU
                if (boucle_menu)
                {
                    if (event.button.x > 430 && event.button.x < 670 && event.button.y > 130 && event.button.y < 265)
                    {
                        SDL_RenderClear(renderer);
                        create_background(renderer);
                        dessiner_grille(renderer);
                        boucle_menu = 0;
                        boucle_jeu = 1;
                        continue;
                    }

                    else if (event.button.x > 430 && event.button.x < 670 && event.button.y > 380 && event.button.y < 515)
                    {
                        ia = 1;
                        SDL_RenderClear(renderer);
                        create_background(renderer);
                        dessiner_grille(renderer);
                        boucle_menu = 0;
                        boucle_jeu = 1;
                        continue;
                    }

                    else if (event.button.x > 430 && event.button.x < 670 && event.button.y > 635 && event.button.y < 770)
                    {
                        ia = 2;
                        SDL_RenderClear(renderer);
                        create_background(renderer);
                        dessiner_grille(renderer);
                        boucle_menu = 0;
                        boucle_jeu = 1;
                        continue;
                    }

                    else
                    {
                        continue;
                    }
                }

                // Jeu
                if (boucle_jeu)
                {
                    int *val[7] = {pc1, pc2, pc3, pc4, pc5, pc6, pc7};
                    int pleine = 1;

                    // Vérifie si l'une des colonnes est vide
                    for (int i = 0; i < 7; i++)
                    {
                        if (*(val[i]) != -100)
                            pleine = 0;
                    }

                    // si la grille n'est pas pleine
                    if (pleine == 0 && gagnant == NULL)
                    {
                        if (ia != 2)
                        {

                            if (event.button.x < 300 && event.button.x > 200)
                            {
                                if (*pc1 > -100)
                                {
                                    dessiner_jeton(renderer, 200, (*pc1) + 150, SIZE, SIZE, tour);
                                    ajouter_grille(0, pc1, tour);
                                    if (verification_partie_gagne(0, *pc1 / 100) == 1)
                                        who_win(tour);
                                    tour++;
                                    *pc1 -= 100;
                                }
                            }
                            // CLIQUE DANS LA DEUXIÈME COLONNE
                            else if (event.button.x < 400 && event.button.x > 300)
                            {
                                if (*pc2 > -100)
                                {
                                    dessiner_jeton(renderer, 300, (*pc2) + 150, SIZE, SIZE, tour);
                                    ajouter_grille(1, pc2, tour);
                                    if (verification_partie_gagne(1, *pc2 / 100) == 1)
                                        who_win(tour);
                                    tour++;
                                    *pc2 -= 100;
                                }
                            }
                            // CLIQUE DANS LA TROISIÈME COLONNE
                            else if (event.button.x < 500 && event.button.x > 400)
                            {
                                if (*pc3 > -100)
                                {
                                    dessiner_jeton(renderer, 400, (*pc3) + 150, SIZE, SIZE, tour);
                                    ajouter_grille(2, pc3, tour);
                                    if (verification_partie_gagne(2, *pc3 / 100) == 1)
                                        who_win(tour);
                                    tour++;
                                    *pc3 -= 100;
                                }
                            }
                            // CLIQUE DANS LA QUATRIÈME COLONNE
                            else if (event.button.x < 600 && event.button.x > 500)
                            {
                                if (*pc4 > -100)
                                {
                                    dessiner_jeton(renderer, 500, (*pc4) + 150, SIZE, SIZE, tour);
                                    ajouter_grille(3, pc4, tour);
                                    if (verification_partie_gagne(3, *pc4 / 100) == 1)
                                        who_win(tour);
                                    tour++;
                                    *pc4 -= 100;
                                }
                            }
                            // CLIQUE DANS LA CINQUIÈME COLONNE
                            else if (event.button.x < 700 && event.button.x > 600)
                            {
                                if (*pc5 > -100)
                                {
                                    dessiner_jeton(renderer, 600, (*pc5) + 150, SIZE, SIZE, tour);
                                    ajouter_grille(4, pc5, tour);
                                    if (verification_partie_gagne(4, *pc5 / 100) == 1)
                                        who_win(tour);
                                    tour++;
                                    *pc5 -= 100;
                                }
                            }
                            // CLIQUE DANS LA SIXIÈME COLONNE
                            else if (event.button.x < 800 && event.button.x > 700)
                            {
                                if (*pc6 > -100)
                                {
                                    dessiner_jeton(renderer, 700, (*pc6) + 150, SIZE, SIZE, tour);
                                    ajouter_grille(5, pc6, tour);
                                    if (verification_partie_gagne(5, *pc6 / 100) == 1)
                                        who_win(tour);
                                    tour++;
                                    *pc6 -= 100;
                                }
                            }
                            // CLIQUE DANS LA SEPTIÈME COLONNE
                            else if (event.button.x < 900 && event.button.x > 800)
                            {
                                if (*pc7 > -100)
                                {
                                    dessiner_jeton(renderer, 800, (*pc7) + 150, SIZE, SIZE, tour);
                                    ajouter_grille(6, pc7, tour);
                                    if (verification_partie_gagne(0, *pc7 / 100) == 1)
                                        who_win(tour);
                                    tour++;
                                    *pc7 -= 100;
                                }
                            }

                            if (ia == 1 && gagnant == NULL)
                            { // si il y a une ia
                                if (tour % 2 == 0)
                                {
                                    int valR; // si le tour est pair
                                    valR = getRand(val);
                                    mySleep(500);
                                    ia_easy(renderer, valR, tour);
                                    tour++;
                                }
                            }
                        }
                        else
                        {
                            while (gagnant == NULL)
                            {
                                printf("%d %d %d %d %d %d %d\n", c1, c2, c3, c4, c5, c6, c7);
                                int valR = getRand(val);
                                ia_easy(renderer, valR, tour);
                                tour++;
                                if (gagnant == NULL)
                                {
                                    valR = getRand(val);
                                    mySleep(1000);
                                    ia_easy(renderer, valR, tour);
                                    tour++;
                                }
                            }
                        }
                    }
                    else
                    {
                        SDL_Surface *surface_end = NULL;
                        SDL_Texture *end = NULL;
                        if (pleine == 1)
                        {
                            surface_end = SDL_LoadBMP("src/images/égalité.bmp");
                        }
                        else if (gagnant == "Joueur 1")
                        {
                            surface_end = SDL_LoadBMP("src/images/j1_win.bmp");
                        }
                        else
                        {
                            surface_end = SDL_LoadBMP("src/images/j2_win.bmp");
                        }
                        end = SDL_CreateTextureFromSurface(renderer, surface_end);
                        SDL_Rect rectangle_end;
                        if (SDL_QueryTexture(menu_iacia, NULL, NULL, &rectangle_end.w, &rectangle_end.h) != 0)
                        {
                            SDL_DestroyRenderer(renderer);
                            SDL_DestroyWindow(window);
                            SDL_ExitWithError("Impossible de charger la texture end_end");
                        }
                        rectangle_end.x = (L + 600) / 3;
                        rectangle_end.y = (H + 600) / 3;
                        if (SDL_RenderCopy(renderer, end, NULL, &rectangle_end) != 0)
                        {
                            SDL_DestroyRenderer(renderer);
                            SDL_DestroyWindow(window);
                            SDL_ExitWithError("Impossible d'afficher la texture end_end");
                        }
                        SDL_RenderPresent(renderer);
                    }
                    continue;
                }

            case SDL_QUIT:
                program_launched = SDL_FALSE;
                break;

            default:
                break;
            }
        }
    }

    //----------------------------------------------------------------------

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

//======================================================================
//================FONCTIONS=============================================

/**
 * @brief Quitte la SDL avec affichage d'erreur.
 *
 * @param message
 */
void SDL_ExitWithError(const char *message)
{
    SDL_Log("Erreur : %s > %s\n", message, SDL_GetError());
    SDL_Quit(); // Il faut faire en sorte de quitter la SDL dans tous les cas
    exit(EXIT_FAILURE);
}

//----------------------------------------------------------------------

/**
 * @brief Dessine la grille à l'aide de la SDL.
 *
 * @param renderer
 */
void dessiner_grille(SDL_Renderer *renderer)
{
    unsigned col;
    unsigned lgn;

    for (col = 0; col < NB_CASE_L; ++col)
        for (lgn = 0; lgn < NB_CASE_H; ++lgn)
            grille[col][lgn] = ' ';

    if (SDL_SetRenderDrawColor(renderer, 147, 112, 219, SDL_ALPHA_OPAQUE) != 0)
        SDL_ExitWithError("Impossible de changer la couleur pour le rendu");

    SDL_Rect rect[NB_CASE_H * NB_CASE_L];
    size_t i = 0;
    for (i = 0; i < (NB_CASE_H * NB_CASE_L); i++)
    {
        rect[i].w = 100;
        rect[i].h = 100;
        rect[i].x = 100 * (i % (NB_CASE_H + 1)) + 200;
        rect[i].y = 100 * (i / NB_CASE_L) + 150;
    }
    SDL_RenderDrawRects(renderer, rect, (NB_CASE_H * NB_CASE_L));
    SDL_RenderPresent(renderer);
}

//----------------------------------------------------------------------

/**
 * @brief Dessine les jetons qui seront placé par l'utilisateur.
 *
 * @param renderer
 * @param x
 * @param y
 * @param w
 * @param h
 * @param tour
 */
void dessiner_jeton(SDL_Renderer *renderer, int x, int y, int w, int h, int tour)
{
    if (tour % 2 == 1) // si impair -> joueur 1 -> rouge
    {
        SDL_Surface *surface_coin1 = NULL;
        SDL_Texture *coin1 = NULL;
        surface_coin1 = SDL_LoadBMP("src/images/pcoin1.bmp");
        coin1 = SDL_CreateTextureFromSurface(renderer, surface_coin1);
        SDL_FreeSurface(surface_coin1);
        SDL_Rect rectangle_coin1;
        if (SDL_QueryTexture(coin1, NULL, NULL, &rectangle_coin1.w, &rectangle_coin1.h) != 0)
        {
            SDL_DestroyRenderer(renderer);

            SDL_ExitWithError("Impossible de charger la texture coin1");
        }

        rectangle_coin1.x = x;
        rectangle_coin1.y = y;
        rectangle_coin1.w = w;
        rectangle_coin1.h = h;
        if (SDL_RenderCopy(renderer, coin1, NULL, &rectangle_coin1) != 0)
        {
            SDL_DestroyRenderer(renderer);

            SDL_ExitWithError("Impossible d'afficher la texture coin1");
        }
        SDL_RenderPresent(renderer);
    }
    else // si pair -> joueur 2 -> jaune
    {
        SDL_Surface *surface_coin2 = NULL;
        SDL_Texture *coin2 = NULL;
        surface_coin2 = SDL_LoadBMP("src/images/pcoin2.bmp");
        coin2 = SDL_CreateTextureFromSurface(renderer, surface_coin2);
        SDL_FreeSurface(surface_coin2);
        SDL_Rect rectangle_coin2;
        if (SDL_QueryTexture(coin2, NULL, NULL, &rectangle_coin2.w, &rectangle_coin2.h) != 0)
        {
            SDL_DestroyRenderer(renderer);
            SDL_ExitWithError("Impossible de charger la texture coin2");
        }

        rectangle_coin2.x = x;
        rectangle_coin2.y = y;
        rectangle_coin2.w = w;
        rectangle_coin2.h = h;
        if (SDL_RenderCopy(renderer, coin2, NULL, &rectangle_coin2) != 0)
        {
            SDL_DestroyRenderer(renderer);
            SDL_ExitWithError("Impossible d'afficher la texture coin2");
        }
        SDL_RenderPresent(renderer);
    }
}

//----------------------------------------------------------------------

/**
 * @brief Verifie si la partie est gagné par l'un des joueurs.
 *
 * @return int
 */
int verification_partie_gagne()
{
    int i, j, k, count;
    for (i = 0; i < NB_CASE_L; i++)
    {
        for (j = 0; j < NB_CASE_H - 3; j++)
        {
            // checks horizontal win
            if (grille[i][j] != ' ' && grille[i][j] == grille[i][j + 1] && grille[i][j] == grille[i][j + 2] && grille[i][j] == grille[i][j + 3])
                return 1;
        }
    }

    for (i = 0; i < NB_CASE_L - 3; i++)
    {
        for (j = 0; j < NB_CASE_H; j++)
        {
            // checks vertical win
            if (grille[i][j] != ' ' && grille[i][j] == grille[i + 1][j] && grille[i][j] == grille[i + 2][j] && grille[i][j] == grille[i + 3][j])
                return 1;
        }
    }
    // checks rigth diagonal win
    for (i = 0; i < NB_CASE_L - 3; i++)
        for (j = 0; j < NB_CASE_H - 3; j++)
            if (grille[i][j] != ' ' && grille[i][j] == grille[i + 1][j + 1] && grille[i][j] == grille[i + 2][j + 2] && grille[i][j] == grille[i + 3][j + 3])
                return 1;

    // checks left diagonal win
    for (i = 0; i < NB_CASE_L - 3; i++)
        for (j = 3; j < NB_CASE_H; j++)
            if (grille[i][j] != ' ' && grille[i][j] == grille[i + 1][j - 1] && grille[i][j] == grille[i + 2][j - 2] && grille[i][j] == grille[i + 3][j - 3])
                return 1;
    return 0;
}

/**
 * @brief Ajoute un élément dans la matrice grille.
 *
 * @param r
 * @param pc
 * @param tour
 */
void ajouter_grille(int r, int *pc, int tour)
{
    if (tour % 2 == 0)
        grille[r][*(pc) / 100] = '0';
    else
        grille[r][*(pc) / 100] = 'X';
}

/**
 * @brief Donne le nom du joueur gagnant.
 *
 * @param tour
 */
void who_win(int tour)
{
    if (tour % 2 == 0)
        gagnant = "Joueur 2";
    else
        gagnant = "Joueur 1";
}

/**
 * @brief fonction permettant à l'ia de jouer.
 *
 * @param renderer
 * @param r
 * @param pc
 * @param tour
 */
void ia_play(SDL_Renderer *renderer, int r, int *pc, int tour)
{
    dessiner_jeton(renderer, (r * 100) + 200, (*pc) + 150, SIZE, SIZE, tour);
    ajouter_grille(r, pc, tour);
    if (verification_partie_gagne(r, (*pc) / 100) == 1)
        who_win(tour);
    *pc -= 100;
}

/**
 * @brief Intelligence artificiel niveau facile.
 *
 * @param renderer
 * @param r
 * @param tour
 */
void ia_easy(SDL_Renderer *renderer, int r, int tour)
{
    switch (r)
    {
    case 0:
        ia_play(renderer, r, pc1, tour);
        break;
    case 1:
        ia_play(renderer, r, pc2, tour);
        break;
    case 2:
        ia_play(renderer, r, pc3, tour);
        break;
    case 3:
        ia_play(renderer, r, pc4, tour);
        break;
    case 4:
        ia_play(renderer, r, pc5, tour);
        break;
    case 5:
        ia_play(renderer, r, pc6, tour);
        break;
    case 6:
        ia_play(renderer, r, pc7, tour);
        break;
    default:
        break;
    }
}

/**
 * @brief Délai d'attente
 *
 * @param sleepMs
 */
void mySleep(int sleepMs)
{
#ifdef _WIN32
    Sleep(sleepMs);
#else
    sleep(sleepMs * 1000);
#endif
}

/**
 * @brief Get the Rand object
 *
 * @param val
 * @return int
 */
int getRand(int *val[7])
{
    int r = (rand() % (7));
    while (*(val[r]) <= -100)
    {
        r = rand() % 7;
    }
    return r;
}

/**
 * @brief Create a background object
 *
 * @param r
 */
void create_background(SDL_Renderer *r)
{
    SDL_Surface *surface_background = NULL;
    SDL_Texture *texture_background = NULL;
    surface_background = SDL_LoadBMP("src/images/big_new_background.bmp");

    if (surface_background == NULL)
    {
        SDL_DestroyRenderer(r);
        SDL_ExitWithError("Impossible de charger l'image du background");
    }

    texture_background = SDL_CreateTextureFromSurface(r, surface_background);
    SDL_FreeSurface(surface_background);

    if (texture_background == NULL)
    {
        SDL_DestroyRenderer(r);

        SDL_ExitWithError("Impossible de créer le background");
    }

    SDL_Rect rectangle_background;
    rectangle_background.x = 0;       // Coordonnee X du point en haut à gauche du rectangle
    rectangle_background.y = 0;       // Coordonnee Y du point en haut à gauche du rectangle
    rectangle_background.w = L + 400; // Largeur du rectangle
    rectangle_background.h = H + 300; // Hauteur du rectangle

    if (SDL_QueryTexture(texture_background, NULL, NULL, &rectangle_background.w, &rectangle_background.h) != 0)
    {
        SDL_DestroyRenderer(r);

        SDL_ExitWithError("Impossible de charger la texture background");
    }

    if (SDL_RenderCopy(r, texture_background, NULL, &rectangle_background) != 0)
    {
        SDL_DestroyRenderer(r);
        SDL_ExitWithError("Impossible d'afficher la texture background");
    }
}