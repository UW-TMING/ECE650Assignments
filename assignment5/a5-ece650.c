#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
pthread_mutex_t control_execute_print = PTHREAD_MUTEX_INITIALIZER;


#define handle_error(msg) \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg) \
do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

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

typedef struct Out_Vertex{
     int data;
     struct Out_Vertex * next;
} Out_Vertex;

typedef struct ArcNode {
    int adjvex ;
    struct ArcNode *nextarc;
} ArcNode ;

typedef struct VNode {
    int data ;
    int degree;
    ArcNode  *firstarc ;
} VNode, AdjList[NUM] ;

typedef struct {
    AdjList vertices;
    int vexnum,arcnum;
} ALGraph ;

Out_Vertex *head_sat = (Out_Vertex *) malloc (sizeof (Out_Vertex));
Out_Vertex *head1 = (Out_Vertex *) malloc (sizeof (Out_Vertex));
Out_Vertex *head2 = (Out_Vertex *) malloc (sizeof (Out_Vertex));


/* print the thread running time */
static void
pclock(char *msg, clockid_t cid)
{
     struct timespec ts;
     
     printf("%s", msg);
     if (clock_gettime(cid, &ts) == -1)
          handle_error("clock_gettime");
     printf("%4ld.%03ld\n", ts.tv_sec, ts.tv_nsec / 1000000);
     fflush(stdout);
}

/* Out_Vertex functions*/

void add_vertex (Out_Vertex * ov, Out_Vertex * head)
{
     //printf("\n%d insert into the link\n", ov->data);
     Out_Vertex *p = head;
     while (p->next != NULL)
     {
          if (p->next->data >= ov->data)
          {
               break;
          }
          p = p->next;
     }
     ov->next = p->next;
     p->next = ov;
}

void print_out_vertex (Out_Vertex *head)
{
     //printf("starting print the link\n");
     pthread_mutex_lock (&control_execute_print);
     
     if (head == head_sat)
     {
          printf("CNF-SAT-VC: ");
     }
     else if (head == head1)
     {
          printf("APPROX-VC-1: ");
     }
     else if (head == head2)
     {
          printf("APPROX-VC-2: ");
     }
     Out_Vertex *p = head->next;
     while (p != NULL)
     {
          Out_Vertex *temp = p;
          p = p->next;
          printf("%d ", temp->data);
          free(temp);
     }
     head->next = NULL;
     printf("\n");
     fflush(stdout);
     pthread_mutex_unlock (&control_execute_print);
}

