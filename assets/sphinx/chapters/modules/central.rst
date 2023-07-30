Central Module
==============

This module contains the entity component system. Each entity is just
some id and components are arbitrary types. It also provides methods
for manipulating and iterating over components.

Overview
~~~~~~~~

All methods start with the ``ECS`` prefix. For example you can create
some new entity with

.. code-block:: cpp

	Entity entity = ECS::create();

This is the prefered way to create entities. The method generates unique
ids for each entity (thread-safe). While possible, this id should never
change, as it is used to identify components.

Each entity contains multiple components added with the update method.

.. code-block:: cpp

	ECS::update<Position>(entity, position);
	ECS::update<Velocity>(entity, velocity);

Each unique type yields a different components. Calling this method twice
with the same type overwrites the previous value. The reverse operation is
given by the remove method.

.. code-block:: cpp

	ECS::remove<Position>(entity);
	ECS::remove<velocity>(entity);

All components are stored in contiguous memory for each unqiue type. So all
positions are next to each other, but not velocities. This makes this
pattern cache friendly.

The last important concepts are archetypes. These combine multiple components
are are used for iteration. We rely heavily on templates to define these.

.. code-block:: cpp

	using System = Archetype<Iterate<Position, Velocity>>;
	for (auto[pos, vel] : ECS::iterate<System>()) {}

Here we iterate over all entities that have both the position and velocity
component. This might be used in some physics system to update the position
based on their velocity.

.. note::

	These are only the most important methods, there are others that will
	be explained later.

Interface
~~~~~~~~~

Archetype
~~~~~~~~~
