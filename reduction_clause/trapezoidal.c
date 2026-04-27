#include <stdio.h>
#include <omp.h>
double f(double x) {
    return x*x;
}
int main() {
    double a,b,h,sum = 0.0,result;
    int n,i;
    printf("Enter lower limit (a): ");
    scanf("%lf",&a);
    printf("Enter upper limit (b): ");
    scanf("%lf",&b);
    printf("Enter number of trapezoids (n): ");
    scanf("%d",&n);
    h=(b-a)/n;
    sum=0.0;
    #pragma omp parallel for reduction(+:sum)
    for(i=1;i<n;i++){
        sum+=f(a+i*h);
    }
    result=h*((f(a)+f(b))/2+sum);
    printf("\nResult using reduction: %.3lf\n", result);
    sum=0.0;
    #pragma omp parallel
    {
        double local_sum=0.0;
        #pragma omp for
        for (i=1;i<n;i++) {
            local_sum+=f(a+i*h);
        }
        #pragma omp critical
        {
            sum+=local_sum;
        }
    }
    result=h*((f(a)+f(b))/2+sum);
    printf("Result using critical: %.3lf\n",result);
    return 0;
}
