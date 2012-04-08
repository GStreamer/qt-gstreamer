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
#ifndef GENERATOR_H
#define GENERATOR_H

#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QTextStream>

class CodeGen
{
public:
    typedef QHash<QByteArray, QByteArray> QByteArrayHash;
    typedef QPair<QByteArray, QByteArray> QByteArrayPair;

    static void parse(const QString & fileName, QTextStream & outStream);
    static void printGlobalWrapperDefinitions(QTextStream & outStream);

    inline CodeGen(const QString & fileName) : m_fileName(fileName) {}

    inline void setCurrentNameSpace(const char *name) { m_currentNamespace = name; }
    inline void setCurrentClass(const char *name) { m_currentClass = name; }

    void addEnum(const QList<QByteArray> & values, const QByteArrayHash & options);
    void addTypeRegistration(const QByteArray & namespaceId, const QByteArray & classId,
                             const QByteArray & enumId, const QByteArrayHash & options);
    void addWrapperDefinition(const QByteArray & classId, const QByteArrayHash & options);
    void addWrapperFakeSubclass(const QByteArray & prefix, const QByteArray & classId,
                                const QByteArrayHash & options);

    void fatalError(const char *msg);

private:
    struct Enum
    {
        QList<QByteArray> values;
        QByteArrayHash options;
    };

    void generateOutput(QTextStream & outStream);

    static void printTypeRegistration(QTextStream & outStream, const QByteArrayHash & typeReg);
    static void printEnumAssertions(QTextStream & outStream, const Enum & enumDef);
    void printWrapperDefinition(QTextStream & outStream, const QByteArrayHash & def);

    static QByteArray toGstStyle(const QByteArray & str);
    static QByteArray namespaceToGstStyle(const QByteArray & ns);

    const QString m_fileName;
    QByteArray m_currentNamespace;
    QByteArray m_currentClass;
    QList<Enum> m_enums;
    QList<QByteArrayHash> m_typeRegistrations;
    QList<QByteArrayHash> m_wrapperDefinitions;
    //< <namespace, class>, <prefix + options> >
    QHash<QByteArrayPair, QList<QByteArrayHash> > m_wrapperSubclasses;

    static QHash<QByteArray, QList<QByteArray> > s_wrapperDefinitions; //<namespace, classes>
};

#endif // GENERATOR_H
