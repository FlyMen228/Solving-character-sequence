#define USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <cmath>
#include <stack>
#include <map>
#include <functional>

/*=========================================================================================================*/

enum termType
{
    consta,
    variable,
    openBracket,
    closeBracket,
    function,
    binary,
    unary
};

std::map <std::string, double> variables = { {"x", 18.}, {"X", 18.}, {"y", 4.}, {"Y", 4.},
                                             {"e", M_E}, {"E", M_E}, {"pi", M_PI}, {"Pi", M_PI}, {"PI", M_PI} };

std::map <std::string, std::function <double(std::stack <double>&)> > math;

/*=========================================================================================================*/

struct Term
{

    std::string name;

    termType type;

    int priority;

public:

    Term(const std::string& name, termType type, const int& priority)
    {
        this->name = name;
        this->type = type;
        this->priority = priority;
    }

    Term(const char& name, termType type, const int& priority)
    {
        std::string temp;
        temp.push_back(name);
        this->name = temp;
        this->type = type;
        this->priority = priority;
    }

};

/*=========================================================================================================*/

std::vector <Term> parseStringToTerms(std::string& sequence);

std::vector <Term> toPostfixEntry(std::vector <Term>& terms);

void fillMath();

double calculatePostfixEntry(std::vector <Term>& postfixEntry);

/*=========================================================================================================*/

int main()
{
    std::string sequence = "21*x + cos(y-tan( sqrt(-pi/2.9)))^(ln(e%0.5))";

    std::vector <Term> terms = parseStringToTerms(sequence);

    std::vector <Term> postfixEntry = toPostfixEntry(terms);

    fillMath();

    double answer = calculatePostfixEntry(postfixEntry);

    std::cout << "The answer is " << answer;
}

/*=========================================================================================================*/

std::vector <Term> parseStringToTerms(std::string& sequence)
{
    std::vector <Term> terms;

    std::string sequenceWOS;

    for (char i : sequence)
        if (i != ' ')
            sequenceWOS.push_back(i);

    for (int i = 0; i < sequenceWOS.size(); ++i)
    {
        if (sequenceWOS[i] >= '0' && sequenceWOS[i] <= '9')
        {
            std::string temp;
            while (i < sequenceWOS.size() && (sequenceWOS[i] >= '0' && sequenceWOS[i] <= '9' || sequenceWOS[i] == '.'))
            {
                temp.push_back(sequenceWOS[i]);
                ++i;
            }
            --i;
            terms.emplace_back(temp, consta, 1);
            variables[temp] = std::stod(temp);
        }

        else if (sequenceWOS[i] == '+' || sequenceWOS[i] == '*' || sequenceWOS[i] == '/' || sequenceWOS[i] == '^')
        {
            if (sequenceWOS[i] == '+')
                terms.emplace_back(sequenceWOS[i], binary, 1);

            else if (sequenceWOS[i] == '*' || sequenceWOS[i] == '/')
                terms.emplace_back(Term(sequenceWOS[i], binary, 2));

            else
                terms.emplace_back(Term(sequenceWOS[i], binary, 3));
        }

        else if (sequenceWOS[i] == '-')
        {
            if (i == 0)
                terms.emplace_back(Term("-", unary, 4));
            else if (terms[terms.size() - 1].type == variable || terms[terms.size() - 1].type == consta ||
                       terms[terms.size() - 1].type == closeBracket || terms[terms.size() - 1].name == "!")
                terms.emplace_back(Term("-", binary, 1));

            else
                terms.emplace_back(Term("-", unary, 4));
        }

        else if (sequenceWOS[i] == '!')
            terms.emplace_back(Term("!", unary, 4));

        else if (sequenceWOS[i] == '(')
            terms.emplace_back(Term("(", openBracket, 0));

        else if (sequenceWOS[i] == ')')
            terms.emplace_back(Term(")", closeBracket, 0));

        else if (sequenceWOS[i] >= 'A' && sequenceWOS[i] <= 'Z' || sequenceWOS[i] >= 'a' && sequenceWOS[i] <= 'z') //A-Za-z
        {
            std::string temp;
            while (i < sequenceWOS.size() && (sequenceWOS[i] >= 'A' && sequenceWOS[i] <= 'Z' || sequenceWOS[i] >= 'a' && sequenceWOS[i] <= 'z'))
            {
                temp.push_back(sequenceWOS[i]);
                ++i;
            }

            if (sequenceWOS[i] == '(')
                terms.emplace_back(Term(temp, function, 5));

            else if (variables.find(temp) != variables.end())
                terms.emplace_back(Term(temp, consta, 0));

            else if (variables.find(temp) != variables.end())
                terms.emplace_back(Term(temp, variable, 0));

            --i;
        }
    }

    return terms;
}

