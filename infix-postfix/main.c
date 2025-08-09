#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
struct stack {
    char *data;
    int top;
    int capacity;
};
struct stack *createstack(int cap){
    struct stack *st;
    st=malloc(sizeof(struct stack));
    st->top=-1;
    st->data=malloc(sizeof(char)*cap);
    st->capacity=cap;
    return st;
}
int push(struct stack *st,char x){
    if(st->top<20){
        st->top++;
        st->data[st->top]=x;
        return (1);
    }
    return 0;
}
char pop(struct stack *st){
    char x;
    if(st->top!=-1){
        x=st->data[st->top];
        st->top--;
        return x;
    }
    else{
        return '0';
    }
}
char peek(struct stack* st){
    char x='0';
    if(st->top!=-1)
        x=st->data[st->top];
    return x;
}
int getpriority(char x){
    if(x=='^')
        return 3;
    else if(x=='*' || x=='/')
        return 2;
    else if(x=='+' || x=='-')
        return 1;
    else
        return -1;
}
void disp(struct stack *st){
    if(st->top!=-1){
        int i;
        for(i=0;i<=st->top;i++){
            char x=st->data[i];
            printf("%c",x);
        }
    }
}
////////
void infToPost(){
    struct stack *st1=createstack(5);
    struct stack *st2=createstack(5);
    char e[7]="(a+(b-c))";
    //scanf("%s",e);
    int i=0;
    while(e[i]!='\0'){
        if(e[i]=='('){
            push(st1,e[i]);
        }
        else if(isalnum(e[i])){
            push(st2,e[i]);
        }
        else if(e[i]==')'){
            while(peek(st1)!='('){
                push(st2,pop(st1));
            }
            pop(st1);
        }
        else{
            if(st1->top!=-1){
                while(getpriority(peek(st1))>=getpriority(e[i]) && st1->top!=-1){
                    char er=pop(st1);
                    push(st2,er);
                }
                push(st1,e[i]);
            }
