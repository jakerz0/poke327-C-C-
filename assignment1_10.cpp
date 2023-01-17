#include <cmath>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cstring>
#include <ncurses.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstdarg>
#include "heap.h"

#define XDIM 80
#define YDIM 21

typedef enum
{
    clearing,
    tall_grass,
    trees,
    rocks,
    road,
    world_border,
    poke_mart,
    poke_center,
    road_border
} terrain;

// values:        0           1         2      3     4      5       6            7        8
int traveltimes[][9] =
    {
        {10, 20, INT_MAX, INT_MAX, 10, INT_MAX, 10, 10, 10},     // PC
        {10, 15, 15, 15, 10, INT_MAX, 50, 50, INT_MAX},          // hiker
        {10, 20, INT_MAX, INT_MAX, 10, INT_MAX, 50, 50, INT_MAX} // rival
};

typedef struct pokemon
{
    int id;
    char identifier[50];
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
} pokemon_t;

typedef struct move
{
    int id;
    char identifier[50];
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;

} move_t;

typedef struct pokemon_move
{
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

} pokemon_move_t;

typedef struct type_name
{
    int type_id;
    int local_language_id;
    char name[20];

} type_name_t;

typedef struct pokemon_stat
{
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;

} pokemon_stat_t;

typedef struct stat
{
    int id;
    int damage_class_id;
    char *identifier;
    int is_battle_only;
    int game_index;

} stat_t;

typedef struct pokemon_type
{
    int pokemon_id;
    int type_id;
    int slot;

} pokemon_type_t;

typedef struct experience
{
    int growth_rate_id;
    int level;
    int experience;

} experience_t;

typedef struct pokemon_species
{
    int id;
    char *identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

} pokemon_species_t;

// class map_t;

class pokemon_character
{
private:
    int hp_b;
    int attack_b;
    int defense_b;
    int speed_b;
    int special_attack_b;
    int special_defense_b;

    int hp_iv;
    int attack_iv;
    int defense_iv;
    int speed_iv;
    int special_attack_iv;
    int special_defense_iv;

public:
    int id_true;
    int hp_true;
    int attack_true;
    int defense_true;
    int speed_true;
    int special_attack_true;
    int special_defense_true;

    int level;

    std::string species;
    int type1_id;
    int type2_id;
    int move1_id;
    int move2_id;
    int experience;
    int gender; // 0 = m, 1 = f
    int shiny;  // 1 is true

    int current_hp;
    bool is_down;
    // bool is_active;

    pokemon_character()
    {
    }
    pokemon_character(int id, int hp, int attack, int defense, int speed, int special_attack, int special_defense,
                      std::string species, int level, int type1, int type2)
    {
        this->id_true = id;
        this->hp_b = hp;
        this->attack_b = attack;
        this->defense_b = defense;
        this->speed_b = speed;
        this->special_attack_b = special_attack;
        this->special_defense_b = special_defense;

        this->species = species;
        this->gender = rand() % 2;
        this->experience = 0;
        this->level = level;
        this->type1_id = type1;
        this->type2_id = type2;

        move1_id = 0;
        move2_id = 0;

        if ((rand() % 8192) == 0)
        {
            shiny = 1;
        }
        else
        {
            shiny = 0;
        }

        hp_iv = rand() % 15;
        attack_iv = rand() % 15;
        defense_iv = rand() % 15;
        speed_iv = rand() % 15;
        special_attack_iv = rand() % 15;
        special_defense_iv = rand() % 15;

        hp_true = (((hp_b + hp_iv) * 2) * level) / 100 + level + 10;
        attack_true = (((attack_b + attack_iv) * 2) * level) / 100 + 5;
        defense_true = (((defense_b + defense_iv) * 2) * level) / 100 + 5;
        speed_true = (((speed_b + speed_iv) * 2) * level) / 100 + 5;
        special_attack_true = (((special_attack_b + special_attack_iv) * 2) * level) / 100 + 5;
        special_defense_true = (((special_defense_b + special_defense_iv) * 2) * level) / 100 + 5;

        current_hp = hp_true; // set hp to full initially
        is_down = false;
    }
    ~pokemon_character()
    {
    }

    int get_b_speed()
    {
        return speed_b;
    }
};

class item
{
public:
    int count;
    char *type;

    item() { count = 1; }
    item(int i) { count = i; };
    void add()
    {
        count++;
    }
    void add(int i)
    {
        count += i;
    }
    virtual ~item()
    { /* */
    }

    virtual int use(pokemon_character *pokemon) = 0;
};

class potion : public item
{
public:
    potion() : item()
    {
        type = (char *)"potion";
    }
    potion(int i) : item(i)
    {
        type = (char *)"potion";
    }
    ~potion()
    { /* */
    }

    int use(pokemon_character *pokemon)
    {
        int heal = 10 + rand() % 10 + 1;
        if (pokemon->current_hp + heal > pokemon->hp_true)
        {
            pokemon->current_hp = pokemon->hp_true;
            mvprintw(22, 0, "%s is now at full hp!", pokemon->species.c_str());
            refresh();
            usleep(2000000);
            move(22, 0);
            clrtoeol();
            refresh();
        }
        else
        {
            pokemon->current_hp += heal;
            mvprintw(22, 0, "%s has recovered %d hp", heal, pokemon->species.c_str());
            refresh();
            usleep(2000000);
            move(22, 0);
            clrtoeol();
            refresh();
        }
        /*
        WINDOW *effect_win = newwin(6, 26, 10, 27);
        keypad(effect_win, TRUE);
        curs_set(0);
        mvwprintw(effect_win, 0, 0, "##########################");
        mvwprintw(effect_win, 1, 0, "#                        #");
        mvwprintw(effect_win, 2, 0, "#                        #");
        mvwprintw(effect_win, 3, 0, "#                        #");
        mvwprintw(effect_win, 4, 0, "#                        #");
        mvwprintw(effect_win, 5, 0, "##########################");
        mvwprintw(effect_win, 2, 4, "%s healed %d", pokemon->species.c_str(), heal);
        mvwprintw(effect_win, 3, 6, "hp now at %d", pokemon->current_hp);
        wrefresh(effect_win);
        getch();
        werase(effect_win);
        wrefresh(effect_win);
        delwin(effect_win);
        //refresh();
        */
        count--;
        return 0;
    }
};

class revive : public item
{
public:
    revive() : item()
    {
        type = (char *)"revive";
    }
    revive(int i) : item(i)
    {
        type = (char *)"revive";
    }
    ~revive()
    { /* */
    }

    int use(pokemon_character *pokemon)
    {
        /*
        WINDOW *effect_win = newwin(6, 26, 10, 27);
        keypad(effect_win, TRUE);
        curs_set(0);
        mvwprintw(effect_win, 0, 0, "##########################");
        mvwprintw(effect_win, 1, 0, "#                        #");
        mvwprintw(effect_win, 2, 0, "#                        #");
        mvwprintw(effect_win, 3, 0, "#                        #");
        mvwprintw(effect_win, 4, 0, "#                        #");
        mvwprintw(effect_win, 5, 0, "##########################");
        */

        if (!pokemon->is_down)
        {
            /*
            mvwprintw(effect_win, 2, 2, "%s is conscious", pokemon->species.c_str());
            mvwprintw(effect_win, 3, 2, "cannot use this item");
            wrefresh(effect_win);
            getch();
            delwin(effect_win);
            */
            mvprintw(22, 0, "%s is conscious, you cannot use that", pokemon->species.c_str());
            refresh();
            usleep(2000000);
            move(22, 0);
            clrtoeol();
            refresh();
            return -1; // if used on an undowned pokemon
        }
        pokemon->is_down = false;
        pokemon->current_hp = pokemon->hp_true;

        mvprintw(23, 0, "%s is revived with full hp!", pokemon->species.c_str());
        /*
        mvwprintw(effect_win, 2, 3, "%s is revived", pokemon->species.c_str());
        mvwprintw(effect_win, 3, 6, "hp is full!");
        wrefresh(effect_win);
        getch();
        werase(effect_win);
        wrefresh(effect_win);
        delwin(effect_win);
        */
        count--;
        return 0;
    }
};

class pokeball : public item
{
public:
    pokeball() : item()
    {
        type = (char *)"pokeball";
    }
    pokeball(int i) : item(i)
    {
        type = (char *)"pokeball";
    }
    ~pokeball()
    { /* */
    }

    int use(pokemon_character *pokemon)
    {
        count--;
        return 0;
    }
};

class character_t
{
public:
    std::vector<item *> bag;
    std::vector<pokemon_character *> my_pokemon;

    pokemon_character *active_pokemon;

    bool is_pc;
    int x, y;
    char type;
    char direction;
    int hostile;
    int pokebucks;

    character_t()
    {
    }
    ~character_t()
    {
    }
};

class player : public character_t
{
public:
    player()
    {
        is_pc = true;
        item *pokeballs = new pokeball(6);
        item *potions = new potion(6);
        item *revives = new revive();
        bag.push_back(pokeballs);
        bag.push_back(potions);
        bag.push_back(revives);
        pokebucks = 100;
    }

    ~player()
    {
    }
};

class npc : public character_t
{
public:
    npc()
    {
        is_pc = false;
    }

    ~npc()
    {
    }
};

class map_t
{
public:
    terrain t[YDIM][XDIM];
    character_t char_map[YDIM][XDIM];
    heap_t move_priority;
    int ns;
    int ew;

    map_t()
    {
    }
};

struct world
{
public:
    map_t *w[401][401]; // pointer so you only save loaded map tiles
    world()
    {
    }

    std::vector<pokemon_character *> pokemon_storage;

    std::vector<pokemon_t *> ptv;
    std::vector<move_t *> mtv;
    std::vector<pokemon_move_t *> pmtv;
    std::vector<type_name_t *> tntv;
    std::vector<pokemon_stat_t *> pstv;
    std::vector<stat_t *> stv;
    std::vector<pokemon_type_t *> pttv;
    std::vector<experience_t *> etv;
    std::vector<pokemon_species_t *> psptv;
};

struct world world;

void print_map(map_t *m, int size)
{
    int y, x;
    // char map[YDIM][XDIM];
    // map = m->t;
    // printf("printing map\n");
    clear();

    printw("  \n");

    for (y = 0; y < YDIM; y++)
    {
        for (x = 0; x < XDIM; x++)
        {
            terrain t = m->t[y][x];

            if (m->char_map[y][x].is_pc) // if it is pc
            {
                attron(COLOR_PAIR(1));
                printw("%c", 64); // '@'
                attroff(COLOR_PAIR(1));
            }
            else if (!m->char_map[y][x].is_pc && m->char_map[y][x].type != '\000')
            {
                attron(COLOR_PAIR(1));
                printw("%c", m->char_map[y][x].type);
                attroff(COLOR_PAIR(1));
            }
            else
            {
                switch (t)
                {
                case clearing:
                    attron(COLOR_PAIR(2));
                    printw("."); // clearing
                    attroff(COLOR_PAIR(2));
                    break;
                case tall_grass:
                    attron(COLOR_PAIR(2));
                    printw(":"); // tall_grass
                    attroff(COLOR_PAIR(2));
                    break;
                case trees:
                    attron(COLOR_PAIR(3));
                    printw("^"); // trees
                    attroff(COLOR_PAIR(3));
                    break;
                case rocks:
                    attron(COLOR_PAIR(4));
                    printw("%%"); // rocks
                    attroff(COLOR_PAIR(4));
                    break;
                case road:
                    attron(COLOR_PAIR(5));
                    printw("#"); // road
                    attroff(COLOR_PAIR(5));
                    break;
                case world_border:
                    attron(COLOR_PAIR(4));
                    printw("%%"); // border (BEHAVES DIF THAN ROCKS)
                    attroff(COLOR_PAIR(4));
                    break;
                case poke_mart:
                    attron(COLOR_PAIR(6));
                    printw("M"); // mart
                    attroff(COLOR_PAIR(6));
                    break;
                case poke_center:
                    attron(COLOR_PAIR(6));
                    printw("C"); // center
                    attroff(COLOR_PAIR(6));
                    break;
                case road_border:
                    attron(COLOR_PAIR(5));
                    printw("#"); // exit (BORDER BUT A ROAD)
                    attroff(COLOR_PAIR(5));
                }
            }
        }
        printw("\n");
    }
    // mvwprintw(stdscr, 0, 0, "top line!");
    refresh();
}

void tree(terrain map[YDIM][XDIM], int y, int x)
{
    int newY, newX;
    int i, j;
    terrain type = trees;
    int w;

    w = 3;
    for (i = -3; i < 5 + w; i++)
    {
        newY = i + y;

        for (j = -w; j < w; j++)
        {
            newX = j + x;

            if (newX < 1 || newX > 79)
                continue; // if it is off edge of x
            if (newY < 1 || newY > 19)
                continue; // if it is off the edge of y

            // if(map[newY][newX] != '.') continue;
            // if(map[newY][newX] != ':') continue;

            map[newY][newX] = type;
        }
        if (i > 0)
            w--;
        if (i < 0)
            w++;
    }
}

void boulder(terrain map[YDIM][XDIM], int y, int x)
{
    int newY, newX;
    int i, j;
    int w = 1;
    int z = -1;
    terrain type = rocks;

    for (i = -2; i < 2 + w; i++)
    {
        newY = y + i;
        for (j = -3 - w; j < 1 + z; j++)
        {
            newX = x + j;
            if (newX < 1 || newX > 79)
                continue; // if it is off edge of x
            if (newY < 1 || newY > 19)
                continue; // if it is off the edge of y
            map[newY][newX] = type;
            if (j > 0)
                z = z - 1;
            else
                z = z + rand() % 4;
        }
        // for(j = -5,
        if (i > 0)
            w = w - 1;
        if (i < 0)
            w = w + rand() % 5;
    }
}

void grass(terrain map[YDIM][XDIM], int y, int x)
{
    int newY, newX;
    int i, j;
    int w;
    terrain type = tall_grass;
    terrain valid = clearing;

    w = 1;
    for (i = -6; i < 6; i++)
    {
        newY = i + y;
        if (i < 0)
            w = w + abs(i);
        if (i > 0)
            w = w - i;
        for (j = 0; j < w; j++)
        {
            newX = j + x;
            if (newX < 1 || newX > 79)
                continue; // if it is off edge of x
            if (newY < 1 || newY > 19)
                continue; // if it is off the edge of y

            if (map[newY][newX] != valid)
                continue;

            map[newY][newX] = type;
            // printMap(map);
            // usleep(10000);
        }
        for (j = 0; j > -w; j--)
        {
            newX = j + x;
            if (newX < 1 || newX > 79)
                continue; // if it is off edge of x
            if (newY < 1 || newY > 19)
                continue; // if it is off the edge of y

            if (map[newY][newX] != valid)
                continue;

            map[newY][newX] = type;
        }
    }
    w = 1;
}

int power(int base, int exp)
{
    int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
}

