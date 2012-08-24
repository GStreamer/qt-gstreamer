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

int yylex(CodeGen *codegen);
void yyerror(CodeGen *codegen, const char *msg);

%}

%parse-param {CodeGen *codegen}
%lex-param {CodeGen *codegen}

%start header

%token REGISTER_TYPE_BEGIN
       SCOPE_RESOLUTION_OPERATOR
       REGISTER_TYPE_END
       REGISTER_WRAPPER_BEGIN
       REGISTER_WRAPPER_SUBCLASS_BEGIN
       REGISTER_WRAPPER_END
       INSTRUCTIONS_BEGIN
       INSTRUCTIONS_ASSIGN_OPERATOR
       INSTRUCTIONS_SEPARATOR
       INSTRUCTIONS_END
       ENUM_KEYWORD
       NAMESPACE_KEYWORD
       CLASS_KEYWORD
       LEFT_BRACE
       RIGHT_BRACE
       COMMA
       SEMICOLON

%token <Id> IDENTIFIER
%destructor { delete $$; } IDENTIFIER

%type <IdList> enum_list
%destructor { delete $$; } enum_list

%type <Id> optional_enum_id
%destructor { delete $$; } optional_enum_id

%type <HashMap> optional_instructions
%destructor { delete $$; } optional_instructions

%type <HashMap> instruction_list
%destructor { delete $$; } instruction_list

%type <Pair> instruction
%destructor { delete $$; } instruction

%%

header: header expression | expression | /*empty*/;

expression : enum_def | class_def | namespace_def | type_registration | wrapper_definition;

enum_def:
    ENUM_KEYWORD IDENTIFIER LEFT_BRACE optional_instructions enum_list RIGHT_BRACE SEMICOLON
    {
        codegen->addEnum(*$5, *$4);
        delete $2;
        delete $4;
        delete $5;
    };

enum_list:
    enum_list COMMA IDENTIFIER
    {
        $$ = $1;
        $$->append(*$3);
        delete $3;
    }
    | IDENTIFIER
    {
        $$ = new QList<QByteArray>();
        $$->append(*$1);
        delete $1;
    };

class_def:
    CLASS_KEYWORD IDENTIFIER
    {
        codegen->setCurrentClass(*$2);
        delete $2;
    };

namespace_def:
    NAMESPACE_KEYWORD IDENTIFIER
    {
        codegen->setCurrentNameSpace(*$2);
        delete $2;
    };

type_registration:
    REGISTER_TYPE_BEGIN IDENTIFIER SCOPE_RESOLUTION_OPERATOR IDENTIFIER optional_enum_id REGISTER_TYPE_END optional_instructions
    {
        codegen->addTypeRegistration(*$2, *$4, *$5, *$7);
        delete $2;
        delete $4;
        delete $5;
        delete $7;
    };

optional_enum_id:
    SCOPE_RESOLUTION_OPERATOR IDENTIFIER
    {
        $$ = $2;
    }
    | /*empty*/
    {
        $$ = new QByteArray();
    };

wrapper_definition:
    REGISTER_WRAPPER_BEGIN IDENTIFIER optional_c_class REGISTER_WRAPPER_END optional_instructions
    {
        codegen->addWrapperDefinition(*$2, *$5);
        delete $2;
        delete $5;
    }
    |
    REGISTER_WRAPPER_SUBCLASS_BEGIN IDENTIFIER COMMA IDENTIFIER REGISTER_WRAPPER_END optional_instructions
    {
        codegen->addWrapperFakeSubclass(*$2, *$4, *$6);
        delete $2;
        delete $4;
        delete $6;
    };

optional_c_class:
    COMMA IDENTIFIER
    {
        delete $2;
    }
    | /* empty */
    ;

optional_instructions:
    INSTRUCTIONS_BEGIN instruction_list INSTRUCTIONS_END
    {
        $$ = $2;
    }
    | /*empty*/
    {
        $$ = new QHash<QByteArray, QByteArray>();
    };

instruction_list:
    instruction_list INSTRUCTIONS_SEPARATOR instruction
    {
        $$ = $1;
        $$->insert($3->first, $3->second);
        delete $3;
    }
    | instruction
    {
        $$ = new QHash<QByteArray, QByteArray>();
        $$->insert($1->first, $1->second);
        delete $1;
    };

instruction:
    IDENTIFIER INSTRUCTIONS_ASSIGN_OPERATOR IDENTIFIER
    {
        $$ = new QPair<QByteArray, QByteArray>();
        $$->first = *$1;
        $$->second = *$3;
        delete $1;
        delete $3;
    }
    | IDENTIFIER
    {
        $$ = new QPair<QByteArray, QByteArray>();
        $$->first = *$1;
        delete $1;
    };


%%

void yyerror(CodeGen *codegen, const char *msg)
{
    codegen->fatalError(msg);
}
