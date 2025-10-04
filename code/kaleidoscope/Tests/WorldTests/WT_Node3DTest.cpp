//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 19:34:08
//

#include <catch2/catch_all.hpp>
#include <KernelCore/KC_Context.h>
#include <World/World_Node3D.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace World;

TEST_CASE("Node3D Local Transform", "[Node3D]")
{
    Node3D node;
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    node.SetLocalTransform(localTransform);

    REQUIRE(node.GetLocalTransform() == localTransform);
}

TEST_CASE("Node3D World Transform Without Parent", "[Node3D]")
{
    Node3D node;
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    node.SetLocalTransform(localTransform);

    REQUIRE(node.GetWorldTransform() == localTransform);
}

TEST_CASE("Node3D World Transform With Parent", "[Node3D]")
{
    Node3D parent;
    glm::mat4 parentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
    parent.SetLocalTransform(parentTransform);

    Node3D child;
    glm::mat4 childTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    child.SetLocalTransform(childTransform);

    parent.AddChild(&child);

    glm::mat4 expectedWorldTransform = parentTransform * childTransform;
    REQUIRE(child.GetWorldTransform() == expectedWorldTransform);
}

TEST_CASE("Node3D Transform Propagation", "[Node3D]")
{
    Node3D parent;
    glm::mat4 parentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
    parent.SetLocalTransform(parentTransform);

    Node3D child;
    glm::mat4 childTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    child.SetLocalTransform(childTransform);

    parent.AddChild(&child);

    // Update parent's transform and check propagation
    glm::mat4 newParentTransform = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
    parent.SetLocalTransform(newParentTransform);

    glm::mat4 expectedWorldTransform = newParentTransform * childTransform;
    REQUIRE(child.GetWorldTransform() == expectedWorldTransform);
}


