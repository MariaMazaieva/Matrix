#include <stdio.h>
#include <stdlib.h>


struct matrix {
    int rows;
    int cols;
    int *elements;
}matrix;

int* control_input(int *ret);
void free_matrix(struct matrix *m);
struct matrix addition(struct matrix *m,struct matrix *m2);
struct matrix subtraction(struct matrix *m,struct matrix *m2);
struct matrix multiplication(struct matrix *m,struct matrix *m2);
int is_valid(char sign, struct matrix *m, struct matrix *m2);
void print_matrix(struct matrix *m);
void count_matrix(struct matrix *m,struct matrix *m2);
void create_matrix(struct matrix *m, int rows, int cols);
int* read_input(int r, int c, struct matrix *m, int *ret);



int main(void) {
    int ret = EXIT_SUCCESS;
    control_input(&ret);
    return ret;
}

/*Function reads numbers rows and columns, math signs(-,+,*); creates needed matrixes and operates with them.
 * @var r,c   - two  numbers  entered from input (rows,columns).
 * @ret - EXIT_SUCCESS, otherwise programme returns error code (100).
 * @var sign - reads FIRST sign(-,+,*) entered from stdin for further operations.
 * @var sign2 - reads SECOND and every next sign(-,+,*) entered from stdin for further operations.
 * struct matrix m_1 - created(func: create_matrix) out of the loop as first, later on is rewritten for further operations.
 * struct matrix m_2 - created(func: create_matrix) in the loop, later on is used to enter new matrix to operate with matrix m_1 =>
    =>rewritten if sign_2 is '*', m_1 stays untouched.
 * struct matrix m_3 - created(func: create_matrix) in the loop IF sign_2 is '*', later on operates with matrix m_2.
    In other words: If sign_2 == '*'-> create matrix m_3 -> m_2 = m_2 * m_3... m_1 operates with m_2
 * For cases when there is more multiplications used loop while(sign_2 == '*')
 * struct temp is used to simplify work with matrixes and later on is  rewritten m_1 = temp
 * For matrixes elements is used dynamic memory allocation.
 * Free all three at the end. !Freeing in struct multiplication TOO!.
 */

int* control_input(int *ret) {
    int r =0;
    int c = 0;
    scanf("%d %d", &r, &c);
    char  sign;
    char sign2 ;

    struct matrix m_1 = {1,2, NULL};
    create_matrix(&m_1, r, c); // creates first matrix
    read_input(r,c ,&m_1, ret); //fills first matrix
    struct matrix temp = {0, 0, NULL};
    struct matrix m_2 = {0, 0, NULL};
    struct matrix m_3 = {0, 0, NULL};
    scanf(" %c",&sign);

    while (1 && *ret == EXIT_SUCCESS) {
        scanf("%d %d", &r, &c);
        free_matrix(&m_2);
        create_matrix(&m_2, r, c);
        read_input(r,c ,&m_2, ret);
        while ((sign2 = getchar()) != '\n' && sign2 != EOF);
        sign2 = getchar();

        while (sign2 == '*') {
            scanf("%d %d", &r, &c);
            free_matrix(&m_3);
            create_matrix(&m_3, r, c);
            read_input(r,c ,&m_3, ret);
            m_2 = multiplication(&m_2, &m_3);
            free_matrix(&m_3);
            while ((sign2 = getchar()) != '\n' && sign2 != EOF); //skips all whitespaces
            sign2 = getchar();
        }
        if (sign == '+' ) {
            *ret= is_valid (sign, &m_1, &m_2);
            temp = addition(&m_1, &m_2);
        }
        if (sign == '-'  ) {
            *ret = is_valid (sign, &m_1, &m_2);
            temp = subtraction(&m_1, &m_2);
        }
        if (sign == '*' ) {
            if(m_1.cols == m_2.rows) {
                free_matrix(&temp);
                temp = multiplication(&m_1, &m_2);
            }else {
                fprintf(stderr, "Error: Chybny vstup!\n");
                *ret = 100;
                break;
            }
        }
        free_matrix(&m_1);
        m_1 = temp;

        if (sign2 == '\n'|| sign2 == EOF ) {
            break;
        }
        sign = sign2;
    }

    if(*ret == EXIT_SUCCESS )
        print_matrix(&m_1);
    free_matrix(&m_1);
    free_matrix(&m_2);
    free_matrix(&m_3);
    return ret;
}
/*Structure adds each number in two matrixes .
 *structure m3 is created as a result of addition; to simplify work.
 *elemnts allocated dunamically using malloc
 *access to elements is: we have 1D array here, so for matrix 3 rows x 4 columns to access position [1][2] is 1*4 + 2;
 */