// a is y, b is x for placement on the whole world map
void paint_map(map_t *m, int a, int b)
{
    int y, x;
    int i, j;
    int center, mart;
    terrain map_border = world_border;
    terrain short_grass = clearing;

    // making the edges
    for (i = 0; i < YDIM; i++)
    {
        m->t[i][0] = world_border;
        m->t[i][XDIM - 1] = map_border;
    }
    for (i = 0; i < XDIM; i++)
    {
        m->t[0][i] = world_border;
        m->t[YDIM - 1][i] = map_border;
    }

    for (i = 1; i < YDIM - 1; i++)
    {
        for (j = 1; j < XDIM - 1; j++)
        {
            m->t[i][j] = short_grass;
        }
    }

    // print_map(m);

    for (i = 0; i < 3; i++)
    {
        x = rand() % 78;
        y = rand() % 18;
        grass(m->t, y + 1, x + 1);
    }

    // placing some trees down
    y = rand() % 19 + 1;
    x = rand() % 8 + 1;
    tree(m->t, y, x);
    // usleep(100);
    y = rand() % 8 + 1;
    x = rand() % 70 + 4;
    tree(m->t, y, x);
    // boulders
    y = rand() % 17 + 1;
    x = rand() % 70 + 1;
    boulder(m->t, y, x);

    // roads
    y = 10;
    x = 35;
    int distance = (int)sqrt((double)(power(a - 200, 2) + power(b - 200, 2)));
    int chance = abs(-45 * distance / 200 + 50);

    terrain mart_terrain = poke_mart;
    terrain center_terrain = poke_center;
    terrain road_terrain = road;
    terrain road_exit = road_border;

    if (distance > 200)
        chance = 5; // flat 5% chance for buildings 200+ out

    // printf("%d -> %d  \n", distance, chance);
    center = 0;
    mart = 0;

    if (a == 0)
    {
        for (i = 21; i > y - 1; i--)
        {
            m->t[i][x] = road_terrain;
            center = -3;
        }
        m->t[21][x] = road_exit; // make the bottom border road tile an exit
    }
    else if (a == 400)
    { // 400 bc edge of array is index 400
        for (i = 0; i < y + 1; i++)
        {
            m->t[i][x] = road_terrain;
            center = -3;
        }
        m->t[0][x] = road_exit; // make the top border road tile an exit
    }
    else
    {
        for (i = 0; i < 21; i++)
        {
            m->t[i][x] = road_terrain;
        }
        m->t[20][x] = road_exit; // make the top and bottom border road tiles exits
        m->t[0][x] = road_exit;
    }

    if (b == 0)
    {
        for (i = 79; i > x - 1; i--)
        {
            m->t[y][i] = road_terrain;
            mart = 3;
            center = 3;
        }
        m->t[y][79] = road_exit; // make the right border road tile an exit
    }
    else if (b == 400)
    {
        for (i = 0; i < x + 1; i++)
        {
            m->t[y][i] = road_terrain;
            center = -3;
            mart = -3;
        }
        m->t[y][0] = road_exit; // make the left border road tile an exit
    }
    else
    {
        for (i = 0; i < 80; i++)
        {
            m->t[y][i] = road_terrain;
        }
        m->t[y][79] = road_exit; // make the right and left border road tiles exits
        m->t[y][0] = road_exit;
    }

    if (rand() % 99 > chance)
    {
        m->t[11][36 + center] = center_terrain;
        m->t[12][36 + center] = center_terrain;
        m->t[11][37 + center] = center_terrain;
        m->t[12][37 + center] = center_terrain;

        m->t[9][36 + mart] = mart_terrain;
        m->t[8][36 + mart] = mart_terrain;
        m->t[9][37 + mart] = mart_terrain;
        m->t[8][37 + mart] = mart_terrain;
    }
}

int get_weight(terrain t, int npc)
{

    switch (t)
    {
    case clearing:
        return traveltimes[npc][0];
        break;
    case tall_grass:
        return traveltimes[npc][1];
        break;
    case trees:
        return traveltimes[npc][2];
        break;
    case rocks:
        return traveltimes[npc][3];
        break;
    case road:
        return traveltimes[npc][4];
        break;
    case world_border:
        return traveltimes[npc][5];
        break;
    case poke_mart:
        return traveltimes[npc][6];
        break;
    case poke_center:
        return traveltimes[npc][7];
        break;
    case road_border:
        return traveltimes[npc][8];
        break;
    }

    return 0;
}

int distance_map(map_t *m, char npc, character_t pc, int weightmap[YDIM][XDIM])
{

    int npc_code = 0;
    // map_t dmap;
    // int weightmap[YDIM][XDIM];
    heap_t h;

    int i, j;

    // int counter = 1;

    if (npc == 'h')
    {
        npc_code = 1; // hiker
    }
    else if (npc == 'r')
    {
        npc_code = 2; // rival
    }
    else
    {
        // printf("bad npc marker\n");
        return 1;
    }

    heap_init(&h, XDIM * YDIM);

    /*
      initializing all travel times to infinity
     */
    for (i = 0; i < YDIM; i++)
    {
        for (j = 0; j < XDIM; j++)
        {

            if (get_weight(m->t[i][j], npc_code) != INT_MAX)
            {
                heap_add(&h, INT_MAX, j, i);
            } // add node
            else
            {
                // printf("x: %d y: %d\n", j, i);
                weightmap[i][j] = INT_MAX;
            }
        }
    }

    // printf("setting pc at %d, %d\n",pc.x,pc.y);
    // weightmap[pc.y][pc.x]->weight = 0;
    for (i = 0; i < h.size; i++)
    {
        if (h.q[i].x == pc.x)
        {
            if (h.q[i].y == pc.y)
            {
                h.q[i].weight = 0; // setting pc position
            }
        }
    }

    heap_sortify(&h);

    // djikstra implementation!!!

    heap_node_t u;
    int k;
    int alt_dist;
    while (!is_empty(&h))
    {

        u = heap_pop(&h);
        if (u.index == 0)
            continue;

        for (i = -1; i <= 1; i++)
        {
            for (j = -1; j <= 1; j++)
            {

                if (i == 0 && j == 0)
                    continue;
                // printf("x: %d y: %d\n",u.x + i, u.y + j);

                int newX = u.x + i;
                int newY = u.y + j;

                if (newX < 1 || newX > 79)
                    continue;
                if (newY < 1 || newY > 20)
                    continue;

                for (k = 0; k < h.size; k++)
                {
                    if (h.q[k].x == newX)
                    {
                        if (h.q[k].y == newY)
                        {
                            // printf("assigning weight\n");

                            // if(h.q[k]) continue;

                            alt_dist = u.weight +
                                       get_weight(m->t[newY][newX], npc_code);

                            // printf("%d vs %d\n", alt_dist, h.q[k].weight);

                            /*
                            if(alt_dist == INT_MIN){
                          print_node(&h.q[k]);
                          print_map(m);
                            }
                            */

                            if (alt_dist < h.q[k].weight)
                            {

                                h.q[k].weight = alt_dist;
                                weightmap[newY][newX] = alt_dist;

                                heapify_down(&h, &h.q[k]);
                            }
                        }
                    }
                } // end of k loop
            }
        }
        heap_sortify(&h);
    }

    // for printing
    /*
    for(i = 0; i < YDIM; i++){
      for(j = 0; j < XDIM; j++){
        if(!weightmap[i][j]){
      printf("   ");
        } else if(weightmap[i][j] < 0) {
      printf("   ");
        } else {
        printf(" %02d", weightmap[i][j] % 100);
        }
      }
      printf("\n");
    }
    */

    heap_deinit(&h);
    return 0;
}

char randNPC()
{
    int chance_rival = 90;
    int chance_hiker = 80;
    int chance_sentry = 60;
    int chance_pacer = 50;
    int chance_wanderer = 20;
    // int chance_explorer = 0;

    int choice = rand() % 100 + 1;
    // printf("choice val: %d\n", choice);

    if (choice > chance_rival)
    {
        return 'r';
    }
    else if (choice > chance_hiker)
    {
        return 'h';
    }
    else if (choice > chance_sentry)
    {
        return 's';
    }
    else if (choice > chance_pacer)
    {
        return 'p';
    }
    else if (choice > chance_wanderer)
    {
        return 'w';
    }
    else
    { /* if(choice > chance_explorer) and nothing else */
        return 'e';
    }
}

char randDirection()
{
    int chance_n = 75;
    int chance_s = 50;
    int chance_e = 25;
    // int chance_w = 0;

    int choice = rand() % 100;
    // printf("choice val: %d\n", choice);

    if (choice > chance_n)
    {
        return 'n';
    }
    else if (choice > chance_s)
    {
        return 's';
    }
    else if (choice > chance_e)
    {
        return 'e';
    }
    else
    { /* if(choice > chance_w) and nothing else */
        return 'w';
    }
}

character_t search_pc(map_t *m)
{
    int y, x;
    for (y = 0; y < YDIM; y++)
    {
        for (x = 0; x < XDIM; x++)
        {
            if (m->char_map[y][x].is_pc) // looks at x,y location, if is_pc
            {
                return m->char_map[y][x]; // return index of the pc
            }
        }
    }
    return m->char_map[0][0];
}

void make_interaction_window(character_t *npc)
{
    WINDOW *interaction = newwin(5, 60, 2, 1);
    std::string enemy_type;
    std::string greeting;
    switch (npc->type)
    {
    case 'h':
        enemy_type = "hiker";
        greeting = "you aren't from around here, are ya?";
        break;
    case 'r':
        enemy_type = "rival";
        greeting = "eat my pokeballs, loser!";
        break;
    case 'e':
        enemy_type = "explorer";
        greeting = "just because I love exploring nature doesn't mean I won't beat your ass";
        break;
    case 's':
        enemy_type = "chiller";
        greeting = "I bet you thought I wouldn't fight you, fool!";
        break;
    case 'p':
        enemy_type = "pacer";
        greeting = "I'm walkin' here!";
        break;
    case 'w':
        enemy_type = "wanderer";
        greeting = "I'm wanderin' here!";
        break;
    }
    mvwprintw(interaction, 0, 0, "a %s approaches", enemy_type.c_str());
    if (npc->hostile == 0)
    {
        mvwprintw(interaction, 2, 0, "but does nothing");
    }
    else
    {
        mvwprintw(interaction, 2, 0, greeting.c_str());
    }
    mvwprintw(interaction, 4, 0, "esc to continue");
    wrefresh(interaction);
    npc->hostile = 0;
    char c = getch();
    while (c != 27) // this will have delay bc getch is looking for something after 27 (esc), just wait!
    {
        c = getch();
    }
    delwin(interaction);
}

void make_trainers_window(map_t *m)
{
    WINDOW *trainers = newwin(6, 20, 10, 30);
    keypad(trainers, TRUE);

    character_t player = search_pc(m);

    int x, y;
    int x_dist, y_dist;
    char x_direction;
    char y_direction;

    character_t trainers_list[XDIM * YDIM];
    int i = 0;

    for (y = 0; y < YDIM; y++)
    {
        for (x = 0; x < XDIM; x++)
        {
            if (!m->char_map[y][x].is_pc && m->char_map[y][x].type != '\000') // getting a list of the npcs
            {
                trainers_list[i] = m->char_map[y][x];
                i++;
            }
        }
    }

    int numtrainers = i;

    int print_depth = 1;
    int print_start = 0;

    int ch;
    do
    {

        print_depth = 1;
        mvwprintw(trainers, 0, 0, "-Local Trainers-");
        // refresh();
        printw("%c", ch);
        refresh();
        if (ch == KEY_DOWN)
        {
            // if there are more trainers to be displayed on the next page, otherwise do nothing
            // int size = sizeof(trainers_list)/sizeof(trainers_list[0]);
            if (i > (print_start + 5))
                print_start += 5;
        }
        else if (ch == KEY_UP)
        {
            print_start -= 5;
            if (print_start < 0)
                print_start = 0;
        }
        for (i = print_start; i <= numtrainers && i < (7 + 7 * print_start); i++)
        {
            x_dist = trainers_list[i].x - player.x;
            y_dist = trainers_list[i].y - player.y;

            if (x_dist > 0)
            {
                x_direction = 'E';
            }
            else
            {
                x_direction = 'W';
            }
            if (y_dist > 0)
            {
                y_direction = 'S';
            }
            else
            {
                y_direction = 'N';
            }
            /*
            char *x_distance = malloc(2);
            sprintf(x_distance, "%d", abs(x_dist)); //converting to string
            char *y_distance = malloc(2);
            sprintf(y_distance, "%d", abs(y_dist)); //converting to string
            */

            char *info = (char *)malloc(25);
            sprintf(info, "%c, %c %d and %c %d", m->char_map[trainers_list[i].y][trainers_list[i].x].type,
                    x_direction, abs(x_dist), y_direction, abs(y_dist)); // converting to string

            mvwprintw(trainers, print_depth, 1, info);
            /*
            mvwprintw(trainers, print_depth, 2, ", ");
            mvwprintw(trainers, print_depth, 4, x_direction);
            mvwprintw(trainers, print_depth, 5, x_distance);
            mvwprintw(trainers, print_depth, 7, y_direction);
            mvwprintw(trainers, print_depth, 8, y_distance);
            */
            print_depth++;
        }
        wrefresh(trainers);
        ch = wgetch(trainers);
        wclear(trainers);

    } while (ch != 27);
    delwin(trainers);
}

