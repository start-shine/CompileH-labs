#include <stdio.h>
void sort(int a[],int n){
    int i,j;
    int temp;
    for (i=0;i<n-1;i++)
        for (j=0;j<n-1-i;j++)
            if (a[j] > a[j + 1]) {
                temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
            }
}
int main (){
    int n;
    int a[100];
    scanf("%d",&n);
    int i;
    for(i=0;i<n;i++){
        scanf("%d",&a[i]);
    }
    
    sort(a,n);
    for(i=0;i<n;i++) printf("%d ",a[i]);
}