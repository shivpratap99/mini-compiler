#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

/* =========================
   Function Declarations
   ========================= */

char current();
void next();
int match_function(const char *func);
int is_valid_expression();

float expression();
float term();
float factor();

/* =========================
   Global Variables
   ========================= */

char expr[200];
int pos = 0;

/* =========================
   Helper Functions
   ========================= */

char current()
{
    return expr[pos] ? expr[pos] : '\0';
}

void next()
{
    if (expr[pos])
        pos++;
}

int match_function(const char *func)
{
    int len = strlen(func);

    if (strncmp(&expr[pos], func, len) == 0)
    {
        pos += len;
        return 1;
    }

    return 0;
}

/* =========================
   Expression Validation
   ========================= */

int is_valid_expression()
{
    int len = strlen(expr);
    int paren = 0;
    int hasDigit = 0;

    char temp[200];
    int j = 0;

    /* Remove spaces */
    for (int i = 0; i < len; ++i)
        if (expr[i] != ' ')
            temp[j++] = expr[i];

    temp[j] = '\0';
    strcpy(expr, temp);

    len = strlen(expr);

    if (len == 0)
        return 0;

    if (strchr("*/^)", expr[0]))
        return 0;

    if (strchr("+-*/^(", expr[len - 1]))
        return 0;

    for (int i = 0; i < len; ++i)
    {
        char c = expr[i];

        if (isdigit((unsigned char)c))
            hasDigit = 1;

        if (c == '(')
            paren++;

        if (c == ')')
            paren--;

        if (paren < 0)
            return 0;

        if (strchr("+-*/^", c))
        {
            if (i + 1 < len && strchr("+-*/^)", expr[i + 1]))
                return 0;
        }

        if (isdigit((unsigned char)c) && i + 1 < len && expr[i + 1] == '(')
            return 0;
    }

    return (paren == 0 && hasDigit);
}

/* =========================
   Parser Functions
   ========================= */

float factor()
{
    int sign = 1;

    while (current() == '+' || current() == '-')
    {
        if (current() == '-')
            sign = -sign;

        next();
    }

    float value = 0.0f;

    /* Functions like sqrt, log, ln */
    if (isalpha((unsigned char)current()))
    {
        if (match_function("sqrt"))
        {
            if (current() == '(')
            {
                next();
                float v = expression();
                if (current() == ')')
                    next();
                value = sqrtf(v);
            }
            else
                value = sqrtf(factor());
        }

        else if (match_function("log"))
        {
            if (current() == '(')
            {
                next();
                float v = expression();
                if (current() == ')')
                    next();
                value = log10f(v);
            }
            else
                value = log10f(factor());
        }

        else if (match_function("ln"))
        {
            if (current() == '(')
            {
                next();
                float v = expression();
                if (current() == ')')
                    next();
                value = logf(v);
            }
            else
                value = logf(factor());
        }

        else
        {
            while (isalpha((unsigned char)current()))
                next();
            value = 0.0f;
        }
    }

    /* Parentheses */
    else if (current() == '(')
    {
        next();
        value = expression();
        if (current() == ')')
            next();
    }

    /* Numbers */
    else
    {
        while (isdigit((unsigned char)current()))
        {
            value = value * 10 + (current() - '0');
            next();
        }

        if (current() == '.')
        {
            next();
            float place = 0.1f;

            while (isdigit((unsigned char)current()))
            {
                value += (current() - '0') * place;
                place *= 0.1f;
                next();
            }
        }
    }

    /* Power operator */
    if (current() == '^')
    {
        next();
        value = powf(value, factor());
    }

    return sign * value;
}

float term()
{
    float result = factor();

    while (current() == '*' || current() == '/')
    {
        char op = current();
        next();

        float rhs = factor();

        if (op == '*')
            result *= rhs;
        else
        {
            if (rhs == 0.0f)
            {
                printf("Error: Division by zero!\n");
                return 0.0f;
            }

            result /= rhs;
        }
    }

    return result;
}

float expression()
{
    float result = term();

    while (current() == '+' || current() == '-')
    {
        char op = current();
        next();

        float rhs = term();

        if (op == '+')
            result += rhs;
        else
            result -= rhs;
    }

    return result;
}

/* =========================
   Main Program
   ========================= */

int main()
{
    printf("Scientific Calculator\n");
    printf("Enter Expression: ");

    if (!fgets(expr, sizeof(expr), stdin))
        return 0;

    expr[strcspn(expr, "\n")] = '\0';

    if (!is_valid_expression())
    {
        printf("Invalid Format!\n");
        return 0;
    }

    pos = 0;

    float ans = expression();

    printf("Result = %.6f\n", ans);

    return 0;
}