heap_node_t handle_movement(character_t c[YDIM][XDIM], map_t *m, int x, int y, int preweight, character_t pc)
{

    // world.w[y][x]->char_map[char_y][char_x] = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};
    character_t cur; // = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};
    int move_x = 0;
    int move_y = 0;
    heap_node_t n;

    n.x = x;
    n.y = y;

    int hdmap[YDIM][XDIM]; // only for hikers
    int rdmap[YDIM][XDIM]; // only for rivals
    int i, j;
    int smallest_dist;

    /* to be used only with wanderer type */
    terrain home_terrain;

    cur = c[y][x];
    switch (cur.type)
    {
    case 'h':
        if (cur.hostile == 0)
            goto NONHOSTILE_BEHAVIOR; // if it is no longer hostile
        // finding shortest distance
        distance_map(m, 'h', pc, hdmap);
        smallest_dist = INT_MAX; // setting them to max s.t. we only look for smaller ones
        for (i = -1; i <= 1; i++)
        {
            for (j = -1; j <= 1; j++)
            {

                if (i == 0 && j == 0)
                    continue;
                if (hdmap[y + i][x + j] < smallest_dist && !c[y + i][x + j].is_pc)
                {
                    move_x = j;
                    move_y = i;
                    smallest_dist = hdmap[y + i][x + j];
                }
                /*
                if (c[y + i][x + j].pc && cur.npc->hostile == 1)
                {
                    make_interaction_window(&cur);
                }
                */
                // printf("weight %d", hdmap[i][j]);
            }
        }
        // printf("taking %d\n", smallest_dist);
        // moving

        // c[y + move_y][x + move_x] = *(new npc());// = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};
        c[y + move_y][x + move_x].direction = cur.direction;
        c[y + move_y][x + move_x].type = cur.type;
        c[y + move_y][x + move_x].x = cur.x;
        c[y + move_y][x + move_x].y = cur.y;
        c[y + move_y][x + move_x].hostile = cur.hostile;

        // delete &(c[y][x]); // = (character_t){NULL, NULL, 0, 0}; // = NULL;

        // destroying the old one
        c[y][x].direction = '\000';
        c[y][x].type = '\000';
        c[y][x].x = 0;
        c[y][x].y = 0;
        c[y][x].hostile = 0;

        cur.x = x + move_x;
        cur.y = y + move_y;

        n.weight = preweight + get_weight(m->t[cur.y][cur.x], 1);
        n.x = cur.x;
        n.y = cur.y;
        break;

    case 'r':
        if (cur.hostile == 0)
            goto NONHOSTILE_BEHAVIOR;
        distance_map(m, 'r', pc, rdmap);
        smallest_dist = INT_MAX; // setting them to max s.t. we only look for smaller ones
        for (i = -1; i <= 1; i++)
        {
            for (j = -1; j <= 1; j++)
            {
                if (j == 0 && i == 0)
                    continue;
                if (rdmap[y + i][x + j] < smallest_dist && !c[y + i][x + j].is_pc)
                {
                    move_x = j;
                    move_y = i;
                    smallest_dist = rdmap[y + i][x + j];
                }
                /*
                if (c[y + i][x + j].pc && cur.npc->hostile == 1)
                {
                    make_interaction_window(&cur);
                }
                */
            }
        }
        // printf("taking %d\n", smallest_dist);
        // moving

        // c[cur.y][cur.x] = *(new npc());// = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};
        c[y + move_y][x + move_x].direction = cur.direction;
        c[y + move_y][x + move_x].type = cur.type;
        c[y + move_y][x + move_x].x = cur.x;
        c[y + move_y][x + move_x].y = cur.y;
        c[y + move_y][x + move_x].hostile = cur.hostile;

        // delete &(c[y][x]); // = (character_t){NULL, NULL, 0, 0}; // = NULL;
        // destroying the old one
        c[y][x].direction = '\000';
        c[y][x].type = '\000';
        c[y][x].x = 0;
        c[y][x].y = 0;
        c[y][x].hostile = 0;

        cur.x = x + move_x;
        cur.y = y + move_y;

        n.weight = preweight + get_weight(m->t[cur.y][cur.x], 2);
        n.x = cur.x;
        n.y = cur.y;
        break;

    case 'e':
    NONHOSTILE_BEHAVIOR:
        if (cur.direction == 'n')
        {
            move_y = -1;
        }
        else if (cur.direction == 's')
        {
            move_y = 1;
        }
        else if (cur.direction == 'w')
        {
            move_x = -1;
        }
        else
        {
            move_x = 1;
        }
        // checking if something is in the way & changing direction until movement is possible
        while (get_weight(m->t[y + move_y][x + move_x], 2) == INT_MAX ||
               c[y + move_y][x + move_x].is_pc /*ie pc is there*/)
        {
            // setting random new direction
            cur.direction = randDirection();
            move_y = 0;
            move_x = 0; // have to reset movements to assign a new one

            if (cur.direction == 'n')
            {
                move_y = -1;
            }
            else if (cur.direction == 's')
            {
                move_y = 1;
            }
            else if (cur.direction == 'w')
            {
                move_x = -1;
            }
            else // east
            {
                move_x = 1;
            }
        }

        // c[cur.y][cur.x] = malloc( sizeof(c[y][x]));

        // c[cur.y][cur.x] = *(new npc());// = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};
        c[y + move_y][x + move_x].direction = cur.direction;
        c[y + move_y][x + move_x].type = cur.type;
        c[y + move_y][x + move_x].x = cur.x;
        c[y + move_y][x + move_x].y = cur.y;
        c[y + move_y][x + move_x].hostile = cur.hostile;

        // delete &(c[y][x]); // = (character_t){NULL, NULL, 0, 0}; // = NULL;
        // destroying the old one
        c[y][x].direction = '\000';
        c[y][x].type = '\000';
        c[y][x].x = 0;
        c[y][x].y = 0;
        c[y][x].hostile = 0;

        cur.x = x + move_x;
        cur.y = y + move_y;

        n.weight = preweight + get_weight(m->t[cur.y][cur.x], 2);
        n.x = cur.x;
        n.y = cur.y;
        // printw(" **** %d, %d\n", cur.x, cur.y);
        // refresh();
        break;

    case 's':
        n.weight = preweight + get_weight(m->t[y + move_y][x + move_x], 2);
        /*
        int i, j;
        for(i = 0; i <= 1; i ++) //checking for nearby pc
        {
            for(j = 0; j <= 1; j++)
            {
                if(m->char_map[y+i][x+j].pc)
                {
                    if (c[y + move_y][x + move_x].pc && cur.npc->hostile == 1)
                    {
                    make_interaction_window(&cur);
                    }
                }
            }
        }
        */
        // do nothing
        break;

    case 'p':
        // printf("doing p move in %c\n", cur.npc->direction);
        //  initial movement based on direction
        if (cur.direction == 'n')
        {
            move_y = -1;
        }
        else if (cur.direction == 's')
        {
            move_y = 1;
        }
        else if (cur.direction == 'w')
        {
            move_x = -1;
        }
        else
        {
            move_x = 1;
        }
        // checking if something is in the way & turning around if so
        if (get_weight(m->t[y + move_y][x + move_x], 2) == INT_MAX ||
            c[y + move_y][x + move_x].is_pc /*ie it has something there*/)
        {
            /*
            if (c[y + move_y][x + move_x].pc && cur.npc->hostile == 1)
            {
                make_interaction_window(&cur);
            }
            */
            if (cur.direction == 'n')
            {
                move_y = 1;
                cur.direction = 's';
            }
            else if (cur.direction == 's')
            {
                move_y = -1;
                cur.direction = 'n';
            }
            else if (cur.direction == 'w')
            {
                move_x = 1;
                cur.direction = 'e';
            }
            else
            {
                move_x = -1;
                cur.direction = 'w';
            }
        }

        // c[cur.y][cur.x] = malloc( sizeof(c[y][x]));

        // c[cur.y][cur.x] = *(new npc());// = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};
        c[y + move_y][x + move_x].direction = cur.direction;
        c[y + move_y][x + move_x].type = cur.type;
        c[y + move_y][x + move_x].x = cur.x;
        c[y + move_y][x + move_x].y = cur.y;
        c[y + move_y][x + move_x].hostile = cur.hostile;

        // delete &(c[y][x]); // = (character_t){NULL, NULL, 0, 0}; // = NULL;
        // destroying the old one
        c[y][x].direction = '\000';
        c[y][x].type = '\000';
        c[y][x].x = 0;
        c[y][x].y = 0;
        c[y][x].hostile = 0;

        cur.x = x + move_x;
        cur.y = y + move_y;

        n.weight = preweight + get_weight(m->t[cur.y][cur.x], 2);
        n.x = cur.x;
        n.y = cur.y;
        break;

    case 'w':
        home_terrain = m->t[y][x]; // recording home terrain to check that it never leaves
        // initial movement based on direction
        if (cur.direction == 'n')
        {
            move_y = -1;
        }
        else if (cur.direction == 's')
        {
            move_y = 1;
        }
        else if (cur.direction == 'w')
        {
            move_x = -1;
        }
        else
        {
            move_x = 1;
        }
        // checking if something is in the way & changing direction until movement is possible
        while (get_weight(m->t[y + move_y][x + move_x], 2) == INT_MAX ||
               c[y + move_y][x + move_x].is_pc || /*ie it has something there*/
               home_terrain != m->t[y + move_y][x + move_x])
        {
            /*
            if (c[y + move_y][x + move_x].pc && cur.npc->hostile == 1)
            {
                make_interaction_window(&cur);
            }
            */
            // setting random new direction
            cur.direction = randDirection();
            move_y = 0;
            move_x = 0; // have to reset movements to assign a new one

            if (cur.direction == 'n')
            {
                move_y = -1;
            }
            else if (cur.direction == 's')
            {
                move_y = 1;
            }
            else if (cur.direction == 'w')
            {
                move_x = -1;
            }
            else // east
            {
                move_x = 1;
            }
        }

        // c[cur.y][cur.x] = malloc( sizeof(c[y][x]));

        // c[cur.y][cur.x] = *(new npc());// = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};
        c[y + move_y][x + move_x].direction = cur.direction;
        c[y + move_y][x + move_x].type = cur.type;
        c[y + move_y][x + move_x].x = cur.x;
        c[y + move_y][x + move_x].y = cur.y;
        c[y + move_y][x + move_x].hostile = cur.hostile;

        // delete &(c[y][x]); // = (character_t){NULL, NULL, 0, 0}; // = NULL;
        // destroying the old one
        c[y][x].direction = '\000';
        c[y][x].type = '\000';
        c[y][x].x = 0;
        c[y][x].y = 0;
        c[y][x].hostile = 0;

        cur.x = x + move_x;
        cur.y = y + move_y;

        n.weight = preweight + get_weight(m->t[cur.y][cur.x], 2);
        n.x = cur.x;
        n.y = cur.y;
        break;
    }
    return n;
}

/*
    expects to be used correctly, as in there is not already a map there
*/
void generate_map(int y, int x, int num_npc, character_t *pc) // do not need the gameworld (world) since it is global
{
    int npc_code;
    int a, b;
    world.w[y][x] = new map_t(); // = malloc(sizeof(cur_map));

    // printf("painting map\n");
    paint_map(world.w[y][x], y, x);

    for (b = 0; b < YDIM; b++)
    {
        for (a = 0; a < XDIM; a++)
        {
            world.w[y][x]->char_map[b][a].direction = '\000';
            world.w[y][x]->char_map[b][a].type = '\000';
            world.w[y][x]->char_map[b][a].x = 0;
            world.w[y][x]->char_map[b][a].y = 0;
            world.w[y][x]->char_map[b][a].hostile = 0;
            world.w[y][x]->char_map[b][a].is_pc = false;
        }
    }

    world.w[y][x]->char_map[pc->y][pc->x] = *pc;

    heap_init(&(world.w[y][x]->move_priority), (num_npc + 1)); // +1 for pc

    // making a hiker
    if (num_npc >= 1)
    {

        int char_y = rand() % 18 + 1;
        int char_x = rand() % 78 + 1;
        char type = 'h';
        char dir = randDirection();

        if (type == 'h')
        {
            npc_code = 1;
        }
        else
        {
            // printf("%d\n", get_weight(tmp, 2));
            npc_code = 2;
        }

        terrain tmp = world.w[y][x]->t[char_y][char_x];
        while (get_weight(tmp, npc_code) == INT_MAX)
        {

            // printf("trying again\n");
            char_y = rand() % 18 + 1;
            char_x = rand() % 78 + 1;
            tmp = world.w[y][x]->t[char_y][char_x];
            // printf("after: %d %d %d\n", get_weight(tmp, npc_code), char_x, char_y);
        }

        world.w[y][x]->char_map[char_y][char_x] = *(new npc()); // = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};

        // world.w[y][x]->char_map[char_y][char_x].npc =  malloc( sizeof(world.w[y][x]->char_map[char_y][char_x]->npc));
        // type = ' ';
        world.w[y][x]->char_map[char_y][char_x].type = ' ';
        world.w[y][x]->char_map[char_y][char_x].type = type;
        // printf("type %c\n", world.w[y][x]->char_map[char_y][char_x].npc->type);

        world.w[y][x]->char_map[char_y][char_x].direction = ' ';
        world.w[y][x]->char_map[char_y][char_x].direction = dir;

        world.w[y][x]->char_map[char_y][char_x].y = char_y;
        world.w[y][x]->char_map[char_y][char_x].x = char_x;

        world.w[y][x]->char_map[char_y][char_x].hostile = 1;

        // printf("%d %d %d\n", get_weight(tmp, npc_code), char_x, char_y);

        // printf("num npcs: %d\n", num_npc);
    }
    if (num_npc >= 2)
    {
        // making a rival

        int char_y = rand() % 18 + 1;
        int char_x = rand() % 78 + 1;
        char type = 'r';
        char dir = randDirection();

        if (type == 'r')
        {
            npc_code = 1;
        }
        else
        {
            // printf("%d\n", get_weight(tmp, 2));
            npc_code = 2;
        }

        terrain tmp = world.w[y][x]->t[char_y][char_x];
        while (get_weight(tmp, npc_code) == INT_MAX)
        {

            // printf("trying again\n");
            char_y = rand() % 18 + 1;
            char_x = rand() % 78 + 1;
            tmp = world.w[y][x]->t[char_y][char_x];
            // printf("after: %d %d %d\n", get_weight(tmp, npc_code), char_x, char_y);
        }

        world.w[y][x]->char_map[char_y][char_x] = *(new npc()); // = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};

        // world.w[y][x]->char_map[char_y][char_x].npc =  malloc( sizeof(world.w[y][x]->char_map[char_y][char_x]->npc));
        // type = ' ';
        world.w[y][x]->char_map[char_y][char_x].type = ' ';
        world.w[y][x]->char_map[char_y][char_x].type = type;
        // printf("type %c\n", world.w[y][x]->char_map[char_y][char_x].npc->type);

        world.w[y][x]->char_map[char_y][char_x].direction = ' ';
        world.w[y][x]->char_map[char_y][char_x].direction = dir;

        world.w[y][x]->char_map[char_y][char_x].y = char_y;
        world.w[y][x]->char_map[char_y][char_x].x = char_x;

        world.w[y][x]->char_map[char_y][char_x].hostile = 1;

        // printf("%d %d %d\n", get_weight(tmp, npc_code), char_x, char_y);
    }
    for (a = 0; a < num_npc - 2; a++)
    {
        // npc_list[a].npc = malloc( sizeof(npc_list[a].npc));
        // npc_list[a].npc->type = randNPC();

        // printf("making npcs\n");

        int char_y = rand() % 18 + 1;
        int char_x = rand() % 78 + 1;
        char type = randNPC();
        char dir = randDirection();

        if (type == 'h')
        {
            npc_code = 1;
        }
        else
        {
            // printf("%d\n", get_weight(tmp, 2));
            npc_code = 2;
        }

        terrain tmp = world.w[y][x]->t[char_y][char_x];
        while (get_weight(tmp, npc_code) == INT_MAX)
        {

            // printf("trying again\n");
            char_y = rand() % 18 + 1;
            char_x = rand() % 78 + 1;
            tmp = world.w[y][x]->t[char_y][char_x];
            // printf("after: %d %d %d\n", get_weight(tmp, npc_code), char_x, char_y);
        }

        world.w[y][x]->char_map[char_y][char_x] = *(new npc()); // = (character_t){malloc(sizeof(npc)), malloc(sizeof(player)), 0, 0};

        // world.w[y][x]->char_map[char_y][char_x].npc =  malloc( sizeof(world.w[y][x]->char_map[char_y][char_x]->npc));
        // type = ' ';
        world.w[y][x]->char_map[char_y][char_x].type = ' ';
        world.w[y][x]->char_map[char_y][char_x].type = type;
        // printf("type %c\n", world.w[y][x]->char_map[char_y][char_x].npc->type);

        world.w[y][x]->char_map[char_y][char_x].direction = ' ';
        world.w[y][x]->char_map[char_y][char_x].direction = dir;

        world.w[y][x]->char_map[char_y][char_x].y = char_y;
        world.w[y][x]->char_map[char_y][char_x].x = char_x;

        world.w[y][x]->char_map[char_y][char_x].hostile = 1;

        // printf("%d %d %d\n", get_weight(tmp, npc_code), char_x, char_y);
    }

    print_map(world.w[y][x], num_npc);
}

void movement_setup(int y, int x)
{
    // putting npcs into the move queue
    int moveweight;
    int npc_code;
    int a, b;

    for (b = 0; b < YDIM; b++)
    {
        for (a = 0; a < XDIM; a++)
        {

            // if (world.w[y][x]->char_map[b][a].is_pc)
            //{
            //  printf("adding %d\n", b * XDIM + a);

            if (world.w[y][x]->char_map[b][a].is_pc)
            {
                // printf("player\n");
                npc_code = 0;
                moveweight = get_weight(world.w[y][x]->t[b][a], npc_code);
                // printf("w%d x%d y%d\n",moveweight,a,b);
                heap_add(&(world.w[y][x]->move_priority), moveweight, world.w[y][x]->char_map[b][a].x, world.w[y][x]->char_map[b][a].y);
            }
            else if (world.w[y][x]->char_map[b][a].type == 'h')
            {
                // printf("hiker\n");
                npc_code = 1;
                moveweight = get_weight(world.w[y][x]->t[b][a], npc_code);
                // printf("w%d x%d y%d\n",moveweight,a,b);
                heap_add(&(world.w[y][x]->move_priority), moveweight, world.w[y][x]->char_map[b][a].x, world.w[y][x]->char_map[b][a].y);
            }
            else if (!world.w[y][x]->char_map[b][a].is_pc && world.w[y][x]->char_map[b][a].type != '\000')
            {
                // printf("not hiker\n");
                npc_code = 2;
                moveweight = get_weight(world.w[y][x]->t[b][a], npc_code);
                // printf("w%d x%d y%d\n",moveweight,a,b);
                heap_add(&(world.w[y][x]->move_priority), moveweight, world.w[y][x]->char_map[b][a].x, world.w[y][x]->char_map[b][a].y);
            }
            //}
        }
    }

    heap_sortify(&(world.w[y][x]->move_priority));
}

