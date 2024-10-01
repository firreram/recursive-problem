#include <stdio.h>
#include <stdlib.h>

int M=4; //param. M come var. globale
int flag=0;

typedef struct tessera{
    int values[4];
    int type; //0-hole 1-source/sink 2-pipe
}tessera_t;

void leggi_grid(int N, FILE *fp, tessera_t **grid){
    int cnt;
    for(int i=0;i<N;i++){
        grid[i] = malloc(N*sizeof(tessera_t));
        for(int j=0;j<N;j++){
            cnt=0;
            for(int k=0;k<4;k++) {fscanf(fp, "%d", &grid[i][j].values[k]); if (grid[i][j].values[k]) cnt++;}
            grid[i][j].type=cnt;
        }
    }
}

int verifica_connR(tessera_t *grid_1, int i, int out, int N, int src_index){
    int j;
    if(grid_1[i].type==1 && i!=src_index){ //al passo 1 i=src_index
        return 1;
    }
    switch(out){
        case 0: //caso out nord
            if (i-N >= 0 && grid_1[i-N].values[out+1]) { //se la tessera superiore si interfaccia con quella inferiore ricorri
                for(j=0;j<4;j++){ //ricerca dell'uscita della tessera superiore
                    if(j==out+1) continue; //se j=in salta per evitare di cosiderare l'ingresso come uscita (out è in per la tessera seguente)
                    if (grid_1[i-N].values[j]) break;
                }
                return verifica_connR(grid_1, i-N, j, N, src_index);
            }
            else{
                return 0;
            }
            break;
        case 1: //uguali a case 0
            if (i+N < N*N && grid_1[i+N].values[out-1]) {
                for(j=0;j<4;j++){
                    if(j==out-1) continue;
                    if (grid_1[i+N].values[j]) break;
                }
                return verifica_connR(grid_1, i+N, j, N, src_index);
            }
            else{
                return 0;
            }
            break;
        case 2:
            if (i%N!=0 && grid_1[i-1].values[out+1]) {
                for(j=0;j<4;j++){
                    if(j==out+1) continue;
                    if (grid_1[i-1].values[j]) break;
                }
                return verifica_connR(grid_1, i-1, j, N, src_index);
            }
            else{
                return 0;
            }
            break;
        case 3:
            if (i+1%N!=0 && grid_1[i+1].values[out-1]) {
                for(j=0;j<4;j++){
                    if(j==out-1) continue;
                    if (grid_1[i+1].values[j]) break;
                }
                return verifica_connR(grid_1, i+1, j, N, src_index);
            }
            else{
                return 0;
            }
            break;
    }
    return 0; //ritorno di sicurezza se caso non coperto
}

int verifica_conn(tessera_t *grid_1, int N){
    int src_index=0;
    int i;
    while(src_index<N*N && grid_1[src_index].type!=1){ //identifica la prima sorgente disponibile e parte dall'indice trovato
        src_index++;
    }
    for (i=0;i<4;i++){
        if(grid_1[src_index].values[i]) break;
    } //cerca orientamento sorgente e parte
    return verifica_connR(grid_1, src_index, i, N, src_index);
}