/* above about the link */

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
         g->vertices[i].data = i ;
         g->vertices[i].firstarc = NULL ;
         g->vertices[i].degree = 0;
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
        printf("%d(%d)", g->vertices[i].data, g->vertices[i].degree);
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
         s->nextarc = g->vertices[e[k].x].firstarc;
         g->vertices[e[k].x].firstarc = s;
         g->vertices[e[k].x].degree++;
        
         ArcNode *s1 = (ArcNode *)malloc(sizeof(ArcNode)) ;
         s1->adjvex = e[k].x;
         s1->nextarc = g->vertices[e[k].y].firstarc;
         g->vertices[e[k].y].firstarc = s1 ;
         g->vertices[e[k].y].degree++;
    }
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
     }
     
     
     int result = SAT_Solve(mgr);
     dup2(bak, STDOUT_FILENO);
     
     if(result == SATISFIABLE) {
          int temp = SAT_NumVariables(mgr);
          for(i = 1; i <= temp; i++) {
               int a = SAT_GetVarAsgnment(mgr, i);
               if(a == 1) {
                    for (j = 1; j <= n; j++) {
                         for (m = 1; m <= k; m++) {
                              if (i == x[j][m]) {
                                   Out_Vertex *ov_temp = (Out_Vertex *)malloc(sizeof(Out_Vertex));
                                   ov_temp->data = j-1;
                                   add_vertex(ov_temp, head_sat);
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
          for (i = 0; i <= k; i++) {
               free(x[i]);
          }
          free(x);
          return 1;
     }
     else {
          for (i = 0; i <= k; i++) {
               free(x[i]);
          }
          free(x);
          return 0;
     }
}

/* CNF-SAT-VC*/
void* readGraphMinCover(void *temp) { //read min cover number from the console

          ALGraph *g = (ALGraph *)temp;
          int k, i;
          k = g->vexnum;
          for (i = 1; i <= k; i++) {
               if (print_min_cover(g, i) == 1) {
                    
                    break ;
               }
          }
     return NULL;
}

/* copy the graph and return the copy*/
void copyGraph (ALGraph *g, ALGraph *copy)
{
     copy->vexnum = g->vexnum;
     copy->arcnum = g->arcnum;
     int i;
     for (i = 0; i < g->vexnum; i++)
     {
          copy->vertices[i].data = g->vertices[i].data;
          copy->vertices[i].degree = g->vertices[i].degree;
          copy->vertices[i].firstarc = NULL;
          ArcNode *t = g->vertices[i].firstarc;
          while (t!=NULL)
          {
               ArcNode *t1 = (ArcNode *) malloc (sizeof(ArcNode));
               t1->adjvex = t->adjvex;
               t1->nextarc = copy->vertices[i].firstarc;
               copy->vertices[i].firstarc = t1;
               t = t->nextarc ;
          }
     }
}

/*  APPROX-VC-1 */

int get_higest_degree (ALGraph *copy, int *temp)
{
     int max, i;
     max = -10;
     for (i = 0; i < copy->vexnum; i++)
     {
          if (copy->vertices[i].degree > max)
          {
               max = copy->vertices[i].degree;
               *temp = i;
          }
     }
     return max;
}

void del_related_edge (ALGraph *copy, int source, int object)
{
     ArcNode *t = copy->vertices[source].firstarc;
     if (t == NULL)
     {
          return;
     }
     if (t->adjvex == object)
     {
          copy->vertices[source].firstarc = t->nextarc;
          copy->vertices[source].degree--;
          free(t);
          return;
     }
     while (t->nextarc != NULL)
     {
          if (t->nextarc->adjvex == object)
          {
               ArcNode *temp = t->nextarc;
               t->nextarc = t->nextarc->nextarc;
               copy->vertices[source].degree--;
               free(temp);
               return;
          }
          t = t->nextarc;
     }
}

void* APPROX_VC_ONE (void *temp)
{
          ALGraph *g_temp = (ALGraph *)temp;
          ALGraph *copy = (ALGraph *)malloc(sizeof(ALGraph));
          copyGraph(g_temp, copy);
          int highest_degree;
          highest_degree = -1;
          while (1)
          {
               if (get_higest_degree(copy, &highest_degree) == 0)
               {
                    break;
               }
               if (highest_degree != -1)
               {
                    ArcNode *t = copy->vertices[highest_degree].firstarc;
                    Out_Vertex *ov_temp = (Out_Vertex *)malloc(sizeof(Out_Vertex));
                    ov_temp->data = copy->vertices[highest_degree].data;
                    add_vertex(ov_temp, head1);
                    while (t != NULL)
                    {
                         ArcNode *t1 = t;
                         del_related_edge(copy, t->adjvex, copy->vertices[highest_degree].data);
                         copy->vertices[highest_degree].degree--;
                         t = t->nextarc;
                         free(t1);
                    }
                    copy->vertices[highest_degree].firstarc = NULL;
               }
               else
               {
                    break;
               }
          }
          free(copy);
     return NULL;
}

/* APPROX-VC-2 */

int del_all_realated_edges (ALGraph *copy, int i)
{
     ArcNode *temp = copy->vertices[i].firstarc;
     while (temp != NULL)
     {
          ArcNode *t = temp;
          temp = temp->nextarc;
          del_related_edge (copy, t->adjvex, copy->vertices[i].data);
          copy->vertices[i].degree--;
          free(t);
     }
     copy->vertices[i].firstarc = NULL;
     if (copy->vertices[i].degree == 0)
     {
          return 1;
     }
     else
     {
          return 0;
     }
}


void* APPROX_VC_TWO (void *temp)
{
          ALGraph *g_temp = (ALGraph *)temp;
          ALGraph *copy = (ALGraph *)malloc(sizeof(ALGraph));
          copyGraph(g_temp, copy);
          int i;
          for (i = 0; i < copy->vexnum; )
          {
               ArcNode* temp = copy->vertices[i].firstarc;
               if (temp != NULL)
               {
                    int data_temp = temp->adjvex;
                    
                    if(del_all_realated_edges(copy, i))
                    {
                         Out_Vertex *ov_temp = (Out_Vertex *)malloc(sizeof(Out_Vertex));
                         ov_temp->data = copy->vertices[i].data;
                         add_vertex(ov_temp, head2);
                    }
                    if(del_all_realated_edges(copy, data_temp))
                    {
                         Out_Vertex *ov_temp = (Out_Vertex *)malloc(sizeof(Out_Vertex));
                         ov_temp->data = copy->vertices[data_temp].data;
                         add_vertex(ov_temp, head2);
                    }
               }
               i++;
          }
          free(copy);
          return NULL;
}

int main()
{
     
     ALGraph *g = (ALGraph *)malloc(sizeof(ALGraph));
     
     pthread_t thread_sat_id;
     pthread_t thread_vc_one_id;
     pthread_t thread_vc_two_id;
     
     clockid_t cid;
     int s;
     
     while (1)
     {
          char ch[100];
          if (scanf("%s",ch) == EOF)
          {
               free(head_sat);
               free(head1);
               free(head2);
               break ;
          }
          if (ch[0] == 'V')
          {
               readGraphVexnum(g);
          }
          else if (ch[0] == 'E')
          {
               readGraphEdges(g);

              
               pthread_create (&thread_sat_id, NULL, &readGraphMinCover, g);
               pthread_create (&thread_vc_one_id, NULL, &APPROX_VC_ONE, g);
               pthread_create (&thread_vc_two_id, NULL, &APPROX_VC_TWO, g);
               
               pthread_join(thread_sat_id, NULL);
               pthread_join(thread_vc_one_id, NULL);
               pthread_join(thread_vc_two_id, NULL);
               
               s = pthread_getcpuclockid(thread_sat_id, &cid);
               if (s != 0)
                    handle_error_en(s, "pthread_getcpuclockid");
               pclock("CNF-SAT:    ", cid);
               
               s = pthread_getcpuclockid(thread_vc_one_id, &cid);
               if (s != 0)
                    handle_error_en(s, "pthread_getcpuclockid");
               pclock("ONE:    ", cid);
               
               s = pthread_getcpuclockid(thread_vc_two_id, &cid);
               if (s != 0)
                    handle_error_en(s, "pthread_getcpuclockid");
               pclock("TWO:    ", cid);
               
               print_out_vertex(head_sat);
               print_out_vertex(head1);
               print_out_vertex(head2);
               
          }
     }
     return 0 ;
}