void make_pokemon_window(pokemon_character *p)
{
    // print the pokemon’s level, stats, and moves and pause for q(uit)
    WINDOW *poke = newwin(10, 25, 10, 30);
    keypad(poke, TRUE);

    char *move1;
    char *move2;
    int i;

    for (i = 0; i < (int)world.mtv.size(); i++)
    {
        if (world.mtv[i]->id == p->move1_id)
        {
            move1 = world.mtv[i]->identifier;
        }
        if (world.mtv[i]->id == p->move2_id)
        {
            move2 = world.mtv[i]->identifier;
        }
    }

    mvwprintw(poke, 0, 0, (p->species).c_str());
    mvwprintw(poke, 1, 0, "lv");
    mvwprintw(poke, 1, 2, "%d", p->level);
    mvwprintw(poke, 2, 0, "hp");
    mvwprintw(poke, 2, 3, "%d/%d", p->current_hp, p->hp_true);
    mvwprintw(poke, 3, 0, "attack");
    mvwprintw(poke, 3, 7, "%d", p->attack_true);
    mvwprintw(poke, 4, 0, "defense");
    mvwprintw(poke, 4, 8, "%d", p->defense_true);
    mvwprintw(poke, 5, 0, "speed");
    mvwprintw(poke, 5, 6, "%d", p->speed_true);
    mvwprintw(poke, 6, 0, "special attack");
    mvwprintw(poke, 6, 15, "%d", p->special_attack_true);
    mvwprintw(poke, 7, 0, "special defense");
    mvwprintw(poke, 7, 16, "%d", p->defense_true);
    mvwprintw(poke, 8, 0, "move 1 - ");
    mvwprintw(poke, 8, 9, move1);
    mvwprintw(poke, 9, 0, "move 2 - ");
    mvwprintw(poke, 9, 9, move2);

    wrefresh(poke);

    getch();
    werase(poke);
    wrefresh(poke);
    delwin(poke);
}

int make_bag_window(character_t *pc, bool in_wild_battle, pokemon_character *enemy)
{
    // va_list enemy;
    // va_start(enemy, e);

    WINDOW *bag = newwin(10, 20, 2, XDIM - 21);
    keypad(bag, TRUE);
    curs_set(0);
    int i;

    mvwprintw(bag, 0, 1, "**Your Bag**");
    for (i = 0; i < (int)pc->bag.size(); i++)
    {
        mvwprintw(bag, i + 1, 0, "(%d)%s: %d", i + 1, pc->bag[i]->type, pc->bag[i]->count);
    }
    mvwprintw(bag, i + 2, 0, "use item by number");
    mvwprintw(bag, i + 3, 0, "q to exit");
    mvwprintw(bag, i + 5, 0, "you have $%d", pc->pokebucks);

    wrefresh(bag);
    char selection = getch();
    int selectionNum = atoi(&selection);
    bool pokemon_selected = false;
    while (selection != 'q')
    {
        if (i > selectionNum - 1)
        {
            // wmove(bag, i+2, 0);
            if (!strcmp(pc->bag[selectionNum - 1]->type, "pokeball") && in_wild_battle) // if it is a pokeball!
            {
                // pokemon_character *capturing = enemy;
                // capturing pokemon
                pc->bag[0]->use(NULL);
                return 1;
            }
            wclear(bag);
            mvwprintw(bag, 0, 1, "**Your Bag**");
            // print pokemon to use item on
            for (i = 0; i < (int)pc->my_pokemon.size(); i++)
            {
                mvwprintw(bag, i + 1, 0, "(%d)%s", i + 1, pc->my_pokemon[i]->species.c_str());
            }
            mvwprintw(bag, i + 2, 0, "use a %s", pc->bag[selectionNum - 1]->type);
            mvwprintw(bag, i + 3, 0, "on who?");
            wrefresh(bag);

            // save item index and get input
            int itemNum = selectionNum;
            selection = getch();
            selectionNum = atoi(&selection);
            while (!pokemon_selected)
            {
                switch (selectionNum)
                {
                case 1:
                    pokemon_selected = true;
                    break;
                case 2:
                    pokemon_selected = true;
                    break;
                case 3:
                    pokemon_selected = true;
                    break;
                case 4:
                    pokemon_selected = true;
                    break;
                case 5:
                    pokemon_selected = true;
                    break;
                case 6:
                    pokemon_selected = true;
                    break;
                }
            }
            for (i = 0; i < 4; i++)
            {
                wmove(bag, i + 1, 0);
                wclrtoeol(bag);
            }
            // mvwprintw(bag, 1, 0, pc.my_pokemon[selectionNum-1]->species.c_str());
            // mvwprintw(bag, 2, 0, "selected");
            // use the item on selected pokemon
            pc->bag[itemNum - 1]->use(pc->my_pokemon[selectionNum - 1]);
            wrefresh(bag);
            // refresh();
            usleep(500000);
            for (i = 0; i < (int)pc->bag.size(); i++)
            {
                mvwprintw(bag, i + 1, 0, "(%d)%s: %d", i + 1, pc->bag[i]->type, pc->bag[i]->count);
            }
        }
        else
        {
            wmove(bag, i + 2, 0);
            wclrtoeol(bag);
            mvwprintw(bag, i + 2, 0, "no such item");
            wrefresh(bag);
            usleep(500000);
        }
        mvwprintw(bag, i + 2, 0, "use item by number");
        mvwprintw(bag, i + 3, 0, "q to exit");
        wrefresh(bag);
        selection = getch();
        selectionNum = atoi(&selection);
    }
    werase(bag);
    wrefresh(bag);
    delwin(bag);
    return 0;
}

void make_pokemon_menu(character_t *pc)
{
    WINDOW *pokelist = newwin(10, 20, 2, XDIM - 21);
    keypad(pokelist, TRUE);
    curs_set(0);
    int i;
reprint:
    mvwprintw(pokelist, 0, 1, "**Your Pokemon**");
    for (i = 0; i < (int)pc->my_pokemon.size(); i++)
    {
        if (pc->my_pokemon[i]->is_down)
        {
            wattron(pokelist, COLOR_PAIR(6));
        }
        if (pc->my_pokemon[i] == pc->active_pokemon)
        {
            mvwprintw(pokelist, i + 1, 0, "{%d}%s", i + 1, pc->my_pokemon[i]->species.c_str());
        }
        else
        {
            mvwprintw(pokelist, i + 1, 0, " %d %s", i + 1, pc->my_pokemon[i]->species.c_str());
        }
        wattroff(pokelist, COLOR_PAIR(6));
    }
    mvwprintw(pokelist, i + 1, 1, "press # for details");
    mvwprintw(pokelist, i + 2, 1, "q to quit");
    mvwprintw(pokelist, i + 3, 1, "a to pick primary");

    wrefresh(pokelist);

    char c = getch();
    int selection = c - 49; //- 49 to shift for char to int
    // mvwprintw(pokelist, i+3, 1, "%d", selection+1);

    while (c != 'q')
    {

        if (c == 'a')
        {
            mvwprintw(pokelist, i + 3, 1, "select with #    ");
            wrefresh(pokelist);
            int new_primary = getch() - 49;
            if (pc->my_pokemon[new_primary]->is_down) // if trying to switch to downed pokemon
            {
                mvprintw(22, 0, "cannot use %s, they are unconscious", pc->active_pokemon->species.c_str());
            }
            else
            {
                pc->active_pokemon = pc->my_pokemon[new_primary];
                mvprintw(22, 0, "changed primary pokemon to %s", pc->active_pokemon->species.c_str());
            }
            refresh();
            usleep(2000000);
            move(22, 0);
            clrtoeol();
            refresh();
            goto reprint;
        }
        else if (selection < 6 && selection > -1)
        {
            make_pokemon_window(pc->my_pokemon[selection]);
        }
        c = getch();
        selection = c - 49;
    }
    // wrefresh(pokelist);
    // getch();
    werase(pokelist);
    wrefresh(pokelist);
    delwin(pokelist);
}

void setup_battle_window(WINDOW *win)
{
    int i;
    mvwprintw(win, 0, 0, "XxxxxxxxxxxxxxxxxxxX");
    for (i = 1; i < 18; i++)
    {
        mvwprintw(win, i, 0, "x                  x");
    }
    mvwprintw(win, 18, 0, "XxxxxxxxxxxxxxxxxxxX");
    mvwprintw(win, 16, 1, "  1-fight 2-bag");
    mvwprintw(win, 17, 1, " 3-run 4-pokemon");
}

move_t *get_move_by_id(int id)
{
    int i;
    for (i = 0; i < (int)world.mtv.size(); i++)
    {
        if (world.mtv[i]->id == id)
        {
            return world.mtv[i];
        }
    }
    return NULL;
}

double get_type_bonus(char *attack_type, char *defender_type)
{
    int at = 0;
    int dt = 0;
    double bonus[18][18] = {
        {1,1,1,1,1,0.5,1,0,0.5,1,1,1,1,1,1,1,1,1}, // normal
        {2,1,0.5,0.5,1,2,0.5,0,2,1,1,1,1,0.5,2,1,2,0.5}, // fighting
        {1,2,1,1,1,0.5,2,1,0.5,1,1,2,0.5,1,1,1,1,1}, // flying
        {1,1,1,0.5,0.5,0.5,1,0.5,0,1,1,2,1,1,1,1,1,2}, // poison
        {1,1,0,2,1,2,0.5,1,2,2,1,0.5,2,1,1,1,1,1}, // ground
        {1,0.5,2,1,0.5,1,2,1,0.5,2,1,1,1,1,2,1,1,1}, // rock
        {1,0.5,0.5,0.5,1,1,1,0.5,0.5,0.5,1,2,1,2,1,1,2,0.5}, // bug
        {0,1,1,1,1,1,1,2,1,1,1,1,1,2,1,1,0.5,1}, // ghost
        {1,1,1,1,1,2,1,1,0.5,0.5,0.5,1,0.5,1,2,1,1,2}, // steel
        {1,1,1,1,1,0.5,2,1,2,0.5,0.5,2,1,1,2,0.5,1,1}, // fire
        {1,1,1,1,2,2,1,1,1,2,0.5,0.5,1,1,1,0.5,1,1}, // water
        {1,1,0.5,0.5,2,2,0.5,1,0.5,0.5,2,0.5,1,1,1,0.5,1,1}, // grass
        {1,1,2,1,0,1,1,1,1,1,2,0.5,0.5,1,1,0.5,1,1}, // electric
        {1,2,1,2,1,1,1,1,0.5,1,1,1,1,0.5,1,1,0,1}, // psychic
        {1,1,2,1,2,1,1,1,0.5,0.5,0.5,2,1,1,0.5,2,1,1}, // ice
        {1,1,1,1,1,1,1,1,0.5,1,1,1,1,1,1,2,1,0}, // dragon
        {1,0.5,1,1,1,1,1,2,1,1,1,1,1,2,1,1,0.5,0.5}, // dark
        {1,2,1,0.5,1,1,1,1,0.5,0.5,1,1,1,1,1,2,2,1} // fairy
    };

    if(!strcmp(attack_type, (char *)"Normal"))
    {
        at = 0;
    }
    if(!strcmp(defender_type, (char *)"Normal"))
    {
        dt = 0;
    }
    if(!strcmp(attack_type, (char *)"Fighting"))
    {
        at = 1;
    }
    if(!strcmp(defender_type, (char *)"Fighting"))
    {
        dt = 1;
    }
    if(!strcmp(attack_type, (char *)"Flying"))
    {
        at = 2;
    }
    if(!strcmp(defender_type, (char *)"Flying"))
    {
        dt = 2;
    }
    if(!strcmp(attack_type, (char *)"Poison"))
    {
        at = 3;
    }
    if(!strcmp(defender_type, (char *)"Poison"))
    {
        dt = 3;
    }
    if(!strcmp(attack_type, (char *)"Ground"))
    {
        at = 4;
    }
    if(!strcmp(defender_type, (char *)"Ground"))
    {
        dt = 4;
    }
    if(!strcmp(attack_type, (char *)"Rock"))
    {
        at = 5;
    }
    if(!strcmp(defender_type, (char *)"Rock"))
    {
        dt = 5;
    }
    if(!strcmp(attack_type, (char *)"Bug"))
    {
        at = 6;
    }
    if(!strcmp(defender_type, (char *)"Bug"))
    {
        dt = 6;
    }
    if(!strcmp(attack_type, (char *)"Ghost"))
    {
        at = 7;
    }
    if(!strcmp(defender_type, (char *)"Ghost"))
    {
        dt = 7;
    }
    if(!strcmp(attack_type, (char *)"Steel"))
    {
        at = 8;
    }
    if(!strcmp(defender_type, (char *)"Steel"))
    {
        dt = 8;
    }
    if(!strcmp(attack_type, (char *)"Fire"))
    {
        at = 9;
    }
    if(!strcmp(defender_type, (char *)"Fire"))
    {
        dt = 9;
    }
    if(!strcmp(attack_type, (char *)"Water"))
    {
        at = 10;
    }
    if(!strcmp(defender_type, (char *)"Water"))
    {
        dt = 10;
    }
    if(!strcmp(attack_type, (char *)"Grass"))
    {
        at = 11;
    }
    if(!strcmp(defender_type, (char *)"Grass"))
    {
        dt = 11;
    }
    if(!strcmp(attack_type, (char *)"Electric"))
    {
        at = 12;
    }
    if(!strcmp(defender_type, (char *)"Electric"))
    {
        dt = 12;
    }
    if(!strcmp(attack_type, (char *)"Psychic"))
    {
        at = 13;
    }
    if(!strcmp(defender_type, (char *)"Psychic"))
    {
        dt = 13;
    }
    if(!strcmp(attack_type, (char *)"Ice"))
    {
        at = 14;
    }
    if(!strcmp(defender_type, (char *)"Ice"))
    {
        dt = 14;
    }
    if(!strcmp(attack_type, (char *)"Dragon"))
    {
        at = 15;
    }
    if(!strcmp(defender_type, (char *)"Dragon"))
    {
        dt = 15;
    }
    if(!strcmp(attack_type, (char *)"Dark"))
    {
        at = 16;
    }
    if(!strcmp(defender_type, (char *)"Dark"))
    {
        dt = 16;
    }
    if(!strcmp(attack_type, (char *)"Fairy"))
    {
        at = 17;
    }
    if(!strcmp(defender_type, (char *)"Fairy"))
    {
        dt = 17;
    }
    return bonus[at][dt];
}

