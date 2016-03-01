#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM 1000000

#ifndef __SAT_HEADER__
#define __SAT_HEADER__
#define SAT_Manager void *
#ifndef _SAT_STATUS_
#define _SAT_STATUS_
enum SAT_StatusT {
     UNDETERMINED,
     UNSATISFIABLE,
     SATISFIABLE,
     TIME_OUT,
     MEM_OUT,
     ABORTED
};
#endif

SAT_Manager SAT_InitManager(void);
void SAT_SetNumVariables(SAT_Manager mng,
                         int num_vars);

void SAT_AddClause(SAT_Manager          mng,
                   int *                clause_lits,
                   int                  num_lits,
                   int                  gid = 0);

int SAT_Solve(SAT_Manager mng);

int SAT_NumVariables(SAT_Manager mng);

int SAT_GetVarAsgnment(SAT_Manager      mng,
                       int              v_idx);

#endif

typedef struct Edges {
    int x;
    int y;
} Edges ;

typedef struct ArcNode {
    int adjvex ;
    struct ArcNode *nextarc;
} ArcNode ;

typedef struct VNode {
    int data ;
    ArcNode  *firstarc ;
} VNode, AdjList[NUM] ;

typedef struct {
    AdjList vertices;
    int vexnum,arcnum;
} ALGraph ;

//abover about the data structure

typedef struct Queue {
    int queue[NUM];
    int front,rear;
} Queue ;

void initQueue(Queue *q) {
    q->front = 0 ;
    q->rear = 0 ;
}

int fullQueue(Queue *q) {
    if (q->front==(q->rear+1)%NUM) {
        return 1 ; //full
    } else {
        return 0 ; //not full
    }
}

int emptyQueue(Queue *q) {
    if (q->front == q->rear){
        return 1 ; //empty
    } else {
        return 0 ;
    }
}

int enQueue(Queue *q, int val) {
    if (fullQueue(q)) {
        return 0 ; //full can not insert
    } else {
        q->queue[q->rear] = val ;
        q->rear = (q->rear+1)%NUM ;
        return 1 ;
    }
}

int deQueue(Queue *q, int *val) {
    if (emptyQueue(q)) {
        return 0 ;//empty can not return
    } else {
        *val = q->queue[q->front] ;
        q->front = (q->front+1)%NUM ;
        return 1 ;
    }
}

void print_Queue(Queue *q) {
    int i = q->front ;
    printf("Queue elements:\n") ;
    while (i%NUM != q->rear) {
        printf("%d ", q->queue[i++]) ;
    }
    printf("<%d,%d>\n", q->front, q->rear) ;
}

// above about the QUEUE

void print_result(int v[] , int end,int end1) { //print the result using recurrence
    if (end == -1) {
        return ;
    } else {
        print_result(v,v[end],end1);
        if (end == end1) {
            printf("%d",end);
        } else {
            printf("%d-",end);
        }
    }
}

void bfs(ALGraph *g, int start, int end){ //bfs algorithm
    int visited[g->vexnum];
    int i ;
    for (i=0; i<g->vexnum; i++) {
        visited[i] = -100 ;
    }
    Queue *q = (Queue *)malloc(sizeof(Queue)) ;
    initQueue(q);
    visited[start] = -1 ;
    enQueue(q,start) ;
    while (emptyQueue(q) == 0) {
        int *i = (int *)malloc(sizeof(int)) ;
        deQueue(q,i) ;
        if (*i == end) {
            if(visited[end] == -1){
                printf("%d-%d\n",end,end) ;
                return ;
            } else {
                print_result(visited,end,end);
                printf("\n") ;
                return ;
            }
        }
        ArcNode *s = g->vertices[*i].firstarc ;
        while (s != NULL) {
            int w = s->adjvex ;
            if (visited[w] == -100) {
                visited[w] = *i ;
                enQueue(q,w) ;
            }
            s = s->nextarc ;
        }
    }
    printf("Error: %d has no path to %d in this graph .\n", start, end) ;
    
}

