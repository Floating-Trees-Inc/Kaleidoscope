//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-10-04 15:26:17
//

#include <catch2/catch_all.hpp>
#include <World/World_Node.h>
#include <glm/gtc/matrix_transform.hpp>

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

TEST_CASE("Node Local Transform", "[Node]")
{
    Node node;
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    node.SetLocalTransform(localTransform);

    REQUIRE(node.GetLocalTransform() == localTransform);
}

TEST_CASE("Node World Transform Without Parent", "[Node]")
{
    Node node;
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    node.SetLocalTransform(localTransform);

    REQUIRE(node.GetWorldTransform() == localTransform);
}

TEST_CASE("Node World Transform With Parent", "[Node]")
{
    Node parent;
    glm::mat4 parentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
    parent.SetLocalTransform(parentTransform);

    Node child;
    glm::mat4 childTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    child.SetLocalTransform(childTransform);

    parent.AddChild(&child);

    glm::mat4 expectedWorldTransform = parentTransform * childTransform;
    REQUIRE(child.GetWorldTransform() == expectedWorldTransform);
}

TEST_CASE("Node Transform Propagation", "[Node]")
{
    Node parent;
    glm::mat4 parentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
    parent.SetLocalTransform(parentTransform);

    Node child;
    glm::mat4 childTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    child.SetLocalTransform(childTransform);

    parent.AddChild(&child);

    // Update parent's transform and check propagation
    glm::mat4 newParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
    parent.SetLocalTransform(newParentTransform);

    glm::mat4 expectedWorldTransform = newParentTransform * childTransform;
    REQUIRE(child.GetWorldTransform() == expectedWorldTransform);
}