int do_battle_round(pokemon_character *p1, move_t *p1move, pokemon_character *p2, move_t *p2move)
{
    int priority;
    int crit;
    int stab;
    double type;
    int p1dmg_dealt;
    int p2dmg_dealt;
    bool p1_done = false;
    bool p2_done = false;

    priority = (p1->speed_true > p2->speed_true) ? 1 : 2; // if p1 is faster, then p1 attack goes first

    if (p1move == NULL) // if no move done by pokemon, skip action by pokemon
    {
        p1_done = true;
        priority = 2;
    }

    // if(p1->)
    while (!p1_done || !p2_done)
    {
        if (priority == 1 && !p1_done && p1move->accuracy > rand() % 100) // if p1move hits
        {
            crit = ((p1->get_b_speed() / 2) == rand() % 255) ? 1.5 : 1;             // calculating crit
            if (p1move->type_id == p1->type1_id || p1move->type_id == p1->type2_id) // if movetype = pokemon1's type
            {
                stab = 1.5;
            }
            else
            {
                stab = 1;
            }
            type = get_type_bonus((char *)world.tntv[p1move->type_id]->name, (char *)world.tntv[p2->type1_id]->name); // could be .25, .5, 1, 2, or 4 based on move type and enemy's type, but i dont want to deal with that
            if(p2->type2_id)
            {
                type = type * get_type_bonus((char *)world.tntv[p1move->type_id]->name, (char *)world.tntv[p2->type2_id]->name);
            }
            p1dmg_dealt = ((((2 * p1->level) / 5 + 2) * p1move->power * (p1->attack_true / p2->defense_true)) / 50 + 2) * crit * (rand() % 10 + 1) * stab * type;

            p2->current_hp = p2->current_hp - p1dmg_dealt; // dealing damage to p2 from p1
            mvprintw(22, 0, "%s hit %s for %d points of dmg", p1->species.c_str(), p2->species.c_str(), p1dmg_dealt);
            refresh();
            usleep(2000000);
            priority = 2; // switch to others move
            p1_done = true;
            if (p2->current_hp <= 0)
            {
                p2->current_hp = 0;
                p2->is_down = true;
                return 1; // enemy defeated
            }
            continue;
        }
        if (priority == 2 && !p2_done && p2move->accuracy > rand() % 100) // if p2move hits
        {
            crit = ((p2->get_b_speed() / 2) == rand() % 255) ? 1.5 : 1;             // calculating crit
            if (p2move->type_id == p2->type1_id || p2move->type_id == p2->type2_id) // if movetype = pokemon1's type
            {
                stab = 1.5;
            }
            else
            {
                stab = 1;
            }
            type = get_type_bonus((char *)world.tntv[p2move->type_id]->name, (char *)world.tntv[p1->type1_id]->name); // could be .25, .5, 1, 2, or 4 based on move type and enemy's type, but i dont want to deal with that
            if(p1->type2_id)
            {
                type = type * get_type_bonus((char *)world.tntv[p2move->type_id]->name, (char *)world.tntv[p1->type2_id]->name);
            }
            p2dmg_dealt = ((((2 * p2->level) / 5 + 2) * p2move->power * (p2->attack_true / p1->defense_true)) / 50 + 2) * crit * (rand() % 10 + 1) * stab * type;

            p1->current_hp = p1->current_hp - p2dmg_dealt; // dealing damage to p1 from p2
            mvprintw(22, 0, "%s hit %s for %d points of dmg", p2->species.c_str(), p1->species.c_str(), p2dmg_dealt);
            refresh();
            usleep(2000000);
            priority = 1; // switch to others move
            if (p1->current_hp <= 0)
            {
                p1->current_hp = 0;
                p1->is_down = true;
                return -1; // you defeated
            }
            p2_done = true;
        }
    }
    return 0;
}

int make_wild_pokemon_battle(pokemon_character *p, character_t *pc)
{
    WINDOW *battle = newwin(19, 21, 2, 1);
    keypad(battle, TRUE);
    curs_set(0);

    int return_state = 0;

    bool enemy_defeated = false;
    bool player_defeated = false;
    setup_battle_window(battle);
    // intitalizing the board
    mvwprintw(battle, 3, 2, "wild %s", p->species.c_str());
    mvwprintw(battle, 4, 7, "vs");
    mvwprintw(battle, 5, 4, "%s", pc->active_pokemon->species.c_str());
    mvwprintw(battle, 6, 2, "any key to begin");
    wrefresh(battle);
    getch();
    // starting the battle loop
    // refresh_battle:

    char selection;   // = getch();
    int selectionNum; // = selection - 48; // shift number from char to int
    int battlestate;
    int firstavailablepokemon = -1;
    int i;
    move_t *my_move_1;
    move_t *my_move_2;
    move_t *enemy_move;
    int move_select;
    int escapechance;
    int escapeattempts = 0;
    while (!enemy_defeated && !player_defeated && selection != '+') // while player and enemy are alive
    {
        // clear bottom line
        move(22, 0);
        clrtoeol();
        refresh();

        // refresh battle screen
        wclear(battle);
        setup_battle_window(battle);

        mvwprintw(battle, 2, 2, "wild %s", p->species.c_str());
        mvwprintw(battle, 3, 2, "hp: %d/%d", p->current_hp, p->hp_true);

        mvwprintw(battle, 6, 2, pc->active_pokemon->species.c_str());
        mvwprintw(battle, 7, 2, "hp: %d/%d", pc->active_pokemon->current_hp, pc->active_pokemon->hp_true);

        wrefresh(battle);
        int captured = 0;
        selection = getch();
        selectionNum = selection - 48;
        switch (selectionNum)
        {
        case 1: // fight opens move menu
            my_move_1 = get_move_by_id(pc->active_pokemon->move1_id);
            my_move_2 = get_move_by_id(pc->active_pokemon->move2_id);
            mvwprintw(battle, 8, 2, "1 - %s", my_move_1->identifier);
            mvwprintw(battle, 9, 2, "2 - %s", my_move_2->identifier);
            wrefresh(battle);

            // getting enemy move randomly
            move_select = rand() % 2;
            if (move_select)
            {
                enemy_move = get_move_by_id(p->move1_id);
            }
            else
            {
                enemy_move = get_move_by_id(p->move2_id);
            }
            move(22, 0);
            clrtoeol();
        wild_battle_move_select:
            mvprintw(22, 0, "select your move: (1 or 2)");
            refresh();
            move_select = getch() - 49;
            if (move_select == 0) // if move_select = '1'
            {
                move(22, 0);
                clrtoeol();
                battlestate = do_battle_round(pc->active_pokemon, my_move_1, p, enemy_move);
            }
            else if (move_select == 1)
            {
                move(22, 0);
                clrtoeol();
                battlestate = do_battle_round(pc->active_pokemon, my_move_2, p, enemy_move);
            }
            else
            {
                goto wild_battle_move_select;
            }

        // mvwprintw(battle, 9, 2, pc->active_pokemon->species.c_str());
        wild_battle_state_label:
            if (battlestate == 1)
            {
                move(22, 0);
                clrtoeol();
                mvprintw(22, 0, "defeated the wild %s!", p->species.c_str());
                refresh();
                usleep(4000000);
                goto end_wild_battle;
            }
            else if (battlestate == -1)
            {
                move(22, 0);
                clrtoeol();
                mvprintw(22, 0, "the wild %s knocked out your pokemon", p->species.c_str());
                refresh();
                usleep(4000000);
                for (i = 0; i < (int)pc->my_pokemon.size(); i++)
                {
                    if (!pc->my_pokemon[i]->is_down)
                    {
                        firstavailablepokemon = i;
                        break;
                    }
                }
                if (firstavailablepokemon == -1) // no available pokemon
                {
                    move(22, 0);
                    clrtoeol();
                    mvprintw(22, 0, "all of your pokemon are defeated... you pass out (any key to continue)");
                    refresh();
                    getch();
                    goto end_wild_battle;
                }
                move(22, 0);
                clrtoeol();
                mvprintw(22, 0, "choose another from the menu:");
                refresh();
                make_pokemon_menu(pc);
                // goto refresh_battle;
            }
            // goto refresh_battle;
            break;
        case 2: // opens bag menu
            captured = make_bag_window(pc, true, p); // gets an int representing some action
            if (captured == 1) // throw a pokeball + room in bag
            {
                werase(battle);
                enemy_defeated = true;
                int N = rand() % 255;
                int threshhold = world.psptv[p->id_true]->capture_rate;
                if(N > threshhold) // first test passed
                {
                    N = rand() % 255;
                    threshhold = (p->hp_true * 255 * 4) / (p->current_hp * 12);
                    if(N > threshhold)
                    {
                        return_state = 1; // 1 for captured
                        mvprintw(22, 0, "you got it!");
                        refresh();
                        usleep(3000000);
                        goto end_wild_battle;
                    }
                    mvprintw(22, 0, "it broke out of the pokeball!");
                    refresh();
                }
                else
                {
                    mvprintw(22, 0, "it avoided the pokeball!");
                    refresh();
                }
                usleep(3000000);
                if(return_state != 1)
                {
                    if(rand() % 100 < 10)
                    {
                        move(22,0);
                        clrtoeol();
                        mvprintw(22, 0, "it ran away!");
                        refresh();
                        usleep(3000000);
                        goto end_wild_battle;
                    }
                }
            }
            //wrefresh(battle);
            break;
        case 3: // run
            escapechance = (pc->active_pokemon->speed_true * 32) / ((p->speed_true / 4) % 256) + 30 * escapeattempts;
            escapeattempts++;
            if (escapechance > rand() % 256)
            {
                mvprintw(22, 0, "ran away successfully!");
                refresh();
                usleep(4000000);
                goto end_wild_battle;
            }
            mvprintw(22, 0, "couldn't get away!");
            refresh();
            usleep(4000000);
            break;
        case 4: // opens pokemon menu
            make_pokemon_menu(pc);
            // battle but only for the enemy
            move_select = rand() % 2;
            if (move_select)
            {
                enemy_move = get_move_by_id(p->move1_id);
            }
            else
            {
                enemy_move = get_move_by_id(p->move2_id);
            }
            battlestate = do_battle_round(pc->active_pokemon, NULL, p, enemy_move);
            goto wild_battle_state_label;
            // goto refresh_battle;
            break;
        default:
            mvwprintw(battle, 14, 2, "unrecognized input");
        }
    }

// getch();
end_wild_battle:
    delwin(battle);
    return return_state;
}

int make_trainer_battle(character_t *trainer, character_t *pc)
{
    refresh();
    WINDOW *battle = newwin(19, 21, 2, 1);
    keypad(battle, TRUE);
    curs_set(0);

    int return_state = 0;
    int money_pot = 10;
    money_pot += trainer->my_pokemon.size() * (rand() % 10);
    bool enemy_defeated = false;
    bool player_defeated = false;

    // bool enemy_defeated = false;
    // bool player_defeated = false;
    setup_battle_window(battle);
    // intitalizing the board
    mvwprintw(battle, 3, 2, "enemy %s", trainer->active_pokemon->species.c_str());
    mvwprintw(battle, 4, 7, "vs");
    mvwprintw(battle, 5, 4, "%s", pc->active_pokemon->species.c_str());
    mvwprintw(battle, 6, 2, "any key to begin");
    wrefresh(battle);
    getch();

    char selection;   // = getch();
    int selectionNum; // = selection - 48; // shift number from char to int
    int battlestate;
    int firstavailablepokemon = -1;
    int i;
    move_t *my_move_1;
    move_t *my_move_2;
    move_t *enemy_move;
    int move_select;
    // int escapechance;
    // int escapeattempts = 0;

    pokemon_character *p = trainer->active_pokemon;
    while (!enemy_defeated && !player_defeated && selection != '+') // while player and enemy have pokemon left
    {
        // clear bottom line
        move(22, 0);
        clrtoeol();
        refresh();

        // refresh battle screen
        wclear(battle);
        setup_battle_window(battle);

        mvwprintw(battle, 2, 2, "%s", p->species.c_str());
        mvwprintw(battle, 3, 2, "hp: %d/%d", p->current_hp, p->hp_true);

        mvwprintw(battle, 6, 2, pc->active_pokemon->species.c_str());
        mvwprintw(battle, 7, 2, "hp: %d/%d", pc->active_pokemon->current_hp, pc->active_pokemon->hp_true);

        wrefresh(battle);
        int captured = 0;
        selection = getch();
        selectionNum = selection - 48;
        switch (selectionNum)
        {
        case 1: // fight opens move menu
            my_move_1 = get_move_by_id(pc->active_pokemon->move1_id);
            my_move_2 = get_move_by_id(pc->active_pokemon->move2_id);
            mvwprintw(battle, 8, 2, "1 - %s", my_move_1->identifier);
            mvwprintw(battle, 9, 2, "2 - %s", my_move_2->identifier);
            wrefresh(battle);

            // getting enemy move randomly
            move_select = rand() % 2;
            if (move_select)
            {
                enemy_move = get_move_by_id(p->move1_id);
            }
            else
            {
                enemy_move = get_move_by_id(p->move2_id);
            }
            move(22, 0);
            clrtoeol();
        trainer_battle_move_select:
            mvprintw(22, 0, "select your move: (1 or 2)");
            refresh();
            move_select = getch() - 49;
            if (move_select == 0) // if move_select = '1'
            {
                move(22, 0);
                clrtoeol();
                battlestate = do_battle_round(pc->active_pokemon, my_move_1, p, enemy_move);
            }
            else if (move_select == 1)
            {
                move(22, 0);
                clrtoeol();
                battlestate = do_battle_round(pc->active_pokemon, my_move_2, p, enemy_move);
            }
            else
            {
                goto trainer_battle_move_select;
            }

        // mvwprintw(battle, 9, 2, pc->active_pokemon->species.c_str());
        trainer_battle_state_label:
            if (battlestate == 1)
            {
                move(22, 0);
                clrtoeol();
                mvprintw(22, 0, "defeated the %s!", p->species.c_str());
                refresh();
                usleep(4000000);
                if (trainer->my_pokemon.size() > 1)
                {
                    for (i = 1; i < (int)trainer->my_pokemon.size(); i++)
                    {
                        if (!trainer->my_pokemon[i]->is_down) // if next pokemon is not down
                        {
                            // set trainer's active pokemon to next undowned
                            trainer->active_pokemon = trainer->my_pokemon[i];
                            p = trainer->active_pokemon; // set p to active pokemon
                            move(22, 0);
                            clrtoeol();
                            mvprintw(22, 0, "they deployed their %s", p->species.c_str());
                            refresh();
                            usleep(4000000);
                        }
                    }
                }
                if (i > (int)trainer->my_pokemon.size())
                {
                    move(22, 0);
                    clrtoeol();
                    mvprintw(22, 0, "you defeated the enemy trainer!", p->species.c_str());
                    refresh();
                    usleep(4000000);
                    pc->pokebucks += money_pot;
                    move(22, 0);
                    clrtoeol();
                    mvprintw(22, 0, "you won $%d", money_pot);
                    refresh();
                    usleep(4000000);
                    goto end_trainer_battle;
                }
            }
            else if (battlestate == -1)
            {
                move(22, 0);
                clrtoeol();
                mvprintw(22, 0, "the %s knocked out your pokemon", p->species.c_str());
                refresh();
                usleep(4000000);
                for (i = 0; i < (int)pc->my_pokemon.size(); i++)
                {
                    if (!pc->my_pokemon[i]->is_down)
                    {
                        firstavailablepokemon = i;
                        break;
                    }
                }
                if (firstavailablepokemon == -1) // no available pokemon
                {
                    move(22, 0);
                    clrtoeol();
                    mvprintw(22, 0, "all of your pokemon are defeated... you pass out (any key to continue)");
                    refresh();
                    getch();
                    goto end_trainer_battle;
                }
                move(22, 0);
                clrtoeol();
                mvprintw(22, 0, "choose another from the menu:");
                refresh();
                make_pokemon_menu(pc);
                // goto refresh_battle;
            }
            // goto refresh_battle;
            break;
        case 2: // opens bag menu
            captured = make_bag_window(pc, true, p);
            if (captured == 1)
            {
                werase(battle);
                enemy_defeated = true;
                return_state = 1; // 1 for captured
                goto end_trainer_battle;
            }
            else
            {
                mvprintw(22, 0, "it ran away!");
                return_state = -1; // -1 for ran away
                goto end_trainer_battle;
            }
            wrefresh(battle); // TODO
            break;
        case 3: // can't run from a trainer :(
            mvprintw(22, 0, "couldn't get away!");
            refresh();
            usleep(4000000);
            break;
        case 4: // opens pokemon menu
            make_pokemon_menu(pc);
            // battle but only for the enemy
            move_select = rand() % 2;
            if (move_select)
            {
                enemy_move = get_move_by_id(p->move1_id);
            }
            else
            {
                enemy_move = get_move_by_id(p->move2_id);
            }
            battlestate = do_battle_round(pc->active_pokemon, NULL, p, enemy_move);
            goto trainer_battle_state_label;
            // goto refresh_battle;
            break;
        default:
            mvwprintw(battle, 14, 2, "unrecognized input");
        }
    }

// getch();
end_trainer_battle:
    delwin(battle);
    return return_state;
}

