/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
%{
#include "generator.h"
#include "yystype.h"

#define YY_DECL int yylex(CodeGen *codegen)
void yyerror(CodeGen *codegen, const char *msg);

%}
%option noyywrap
%option yylineno
%option stack

id      [a-zA-Z][a-zA-Z_0-9]*

%x REGISTER_TYPE
%x REGISTER_WRAPPER
%x ENUM
%x ENUM_EAT_ASSIGNMENT
%x INSTRUCTION
%x LINE_COMMENT
%x C_STYLE_COMMENT
%x NAMESPACE
%x CLASS
%x EAT_TEMPLATE
%x MACRO

%%

<REGISTER_TYPE,REGISTER_WRAPPER,ENUM,INSTRUCTION>{id} { yylval.Id = new QByteArray(yytext); return IDENTIFIER;  }

Q[A-Z]+_REGISTER_TYPE\(  { yy_push_state(REGISTER_TYPE); return REGISTER_TYPE_BEGIN; }
<REGISTER_TYPE>{
    ::                   { return SCOPE_RESOLUTION_OPERATOR; }
    \)                   { yy_pop_state(); return REGISTER_TYPE_END; }
    [[:space:]]*
    .                    { yyerror(codegen, "Syntax error in QGLIB_REGISTER_TYPE"); }
}

Q[A-Z]+_WRAPPER\(         { yy_push_state(REGISTER_WRAPPER); return REGISTER_WRAPPER_BEGIN; }
Q[A-Z]+_WRAPPER_DIFFERENT_C_CLASS\(  { yy_push_state(REGISTER_WRAPPER); return REGISTER_WRAPPER_BEGIN; }
Q[A-Z]+_WRAPPER_FAKE_SUBCLASS\(      { yy_push_state(REGISTER_WRAPPER); return REGISTER_WRAPPER_SUBCLASS_BEGIN; }
<REGISTER_WRAPPER>{
    ,                    { return COMMA; }
    \)                   { yy_pop_state(); return REGISTER_WRAPPER_END; }
    [[:space:]]*
    .                    { yyerror(codegen, "Syntax error in wrapper definition"); }
}

[[:space:]]enum          { yy_push_state(ENUM); return ENUM_KEYWORD; }
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

[[:space:]]namespace     { yy_push_state(NAMESPACE); return NAMESPACE_KEYWORD; }
<NAMESPACE>{
    {id}                 { yylval.Id = new QByteArray(yytext); yy_pop_state(); return IDENTIFIER; }
    [[:space:]]*
    .                    { yyerror(codegen, "Expected identifier after namespace keyword"); }
}

[[:space:]]class         { yy_push_state(CLASS); return CLASS_KEYWORD; }
<CLASS>{
    [A-Z]+_EXPORT
    {id}                 { yylval.Id = new QByteArray(yytext); yy_pop_state(); return IDENTIFIER; }
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

 /* Eats only one-line defines. Used to eat the #define QGLIB_REGISTER_TYPE(T),
    which would be a syntax error for codegen otherwise. */
#define                  { yy_push_state(MACRO); }
<MACRO>{
    \n                   { yy_pop_state(); }
    .
}

<*><<EOF>>               { return EOF; }
\n
.

%%

