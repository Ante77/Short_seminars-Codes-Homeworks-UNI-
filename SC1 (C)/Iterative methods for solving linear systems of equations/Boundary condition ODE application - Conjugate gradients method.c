#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "f2c.h"
#include "fblaswr.h"
#include "clapack.h"

// qr funkcija izracuna gornjetr R ali ne i Q nego sprema informacije o HH reflektorima pa dormqr koristimo da dobijemo matricu A
// u dormqr tau i a su iz qr funkcije a c je nasa dijagonalna matrica

void cg(integer n, doublereal* A, doublereal* b, doublereal* x_0, doublereal tol){
    
integer i, j;
// pravim rezidual kao u proslom zadatku
integer incb=1, incr=1;
char trans='N';
doublereal alfa=-1.0, beta= 1.0;
doublereal *rezidual=malloc(n*sizeof(doublereal));
dcopy_(&n, b, &incb, rezidual, &incr);
    
dgemv_(&trans, &n, &n, &alfa, A, &n, x_0, &incb, &beta, rezidual, &incr);
    
doublereal norma_reziduala=dnrm2_(&n, rezidual, &incr);
doublereal norma_b=dnrm2_(&n, b, &incb);
doublereal uvjet=norma_reziduala/norma_b;
// cg algoritam    
doublereal *d=malloc(n*sizeof(doublereal));
doublereal *adk=malloc(n*sizeof(doublereal));
doublereal *rezerva=malloc(n*sizeof(doublereal));
integer k=0;
char uplo='U';
doublereal alfica=1.0, betica=0.0;
integer incx=1, incy=1;

for(i=0; i<n; i++){
    d[i]=0.0;
    adk[i]=0.0;
    rezerva[i]=0.0;
}

dcopy_(&n, rezidual, &incb, d, &incr);

doublereal stari, produkt_dkaiAdka, novi;
doublereal aleph_k, minus_aleph_k, betath_k;

stari=ddot_(&n, rezidual, &incx, rezidual, &incy);

while(uvjet>tol){
/*
    printf("\n x_0 je: \n");
    for(i=0; i<n; i++){
        printf(" %lf ", x_0[i]);
    }
*/        
    dsymv_(&uplo, &n, &alfica, A, &n, d, &incx, &betica, adk, &incy); // produkt A*d_k spremljen u adk
    
    produkt_dkaiAdka=ddot_(&n, d, &incx, adk, &incy);
    
//    printf("\n Produkt r_k ova je %e \n", produkt_rkova);
//    printf("\n Produkt d_k i Ad_k je %e \n", produkt_dkaiAdka);
    
    aleph_k=stari/produkt_dkaiAdka;
//    printf("\n Aleph k je %e \n", aleph_k);
    minus_aleph_k=aleph_k*(-1);
//    printf("\n Minus aleph k je %e \n", minus_aleph_k);
    
    daxpy_(&n, &aleph_k, d, &incx, x_0, &incy);  // racunanje x_(k+1)
    daxpy_(&n, &minus_aleph_k, adk, &incx, rezidual, &incy);  // racunanje r_(k+1)
    
    novi=ddot_(&n, rezidual, &incx, rezidual, &incy);
    betath_k=novi/stari;
    
//    printf("\n Betica k je %e \n", betath_k);

    dcopy_(&n, rezidual, &incx, rezerva, &incy);
    daxpy_(&n, &betath_k, d, &incx, rezerva, &incy); // pravim d_(k+1)
    dcopy_(&n, rezerva, &incx, d, &incy);
    
    stari=novi;
    norma_reziduala=sqrt(novi);
    uvjet=norma_reziduala/norma_b;
    
    printf("\n Relativna norma reziduala u %ld koraku je: %e \n", k+1, uvjet);
    
    k++;
}

printf("\n Broj iteracija: %ld \n", k);

doublereal *vektor_razlike=malloc(n*sizeof(doublereal));
for(i=1; i<n+1; i++){
    vektor_razlike[i-1]=0.01*i*cos(0.01*i)-x_0[i-1];
}

printf("\n Razlika aproksimacije i rjesenja je: \n");
printf("\n {");
for(i=0; i<n;i++) printf(" %e ",vektor_razlike[i]);
printf("} \n");

}

int main(integer argc, char*argv[]){

integer n=99, i, j;
// lambdi punim dijagunale
doublereal *lambda=malloc(n*n*sizeof(doublereal));
for(i=0; i<n; i++){
    for(j=0; j<n; j++){
        lambda[i+j*n]=0;
        if(i==j) lambda[i+j*n]=(i+1)*(i+1);
    }
}
// generiram slucajnu matricu za ortogonalizirat
doublereal *A=malloc(n*n*sizeof(doublereal));
integer iseed[]={404,42,777,69};
integer idist=1;
integer nxn=n*n;

dlarnv_(&idist, iseed, &nxn, A);
// dgeqrf funkcija koja napravi gornji trokut R a dolje popuni info o HH reflektorima matrice Q u QR dekompoziciji
doublereal *tau=malloc(n*n*sizeof(doublereal));
doublereal *work=malloc(n*n*sizeof(doublereal));
integer info;

dgeqrf_(&n, &n, A, &n, tau, work, &n, &info);
// prvi put L N drugi put R T slova, rezultat je u lambdi
char side_prvi='L', trans_prvi='N';
char side_drugi='R', trans_drugi='T';
doublereal *work2=malloc(n*n*sizeof(doublereal));
doublereal *work3=malloc(n*n*sizeof(doublereal));

dormqr_(&side_prvi, &trans_prvi, &n, &n, &n, A, &n, tau, lambda, &n, work2, &n, &info);
dormqr_(&side_drugi, &trans_drugi, &n, &n, &n, A, &n, tau, lambda, &n, work2, &n, &info);

doublereal *b=malloc(n*sizeof(doublereal));
doublereal *x_T=malloc(n*sizeof(doublereal));
doublereal *x_0=malloc(n*sizeof(doublereal));
         
for(i=0; i<n; i++){
    x_T[i]=1.0;
    x_0[i]=0.0;
    b[i]=0.0;
}
// pravim b kao u proslom zadatku
char trans='N';
doublereal alfa=1.0, beta=0.0;
integer incx=1, incy=1;
dgemv_(&trans, &n, &n, &alfa, lambda, &n, x_T, &incx, &beta, b, &incy);

doublereal tol=1e-8;

// scanf("tol = %lf", &tol);

doublereal *B=malloc(n*sizeof(double));


for(i=0; i<n*n; i++){
    A[i]=0.0;
}
for(i=0; i<n; i++){
    A[i+i*n]=1.9999;
}
for(i=0; i<n-1; i++){
    A[i+i*n+1]=-1;
}
for(i=1; i<n; i++){
    A[i+i*n-1]=-1;
}

for(i=1; i<n; i++){
    B[i-1]=0.0002*sin(0.01*i);
}
B[n-1]=0.0002*sin(0.99)+cos(1);

cg(n, A, B, x_0, tol);

return 0;
}