pokemon_character *make_pokemon(int distance, pokemon_character *temp)
{
    int id;
    int level;
    int min_level;
    int max_level;
    std::string type;
    if (distance < 200)
    {
        min_level = 1;
        max_level = distance / 2;
    }
    else
    {
        min_level = (distance - 200) / 2;
        max_level = 100;
    }

    level = rand() % (max_level - min_level) + min_level;

    id = rand() % 151 + 1;
    // pokemon, moves, pokemon_moves, type_name, pokemon_stats, stats, pokemon_types, experience, pokemon_species
    std::string species = world.ptv[id - 1]->identifier;
    int type_id1 = 0; // = world.pttv[id-1]->type_id;
    int type_id2 = 0;
    std::string poke_type1;
    std::string poke_type2;
    int i;
    for (i = 0; i < (int)world.pttv.size(); i++)
    {
        if (world.pttv[i]->pokemon_id == id)
        {
            type_id1 = world.pttv[i]->type_id;
            continue;
        }
        if (world.pttv[i]->pokemon_id == id)
        {
            type_id2 = world.pttv[i]->type_id;
            break;
        }
        if (world.pttv[i]->pokemon_id > id)
        {
            break;
        }
    }
    for (i = 0; i < (int)world.tntv.size(); i++)
    {
        if (world.tntv[i]->type_id == type_id1)
        {
            poke_type1 = world.tntv[i]->name;
        }
        if (!type_id2 && world.tntv[i]->type_id == type_id2)
        {
            poke_type2 = world.tntv[i]->name;
        }
    }
    // poke_type1 = world.tntv[type_id]->name;

    // assigning all the base stats
    int hp, attack, defense, speed, special_attack, special_defense;
    int stat_index = (id - 1) * 6;

    hp = world.pstv[stat_index++]->base_stat;
    attack = world.pstv[stat_index++]->base_stat;
    defense = world.pstv[stat_index++]->base_stat;
    speed = world.pstv[stat_index++]->base_stat;
    special_attack = world.pstv[stat_index++]->base_stat;
    special_defense = world.pstv[stat_index++]->base_stat;

    // pokemon_character pokemon(hp, attack, defense, speed, special_attack, special_defense, species, level);
    temp = new pokemon_character(id, hp, attack, defense, speed, special_attack, special_defense,
                                 species, level, type_id1, type_id2);
    // assigning moves
    int first_available_index, cur_move, loopcount;

    for (i = 0; i < (int)world.pmtv.size(); i++)
    {
        if (world.pmtv[i]->pokemon_id == id) // first element where ids match
        {
            first_available_index = i; // start of possible moves for this pokemon
            break;
        }
    }

    // copy over moveset of the pokemon
    std::vector<pokemon_move_t *> move_set;
    for (i = first_available_index; world.pmtv[i]->pokemon_id == id; i++)
    {
        move_set.push_back(world.pmtv[i]);
    }
    int size = move_set.size();
    cur_move = rand() % size;
    bool cond = false;
    loopcount = 0;
    while (!cond)
    {
        if (move_set[cur_move]->level > temp->level || move_set[cur_move]->pokemon_move_method_id != 1) // if not level appropriate
        {
            cur_move = rand() % size; // new spot
            continue;
        }

        if (!temp->move1_id) // if no move 1, assign
        {
            temp->move1_id = move_set[cur_move]->move_id;
        }
        else if (!temp->move2_id && move_set[cur_move]->move_id != temp->move1_id) // if no move 2, assign
        {
            temp->move2_id = move_set[cur_move]->move_id;
            cond = true;
        }
        if (loopcount == size)
        {
            temp->level++;
            loopcount = 0;
        }
        cur_move = rand() % size; // new spot
        loopcount++;
    }

    return temp;
}

void make_pokemart_menu(character_t *pc)
{
    //int i;
    WINDOW *menu = newwin(10, 25, 10, 30);
    mvwprintw(menu, 1, 1, "buy:");
    mvwprintw(menu, 2, 1, "(1)pokeball - $20");
    mvwprintw(menu, 3, 1, "(2)potion - $20");
    mvwprintw(menu, 4, 1, "(3)revive - $100");

    mvwprintw(menu, 6, 1, "your money:");
    mvwprintw(menu, 7, 1, "$%d", pc->pokebucks);
    wrefresh(menu);


    char input = getch();
    while (input != '<')
    {
        move(22,0);
        clrtoeol();
        if(input == '1')
        {
            if(pc->pokebucks >= 20)
            {
                pc->bag[0]->count++;
                pc->pokebucks = pc->pokebucks - 20;
                
                mvprintw(22,0,"bought a pokeball");              
            }
            else
            {
                mvprintw(22,0,"not enough cash");
            }
            refresh();
        }
        else if(input == '2')
        {
            if(pc->pokebucks >= 20)
            {
                pc->bag[1]->count++;
                pc->pokebucks = pc->pokebucks - 20;
                mvprintw(22,0,"bought a potion");
            }
            else
            {
                mvprintw(22,0,"not enough cash");
            }
            refresh();
        }
        else if(input == '3')
        {
            if(pc->pokebucks >= 100)
            {
                pc->bag[3]->count++;
                pc->pokebucks = pc->pokebucks - 100;
                mvprintw(22,0,"bought a revive");
            }
            else
            {
                mvprintw(22,0,"not enough cash");
            }
            refresh();
        }
        usleep(2000000);
        move(22,0);
        clrtoeol();
        refresh();

        wmove(menu,7,0);
        wclrtoeol(menu);
        wmove(menu,7,0);
        mvwprintw(menu, 7, 1, "$%d", pc->pokebucks);
        wrefresh(menu);

        input = getch();
    }
    delwin(menu);
}

