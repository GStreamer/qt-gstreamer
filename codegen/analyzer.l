/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
%{
#include "yystype.h"
#include "generator.h"

#define YY_DECL int yylex(CodeGen *codegen)
void yyerror(CodeGen *codegen, const char *msg);

%}
%option noyywrap
%option yylineno
%option stack

id      [a-zA-Z][a-zA-Z_0-9]*

%x REGISTER_TYPE
%x ENUM
%x ENUM_EAT_ASSIGNMENT
%x INSTRUCTION
%x LINE_COMMENT
%x C_STYLE_COMMENT
%x NAMESPACE
%x CLASS
%x EAT_TEMPLATE

%%

<REGISTER_TYPE,ENUM,INSTRUCTION>{id} { yylval.Id = new QByteArray(yytext); return IDENTIFIER;  }

QGLIB_REGISTER_TYPE\(    { yy_push_state(REGISTER_TYPE); return REGISTER_TYPE_BEGIN; }
<REGISTER_TYPE>{
    ::                   { return SCOPE_RESOLUTION_OPERATOR; }
    \)                   { yy_pop_state(); return REGISTER_TYPE_END; }
    [[:space:]]*
    .                    { yyerror(codegen, "Syntax error in QGLIB_REGISTER_TYPE"); }
}

enum                     { yy_push_state(ENUM); return ENUM_KEYWORD; }
<ENUM>{
    \{                   { return LEFT_BRACE; }
    \}                   { return RIGHT_BRACE; }
    ,                    { return COMMA; }
    =                    { yy_push_state(ENUM_EAT_ASSIGNMENT); }
    ;                    { yy_pop_state(); return SEMICOLON; }
    [[:space:]]*
    .                    { yyerror(codegen, "Syntax error in enum definition"); }
}

<ENUM_EAT_ASSIGNMENT>{
    ,                    { unput(','); yy_pop_state(); }
    \}                   { unput('}'); yy_pop_state(); }
    \n
    .
}

<*>\/\/                  { yy_push_state(LINE_COMMENT); }
<LINE_COMMENT>{
    \n                   { yy_pop_state(); }
    codegen:             { yy_push_state(INSTRUCTION); return INSTRUCTIONS_BEGIN; }
    .
}

<INSTRUCTION>{
    \n                   { unput('\n'); yy_pop_state(); return INSTRUCTIONS_END; }
    =                    { return INSTRUCTIONS_ASSIGN_OPERATOR; }
    ,                    { return INSTRUCTIONS_SEPARATOR; }
    [[:space:]]
    .                    { yyerror(codegen, "Syntax error in instruction comment"); }
}

<*>\/\*                  { yy_push_state(C_STYLE_COMMENT); }
<C_STYLE_COMMENT>{
    \*\/                 { yy_pop_state(); }
    \n
    .
}

namespace                { yy_push_state(NAMESPACE); }
<NAMESPACE>{
    {id}                 { codegen->setCurrentNameSpace(yytext); yy_pop_state(); }
    [[:space:]]*
    .                    { yyerror(codegen, "Expected identifier after namespace keyword"); }
}

class                    { yy_push_state(CLASS); }
<CLASS>{
    {id}                 { codegen->setCurrentClass(yytext); yy_pop_state(); }
    [[:space:]]*
    .                    { yyerror(codegen, "Expected identifier after class keyword"); }
}

template[[:space:]]*\<   { yy_push_state(EAT_TEMPLATE); }
<EAT_TEMPLATE>{
    \<                   { yy_push_state(EAT_TEMPLATE); };
    \>                   { yy_pop_state(); }
    \n
    .
}

<*><<EOF>>               { return EOF; }
\n
.

%%

