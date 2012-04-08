# This file defines all the compilation tests

# Copyright (C) 2010 Collabora Ltd.
#   @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

##########
# Tests for testing whether the compilation tests system works as expected.
# If at least one of those tests fails, something is wrong with the cmake scripts.

cxx_compilation_test(compilation_test_system_test_1 SHOULD_COMPILE "
int main() { return 0; }
")

cxx_compilation_test(compilation_test_system_test_2 SHOULD_FAIL "
int main(float i) { return foobar; }
")
#########

######### BEGIN RefPointer tests ########

cxx_compilation_test(refpointer_test_casts SHOULD_COMPILE "
#include <QGst/Pipeline>
#include <QGst/Message>
#include <iostream>

static void testMethod(const QGlib::ObjectPtr & obj)
{
    std::cout << obj.dynamicCast<QGst::Bin>().isNull();
};

int main()
{
    QGst::BinPtr bin = QGst::Bin::create();

    {
        //operator=()
        QGst::ObjectPtr obj = bin;
        std::cout << obj.dynamicCast<QGst::Bin>().isNull();
    }

    {
        //copy constructor
        QGst::ObjectPtr obj(bin);
        std::cout << obj.dynamicCast<QGst::Bin>().isNull();
    }

    {
        //implicit cast
        testMethod(bin);
    }

    {
        QGst::ElementPtr element = bin.staticCast<QGst::Element>();
        QGst::PipelinePtr pipeline = bin.staticCast<QGst::Pipeline>();
    }
}
")


cxx_compilation_test(refpointer_test_invalid_upcast SHOULD_FAIL "
#include <QGst/Bin>
#include <QGst/Message>

int main()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QGst::ObjectPtr obj = bin;
    QGst::BinPtr bin2 = obj; //upcast not allowed
}
")

cxx_compilation_test(refpointer_test_invalid_staticCast SHOULD_FAIL "
#include <QGst/Bin>
#include <QGst/Message>

int main()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QGst::MessagePtr message = bin.staticCast<QGst::Message>(); //bin is not a message
}
")

cxx_compilation_test(refpointer_test_integral_cast SHOULD_FAIL "
#include <QGlib/Object>
#include <iostream>

void func(int b) { std::cout << b; }

int main()
{
    QGlib::ObjectPtr ptr;
    func(ptr);
}
")

cxx_compilation_test(refpointer_test_bool_cast SHOULD_COMPILE "
#include <QGlib/Object>
#include <iostream>

int main()
{
    QGlib::ObjectPtr ptr;
    if (ptr) {
        std::cout << \"foo\";
    }
}
")

######### END RefPointer tests ########
######### BEGIN disconnect tests ########

cxx_compilation_test(disconnect_full SHOULD_COMPILE "
#include <QGlib/Connect>

int main()
{
    QGlib::disconnect(0, 0, 0, 0);
    QGlib::disconnect(NULL, NULL, NULL, NULL);
}
")

cxx_compilation_test(disconnect_default_args SHOULD_COMPILE "
#include <QGlib/Connect>

int main()
{
    QGlib::disconnect(0, 0, 0);
    QGlib::disconnect(0, 0);
    QGlib::disconnect(0);
}
")

cxx_compilation_test(disconnect_non_member_slot SHOULD_FAIL "
#include <QGlib/Connect>

int foobar() {}

int main()
{
    QGlib::disconnect(0, 0, 0, &foobar);
}
")

cxx_compilation_test(disconnect_string_slot SHOULD_FAIL "
#include <QGlib/Connect>

int main()
{
    QGlib::disconnect(0, 0, 0, \"hello world\");
}
")

######### END disconnect tests ########
######### BEGIN bin add() tests ########

cxx_compilation_test(bin_add_test_1 SHOULD_COMPILE "
#include <QGst/Bin>

int main()
{
    QGst::ElementPtr tee;
    QGst::BinPtr bin;

    bool b = bin->add(tee);
    bin->add(tee, tee);
    bin->add(tee, tee, tee);
    bin->add(tee, tee, tee, tee);
    bin->add(tee, tee, tee, tee, tee);
    bin->add(tee, tee, tee, tee, tee, tee);
    bin->add(tee, tee, tee, tee, tee, tee, tee);
    bin->add(tee, tee, tee, tee, tee, tee, tee, tee);
    bin->add(tee, tee, tee, tee, tee, tee, tee, tee, tee);
    bin->add(tee, tee, tee, tee, tee, tee, tee, tee, tee, tee);
}
")

cxx_compilation_test(bin_add_test_2 SHOULD_COMPILE "
#define QGST_BIN_ADD_MAX_ARGS 12
#include <QGst/Bin>

int main()
{
    QGst::ElementPtr tee;
    QGst::BinPtr bin;
    bin->add(tee, tee, tee, tee, tee, tee, tee, tee, tee, tee, tee, tee);
}
")
######### END bin add() tests ########
######### BEGIN element linkMany/unlinkMany tests ########

cxx_compilation_test(element_linkMany_test_1 SHOULD_COMPILE "
#include <QGst/Element>

int main()
{
    QGst::ElementPtr tee;
    bool b = QGst::Element::linkMany(tee, tee, tee);
    b = QGst::Element::linkMany(tee, tee, tee, tee);
    b = QGst::Element::linkMany(tee, tee, tee, tee, tee);
    b = QGst::Element::linkMany(tee, tee, tee, tee, tee, tee);
    b = QGst::Element::linkMany(tee, tee, tee, tee, tee, tee, tee);
    b = QGst::Element::linkMany(tee, tee, tee, tee, tee, tee, tee, tee);
    b = QGst::Element::linkMany(tee, tee, tee, tee, tee, tee, tee, tee, tee);
    b = QGst::Element::linkMany(tee, tee, tee, tee, tee, tee, tee, tee, tee, tee);

    QGst::Element::unlinkMany(tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee, tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee, tee, tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee, tee, tee, tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee, tee, tee, tee, tee, tee, tee);
}
")

cxx_compilation_test(element_linkMany_test_2 SHOULD_COMPILE "
#define QGST_ELEMENT_LINK_MANY_MAX_ARGS 12
#define QGST_ELEMENT_UNLINK_MANY_MAX_ARGS 12
#include <QGst/Element>

int main()
{
    QGst::ElementPtr tee;
    bool b = QGst::Element::linkMany(tee, tee, tee, tee, tee, tee, tee, tee, tee, tee, tee, tee);
    QGst::Element::unlinkMany(tee, tee, tee, tee, tee, tee, tee, tee, tee, tee, tee, tee);
}
")
######### END element linkMany/unlinkMany tests ########
