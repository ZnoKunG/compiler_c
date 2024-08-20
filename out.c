#include <stdio.h>
int main(){
float a;
float b;
float s;
float c;
a = 0;
while (a<1) {
printf("Enter number of scores: \n");
scanf("%f", &a);
}
b = 0;
s = 0;
printf("Enter one score at a time: \n");
while (b<a) {
scanf("%f", &c);
s = s+c;
b = b+1;
}
printf("Average: \n");
printf("%.2f\n",(float)(s/a));
return 0;
}
