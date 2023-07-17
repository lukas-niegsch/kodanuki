#include <doctest/doctest.h>
#include <bits/stdc++.h>
#include "engine/central/entity.h"
#include "engine/central/family.h"
using namespace kodanuki;

struct A {};
struct B {};
struct C {};

TEST_CASE("example usage")
{
    for (int k = 0; k < 10; k++) {
        Entity root = ECS::create();

        for (int i = 0; i < 100; i++) {
            Entity child = ECS::create(root);

            ECS::update<A>(child);
            ECS::update<B>(child);

            for (int j = 0; j < 100; j++) {
                Entity grandchild = ECS::create(child);

                ECS::bind<A>(grandchild, child);
                ECS::update<C>(grandchild);
                ECS::copy<B>(child, grandchild);

                CHECK(ECS::has<A>(grandchild));
                CHECK(ECS::has<B>(grandchild));
                CHECK(ECS::has<C>(grandchild));

                CHECK(ECS::get<Family>(grandchild).root == root);
            }

            ECS::remove<A>(child);
        }

        ECS::remove<Entity>(root);
    }
}