void freeFormerGraph(ALGraph *g) { //when get another V , free this graph we created before
    int i ;
    for (i = 0; i<g->vexnum; i++) {
        g->vertices[i].data = 0 ;
        ArcNode *t = g->vertices[i].firstarc ;
        while (t != NULL) {
            ArcNode *t1 = t ;
            t = t->nextarc ;
            free(t1);
        }
        g->vertices[i].firstarc = NULL ;
    }
    g->vexnum = 0 ;
    g->arcnum = 0 ;
    
}

void readGraphVexnum(ALGraph *g) { //read vexnum from the console
    if (g->vexnum != 0) {
        freeFormerGraph(g);
    }
    int ch = getchar();
    int k = 0 ;
    while (ch!='\n') {
        if (ch>='0' && ch<='9') {
            k = k*10 + ch-'0';
        }
        ch = getchar();
    }
    (*g).vexnum = k ;
    int i;
    for (i=0;i<k;i++) {
        (*g).vertices[i].data = i ;
        (*g).vertices[i].firstarc = NULL ;
    }
}

int isAlreadyExist(Edges e[], int count, int sp, int ep){ //judge the <sp,ep> whether already exists
    int i;
    for (i = 0; i<count; i++) {
        if ((e[i].x == sp&&e[i].y==ep) || (e[i].x==ep&&e[i].y==sp)){
            return 1 ;
        }
    }
    return 0 ;
}

void print_Graph(ALGraph *g){ //print the structure of this graph
    int i ;
    for (i = 0 ;i < g->vexnum ;i++) {
        printf("%d", g->vertices[i].data);
        ArcNode *t = g->vertices[i].firstarc;
        while (t!=NULL) {
            printf("-->%d", t->adjvex);
            t = t->nextarc ;
        }
        printf("\n");
    }
}

void readGraphEdges(ALGraph *g){  //read edges from console and create edges for the graph
    Edges e[NUM];
    int count =0 ;
    int ch = getchar();
    while (ch!='\n') {
        if(ch=='<'){
            int sp=0,ep=0;
            while ((ch=getchar())!=',') {
                sp = sp*10+(ch-'0');
            }
            while ((ch=getchar())!='>') {
                ep = ep*10+(ch-'0');
            }
            e[count].x = sp;
            e[count++].y = ep ;
        }
        ch = getchar();
    }
    int i ;
    for (i = 0; i < count; i++) {
        if (e[i].x >= (*g).vexnum || e[i].y >= (*g).vexnum){
            printf("Error:vertices in edges <%d,%d> must be smaller than the index of the number of vexnum.\n", e[i].x, e[i].y);
            return ;
        }
    }
    g->arcnum = count;
    int k ;// the folling is to create a linklist of the graph
    for (k=0;k<count;k++) {
        ArcNode *s = (ArcNode *)malloc(sizeof(ArcNode));
        s->adjvex = e[k].y;
        s->nextarc = (*g).vertices[e[k].x].firstarc;
        (*g).vertices[e[k].x].firstarc = s;
        
        ArcNode *s1 = (ArcNode *)malloc(sizeof(ArcNode)) ;
        s1->adjvex = e[k].x;
        s1->nextarc = (*g).vertices[e[k].y].firstarc;
        (*g).vertices[e[k].y].firstarc = s1 ;
    }
}

void readGraphShortPath(ALGraph *g){ //read start and end from input
    int start, end ;
    scanf("%d %d",&start,&end);
    if (start >= g->vexnum || end >= g->vexnum) {
        printf("Error:vertices %d or %d must be smaller than the index of the number of vexnum.\n", start, end);
        return ;
    }
    bfs(g,start,end);
}

