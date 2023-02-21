#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define NB_COLONNES (7)
#define NB_LIGNES (6)

#define P1_PION ('O')
#define P2_PION ('X')

#define ACT_ERR (0)
#define ACT_JOUER (1)
#define ACT_NOUVELLE_SAISIE (2)
#define ACT_QUITTER (3)

#define CONTINUE (0)
#define STATUT_GAGNE (1)
#define STATUT_EGALITE (2)

struct position
{
    int colonne;
    int ligne;
};

static void affiche_grille(void);
//Affiche la grille du puissance 4.

static void calcule_position(int, struct position *);
static unsigned calcule_nb_jetons_depuis_vers(struct position *, int, int, char);
static unsigned calcule_nb_jetons_depuis(struct position *, char);
static int test_coup(int);
static int demande_action(int *);
static int grille_complete(void);
static void initialise_grille(void);
static int test_position(struct position *);
static int statut_jeu(struct position *pos, char);
static unsigned maxi(unsigned, unsigned);
static int vider_tampon(FILE *);

static char grille[NB_COLONNES][NB_LIGNES];

/**
 * @brief Affiche la grille du puissance 4.
 *
 */
static void affiche_grille(void)
{
    int col;
    int lgn;

    putchar('\n');

    for (col = 1; col <= NB_COLONNES; ++col)
        printf("  %d ", col);

    putchar('\n');
    putchar('+');

    for (col = 1; col <= NB_COLONNES; ++col)
        printf("---+");

    putchar('\n');

    for (lgn = 0; lgn < NB_LIGNES; ++lgn)
    {
        putchar('|');

        for (col = 0; col < NB_COLONNES; ++col)
            if (isalpha(grille[col][lgn]))
                printf(" %c |", grille[col][lgn]);
            else
                printf(" %c |", ' ');

        putchar('\n');
        putchar('+');

        for (col = 1; col <= NB_COLONNES; ++col)
            printf("---+");

        putchar('\n');
    }

    for (col = 1; col <= NB_COLONNES; ++col)
        printf("  %d ", col);

    putchar('\n');
}

/**
 * @brief Traduit le coup joué en un numéro de colonne et de ligne.
 *
 * @param coup
 * @param pos
 */
static void calcule_position(int coup, struct position *pos)
{
    int lgn;

    pos->colonne = coup;

    for (lgn = NB_LIGNES - 1; lgn >= 0; --lgn)
        if (grille[pos->colonne][lgn] == ' ')
        {
            pos->ligne = lgn;
            break;
        }
}

/**
 * @brief Calcule le nombre de jetons adajcents identiques depuis une position donnée en se
 * déplaçant de `dpl_hrz` horizontalement et `dpl_vrt` verticalement.
 * La fonction s'arrête si un jeton différent ou une case vide est rencontrée ou si les limites de la grille sont atteintes.
 *
 * @param pos
 * @param dpl_hrz
 * @param dpl_vrt
 * @param jeton
 * @return unsigned
 */
static unsigned calcule_nb_jetons_depuis_vers(struct position *pos, int dpl_hrz, int dpl_vrt, char jeton)
{

    struct position tmp;
    unsigned nb = 1;

    tmp.colonne = pos->colonne + dpl_hrz;
    tmp.ligne = pos->ligne + dpl_vrt;

    while (test_position(&tmp))
    {
        if (grille[tmp.colonne][tmp.ligne] == jeton)
            ++nb;
        else
            break;

        tmp.colonne += dpl_hrz;
        tmp.ligne += dpl_vrt;
    }

    return nb;
}

/**
 * @brief Calcule le nombre de jetons adjacents en vérifant la colonne courante,
 * de la ligne courante et des deux obliques courantes.
 * Pour ce faire, la fonction calcule_nb_jeton_depuis_vers() est appelé à
 * plusieurs reprises afin de parcourir la grille suivant la vérification
 * à effectuer.
 *
 * @param pos
 * @param jeton
 * @return unsigned
 */
static unsigned calcule_nb_jetons_depuis(struct position *pos, char jeton)
{
    /*
     * Calcule le nombre de jetons adjacents en vérifant la colonne courante,
     * de la ligne courante et des deux obliques courantes.
     * Pour ce faire, la fonction calcule_nb_jeton_depuis_vers() est appelé à
     * plusieurs reprises afin de parcourir la grille suivant la vérification
     * à effectuer.
     */

    unsigned max;

    max = calcule_nb_jetons_depuis_vers(pos, 0, 1, jeton);
    max = maxi(max, calcule_nb_jetons_depuis_vers(pos, 1, 0, jeton) +
                        calcule_nb_jetons_depuis_vers(pos, -1, 0, jeton) - 1);
    max = maxi(max, calcule_nb_jetons_depuis_vers(pos, 1, 1, jeton) +
                        calcule_nb_jetons_depuis_vers(pos, -1, -1, jeton) - 1);
    max = maxi(max, calcule_nb_jetons_depuis_vers(pos, 1, -1, jeton) +
                        calcule_nb_jetons_depuis_vers(pos, -1, 1, jeton) - 1);

    return max;
}

