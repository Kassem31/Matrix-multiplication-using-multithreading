#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

void *matrixMul_2(void *arg)
{
    int *data = (int *)arg;
    int i, j;
    int result;
    int *p;
    p = malloc(data[1] * sizeof(int*));
    for (i = 2; i < data[1] + 2; i++)
    {   
        result=0;
        for (j = 2; j < data[0] + 2; j++) // data[0] represents the number of coloumns of the final matrix
            result = result + data[j] * data[j + data[0] * (i - 1)];
        p[i - 2]= result;
    }
    pthread_exit(p);
}

void *matrixMul_1(void *arg)
{
    int *data = (int *)arg;
    int i;
    int result = 0;
    for (i = 1; i <= data[0]; i++) // data[0] represents the number of coloumns of the final matrix
        result = result + data[i] * data[i + data[0]];
    int *p = (int *)malloc(sizeof(int));
    *p = result;
    pthread_exit(p);
}

int main(int argc,char* argv[])
{
    clock_t begin , end;
    double time;

    char *file;
    file = argv[1];
    FILE *f = fopen(file,"r");
    char buffer[256];
    fgets(buffer, 256, f);
    char *ptr = strtok(buffer, " ");
    const int r1 = atoi(ptr);
    ptr = strtok(NULL, "\n");
    const int c1 = atoi(ptr);
    char arrA[r1][c1];
    int i, j;
    for (i = 0; i < r1; i++)
    {
        j = 0;
        fgets(buffer, 256, f);
        ptr = strtok(buffer, " ");
        arrA[i][j] = atoi(ptr);
        j++;
        while (j < c1 - 1)
        {
            ptr = strtok(NULL, " ");
            arrA[i][j] = atoi(ptr);
            j++;
        }
        ptr = strtok(NULL, "\n");
        arrA[i][j] = atoi(ptr);
    }
    // extracting second array
    fgets(buffer, 256, f);
    ptr = strtok(buffer, " ");
    const int r2 = atoi(ptr);
    ptr = strtok(NULL, "\n");
    const int c2 = atoi(ptr);
    char arrB[r2][c2];
    for (i = 0; i < r2; i++)
    {
        j = 0;
        fgets(buffer, 256, f);
        ptr = strtok(buffer, " ");
        arrB[i][j] = atoi(ptr);
        j++;
        while (j < c2 - 1)
        {
            ptr = strtok(NULL, " ");
            arrB[i][j] = atoi(ptr);
            j++;
        }
        ptr = strtok(NULL, "\n");
        arrB[i][j] = atoi(ptr);
    }
    fclose(f);
    int count = 0;
    int *arrC;
    int productSize = r1 * c2;
    int l;
    // first method (thread for each element)
    begin = clock();
    pthread_t threads[productSize];
    for (i = 0; i < r1; i++)
        for (j = 0; j < c2; j++)
        {
            arrC = (int *)malloc((c1 + r2 + 1) * sizeof(int *));
            arrC[0] = c1;
            for (l = 0; l < c1; l++)
                *(arrC + l + 1) = arrA[i][l];
            for (l = 0; l < r2; l++)
                *(arrC + c1 + l + 1) = arrB[l][j];
            pthread_create(&threads[count], NULL, matrixMul_1, (void *)arrC);
            count++;
        }
    int row_count = 0, coloumn_count = 0;
    printf("by element method : \n");
    f = fopen("output-matrix.txt","w");
    for (i = 0; i < productSize; i++)
    {
        if (coloumn_count == c2)
        {
            printf("\n");
            fprintf(f,"\n");
            coloumn_count = 0;
            row_count++;
        }
        void *thread_ret;

        //Joining all threads and collecting return value
        pthread_join(threads[i], &thread_ret);
        int *p = (int *)thread_ret;
        printf("%d ", *p);
        fprintf(f,"%d ",*p);
        coloumn_count++;
    }
    end = clock();
    time = (double)(end-begin)/CLOCKS_PER_SEC;
    printf("\n\nexecution time = %f\n\n",time);
    fprintf(f,"\n\nexecution time = %f\n\n",time);
    //2nd method
    int *arrC2;
    int k;
    begin = clock();
    pthread_t threads2[r1];
    for (i = 0; i < r1; i++)
    {
        arrC2 = (int *)malloc((c1 + r2 * c2 + 2) * sizeof(int *));
        arrC2[0] = c1;
        arrC2[1] = c2;
        for (l = 0; l < c1; l++)
            *(arrC2 + l + 2) = arrA[i][l];
        for (l = 0; l < c2; l++)
        for(j = 0; j < r2; j++)
            *(arrC2 + 2 + c1 + j + r2*l) = arrB[j][l];
        pthread_create(&threads2[i], NULL, matrixMul_2, (void *)arrC2);
    }
//joining

//row_count = 0;
//coloumn_count = 0;
int *p2;
int matC[r1][c2];
printf("by row method : \n");
    for (i = 0; i < r1; i++)
    {
        //Joining all threads and collecting return value
        void *thread_ret2;
        pthread_join(threads2[i],&thread_ret2);
        p2 = (int *)thread_ret2;
        for (j = 0;j<c2;j++)
        {
          matC[i][j]=p2[j];
          printf("%d ", matC[i][j]);
          fprintf(f, "%d ",matC[i][j]);
        }  
         fprintf(f,"\n");
         printf("\n");
    }
    end = clock();
    time = (double)(end-begin)/CLOCKS_PER_SEC;
    printf("\nexecution time = %f\n",time);
    fprintf(f,"\nexecution time = %f\n",time);
    fclose(f);
    return 0;
}