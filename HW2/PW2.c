#include <stdio.h>
#include <string.h>
int main(){
    char str[100];
    char c;
    int n;
    int i = 0;
    while((c = getchar())!='\n'){
        str[i] = c;
        i++;
    }
    n = i;
    int state = 0;
    int length = 0;
    int first = 1;
    int digit = 1;
    //printf("%d",n);
    for(i=0;i<n;i++){
        switch (state)
        {
        case 0:
            // if(str[i-1] == '<') printf("(relop,<)");
            // if(str[i-1] == '>') printf("(relop,>)");
            // if(str[i-1] == '=') printf("(relop,=)");
            // else{
                if((str[i]=='<' || str[i]=='='||str[i]=='>')&&str[i-1]!='<'&&str[i-1]!='>'&&str[i-1]!='='){
                    if(first) {
                        first = 0;
                        if(length != 0) printf("(other,%d)",length);
                    }
                    else printf("(other,%d)",length);
                }
                
           // }
            switch (str[i])
            {
            case '<':
                state = 1;
                
                length = 0;
                break;
            case '=':
                state = 5;
                //printf("(relop,=)");
                digit = 0;
                length = 0;
                break;
            case '>':
                state = 6;
                length = 0;
                break;
            default:
                state = 0;
                length++;
                digit = 1;
                //printf("%d",length);
                break;
            }
            break;
        case 1:
            switch (str[i])
            {
            case '=':
                state = 2;
                break;
            case '>':
                state = 3;
                break;
            default:
                state = 4;
                i--;
                break;
            }
            break;
        case 2:
            printf("(relop,<=)");
            i--;
            //length++;
            state = 0;
            break;
        case 3:
            printf("(relop,<)");
            printf("(relop,>)");
            i--;
            //length++;
            state = 0;
            break;
        case 4:
            printf("(relop,<)");
            //length++;
            state = 0;
            i--;
            break;
        case 5:
            printf("(relop,=)");
            state = 0;
            i--;
            // switch (str[i])
            // {
            // case '=':
            //     state = 0;
            //     i--;
            //     //printf("S");
            //     break;
            // default:
            //     state = 0;
            //     break;
            // }
            break;
        case 6:
            switch (str[i])
            {
            case '=':
                //printf("(relop,>=)");
                state = 7;
                break;
            default:
                state = 8;
                i--;
                break;
            }
            break;
        case 7:
            printf("(relop,>=)");
            state = 0;
            i--;
            break;
        case 8:
            printf("(relop,>)");
            i--;
            state = 0;
            //length = 1;
            break;
        // case 9:
        //     printf("(relop,=)");
        //     i--;
        //     state = 0;
        //     break;
        }
        if(i == n-1){
            //printf("%dstate",state);
            if(state==0){
                if(first) {
                    first = 0;
                    if(length != 0) printf("(other,%d)",length);
                }
                else printf("(other,%d)",length);
            }
                else{
                    switch (state)
                    {
                    case 1:
                        printf("(relop,<)");
                        break;
                    case 2:
                        printf("(relop,<=)");
                        break;
                    case 3:
                        printf("(relop,<>)");
                        break;
                    case 4:
                        printf("(relop,<)");
                        break;
                    case 5:
                    case 9:
                        printf("(relop,=)");
                        break;
                    case 6:
                    case 8:
                        printf("(relop,>)");
                        break;
                    case 7:
                        printf("(relop,>=)");
                        break;
                    default:
                        break;
                    }
                }
            }
    }
}