/**
 * @brief Si la colonne renseignée est inférieure ou égal à zéro
 * ou que celle-ci est supérieure à la longueur du tableau
 * ou que la colonne indiquée est saturée
 * alors le coup est invalide.
 *
 * @param col
 * @return int
 */
static int test_coup(int col)
{

    if (col <= 0 || col > NB_COLONNES || grille[col - 1][0] != ' ')
        return 0;

    return 1;
}

/**
 * @brief Demande l'action à effectuer au joueur courant.
 * S'il entre un chiffre, c'est qu'il souhaite jouer.
 * S'il entre la lettre « Q » ou « q », c'est qu'il souhaite quitter.
 * S'il entre autre chose, une nouvelle saisie sera demandée.
 *
 * @param coup
 * @return int
 */
static int demande_action(int *coup)
{

    char c;
    int ret = ACT_ERR;

    if (scanf("%d", coup) != 1)
    {
        if (scanf("%c", &c) != 1)
        {
            fprintf(stderr, "Erreur lors de la saisie\n");
            return ret;
        }

        switch (c)
        {
        case 'Q':
        case 'q':
            ret = ACT_QUITTER;
            break;
        default:
            ret = ACT_NOUVELLE_SAISIE;
            break;
        }
    }
    else
        ret = ACT_JOUER;

    if (!vider_tampon(stdin))
    {
        fprintf(stderr, "Erreur lors de la vidange du tampon.\n");
        ret = ACT_ERR;
    }

    return ret;
}

/**
 * @brief Détermine si la grille de jeu est complète.
 *
 * @return int
 */
static int grille_complete(void)
{

    unsigned col;
    unsigned lgn;

    for (col = 0; col < NB_COLONNES; ++col)
        for (lgn = 0; lgn < NB_LIGNES; ++lgn)
            if (grille[col][lgn] == ' ')
                return 0;

    return 1;
}

/**
 * @brief Initalise les caractères de la grille.
 *
 */
static void initialise_grille(void)
{

    unsigned col;
    unsigned lgn;

    for (col = 0; col < NB_COLONNES; ++col)
        for (lgn = 0; lgn < NB_LIGNES; ++lgn)
            grille[col][lgn] = ' ';
}

/**
 * @brief Vérifie que la position fournie est bien comprise dans la grille.
 *
 * @param pos
 * @return int
 */
static int test_position(struct position *pos)
{
    int ret = 1;

    if (pos->colonne >= NB_COLONNES || pos->colonne < 0)
        ret = 0;
    else if (pos->ligne >= NB_LIGNES || pos->ligne < 0)
        ret = 0;

    return ret;
}

/**
 * @brief Détermine s'il y a lieu de continuer le jeu ou s'il doit être
 * arrêté parce qu'un joueur a gagné ou que la grille est complète.
 *
 * @param pos
 * @param jeton
 * @return int
 */
static int statut_jeu(struct position *pos, char jeton)
{

    if (grille_complete())
        return STATUT_EGALITE;
    else if (calcule_nb_jetons_depuis(pos, jeton) >= 4)
        return STATUT_GAGNE;

    return CONTINUE;
}

/**
 * @brief Retourne le plus grand des deux arguments.
 * 
 * @param a 
 * @param b 
 * @return unsigned 
 */
static unsigned maxi(unsigned a, unsigned b)
{

    return (a > b) ? a : b;
}

/**
 * @brief Vide les données en attente de lecture du flux spécifié.
 * 
 * @param fp 
 * @return int 
 */
static int vider_tampon(FILE *fp)
{

    int c;

    do
        c = fgetc(fp);
    while (c != '\n' && c != EOF);

    return ferror(fp) ? 0 : 1;
}

int main(void)
{
    int statut;
    char jeton = P1_PION;

    initialise_grille();
    affiche_grille();

    while (1)
    {
        struct position pos;
        int action;
        int coup;

        printf("Joueur %d : ", (jeton == P1_PION) ? 1 : 2);

        action = demande_action(&coup);

        if (action == ACT_ERR)
            return EXIT_FAILURE;
        else if (action == ACT_QUITTER)
            return 0;
        else if (action == ACT_NOUVELLE_SAISIE || !test_coup(coup))
        {
            fprintf(stderr, "Vous ne pouvez pas jouer à cet endroit\n");
            continue;
        }

        calcule_position(coup - 1, &pos);
        grille[pos.colonne][pos.ligne] = jeton;
        affiche_grille();
        statut = statut_jeu(&pos, jeton);

        if (statut != CONTINUE)
            break;

        jeton = (jeton == P1_PION) ? P2_PION : P1_PION;
    }

    if (statut == STATUT_GAGNE)
        printf("Le joueur %d a gagné\n", (jeton == P1_PION) ? 1 : 2);
    else if (statut == STATUT_EGALITE)
        printf("Égalité\n");

    return 0;
}