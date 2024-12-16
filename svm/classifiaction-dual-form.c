#include <stdlib.h>
#include <stdbool.h>
// Hard Margin SVM model for battery estimation based on read voltage (solved through the dual formulation)

// hardcoded size N of the training dataset
const int n = 200;

// computes the doct product between a and b, where a and b are vectors of size "size" and stores the result onto the variable "result"
// will be used as the kernel function k(xn, xm)
void dotProduct (float* a, float* b, unsigned int d, float* result) {
    (* result) = 0;
    for (int i = 0; i < d; i++)
        (* result) += a[i] * b[i];
}

// computes the resulting vector of the nonlinear transformation ?(x)
// will be used to enable the model to pick from a hypothesis set where the relation between input and output is nonlinear
/* 
two good possible hypothesis sets are:
h(x) = w1 + w2.x1 + w3.x1^2
h(x) = w1 + w2.e^x1
h(x) = w1 + w2.x1 (no nonlinear transformation)
*/
void linearTransformation(float* x, float* w, float* result) {
    (* result) = w[0] + w[1]*x[0] + w[2]*x[0]*x[0];
}

// computes estimation of the model for the output corresponding to the input vector xn
// where s is the set of support vectors
float estimate(float* a, float** s, float* xn, float* y, int d) {}

// checks if lagrange multiplier a follows the kkt conditions
bool kkt(float* w, float a, float* x, float y, float c, float error) {

    float yhxn;
    linearTransformation(x, w, &yhxn);
    yhxn = yhxn * y;
    bool followsKKT = false;
    float diff = yhxn - 1;
    // a(n) = 0 y(n)h(x(n)) ? 1 | Not support vectors
    if (!a)
        followsKKT = diff >= error;
    // a(n) = C y(n)h(x(n)) ? 1 | Support vectors on or violating the margin
    else if (a == c) 
        followsKKT = yhxn <= error;
    // 0 < a(n) < C y(n) h(x(n)) = 1 | Support vectors on the margin
    else
        followsKKT = yhxn <= error && yhxn >= 0;

    return followsKKT;
}

// implementation of an array list
// to list lagrange multipliers that don't follow kkt conditions
typedef struct arrayList
{
    int a[n];
    int max_size;
    int size;
    int first;
} arrayList;

// creates list
arrayList* createList() {
    arrayList* l = (arrayList *) malloc(sizeof(arrayList));
    l -> size = 0;

    return l;
}

// adds element e of type int to the array list of first element "first"
// assumes size n is always big enough as n is the number of lagrange multipliers for the problem
void addToList(arrayList* l, int e) {
    l -> size++;
    l -> a[l -> size] = e;
}

// returns element e of index i of the array list of first element "first"
int at(arrayList* l, int i) {
    if (i <= l -> size)
        return l -> a[i];
    else
        return NULL;
}

// returns the size of the array list refered to by the first element "first"
int getSize(arrayList* l) {
    return l -> size;
}

// resets array list l
void reset (arrayList* l) {
    l -> size = 0;
}

// deletes the liked list refered to by the first element "first"
void delete(arrayList* l) {
    free(l);
}

void updateLangrangeMultipliers(float* a, int i, int j, float c, float* y, float** x, int d) {
    // estimating boundaries
    
    // computing zeta = -? an.yn, given that n != i, y
    float z = 0;
    for (int k; k < n; k++) {
        if (k != i && k != j)
            z += a[k] * y[k];
    }
    z = -z;
    // optimizing for aj 
    /*
    aj,new = aj + (yj.(Ei - Ej)/k(xi, xi) + k(xj, xj) -2k(xi, xj))
    Ei = h(xi) - yi
    */
    float kernel_sum, result;
    dotProduct(x[i], x[i], d, &kernel_sum);
    dotProduct(x[j], x[j], d, &result);
    kernel_sum += result;
    dotProduct(x[i], x[j], d, &result);
    kernel_sum -= 2 * result; 
    float ei, ej;
    ei = estimate(a, x, x[i], y, d) - y[i];
    ej = estimate(a, x, x[j], y, d) - y[j];
    float new_aj = a[j] + (y[j] * (ei - ej) / kernel_sum);


}

// optimizes the lagrange multipliers in order to train the model
void train(float* w, float* a, float** x, int size, float* y, float c, float error, int max_iterations) {
    float ai, aj;

    for (int i = 0; i < max_iterations; i++) {
        // selecting ai
        if (i % 2 == 0) {
            arrayList* l = createList();
            for (int ii = 0; ii < size; ii++) {
                if (!kkt(w, a[ii], x[ii], y[ii], c, error))
                    addToList(l, i);
            }
            int index = rand() % getSize(l);
            ai = at(l, index);
        } else {
            arrayList* l = createList();
            for (int ii = 0; ii < size; ii++) {
                if (!kkt(w, a[ii], x[ii], y[ii], c, error) && 0 < a[ii] && a[ii] < c)
                    addToList(l, i);
            }
            int index = rand() % getSize(l);
            ai = at(l, index);
        }
    }
}

void main() {}