#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAXCHAR 75
char expression[MAXCHAR+1];
int isSynError(char *expression);
int isMathError(char *expression);
void parseBracket(char *f);
void parse(char *f);
void calculate(char *f, char *s);
int isDigit(char c);
float stringToFloat(char *f, char *s);
char *floatToString(float num, char *s);
void swap(char *a, char *b);
void e_d_m_AndAppend(char *fop, char *sop, char choice);
void addAndAppend(char *fop, char *sop);
int main()
{
    while(1)
    {
        scanf("%s", expression);
        if(isSynError(expression))
        {
            printf("Syntax Error\n");
            continue;
        }
        if(isMathError(expression))
        {
            printf("Math Error\n");
            continue;
        }
        parseBracket(expression);
        printf("%s\n", expression);
    }
    return 0;
}
int isSynError(char *expression)
{
    int i = 0, opbracket = 0, clbracket = 0;
    while(*(expression + i))
    {
        if(*(expression + i) == '(')
            opbracket++;
        else if(*(expression + i) == ')')
            clbracket++;
        else if(*(expression + i) == '/' || *(expression + i) == '*' || *(expression + i) == '^') // CONSECUTIVE / * ^ or operator in the start or end of expression
        {
            if(*(expression + i + 1) == '/' || *(expression + i + 1) == '*' || *(expression + i + 1) == '^' || i == 0 || !*(expression + i + 1)) // + - after / * ^ allowed
                return 1;
            if(!isDigit(*(expression + i - 1)) && *(expression + i - 1) != ')') //ONLY digit and ) before * / ^ allowed
                return 1;
        }
        if(clbracket > opbracket)
            return 1;
        i++;
    } //MORE THAN ONE . IN A NUMBER
    if(opbracket != clbracket)
        return 1;
    return 0;
}
int isMathError(char *expression)
{
    int i = 0, j = 1;
    while(*(expression + i))
    {
        if(*(expression + i) == '/' && *(expression + i + 1) == '0') //DIVISION BY ZERO
            return 1;
        else if(*(expression + i) == '^')//NEGATIVE BASE TO FRACTIONAL POWER (-2)^.5, ...3*-2^.5
        {
            while((isDigit(*(expression + i - j)) || *(expression + i - j) == '.') && j <= i)
                j++;
            if(*(expression + i - j) == '-' && (*(expression + i - j - 1) == '^' || *(expression + i - j - 1) == '/' || *(expression + i - j - 1) == '*'))
            {
                j = 1;
                if(*(expression + i + j) == '-' || *(expression + i + j) == '+')
                    j++;
                while(isDigit(*(expression + i + j)))
                    j++;
                if(*(expression + i + j) == '.')
                    return 1;
            }
        }
        i++;
    }
    return 0;
}
void parseBracket(char *f) //Iterating through the expression in search of open brackets -> passing to parse() -> ONLY recursive parse() misses (xx)+(xx) situations...
{
    //so iterative function parseBracket() which searches through out expression is needed
    //Continues till EOS to find all Open Brackets.
    int i = 0;
    while(*(f + i))
    {
        if(*(f + i) == '(')
            parse(f + i);
        i++;
    }
    char *s = f; //s position changes as expression is trimmed or extended, so s readjustment must be after all brackets are dine processing
    while(*s)
        s++;
    calculate(f, s); //calculate the whole bracket-less expression after internal parsing and calculation.
    return;
}
void parse(char *f) // Recursive tree till end bracket is found, so multiple brackets can be solved by last come first solve basis.
{
    //ALL parse is called by an open bracket, so close bracket is a must, other wise syntax error would occur
    int i = 1; //if i = 0, same element ( will be checked and parse() will recur indefinitely
    while(*(f + i) != ')')
    {
        if(*(f + i) == '(')
        {
            parse(f + i);
        }
        i++;
    } //exits once ) is found, then segment passed to calculate()
    if(*(f + i + 1) == '^') //very special case 3+(-9)^2 -> 3+s(-9)^2 -> calculate() detects special marker s -> includes - in -9 when passing to pow()
    {
        char temp[MAXCHAR];
        strcpy(temp, f);
        *f = 's'; //special marker
        strcpy(f + 1, temp);
        f++;
    }
    calculate(f, f + i); //No brackets inside, either all eliminated by parse() -> calculate() tree, or first ) encounter
    return;
}
void calculate(char *f, char *s) // *f and *s are ( and ), no brackets inside. Applies BEDMAS sequence for calculation.
{
    int i = 0, fop = 1, sop = 1;
    //if(!*s) //for the last calculate() call in parseBracket()
    while(s - i >= f) // *(s - i) is the operator, *(s - i - fop) and *(s - i + sop) are starting and ending digits in a single operation
    {
        //Exponent has right to left associativity
        // SEEKING EXPONENT
        if(*(s - i) == '^')
        {
            while((isDigit(*(s - i - fop)) || *(s - i - fop) == '.'))// && s - i - fop >= f)
                fop++;
            if(*(s - i - fop) == '-' || *(s - i - fop) == '+')
            {
                if(*(s - i - fop - 1) == '/' || *(s - i - fop - 1) == '*' || *(s - i - fop - 1) == '^') // 3*-4^3 or 3/4^-6 or 3^-4^3 here minus or plus has calculative meaning...
                    fop++;
                else if(*(s - i - fop - 1) == 's')
                {
                    char temp[MAXCHAR];
                    strcpy(temp, s - i - fop);
                    strcpy(s - i - fop - 1, temp);
                    fop += 2;
                    s--, i--;
                }
            }                                                                              //...thus will be part of calculation
            fop--;
            if(*(s - i + 1) == '-' || *(s - i + 1) == '+')
                sop++;
            while((isDigit(*(s - i + sop)) || *(s - i + sop) == '.'))// && *(s - i + sop)) // 3/4^-6 // ALLOWING . TO BE F op term or SECOND op term, HANDLE IT IN INDIVIDUAL CALC
                sop++;
            sop--;
            //printf("fop %c sop %c\n", *(s - i - fop), *(s - i + sop));
            e_d_m_AndAppend(s - i - fop, s - i + sop, 'e'); //after each append old s has no value, because segment could be trimmed or expanded
            fop = 1, sop = 1;
            s = f;
            while(*s != ')' && *s) //first condition for bracket segments in parse(), second condition for last calculate() call
                s++;
        }
        i++;
    }
    //printf("%s\n", f); getche();
    i = 0;
    while(f + i <= s) //SEEKING DIVISION
    {
        if(*(f + i) == '/')
        {
            while(isDigit(*(f + i - fop)) || *(f + i - fop) == '.')
                fop++;
            if(*(f + i - fop) == '+' || *(f + i - fop) == '-')// 3*-4/3
                if(*(f + i - fop - 1) == '*')// / is processed left to right so no / occurring before current /, ^ processed in previous sequence, so only * can occur
                    fop++;
            fop--;
            if(*(f + i + 1) == '-' || *(f + i + 1) == '+')
                sop++;
            while(isDigit(*(f + i + sop)) || *(f + i + sop) == '.')
                sop++;
            sop--;
            e_d_m_AndAppend(f + i - fop, f + i + sop, 'd');
            fop = 1, sop = 1;
            s = f;
            while(*s != ')' && *s) //second condition for last calculate() call
                s++;
        }
        i++;
    }
    //printf("%s", f); getche();
    i = 0;
    while(f + i <= s) //SEEKING MULTIPLICATION
    {
        if(*(f + i) == '*')
        {
            while(isDigit(*(f + i - fop)) || *(f + i - fop) == '.')
                fop++;
            fop--;
            if(*(f + i + sop) == '-' || *(f + i + sop) == '+')
                sop++;
            while(isDigit(*(f + i + sop)) || *(f + i + sop) == '.')
                sop++;
            sop--;
            e_d_m_AndAppend(f + i - fop, f + i + sop, 'm');
            fop = 1, sop = 1;
            s = f;
            while(*s != ')' && *s) //second condition for last calculate() call
                s++;
        }
        i++;
    }
    i = 0;
    //printf("%s\n", f); getche();
    addAndAppend(f, s);
    //printf("%s\n", f); getche();
}
int isDigit(char c) //checked
{
    if(c >= '0' && c <= '9')
        return 1;
    return 0;
}
float stringToFloat(char *f, char *s) //checked
{
    int i = 0;
    float sign = 1, sum = 0, decpow = 1;
    if(*f == '-')
    {
        sign = -1;
        i++;
    }
    else if(*f == '+')
        i++;
    while(f + i <= s && *(f + i) != '.')
    {
        sum = sum * 10 + (*(f + i++) - '0');
    }
    if(*(f + i) == '.')
    {
        i++;
        while(f + i <= s)
        {
            sum = sum + (*(f + i++) - '0') * pow(.1, decpow++);
        }
    }
    return sign * sum;
}