int print_min_cover(ALGraph *g, int k) {
     
     
     int n;
     n = g->vexnum;
//     printf("in print_min_cover n:%d k:%d\n", n, k);
     
     int p[2], bak;
     pipe(p);
     bak = dup(STDOUT_FILENO);
     dup2(p[1], STDOUT_FILENO);
     
     SAT_Manager mgr = SAT_InitManager();
     SAT_SetNumVariables(mgr, n * k);
     int c[n * k];
     int i, j;
     
     int **x = (int **) malloc (sizeof(int *) * (n + 1));
     for (i = 0; i <= n; i++) {
          x[i] = (int *) malloc (sizeof(int) * (k + 1));
     }
     
     
     int count = 1;
     for (i = 1; i <= n ; i++) {
          for (j = 1; j <= k; j++) {
               x[i][j] = count++;
          }
     }
     
     //add the first condition
     for (j = 1; j <= k; j++) {
          count = 0;
          for (i = 1; i <= n; i++) {
               c[count++] = (x[i][j] << 1);
          }
          SAT_AddClause(mgr, c, n);
     }
     
     //add the second condition
     int m;
     for (i = 1; i <= n; i++) {
          for (j = 1; j <= k-1; j++) {
               c[0] = (x[i][j] << 1) + 1;
               for (m = j + 1; m <= k; m++) {
                    c[1] = (x[i][m] << 1) + 1;
                    SAT_AddClause(mgr, c, 2);
               }
          }
     }
     
     
     //add the third condition
     for (j = 1; j <= k; j++) {
          for (i = 1; i <= n - 1; i++) {
               c[0] = (x[i][j] << 1) + 1;
               for (m = i+1; m <= n; m++) {
                    c[1] = (x[m][j] << 1) + 1;
                    SAT_AddClause(mgr, c, 2);
               }
          }
     }
     
     // add the last condition
     for (i = 0 ;i < g->vexnum ;i++) {
          int former, later;
          former = g->vertices[i].data;
          ArcNode *t = g->vertices[i].firstarc;
          while (t!=NULL) {
               count = 0;
               later = t->adjvex;
               for (j = 1; j <= k; j++) {
                    c[count++] = (x[former + 1][j] << 1);
                    c[count++] = (x[later + 1][j] << 1);
               }
               SAT_AddClause(mgr, c, 2 * k);
               t = t->nextarc ;
          }
          printf("\n");
     }
     
     
     int result = SAT_Solve(mgr);
     dup2(bak, STDOUT_FILENO);
     if(result == SATISFIABLE) {
//          printf("sat: "); fflush(stdout);
          int temp = SAT_NumVariables(mgr);
          for(i = 1; i <= temp; i++) {
               int a = SAT_GetVarAsgnment(mgr, i);
               if(a == 1) {
                    for (j = 1; j <= n; j++) {
                         for (m = 1; m <= k; m++) {
                              if (i == x[j][m]) {
                                   printf("%d ", j-1); fflush(stdout);
                              }
                         }
                    }
                    
               }
               else if(a == 0) {
                    //printf("%d ", -1*i); fflush(stdout);
               }
               else {
                    printf("Error!"); fflush(stdout);
               }
          }
          printf("\n"); fflush(stdout);
          for (i = 0; i <= k; i++) {
               free(x[i]);
          }
          free(x);
          return 1;
     }
     else {
//          printf("unsat.\n"); fflush(stdout);
          for (i = 0; i <= k; i++) {
               free(x[i]);
          }
          free(x);
          return 0;
     }
     
}

void readGraphMinCover(ALGraph *g) { //read min cover number from the console
     int k, i;
     k = g->vexnum;
     if (g->arcnum == 0) {
          return ;
     }
     for (i = 1; i <= k; i++) {
          if (print_min_cover(g, i)) {
               return ;
          }
     }
     
}

int main(){
    ALGraph *g = (ALGraph *)malloc(sizeof(ALGraph));
    while (1){
        char ch[100];
        if (scanf("%s",ch) == EOF) {
            break ;
        }
        if (ch[0] == 'V'){
            readGraphVexnum(g);
        } else if (ch[0] == 'E') {
            readGraphEdges(g);
            readGraphMinCover(g);
        } /*else if (ch[0] == 's') {
            readGraphShortPath(g);
        }*/
    }
    return 0 ;
}