void swap_pokemon(pokemon_character *p1, pokemon_character *p2)
{
    pokemon_character tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

void make_pokecenter_menu(character_t *pc)
{
    int i;
    WINDOW *menu = newwin(10, 25, 10, 30);
    mvwprintw(menu, 1, 1, "(1)heal pokemon");
    mvwprintw(menu, 2, 1, "(2)access computer");
    wrefresh(menu);

    char input = 'c';// = getch();
    while (input != '<')
    {
        mvwprintw(menu, 1, 1, "(1)heal pokemon");
        mvwprintw(menu, 2, 1, "(2)access computer");
        wrefresh(menu);
        if(input == '1')
        {
            for (i = 0; i < (int)pc->my_pokemon.size(); i++)
            {
                pc->my_pokemon[i]->current_hp = pc->my_pokemon[i]->hp_true;
                pc->my_pokemon[i]->is_down = false;
                move(22,0);
                clrtoeol();
                mvprintw(22,0,"all pokemon are healed!");
                refresh();
                usleep(3000000);
                move(22,0);
                clrtoeol();
                refresh();
            }
        }
        if(input == '2')
        {
            //open up a trainer's menu type thing but with stored pokemon
            WINDOW *pokelist = newwin(10, 20, 2, 1);
            keypad(pokelist, TRUE);
            curs_set(0);
            int i;
            int print_start = 0;
            int print_depth;
            int pokesize = world.pokemon_storage.size();

            //int ch;
            while(input != 'q')
            {

                print_depth = 1;
                //refresh();
                //printw("%c", ch);
                wrefresh(pokelist);
                if (input == '\002') // down
                {
                    // if there are more trainers to be displayed on the next page, otherwise do nothing
                    // int size = sizeof(trainers_list)/sizeof(trainers_list[0]);
                    if (pokesize > (print_start + 5))
                        print_start += 5;
                }
                else if (input == '\003') // up
                {
                    print_start -= 5;
                    if (print_start < 0)
                        print_start = 0;
                }
                wclear(pokelist);
                for (i = print_start; i < pokesize && i < (5 + 5 * print_start); i++)
                {
                    
                    mvwprintw(pokelist, print_depth, 0, "(%d)%s", (print_depth), world.pokemon_storage[print_depth-1+print_start]->species.c_str());
                    print_depth++;
                }
                mvwprintw(pokelist, print_depth, 0, (char *)"# to swap");
                mvwprintw(pokelist, 0, 0, "  -Stored 'Mons-");
                wrefresh(pokelist);
                input = getch();
                if(input > 48 && input < 54)
                {
                    int selection = atoi(&input) + print_start - 1;
                    int swapper;
                    mvprintw(22,0,"swapping %s with", world.pokemon_storage[selection]->species.c_str());
                    refresh();

                    WINDOW *curlist = newwin(10, 20, 2, XDIM - 21);
                    keypad(curlist, TRUE);
                    curs_set(0);
                    int j;
                    mvwprintw(curlist, 0, 1, "**Your Pokemon**");
                    for (j = 0; j < (int)pc->my_pokemon.size(); j++)
                    {
                        if (pc->my_pokemon[j]->is_down)
                        {
                            wattron(curlist, COLOR_PAIR(6));
                        }
                        if (pc->my_pokemon[j] == pc->active_pokemon)
                        {
                            mvwprintw(curlist, j + 1, 0, "{%d}%s", j + 1, pc->my_pokemon[j]->species.c_str());
                        }
                        else
                        {
                        mvwprintw(curlist, j + 1, 0, " %d %s", j + 1, pc->my_pokemon[j]->species.c_str());
                        }
                        wattroff(curlist, COLOR_PAIR(6));
                    }
                    mvwprintw(curlist, i + 1, 1, "press # to swap");
                    wrefresh(curlist);
                    input = getch();
                    swapper = atoi(&input) - 1;
                    mvprintw(22,0,"swapping %s with %s", world.pokemon_storage[selection]->species.c_str(), pc->my_pokemon[swapper]->species.c_str());
                    refresh();
                    //change active pokemon to chosen 
                    pc->active_pokemon = pc->my_pokemon[swapper];
                    //swap stored and chosen
                    swap_pokemon(world.pokemon_storage[selection], pc->my_pokemon[swapper]);
                    
                    wclear(curlist);
                    wrefresh(curlist);
                    delwin(curlist);
                }
            };
            wclear(pokelist);
            wrefresh(pokelist);
            delwin(pokelist);
        }
        input = getch();
    }
    wclear(menu);
    wrefresh(menu);
    delwin(menu);
}
/*
    1.05 stuff
    7 or y Attempt to move PC one cell to the upper left.
    8 or k Attempt to move PC one cell up.
    9 or u Attempt to move PC one cell to the upper right.
    6 or l Attempt to move PC one cell to the right.
    3 or n Attempt to move PC one cell to the lower right.
    2 or j Attempt to move PC one cell down.
    1 or b Attempt to move PC one cell to the lower left.
    4 or h
*/

heap_node_t handle_PC_move(char move, character_t *pc, map_t *m, int preweight, int globalX, int globalY, int num_npc)
{
    int newX = 0;
    int newY = 0;
    int pokecenter_flag = 0;
    int pokemart_flag = 0;
    int talking_flag = 0;
    int enter_flag = 0;
    int trainers_flag = 0;

    heap_node_t newLoc;

    // distance calculation as distance from 0,0
    int distance = (int)sqrt((double)(power(globalY - 200, 2) + power(globalX - 200, 2)));
    // mvprintw(22,0,"%d", distance);
    //  checking for hostile npcs
    /*
        doing this at the beginning of the player turn is important because
        otherwise it will ask for a move and THEN do the checking, which we dont want
    */
    int i, j;
    for (i = -1; i < 2; i++) // checking for nearby pc
    {
        for (j = -1; j < 2; j++)
        {
            // printw("%d - %d\n", pc->x+j, pc->y+i);

            if (i == 0 && j == 0)
                continue;

            if (pc->y + i < YDIM && pc->y + i > 0 && pc->x + j < XDIM && pc->x + j > 0)
            {
                if (!m->char_map[pc->y + i][pc->x + j].is_pc && m->char_map[pc->y + i][pc->x + j].type != '\000' && m->char_map[pc->y + i][pc->x + j].hostile == 1)
                {
                    // printw("interaction\n");
                    // int pokecounter = 0;
                    character_t *enemy_trainer = &m->char_map[pc->y + i][pc->x + j];
                    make_interaction_window(enemy_trainer); // trainer greeting
                    int firstavailablepokemon = -1;

                    if (enemy_trainer->active_pokemon == NULL)
                    {
                        pokemon_character *pokemon = new pokemon_character;

                        pokemon = make_pokemon(distance, pokemon);

                        enemy_trainer->my_pokemon.push_back(pokemon); // give them their initial pokemon
                        enemy_trainer->active_pokemon = enemy_trainer->my_pokemon[0];

                        while (enemy_trainer->my_pokemon.size() < 6) // while they have < 6 pokemon
                        {
                            if (rand() % 100 < 60) // 60 % chance to gen n+1th pokemon
                            {
                                pokemon_character *anotherpokemon = new pokemon_character;

                                anotherpokemon = make_pokemon(distance, anotherpokemon);

                                enemy_trainer->my_pokemon.push_back(anotherpokemon);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    for (i = 0; i < (int)pc->my_pokemon.size(); i++)
                    {
                        if (!pc->my_pokemon[i]->is_down)
                        {
                            firstavailablepokemon = i;
                            break;
                        }
                    }
                    if (firstavailablepokemon == -1)
                    {
                        mvprintw(22, 0, "you have no conscious pokemon... the trainer refuses to fight");
                        refresh();
                        usleep(4000000);
                        wmove(stdscr, 22, 0);
                        clrtoeol();
                        refresh();
                    }
                    else
                    {
                        make_trainer_battle(&m->char_map[pc->y + i][pc->x + j], pc); // trainer battle
                    }

                    // refresh();
                    newLoc.x = pc->x;
                    newLoc.y = pc->y;
                    newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
                    return newLoc; // lose turn bc of attack
                }
            }
        }
    }

    // printw("%d %d\n", pc->x, pc->y);
    pokemon_character *p = new pokemon_character;
    switch (move)
    {
    case 'y':
    case '7': // upper left
        newX = pc->x - 1;
        newY = pc->y - 1;
        break;
    case 'k':
    case '8': // up
        newY = pc->y - 1;
        newX = pc->x;
        break;
    case 'u':
    case '9': // upper right
        newY = pc->y - 1;
        newX = pc->x + 1;
        break;
    case 'l':
    case '6': // right
        newX = pc->x + 1;
        newY = pc->y;
        break;
    case 'n':
    case '3': // lower right
        newX = pc->x + 1;
        newY = pc->y + 1;
        break;
    case 'j':
    case '2': // down
        newY = pc->y + 1;
        newX = pc->x;
        break;
    case 'b':
    case '1': // lower left
        newX = pc->x - 1;
        newY = pc->y + 1;
        break;
    case 'h':
    case '4': // left
        newX = pc->x - 1;
        newY = pc->y;
        break;
    case ' ':
    case '.':
    case '5':
        // do nothing!
        newLoc.x = pc->x;
        newLoc.y = pc->y;
        newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
        return newLoc;

        break;
    case 'g':
        //pokemon_character *p = new pokemon_character;
        p = make_pokemon(0, p);
        world.pokemon_storage.push_back(p);

        wmove(stdscr, 22,0);
        mvprintw(22,0,"got a new %s", p->species.c_str());
        refresh();
        break;
    case '>':
        enter_flag = 1;
        break;
    case 't':
        trainers_flag = 1;
        break;
    case 'i':
        make_bag_window(pc, false, NULL);
        break;
    case 'p':
        make_pokemon_menu(pc);
        m->char_map[pc->y][pc->x] = *pc;
        newLoc.x = pc->x;
        newLoc.y = pc->y;
        newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
        return newLoc;
        break;
    case 'f':
        // fly to coordinates
        int flyX, flyY;
        // get coords
        mvprintw(22, 0, "Enter x to fly to, then q to begin entering y: ");
        refresh();
        flyX = 0;
        flyY = 0;
        char num = getch();
        int neg = 1;
        while (num != 'q')
        {
            if (num == '-')
            {
                neg = -1;
                continue;
            }
            flyX = flyX * 10;
            flyX += atoi(&num);
            num = getch();
        }
        flyX = flyX * neg;
        mvprintw(22, 0, "                                               ");
        mvprintw(22, 0, "Enter y to fly to, then q to travel: ");
        refresh();
        num = getch();
        neg = 1;
        while (num != 'q')
        {
            if (num == '-')
            {
                neg = -1;
                continue;
            }
            flyY = flyY * 10;
            flyY += atoi(&num);
            num = getch();
        }
        flyY = flyY * neg;
        // set playerloc to center of map
        pc->x = 35;
        pc->y = 10;
        // set travel location
        newLoc.x = flyX;
        newLoc.y = flyY;
        newLoc.weight = -1;

        if (!world.w[newLoc.y][newLoc.x]->t) // if there is no map there
        {
            generate_map(newLoc.y, newLoc.x, num_npc, pc);
            movement_setup(newLoc.y, newLoc.x);
        }
        printw("%d %d %d", newLoc.x, newLoc.y, newLoc.weight);
        return newLoc;

        break;
    }

    // refresh();
    // usleep(1000000);
    if (m->t[newY][newX] == road_border) // exit map behavior
    {

        if (newY == 0) // move north
        {
            pc->x = 35;
            pc->y = 19;
            // this is a dummy location to change map in the main
            newLoc.x = globalX;
            newLoc.y = globalY - 1;
            newLoc.weight = -1;

            if (!world.w[globalY - 1][globalX]->t) // if there is no map there
            {
                generate_map(globalY - 1, globalX, num_npc, pc);
                movement_setup(globalY - 1, globalX);
            }
            return newLoc;
        }
        else if (newY == 20) // move south
        {
            pc->x = 35;
            pc->y = 1;
            if (!world.w[globalY + 1][globalX]->t) // if there is no map there
            {
                generate_map(globalY + 1, globalX, num_npc, pc);
                movement_setup(globalY + 1, globalX);
            }
            newLoc.x = globalX;
            newLoc.y = globalY + 1;
            newLoc.weight = -1;
            return newLoc;
        }
        else if (newX == 79) // move east
        {
            pc->x = 1;
            pc->y = 10;
            if (!world.w[globalY][globalX + 1]->t) // if there is no map there
            {
                generate_map(globalY, globalX + 1, num_npc, pc);
                movement_setup(globalY, globalX + 1);
            }
            newLoc.x = globalX + 1;
            newLoc.y = globalY;
            newLoc.weight = -1;
            return newLoc;
        }
        else if (newX == 0) // move west
        {
            pc->x = 78;
            pc->y = 10;
            if (!world.w[globalY][globalX - 1]->t) // if there is no map there
            {
                generate_map(globalY, globalX - 1, num_npc, pc);
                movement_setup(globalY, globalX - 1);
            }
            newLoc.x = globalX - 1;
            newLoc.y = globalY;
            newLoc.weight = -1;
            return newLoc;
        }
    }

    if (!m->char_map[newY][newX].is_pc && m->char_map[newY][newX].type != '\000') // if there is an npc there
    {
        talking_flag = 1;
    }
    else if (m->t[pc->y][pc->x] == poke_mart) // if there is a mart there
    {
        pokemart_flag = 1;
    }
    else if (m->t[pc->y][pc->x] == poke_center) // if there is a center there
    {
        pokecenter_flag = 1;
    }

    if (m->t[newY][newX] == tall_grass)
    {
        int chance = rand() % 5;
        if (!chance)
        {
            pokemon_character *pokemon = new pokemon_character;

            pokemon = make_pokemon(distance, pokemon);

            int firstavailablepokemon = -1;
            for (i = 0; i < (int)pc->my_pokemon.size(); i++)
            {
                if (!pc->my_pokemon[i]->is_down)
                {
                    firstavailablepokemon = i;
                    break;
                }
            }
            if (firstavailablepokemon == -1)
            {
                mvprintw(22, 0, "a wild pokemon approaches... but runs away");
                refresh();
                usleep(4000000);
                wmove(stdscr, 22, 0);
                clrtoeol();
                refresh();
            }
            else
            {
                int result = make_wild_pokemon_battle(pokemon, pc);
                // distance = 0;

                // m->char_map[pc->y][pc->x]
                if (result == 1)
                {
                    
                    if(pc->my_pokemon.size() >= 6)
                    {
                        world.pokemon_storage.push_back(pokemon); // putting the captured fella into storage
                        mvprintw(22,0,"pokemon full, putting %s into storage", pokemon->species);
                        refresh();
                        usleep(2000000);
                        wmove(stdscr,22,0);
                        clrtoeol();
                        refresh();
                    }
                    else
                    {
                        pc->my_pokemon.push_back(pokemon); // put captured into my_pokemon
                    }
        
                }
            }
            m->char_map[pc->y][pc->x] = *pc;
            newLoc.x = pc->x;
            newLoc.y = pc->y;
            newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
            return newLoc;
        }
    }

    // printw("%d - %d - %d\n", building_flag, enter_flag, talking_flag);

    if (pokecenter_flag && enter_flag) // enter center behavior
    {
        make_pokecenter_menu(pc);
        newLoc.x = pc->x;
        newLoc.y = pc->y;
        newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
        return newLoc;
    }

    if (pokemart_flag && enter_flag) // enter center behavior
    {
        make_pokemart_menu(pc);
        newLoc.x = pc->x;
        newLoc.y = pc->y;
        newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
        return newLoc;
    }

    if (trainers_flag) // trainer menu
    {
        make_trainers_window(m);
    }

    if (talking_flag) // moving into a non-hostile trainer
    {
        make_interaction_window(&m->char_map[newY][newX]);
        newLoc.x = pc->x;
        newLoc.y = pc->y;
        newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
        return newLoc;
    }

    if (m->t[newY][newX] == trees || m->t[newY][newX] == rocks ||
        m->t[newY][newX] == world_border)
    {
        mvwprintw(stdscr, 0, 0, "You can't move there, silly!");
        usleep(1000000);
        newLoc.x = pc->x;
        newLoc.y = pc->y;
        newLoc.weight = preweight + get_weight(m->t[pc->y][pc->x], 0);
        return newLoc; // for "unable to move into location" -1 index = null
    }

    newLoc.weight = get_weight(m->t[pc->y][pc->x], 0); // old weight into return node
    newLoc.x = newX;                                   // set x and y
    newLoc.y = newY;

    // m->char_map[newY][newX] = *(new player());// = (character_t){NULL, malloc(sizeof(player)), newX, newY}; // malloc for new location
    m->char_map[newY][newX] = m->char_map[pc->y][pc->x]; // copy pc over
    m->char_map[newY][newX].x = newX;                    // change x
    m->char_map[newY][newX].y = newY;                    // change y

    m->char_map[pc->y][pc->x].is_pc = false;
    /*
        TEMPORARY THING FOR TRANFERING PC OVER TO ANOTHER SPOT, TO BE REVISED LATER

    character_t newpc;
    player newp;
    newpc->pc = &newp;
    m->char_map[newY][newX] = newpc; //put updated pc in new spot

        END OF THAT
    */
    // delete &(m->char_map[pc->y][pc->x]); // = (character_t){NULL, NULL, 0, 0}; // free the old pc

    newLoc.weight = preweight + get_weight(m->t[newY][newX], 0); // add on new weight

    printw("%d, %d - new place", m->char_map[newY][newX].x, m->char_map[newY][newX].y, talking_flag);

    // refresh();
    return newLoc;
}

int parsefiles(int print)
{
    int val;
    /* 1.07 stuff */
    // char *filename = (char *)malloc(20);

    std::string curFilePath; // = (char *)malloc(100);
    std::string base = "share/cs327/pokedex/pokedex/data/csv/";

    std::string filename = "pokemon";
    curFilePath = base + filename + ".csv";

    std::ifstream i(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    char *curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');
    int j;

    while (i.peek() != -1)
    {
        pokemon_t *pt = (pokemon_t *)malloc(sizeof(pokemon_t));
        // std::cout << i.peek() << std::endl;

        int val = 0; // this is the container number for everything
        for (j = 0; i.peek() != ','; j++)
        {
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pt->id = val;
        i.get(); // skip comma

        // char pokename[50];
        // std::string name;
        for (j = 0; i.peek() != ','; j++)
        {
            pt->identifier[j] = (char)i.get();
        }
        // char *s = pokename;
        // name = strdup(pokename);
        // pt->identifier = pokename;
        i.get(); // skip comma

        val = 0;
        for (j = 0; i.peek() != ','; j++)
        {
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pt->species_id = val;
        i.get(); // skip comma

        val = 0;
        for (j = 0; i.peek() != ','; j++)
        {
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pt->height = val;
        i.get(); // skip comma

        val = 0;
        for (j = 0; i.peek() != ','; j++)
        {
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pt->weight = val;
        i.get(); // skip comma

        val = 0;
        for (j = 0; i.peek() != ','; j++)
        {
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pt->base_experience = val;
        i.get(); // skip comma

        val = 0;
        for (j = 0; i.peek() != ','; j++)
        {
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pt->order = val;
        i.get(); // skip comma

        val = 0;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pt->is_default = val;
        i.get(); // skip comma

        // printing
        if (print == 1)
        {
            std::cout << pt->id << " " << pt->identifier << " " << pt->species_id << " " << pt->height << " " << pt->weight
                      << " " << pt->base_experience << " " << pt->order << " " << pt->is_default << std::endl;
        }
        world.ptv.push_back(pt);
    }

    // moves

    // curFilePath; // = (char *)malloc(100);
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "moves";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath)
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        // pokemon_t *pt = (pokemon_t *)malloc(sizeof(pokemon_t));
        move_t *mt = (move_t *)malloc(sizeof(move_t));

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->id = val;
        i.get(); // skip comma

        // char movename[50] = "";
        for (j = 0; j < 50; j++)
        {
            mt->identifier[j] = '\000';
        }
        for (j = 0; i.peek() != ','; j++)
        {
            mt->identifier[j] = (char)i.get();
        }
        // char *s = movename;
        // mt->identifier = s;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->generation_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->type_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->power = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->pp = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->accuracy = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->priority = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->target_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->damage_class_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->effect_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->effect_chance = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->contest_type_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->contest_effect_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        mt->super_contest_effect_id = val;
        i.get(); // skip comma
        world.mtv.push_back(mt);
        if (print == 1)
        {
            (mt->id != INT_MAX) ? std::cout << mt->id : std::cout << "_";
            std::cout << " ";
            std::cout << mt->identifier << " ";
            (mt->generation_id != INT_MAX) ? std::cout << mt->generation_id : std::cout << "_";
            std::cout << " ";
            (mt->type_id != INT_MAX) ? std::cout << mt->type_id : std::cout << "_";
            std::cout << " ";
            (mt->power != INT_MAX) ? std::cout << mt->power : std::cout << "_";
            std::cout << " ";
            (mt->pp != INT_MAX) ? std::cout << mt->pp : std::cout << "_";
            std::cout << " ";
            (mt->accuracy != INT_MAX) ? std::cout << mt->accuracy : std::cout << "_";
            std::cout << " ";
            (mt->priority != INT_MAX) ? std::cout << mt->priority : std::cout << "_";
            std::cout << " ";
            (mt->target_id != INT_MAX) ? std::cout << mt->target_id : std::cout << "_";
            std::cout << " ";
            (mt->damage_class_id != INT_MAX) ? std::cout << mt->damage_class_id : std::cout << "_";
            std::cout << " ";
            (mt->effect_id != INT_MAX) ? std::cout << mt->effect_id : std::cout << "_";
            std::cout << " ";
            (mt->effect_chance != INT_MAX) ? std::cout << mt->effect_chance : std::cout << "_";
            std::cout << " ";
            (mt->contest_type_id != INT_MAX) ? std::cout << mt->contest_type_id : std::cout << "_";
            std::cout << " ";
            (mt->contest_effect_id != INT_MAX) ? std::cout << mt->contest_effect_id : std::cout << "_";
            std::cout << " ";
            (mt->super_contest_effect_id != INT_MAX) ? std::cout << mt->super_contest_effect_id : std::cout << "_";
            std::cout << std::endl;
        }
    }

    // pokemon moves
    // curFilePath; // = (char *)malloc(100);
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "pokemon_moves";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath);
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        pokemon_move_t *pmt = (pokemon_move_t *)malloc(sizeof(pokemon_move_t));
        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pmt->pokemon_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pmt->version_group_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pmt->move_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pmt->pokemon_move_method_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pmt->level = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pmt->order = val;
        i.get(); // skip comma

        world.pmtv.push_back(pmt);
        if (print == 1)
        {
            (pmt->pokemon_id != INT_MAX) ? std::cout << pmt->pokemon_id : std::cout << "_";
            std::cout << " ";
            (pmt->version_group_id != INT_MAX) ? std::cout << pmt->version_group_id : std::cout << "_";
            std::cout << " ";
            (pmt->move_id != INT_MAX) ? std::cout << pmt->move_id : std::cout << "_";
            std::cout << " ";
            (pmt->pokemon_move_method_id != INT_MAX) ? std::cout << pmt->pokemon_move_method_id : std::cout << "_";
            std::cout << " ";
            (pmt->level != INT_MAX) ? std::cout << pmt->level : std::cout << "_";
            std::cout << " ";
            (pmt->order != INT_MAX) ? std::cout << pmt->order : std::cout << "_";
            std::cout << std::endl;
        }
    }

    // type names
    // curFilePath; // = (char *)malloc(100);
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "type_names";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath);
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        type_name_t *tnt = (type_name_t *)malloc(sizeof(type_name_t));
        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        tnt->type_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        tnt->local_language_id = val;
        if (tnt->local_language_id != 9)
        {
            i.getline(curline, 300, '\n'); // go to end of line
            continue;                      // if the entry is not english i do not care
        }
        i.get(); // skip comma

        // char movename[50] = "";
        for (j = 0; i.peek() != '\n'; j++)
        {
            tnt->name[j] = i.get();
        }
        // char *s = movename;
        // tnt->name = s;
        i.get(); // skip comma

        world.tntv.push_back(tnt);
        if (print == 1)
        {
            (tnt->type_id != INT_MAX) ? std::cout << tnt->type_id : std::cout << "_";
            std::cout << " ";
            (tnt->local_language_id != INT_MAX) ? std::cout << tnt->local_language_id : std::cout << "_";
            std::cout << " ";
            std::cout << tnt->name << std::endl;
        }
    }

    // pokemon stats
    // curFilePath; // = (char *)malloc(100);
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "pokemon_stats";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath);
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        pokemon_stat_t *pst = (pokemon_stat_t *)malloc(sizeof(pokemon_stat_t));
        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->pokemon_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->stat_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->base_stat = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->effort = val;
        i.get(); // skip comma

        world.pstv.push_back(pst);
        if (print == 1)
        {
            (pst->pokemon_id != INT_MAX) ? std::cout << pst->pokemon_id : std::cout << "_";
            std::cout << " ";
            (pst->stat_id != INT_MAX) ? std::cout << pst->stat_id : std::cout << "_";
            std::cout << " ";
            (pst->base_stat != INT_MAX) ? std::cout << pst->base_stat : std::cout << "_";
            std::cout << " ";
            (pst->effort != INT_MAX) ? std::cout << pst->effort : std::cout << "_";
            std::cout << std::endl;
        }
    }

    // stats
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "stats";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath);
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        stat_t *st = (stat_t *)malloc(sizeof(stat_t));
        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        st->id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        st->damage_class_id = val;
        i.get(); // skip comma

        char statname[50] = "";
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            statname[j] = i.get();
        }
        char *s = statname;
        st->identifier = s;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        st->is_battle_only = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        st->game_index = val;
        i.get(); // skip comma

        world.stv.push_back(st);
        if (print == 1)
        {
            (st->id != INT_MAX) ? std::cout << st->id : std::cout << "_";
            std::cout << " ";
            (st->damage_class_id != INT_MAX) ? std::cout << st->damage_class_id : std::cout << "_";
            std::cout << " ";
            std::cout << st->identifier << " ";
            (st->is_battle_only != INT_MAX) ? std::cout << st->is_battle_only : std::cout << "_";
            std::cout << " ";
            (st->game_index != INT_MAX) ? std::cout << st->game_index : std::cout << "_";
            std::cout << std::endl;
        }
    }

    // pokemon types
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "pokemon_types";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath);
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        pokemon_type_t *ptt = (pokemon_type_t *)malloc(sizeof(pokemon_type_t));
        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        ptt->pokemon_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        ptt->type_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        ptt->slot = val;
        i.get(); // skip comma

        world.pttv.push_back(ptt);
        if (print == 1)
        {
            (ptt->pokemon_id != INT_MAX) ? std::cout << ptt->pokemon_id : std::cout << "_";
            std::cout << " ";
            (ptt->type_id != INT_MAX) ? std::cout << ptt->type_id : std::cout << "_";
            std::cout << " ";
            (ptt->slot != INT_MAX) ? std::cout << ptt->slot : std::cout << "_";
            std::cout << std::endl;
        }
    }

    // experience
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "experience";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath);
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        experience_t *et = (experience_t *)malloc(sizeof(experience_t));
        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        et->growth_rate_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        et->level = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        et->experience = val;
        i.get(); // skip comma

        world.etv.push_back(et);
        if (print == 1)
        {
            (et->growth_rate_id != INT_MAX) ? std::cout << et->growth_rate_id : std::cout << "_";
            std::cout << " ";
            (et->level != INT_MAX) ? std::cout << et->level : std::cout << "_";
            std::cout << " ";
            (et->experience != INT_MAX) ? std::cout << et->experience : std::cout << "_";
            std::cout << std::endl;
        }
    }

    // pokemon species
    base = "share/cs327/pokedex/pokedex/data/csv/";

    filename = "pokemon_species";
    curFilePath = base + filename + ".csv";

    // std::ifstream i(curFilePath);
    i.close();
    i.open(curFilePath);

    if (!i.is_open()) // if initial location doesnt exist
    {
        base = getenv("HOME");
        curFilePath = base + "/.poke327/pokedex/pokedex/data/csv/" + filename + ".csv";
    }

    i.open(curFilePath);

    if (!i.is_open()) // if my spot doesnt exist :(
    {
        std::cout << "file " << filename << " not found" << std::endl;
        return -1;
    }

    // file is there!
    curline = (char *)malloc(300);
    i.getline(curline, 300, '\n');

    while (i.peek() != -1)
    {
        pokemon_species_t *pst = (pokemon_species_t *)malloc(sizeof(pokemon_species_t));
        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->id = val;
        i.get(); // skip comma

        char pokename[20] = "";
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            pokename[j] = i.get();
        }
        char *s = pokename;
        pst->identifier = s;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->generation_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->evolves_from_species_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->evolution_chain_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->color_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->shape_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->habitat_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->gender_rate = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->capture_rate = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->base_happiness = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->is_baby = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->hatch_counter = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->has_gender_differences = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->growth_rate_id = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->forms_switchable = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->is_legendary = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->is_mythical = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->order = val;
        i.get(); // skip comma

        val = INT_MAX;
        for (j = 0; i.peek() != ',' && i.peek() != '\n'; j++)
        {
            if (val == INT_MAX)
                val = 0;
            val = val * 10;
            val += (i.get() - 48); // -48 bc ascii numbers 'start' with id 48 for 0
            // std::cout << val << std::endl;
        }
        pst->conquest_order = val;
        i.get(); // skip comma

        world.psptv.push_back(pst);

        if (print == 1)
        {
            (pst->id != INT_MAX) ? std::cout << pst->id : std::cout << "_";
            std::cout << " ";
            std::cout << pst->identifier << " ";
            (pst->generation_id != INT_MAX) ? std::cout << pst->generation_id : std::cout << "_";
            std::cout << " ";
            (pst->evolves_from_species_id != INT_MAX) ? std::cout << pst->evolves_from_species_id : std::cout << "_";
            std::cout << " ";
            (pst->evolution_chain_id != INT_MAX) ? std::cout << pst->evolution_chain_id : std::cout << "_";
            std::cout << " ";
            (pst->color_id != INT_MAX) ? std::cout << pst->color_id : std::cout << "_";
            std::cout << " ";
            (pst->shape_id != INT_MAX) ? std::cout << pst->shape_id : std::cout << "_";
            std::cout << " ";
            (pst->habitat_id != INT_MAX) ? std::cout << pst->habitat_id : std::cout << "_";
            std::cout << " ";
            (pst->gender_rate != INT_MAX) ? std::cout << pst->gender_rate : std::cout << "_";
            std::cout << " ";
            (pst->capture_rate != INT_MAX) ? std::cout << pst->capture_rate : std::cout << "_";
            std::cout << " ";
            (pst->base_happiness != INT_MAX) ? std::cout << pst->base_happiness : std::cout << "_";
            std::cout << " ";
            (pst->is_baby != INT_MAX) ? std::cout << pst->is_baby : std::cout << "_";
            std::cout << " ";
            (pst->hatch_counter != INT_MAX) ? std::cout << pst->hatch_counter : std::cout << "_";
            std::cout << " ";
            (pst->has_gender_differences != INT_MAX) ? std::cout << pst->has_gender_differences : std::cout << "_";
            std::cout << " ";
            (pst->growth_rate_id != INT_MAX) ? std::cout << pst->growth_rate_id : std::cout << "_";
            std::cout << " ";
            (pst->forms_switchable != INT_MAX) ? std::cout << pst->forms_switchable : std::cout << "_";
            std::cout << " ";
            (pst->is_legendary != INT_MAX) ? std::cout << pst->is_legendary : std::cout << "_";
            std::cout << " ";
            (pst->order != INT_MAX) ? std::cout << pst->order : std::cout << "_";
            std::cout << " ";
            (pst->conquest_order != INT_MAX) ? std::cout << pst->conquest_order : std::cout << "_";
            std::cout << std::endl;
        }
    }

    return 0;
}

