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
#ifndef GENERATOR_H
#define GENERATOR_H

#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QTextStream>

class CodeGen
{
public:
    static void parse(const QString & fileName);

    inline CodeGen(const QString & fileName) : m_fileName(fileName) {}

    inline void setCurrentNameSpace(const char *name) { m_currentNamespace = name; }
    inline void setCurrentClass(const char *name) { m_currentClass = name; }

    void addEnum(const QList<QByteArray> & values, const QHash<QByteArray, QByteArray> & options);
    void addTypeRegistration(const QByteArray & namespaceId, const QByteArray & classId,
                             const QByteArray & enumId, const QHash<QByteArray, QByteArray> & options);
    void fatalError(const char *msg);

private:
    struct Enum
    {
        QList<QByteArray> values;
        QHash<QByteArray, QByteArray> options;
    };

    typedef QHash<QByteArray, QByteArray> TypeRegistration;

    void generateOutput();
    static void printTypeRegistration(QTextStream & outStream, const TypeRegistration & typeReg);
    static void printEnumAssertions(QTextStream & outStream, const Enum & enumDef);
    static QByteArray toGstStyle(const QByteArray & str);

    const QString m_fileName;
    QByteArray m_currentNamespace;
    QByteArray m_currentClass;
    QList<Enum> m_enums;
    QList<TypeRegistration> m_typeRegistrations;
};

#endif // GENERATOR_H
