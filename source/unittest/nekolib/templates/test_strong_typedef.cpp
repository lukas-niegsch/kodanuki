#include "engine/nekolib/templates/strong_typedef.h"
#include "engine/central/entity.h"
#include <doctest/doctest.h>
#include <bits/stdc++.h>
using namespace kodanuki;

// Implementation not correct ... just used for the interface!
class Example
{
public:
	struct Builder { int value; };
	static Example create(const Builder builder) { return Example(builder); }
	Example(const Builder) {}
	~Example() = default;
	Example(const Example&) {}
	Example& operator=(const Example&) { return *this; }
	Example(Example&& ) noexcept {}
	Example& operator=(Example&&) { return *this; }
	bool operator<(const Example&) const { return true; }
	bool operator==(const Example&) const { return true; }

private:
	int data;
};

KODANUKI_STRONG_TYPEDEF(Example, ExampleLike);

TEST_CASE("strong_typedef")
{
	static_assert(buildable<Example>);
	static_assert(buildable<ExampleLike>);
    static_assert(std::move_constructible<Example>);
    static_assert(std::move_constructible<ExampleLike>);
    static_assert(std::copy_constructible<Example>);
    static_assert(std::copy_constructible<ExampleLike>);
    static_assert(buildable<Example>);
    static_assert(buildable<ExampleLike>);

    Example original = Example::create({.value = 10});
    ExampleLike copy = original;
    original = copy;
    copy = std::move(original);
    original = std::move(copy);
    copy = ExampleLike::create({.value = 5});
    CHECK(original == copy); // Only syntax!
    CHECK(original < copy);

    Entity entity = ECS::create();
    ECS::update(entity, ExampleLike::create({.value = 5}));
    CHECK(ECS::has<Example>(entity) == false);
    ECS::remove<ExampleLike>(entity);
    ECS::update(entity, Example::create({.value = 5}));
    CHECK(ECS::has<ExampleLike>(entity) == false);
}
