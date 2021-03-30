#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minesweeper.h"

// Implement my N-dimensional minesweeper
int TOTAL_CELL_NUM = 1;
int SELECT_NUM = 0;
int TOTAL_NON_MINE_CELL_NUM = -1;

// get the dim weight
int get_dim_weight(int dim, int* dim_sizes, int i){
    int ret = 1;
    for(int j = i+1; j < dim; j++){
        ret *= dim_sizes[j];
    }
    return ret;
}

//convert N-D to 1-D coordinate
int n_dim_to_one_dim(int dim, int *dim_sizes, int *coords){
    int ret = 0;
    for (int i = 0; i < dim; i++){
        ret += coords[i] * get_dim_weight(dim, dim_sizes, i);
    }
    return ret;
}


//convert 1-D to N-D coordinate
void one_dim_to_n_dim(int index, int dim, int *dim_sizes, int *coords){
    for (int i = 0; i < dim; i++){
        int dim_weight = get_dim_weight(dim, dim_sizes, i);
        // printf("dim_w=%d\n", dim_weight);
        coords[i] = index / dim_weight;
        // printf("index=%d\n", index);
        index = index % dim_weight;
    }
}

void test__get_dim_weight() {
    int dim_sizes[] = {3,4,5};
    int res;
    res = get_dim_weight(3, dim_sizes, 0);
    printf("%d\n", res); // expect 20
    res = get_dim_weight(3, dim_sizes, 1);
    printf("%d\n", res); // expect 5
    res = get_dim_weight(3, dim_sizes, 2);
    printf("%d\n", res); // expect 1
}

void init_game(struct cell *game, int dim, int *dim_sizes, int num_mines, int **mined_cells){
    // get total cell number
    for (int i = 0; i < dim; i++){
        TOTAL_CELL_NUM *= dim_sizes[i];
    }
    //init array with the size of cell number
    memset(game, 0, (int)sizeof(struct cell) * TOTAL_CELL_NUM); 
    
    TOTAL_NON_MINE_CELL_NUM = TOTAL_CELL_NUM - num_mines;

    //calc coords for each cell and convert it to N-D coordinate
    for (int i = 0; i < TOTAL_CELL_NUM; i++){
        one_dim_to_n_dim(i, dim, dim_sizes, game[i].coords);
    }

    // calculate the adjacent relation of each cell
    for (int i = 0; i < TOTAL_CELL_NUM; i++){
        int append_index = 0;
        for (int j = 0; j < TOTAL_CELL_NUM; j++){
            if (i == j) {
                continue;
            }
            int is_adjacent = 1;
            for (int k = 0; k < dim; k++){
                if (abs(game[i].coords[k] - game[j].coords[k]) > 1) {
                    is_adjacent = 0;
                    break;
                }
            }
            if (is_adjacent) {
                game[i].adjacent[append_index] = &game[j];
                append_index++;
                game[i].num_adjacent++;
            }
        }
    }

    // calculate whether there's mine
    for (int i = 0; i < num_mines; i++)
    {
        int index= n_dim_to_one_dim(dim, dim_sizes, mined_cells[i]);
        game[index].mined = 1;
    }

    // calculate hint for each cell
    for (int i = 0; i < num_mines; i++)
    {
        int cur_mine_index = n_dim_to_one_dim(dim, dim_sizes, mined_cells[i]);
        // printf("mine[%d]->%d\n", i, game[cur_mine_index].num_adjacent);
        for (int j = 0; j < game[cur_mine_index].num_adjacent; j++){
            struct cell* nei = game[cur_mine_index].adjacent[j];
            // if it's neighbour is not mine, do +1 to hint
            if (nei->mined != 1) {
                nei->hint+=1;
            }
        }
        // printf("\n");
    }
    return;
}

void select_recursion(struct cell* cur_cell) {
    // by citing the given pseudo code in "assignment-1 specification":
    if (cur_cell->selected == 0) {
        if (cur_cell->hint != 0) {
            SELECT_NUM++;
            cur_cell->selected = 1;
        }
        else {
            // (cur_cell->selected != 1 && cur_cell->mined != 1) 
            // cur_cell->hint == 0
            SELECT_NUM++;
            cur_cell->selected = 1;
            for (int i = 0; i < cur_cell->num_adjacent; i++){
                struct cell* x = cur_cell->adjacent[i];
                if (x->selected == 0) {
                    select_recursion(x);
                }
            }
        }
    }
}

int select_cell(struct cell *game, int dim, int *dim_sizes, int *coords){

    int index = n_dim_to_one_dim(dim, dim_sizes, coords);
    struct cell* cur_cell = &game[index];

    if (cur_cell->mined == 1){
        cur_cell->selected = 1;
        return 1;
    }
    else if (cur_cell->selected == 1){
        return 0;
    }
    else{
        if (cur_cell->hint != 0) {
            cur_cell->selected = 1;
            SELECT_NUM++;
        } else {
            // when cur_cell->hint == 0, do recursion selecting
            if (cur_cell->selected == 0){
                select_recursion(cur_cell);
            }
        }
        // check whether we win
        if (SELECT_NUM == TOTAL_NON_MINE_CELL_NUM) {
            return 2;
        } else {
            return 0;
        }
    }
}