int verifica_sol(int *sol, int N, tessera_t **grid, int n){
    int cnt;

    tessera_t *grid_1 = malloc((N*N)*sizeof(tessera_t));//semplice funzione che realizza la copia della griglia originale
    for(int i=0;i<N*N;i++){
        for(int k=0;k<4;k++){
            grid_1[i].values[k] = grid[i/3][i%3].values[k];

        }
        grid_1[i].type = grid[i/3][i%3].type;
    }

    //printf("%d", grid_1[3].values[2]); DBG copia matrice

    tessera_t tmp;
    if(n<=M){  //n=numero di mosse effettuate N=dim.tabella
        for(int i=0;i<n;i++){
            switch(sol[i]){
                case 1:
                    cnt=0;
                    for(int j=N*N-1;j>N-1;j--){  //se mossa su, parto da sotto e verifico, il for si ferma alla penultima riga, condizione di spostamento fuori da griglia verificata
                        if(grid_1[j-N].type == 0) { //se tessera immediatamente sopra è 0(vuota) switch
                            tmp = grid_1[j];
                            grid_1[j] = grid_1[j-N];
                            grid_1[j-N] = tmp;
                            cnt++;
                        } //funzione switch sotto
                    }
                    if(!cnt) return 0; //se cnt=0 soluzione non valida
                    break;
                case 2:
                    cnt=0;
                    for(int j=0;j<(N*N)-N;j++){
                        if(grid_1[j+N].type == 0) {
                            tmp = grid_1[j];
                            grid_1[j] = grid_1[j+N];
                            grid_1[j+N] = tmp;
                            cnt++;
                        }
                    }
                    if(!cnt) return 0;
                    break;
                case 3: cnt = 0;
                    for (int j=N*N-1;j>0;j--){
                        if (j%N==0) continue;
                        if (grid_1[j-1].type == 0){
                            tmp = grid_1[j];
                            grid_1[j] = grid_1[j-1];
                            grid_1[j-1] = tmp;
                            cnt++;
                        }
                    }
                    if(!cnt) return 0;
                    break;
                case 4: cnt=0;
                    for (int j=0;j<(N*N)-1;j++){
                        if ((j+1)%N==0) continue;
                        if (grid_1[j+1].type == 0){
                            tmp = grid_1[j];
                            grid_1[j] = grid_1[j+1];
                            grid_1[j+1] = tmp;
                            cnt++;
                        }
                    }
                    if(!cnt) return 0; //come case 3 ma verso dx
                    break;
            }
        }
        /* //DBG spostamenti
        for(int x=0;x<N*N;x++){
            for(int y=0;y<4;y++){
                printf("%d", grid_1[x].values[y]);
            }
            printf("\n");
        }
        */
        return verifica_conn(grid_1, N);
    }
    return 0;
}

void leggi_sol(FILE *fp, tessera_t **grid, int N){
    int n, *sol;
    fscanf(fp, "%d", &n);
    sol = malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
        fscanf(fp, "%d", &sol[i]);
    }
    //stampa esito verifica

    if (verifica_sol(sol, N, grid, n)){
        printf("soluzione valida!\n");
    }
    else {
        printf("soluzione non valida\n");
    }
}

void genera_sol(int pos,tessera_t **grid,int i,int *sol,int N){ //disposizioni ripetute delle 4 mosse possibili
    if(pos>=i){
        //DBG disposizioni
        /*
        for(int k=0;k<i;k++){
            printf("%d", sol[k]);
        }
        printf("\n");
        */
        if(verifica_sol(sol, N, grid, i)) flag=1; //utilizzo la stessa funzione del punto 2, i=numero di mosse
        //printf("%d\n", flag);
        return;
    }
    for(int j=1;j<5;j++){
        sol[pos] = j;
        genera_sol(pos+1, grid, i, sol, N);
    }
}

void cerca_sol(tessera_t **grid, int N){
    for(int i=1;i<=M;i++) {
        int *sol=malloc(i*sizeof(int));
        if(!flag){
            genera_sol(0, grid, i, sol, N);
        }
        if(flag){
            printf("soluzione ottima: %d", i);
            return;
        }
        free(sol);

    }
    printf("soluzione non trovata!\n");
}

int main(int argc, char **argv){
    FILE *fp = fopen("../grid.txt", "r");
    int N, T;
    fscanf(fp, "%d %d", &N, &T);
    tessera_t **grid = malloc(N*sizeof(tessera_t*));
    leggi_grid(N, fp, grid);
    fclose(fp);

    //printf("%d", grid[2][0].values[2]); //DBG struttura dati

    fp = fopen("../soluzione.txt", "r");
    leggi_sol(fp, grid, N);
    fclose(fp);
    cerca_sol(grid, N);
    return 0;
}