char *floatToString(float num, char *s) //takes float num, converts and stores it as string s
{
    //CHECKED
    char tempi[10], tempf[10];
    int i = 0, inte, neg = 0;
    float frac;
    if(num < 0)
        neg = 1;
    inte = (int) num;
    frac = num - inte;
    if(neg)
        frac = -frac, inte = -inte;
    if(frac != 0)
        tempf[i++] = '.';
    while(frac != 0 && i <= 6)//6 decimal accuracy
    {
        frac *= 10;
        tempf[i++] = (int)frac % 10 + '0';
        frac = frac - (int)frac;
    }
    tempf[i] = '\0';
    i = 0;
    while(inte)
    {
        tempi[i++] = inte % 10 + '0';
        inte /= 10;
    }
    if(neg)
        tempi[i++] = '-';
    tempi[i] = '\0';
    strrev(tempi);
    strcpy(s, tempi);
    strcat(s, tempf);
    return s;
}
void e_d_m_AndAppend(char *fop, char *sop, char choice) //checked
{
    char *op = fop, rstr[16], temp[MAXCHAR - 3]; //doubles can store 15 digits, at least first 3 expression characters are operand operator operand
    float fnum, snum, rnum;
    int i, rlen;
    if(choice == 'e')
    {
        while(*op != '^')
            op++;
    }
    else if(choice == 'd')
    {
        while(*op != '/')
            op++;
    }
    else if(choice == 'm')
    {
        while(*op != '*')
            op++;
    }
    fnum = stringToFloat(fop, op - 1);
    snum = stringToFloat(op + 1, sop);
    //printf("%f %f\n", fnum, snum);
    if(choice == 'e')
        rnum = pow(fnum, snum);
    else if(choice == 'd')
        rnum = fnum / snum;
    else if(choice == 'm')
        rnum = fnum * snum;
    floatToString(rnum, rstr);
    //append rsrt to expression from address fop to sop
    rlen = strlen(rstr);
    if(rlen - (sop - fop + 1) != 0) //space needs to be created or trimmed
    {
        strcpy(temp, sop + 1);
        strcpy(fop + rlen, temp);
    }
    for(i = 0; i < rlen; i++)//write result in expression
    {
        *(fop + i) = *(rstr + i);
    }
}
void addAndAppend(char *fop, char *sop)
{
    float sum = 0;
    int f = 0, s = 0, slen, i, brackets = 0, doubsign = 1;
    char sumstr[16], temp[MAXCHAR - 3];
    while(*(fop + f) == '(')
    {
        f++;
        s++;
    }
    while(*sop == ')' || !*sop)
    {
        sop--;
        brackets++;
    }
    while(fop +  s <= sop)
    {
        if(*(fop + f) == '+')
        {
            s++;
            if(*(fop + f + 1) == '-' || *(fop + f + 1) == '+')
            {
                s++;
                f++;
            }
        }
        else if(*(fop + f) == '-') //the +- immediately before number will be calculated in stringToFloat(), but if 3++3, 3-+3 occurs, the + or - before...
        {
            //...the immediate + dictates the sign that will be considered in sum
            s++;
            if(*(fop + f + 1) == '+' || *(fop + f + 1) == '-')
            {
                s++;
                f++;
                doubsign = -1;
            }
        }

        while((isDigit(*(fop + s)) || *(fop + s) == '.') && fop + s <= sop)
            s++;
        sum += doubsign * stringToFloat(fop + f, fop + s - 1);
        f = s;
        doubsign = 1;
    }
    floatToString(sum, sumstr);
    slen = strlen(sumstr);
    if(slen - (sop - fop + 1) != 0) //space needs to be created or trimmed
    {
        strcpy(temp, sop + 1 + brackets);
        strcpy(fop + slen, temp);
    }
    for(i = 0; i < slen; i++)//write result in expression
    {
        *(fop + i) = *(sumstr + i);
    }
}