void choose_starting_pokemon(character_t *pc)
{
    WINDOW *choose_starter = newwin(5, 14, 10, 33);
    keypad(choose_starter, TRUE);
    curs_set(0);

    pokemon_character *p1;
    pokemon_character *p2;
    pokemon_character *p3;
    p1 = make_pokemon(0, p1);
    p2 = make_pokemon(0, p2);
    p3 = make_pokemon(0, p3);

    mvwprintw(choose_starter, 0, 0, "Choose Starter");
    mvwprintw(choose_starter, 2, 0, "(1) %s", p1->species.c_str());
    mvwprintw(choose_starter, 3, 0, "(2) %s", p2->species.c_str());
    mvwprintw(choose_starter, 4, 0, "(3) %s", p3->species.c_str());

    wrefresh(choose_starter);
    // getch();

    char choice = getch();
    int select = atoi(&choice);
    while (select - 3 > 0)
    {
        choice = getch();
        select = atoi(&choice);
    }

    if (select == 1)
    {
        pc->my_pokemon.push_back(p1);
        // pc->my_pokemon.push_back(p3);
    }
    else if (select == 2)
    {
        pc->my_pokemon.push_back(p2);
        // pc->my_pokemon.push_back(p1);
    }
    else
    {
        pc->my_pokemon.push_back(p3);
        // pc->my_pokemon.push_back(p1);
    }

    wclear(choose_starter);

    mvwprintw(choose_starter, 0, 0, "Choose Starter");
    mvwprintw(choose_starter, 2, 2, "You chose:");
    mvwprintw(choose_starter, 3, 2, "%s", pc->my_pokemon[0]->species.c_str());
    pc->active_pokemon = pc->my_pokemon[0];

    wrefresh(choose_starter);

    getch();
    delwin(choose_starter);
}

int main(int argc, char *argv[])
{
    parsefiles(0);

    initscr(); /* Start curses mode 		*/
    raw();     /* Line buffering disabled	*/
    noecho();  /* Don't echo() while we do getch */

    keypad(stdscr, TRUE); /* We get F1, F2 etc..		*/

    start_color();

    refresh();

    // testing pokemon creation
    // pokemon_character pokemon;// = (pokemon_character *)malloc( sizeof(pokemon_character));// = new pokemon_character();
    // pokemon->attack_true = 0;
    // int distance = (int)sqrt((double)(power(200 - 200, 2) + power(200 - 200, 2)));
    // pokemon = *(make_pokemon(distance, &pokemon));

    // make_pokemon_window(pokemon);

    init_pair(1, COLOR_CYAN, COLOR_BLACK);    // for npcs
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // for grass
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);  // for trees
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK); // for rocks
    init_pair(5, COLOR_WHITE, COLOR_BLACK);   // for roads
    init_pair(6, COLOR_WHITE, COLOR_RED);     // for buildings

    srand(time(NULL));

    map_t cur_map;
    // char in;
    int x, y;
    // int a, b;
    int num_npc = 6;
    // int npc_code;
    // heap_t move_priority;

    character_t *pc = new player();
    // player playercharacter;
    // pc->pc = &playercharacter;

    x = 200; // not 201 bc arrays start at 0
    y = 200;

    if (argc > 1)
    {

        if (!strcmp(argv[1], "--numtrainers"))
        {
            // char *num = argv[3];

            num_npc = atoi(argv[2]);
        }
    }

    // assigning pc loc
    if (rand() % 2 > 0)
    {
        pc->x = 35;
        pc->y = rand() % 18 + 1;
    }
    else
    {
        pc->x = rand() % 78 + 1;
        pc->y = 10;
    }
    choose_starting_pokemon(pc);

    // world.w[y][x]->char_map[pc->y][pc->x] = malloc( sizeof(pc));

    if (!world.w[y][x])
    { // if there is no  world yet
        // printf("shoud not print ususally");
        // world.w[y][x] = new map_t();// = malloc(sizeof(cur_map));

        // printf("painting map\n");
        // paint_map(world.w[y][x], y, x);
        generate_map(y, x, num_npc, pc);
        movement_setup(y, x);
        print_map(world.w[y][x], num_npc);
        // character_t found_pc = search_pc(world.w[y][x]);
        // choose_starting_pokemon(&found_pc);
        x = 200;
    }

    // print_map(world.w[y][x], pc, npc_list, num_npc);

    // printf("w%d x%d y%d", move_priority.q[1].weight, move_priority.q[1].x, move_priority.q[1].y);

    /*
    for (a = 1; a < (num_npc + 2); a++)
    {
        int i = move_priority.q[a].x;
        int j = move_priority.q[a].y;
        printf("%d: w%d x%d y%d\n", a, move_priority.q[a].weight, i, j);
    }
    */

    // printf("sorted\n");

    /*
    for (a = 1; a < (num_npc + 2); a++)
    {
        int i = move_priority.q[a].x;
        int j = move_priority.q[a].y;
        //printf("%d: w%d x%d y%d\n", a, move_priority.q[a].weight, j, i);
        // if(world.w[y][x]->char_map[i][j]->npc){
        //   printf("%c\n", world.w[y][x]->char_map[i][j]->npc->type);
        //   }
    }
    */

    while (!is_empty(&(world.w[y][x]->move_priority)))
    { // runtime loop
        heap_node_t tmp = heap_pop(&(world.w[y][x]->move_priority));

        /*
        if (world.w[y][x]->char_map[tmp.y][tmp.x].npc)
        {
            printf("type %c\n", world.w[y][x]->char_map[tmp.y][tmp.x].npc->type);
        }
        */

        if (world.w[y][x]->char_map[tmp.y][tmp.x].is_pc)
        { // if it is the pc
            print_map(world.w[y][x], num_npc);
            char ch = getch();
            if (ch == 'Q') // quit the game
            {
                break;
            }
            character_t thispc = search_pc(world.w[y][x]);
            tmp = handle_PC_move(ch, &thispc /*world.w[y][x]->char_map[tmp.y][tmp.x]*/, world.w[y][x], tmp.weight, x, y, num_npc);

            // handling changing maps
            if (tmp.weight == -1)
            {
                x = tmp.x;
                y = tmp.y;
                print_map(world.w[y][x], num_npc);
                // reset tmp with correct pc vals
                character_t tmpc = search_pc(world.w[y][x]);
                tmp.x = tmpc.x;
                tmp.y = tmpc.y;
                tmp.weight = get_weight(world.w[y][x]->t[tmp.y][tmp.x], 0);
            }
            // printw(" now at %d %d with weight %d\n", tmp.x, tmp.y, tmp.weight);
            refresh();
            usleep(250000);

            heap_add(&(world.w[y][x]->move_priority), tmp.weight, tmp.x, tmp.y);
        }
        else // if (world.w[y][x]->char_map[tmp.y][tmp.x].npc->type == 'p')
        {
            // printw("j"); //checking npc movements
            // refresh();
            character_t foundPc = search_pc(world.w[y][x]);

            tmp = handle_movement(world.w[y][x]->char_map, world.w[y][x], tmp.x, tmp.y, tmp.weight, foundPc);

            heap_add(&(world.w[y][x]->move_priority), tmp.weight, tmp.x, tmp.y);
            // print_map(world.w[y][x], num_npc);
        }
        heap_sortify(&(world.w[y][x]->move_priority));
    }

    endwin();

    return 0;
}