std::vector <Term> toPostfixEntry(std::vector <Term>& terms)
{
    std::stack <Term> buffer;

    std::vector <Term> output;

    for (const auto& term : terms)
        switch (term.type)
        {
            case consta:
            {
                output.push_back(term);
                break;
            }

            case variable:
            {
                output.push_back(term);
                break;
            }

            case function:
            {
                buffer.push(term);
                break;
            }

            case openBracket:
            {
                buffer.push(term);
                break;
            }

            case closeBracket:
            {
                while (buffer.top().type != openBracket)
                {
                    output.push_back(buffer.top());
                    buffer.pop();
                }
                buffer.pop();
                break;
            }

            case binary:
            {
                while (!buffer.empty() && (buffer.top().priority >= term.priority))
                {
                    output.push_back(buffer.top());
                    buffer.pop();
                }
                buffer.push(term);
                break;
            }

            case unary:
            {
                while (!buffer.empty() && (buffer.top().priority >= term.priority))
                {
                    output.push_back(buffer.top());
                    buffer.pop();
                }
                buffer.push(term);
                break;
            }
        }

    while (!buffer.empty())
    {
        output.push_back(buffer.top());
        buffer.pop();
    }

    return output;
}

void fillMath()
{
    math["+"] = [](std::stack <double>& stack) -> double { double temp1 = stack.top(); stack.pop(); double temp2 = stack.top(); stack.pop(); return temp1 + temp2; };

    math["-"] = [](std::stack <double>& stack) -> double { double temp1 = stack.top(); stack.pop(); double temp2 = stack.top(); stack.pop(); return temp1 - temp2; };

    math["*"] = [](std::stack <double>& stack) -> double { double temp1 = stack.top(); stack.pop(); double temp2 = stack.top(); stack.pop(); return temp1 * temp2; };

    math["/"] = [](std::stack <double>& stack) -> double { double temp1 = stack.top(); stack.pop(); double temp2 = stack.top(); stack.pop(); return temp1 / temp2; };

    math["%"] = [](std::stack <double>& stack) -> double { double temp1 = stack.top(); stack.pop(); double temp2 = stack.top(); stack.pop(); return std::fmod(temp1, temp2); };

    math["^"] = [](std::stack <double>& stack) -> double { double temp1 = stack.top(); stack.pop(); double temp2 = stack.top(); stack.pop(); return std::pow(temp1, temp2); };

    math["sqrt"] = [](std::stack <double>& stack) -> double { double temp = stack.top(); stack.pop(); return std::sqrt(temp); };

    math["sin"] = [](std::stack <double>& stack) -> double { double temp = stack.top(); stack.pop(); return std::sin(temp); };

    math["cos"] = [](std::stack <double>& stack) -> double { double temp = stack.top(); stack.pop(); return std::cos(temp); };

    math["tan"] = [](std::stack <double>& stack) -> double { double temp = stack.top(); stack.pop(); return std::tan(temp); };

    math["ln"] = [](std::stack <double>& stack) -> double { double temp = stack.top(); stack.pop(); return std::log(temp); };

    math["exp"] = [](std::stack <double>& stack) -> double { double temp = stack.top(); stack.pop(); return std::exp(temp); };

    math["unaryMinus"] = [](std::stack <double>& stack) -> double { double temp = stack.top(); stack.pop(); return -1 * temp; };
}

double calculatePostfixEntry(std::vector <Term>& postfixEntry) // 21*x+cos(y-tan(sqrt(-pi/2.9)))^(ln(e%0.5))
{                                                              // 21 x  * y pi - 2.9 / sqrt tan - cos e 0.5 ln ^ +
    std::stack <double> result;

    for (const auto& term : postfixEntry)
    {
        if (term.type == consta || term.type == variable)
            result.push(variables.find(term.name)->second);
        else
            if (term.type == unary && term.name == "-")
                result.push(math["unaryMinus"](result));
            else
                result.push(math[term.name](result));
        std::cout << term.name << " " << result.top() << std::endl;
    }

    return result.top();
}