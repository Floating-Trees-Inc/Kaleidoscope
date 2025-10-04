#include <catch2/catch_all.hpp>
#include <World/World_Node.h>

using namespace World;

TEST_CASE("Node AddChild and RemoveChild", "[Node]")
{
    Node parent("Parent");
    Node child("Child");

    REQUIRE(parent.GetChildren().empty());

    parent.AddChild(&child);
    REQUIRE(parent.GetChildren().size() == 1);
    REQUIRE(parent.GetChildren()[0] == &child);
    REQUIRE(child.GetParent() == &parent);

    parent.RemoveChild(&child);
    REQUIRE(parent.GetChildren().empty());
    REQUIRE(child.GetParent() == nullptr);
}

TEST_CASE("Node Prevent Cycles", "[Node]")
{
    Node parent("Parent");
    Node child("Child");

    parent.AddChild(&child);
    REQUIRE(parent.GetChildren().size() == 1);

    child.AddChild(&parent); // Attempt to create a cycle
    REQUIRE(child.GetChildren().empty());
    REQUIRE(parent.GetParent() == nullptr);
}

TEST_CASE("Node Unique Sibling Names", "[Node]")
{
    Node parent("Parent");
    Node child1("Child");
    Node child2("Child");

    parent.AddChild(&child1);
    parent.AddChild(&child2);

    REQUIRE(child1.GetName() == "Child");
    REQUIRE(child2.GetName() == "Child_2");
}

TEST_CASE("Node Path", "[Node]")
{
    Node root("Root");
    Node child1("Child1");
    Node child2("Child2");

    root.AddChild(&child1);
    child1.AddChild(&child2);

    REQUIRE(root.GetPath() == "/Root");
    REQUIRE(child1.GetPath() == "/Root/Child1");
    REQUIRE(child2.GetPath() == "/Root/Child1/Child2");
}