struct matrix addition(struct matrix *m,struct matrix *m2) {
    struct matrix m3 = {0, 0, NULL};
    int rows = m -> rows;
    int cols = m -> cols;
    m3.elements = malloc(sizeof(int) * rows * cols);
    m3.rows = rows;
    m3.cols = cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m3.elements[i * cols + j] = 0;
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m3.elements[i * cols + j] = m -> elements[i * cols + j] + (m2 -> elements[i * cols + j]);
        }
    }
    return m3;
}

struct matrix subtraction(struct matrix *m,struct matrix *m2) {
    struct matrix m3 = {0, 0, NULL};
    int rows = m -> rows;
    int cols = m -> cols;
    m3.elements = malloc(sizeof(int) * rows * cols);
    m3.rows = rows;
    m3.cols = cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m3.elements[i * cols + j] = 0;
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m3.elements[i * cols + j] = m -> elements[i * cols + j] - (m2 -> elements[i * cols + j]);
        }
    }
    return m3;
}
/*Structure multiplyes each number in two matrixes .
 *structure m3 is created as a result of multiplication; to simplify work.
 *elemnts allocated dunamically using malloc
 *access to elements is: we have 1D array here, so for matrix 3 rows x 4 columns to access position [1][2] is 1*4 + 2;
 * third loop for (int k = 0; k < m->cols; k++)  is used to solve problem of multiplication
 * RULES for multiplying: N, P - rows and columns for m1, P, L - rows and columns for _1
    m1 columns   == m2 rows ---> m1 has to be N x P, m2 has to be P x L
 * k is used to track needed positions on matrixes:  m1[i][k] * m2[k][j]
    In English: m2 just turn left on 90 degrees and multiply each number for each row in m1. Then in row add its numbers between each other
 */

struct matrix multiplication(struct matrix *m,struct matrix *m2) {
    struct matrix m3 = {0, 0, NULL};
    int rows = m -> rows;
    int cols = m2 -> cols;
    m3.elements = malloc(sizeof(int) * rows * cols);
    m3.rows = rows;
    m3.cols = cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            m3.elements[i * cols + j] = 0;
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < m->cols; k++) {
                m3.elements[i * cols + j] += m->elements[i * m->cols + k] * m2->elements[k * m2->cols + j];
            }
        }
    }
    free_matrix(m); //free matrix for future uses
    free_matrix(m2); //free matrix for future uses
    return m3;
  }



int * read_input(int r, int c, struct matrix *m, int *ret) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            // * c, because we have 1D array here, so for matrix 3 rows x 4 columns to access position [1][2] is 1*4 + 2;
            if (scanf("%d", &m->elements[i * c + j]) != 1) {
                free_matrix(m);
                *ret = 100;
                fprintf(stderr,"Error: Chybny vstup!\n");
                return ret;
            }
        }
    }
    return 0;
}

void create_matrix(struct matrix *m, int rows, int cols) {
    m -> rows = rows;
    m -> cols = cols;
    m -> elements = malloc(sizeof(int)* rows *cols);
    if (m->elements == NULL) {
        exit(EXIT_FAILURE); // Terminate the program
    }

}
void free_matrix(struct matrix *m) {
    if (m->elements != NULL) {
        free(m->elements);
        m->elements = NULL;
    }
}

void print_matrix(struct matrix *m) {
    printf("%d %d\n", m->rows, m->cols);
    for (size_t i = 0; i < m->rows; i++) {
        for (size_t j = 0; j < m->cols; j++) {
            if(j == 0) {
                printf("%d", m->elements[i * m->cols + j]);
                continue;
            }
            printf(" %d", m->elements[i * m->cols + j]);
        }
        printf("\n");
    }
}

int is_valid(char sign, struct matrix *m, struct matrix *m2) {
    if (sign == '*' && m->cols != m2->rows) {
        fprintf(stderr, "Error: Chybny vstup!\n");
        return 100; // Invalid multiplication dimensions
    }

    if ((sign == '+' || sign == '-') && (m->cols != m2->cols || m->rows != m2->rows)) {
        fprintf(stderr, "Error: Chybny vstup!\n");
        return 100; // Invalid addition or subtraction dimensions
    }

    if (sign != '*' && sign != '+' && sign != '-') {
        fprintf(stderr, "Error: Chybny vstup!\n");
        return 100; // Unsupported operation
    }

    return EXIT_SUCCESS; // Valid operation
}



