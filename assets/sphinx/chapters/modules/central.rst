Central Module
==============

This module contains the entity component system. Each entity is
an integer ID and components are arbitrary types. It provides methods
for manipulating the components of each entity. Systems iterate over
archetypes which are collections of multiple components. It allows the
systems to modify all entities which have the required components.

Overview
~~~~~~~~

The following UML class diagram shows an overview of the entity component
system (`ECS <https://en.wikipedia.org/wiki/Entity_component_system>`_).

.. image:: ../../images/central_overview.svg
	:align: center

|

The class ``ECS`` provides the main interface for this module. It provides
methods for creating entities, and updating or removing components. There
is one ``storage`` class for each component type which stores them in
contiguous memory. The ECS maps the ``entity`` keys to positions inside
the storage. Each ``archetype`` can iterate over multiple storage types.

This software pattern splits the responsibilities of data and behavior
between entities and systems. Entities only define which components they
have which are usually only data classes with utility methods. Systems
define the logic between components belonging to specific archetypes. Thus,
this pattern favors composition and the reusability of components.

For example consider a player inside some game. This player can have some
position and velocity.

.. code-block::

	Entity player = ECS::create();
	ECS::update<Position>(player, position);
	ECS::update<Velocity>(player, velocity);

Then, the player moves by iterating over all entities with the position and
velocity components.

.. code-block::

	using MoveSystem = Archetype<Iterate<Position, Velocity>>;
	for (auto[pos, vel] : ECS::iterate<MoveSystem>()) {
		pos += delta_time * velocity;
	}

An enemy entity could also have both components and thus is also affected
by the movement system. But a tree entity which probably has no velocity
would not be affected. We will cover all methods inside the ECS in the
following chapters.
