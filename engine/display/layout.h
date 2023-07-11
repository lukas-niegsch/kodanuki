#pragma once
#include "engine/central/entity.h"
#include <cstdint>

namespace kodanuki
{

/**
 * The direction inside some layout.
 *
 * Some layouts (e.g. linear) define a relative primary direction.
 * The direction can be absolute or relative to the parent direction.
 * If the parent has no direction then relative equals absolute.
 *
 * The absolute direction will be defined by the layout. It is the
 * most logical one, e.g. border: front is up.
 *
 * The enum is defined as namespace to enable bitwise operations.
 */
namespace Direction
{
	constexpr uint32_t NONE      = 0;
	constexpr uint32_t REL_FRONT = 1 << 0;
	constexpr uint32_t REL_BACK  = 1 << 1;
	constexpr uint32_t REL_LEFT  = 1 << 2;
	constexpr uint32_t REL_RIGHT = 1 << 3;
	constexpr uint32_t ABS_FRONT = 1 << 4;
	constexpr uint32_t ABS_BACK  = 1 << 5;
	constexpr uint32_t ABS_LEFT  = 1 << 6;
	constexpr uint32_t ABS_RIGHT = 1 << 7;
	constexpr uint32_t REL_EVERY = REL_FRONT | REL_BACK | REL_LEFT | REL_RIGHT;
	constexpr uint32_t ABS_EVERY = ABS_FRONT | ABS_BACK | ABS_LEFT | ABS_RIGHT;
	constexpr uint32_t EVERY     = REL_EVERY | ABS_EVERY;
}

/**
 * The border layout defines a small edge around the parent layout.
 *
 * The border is a property of the current layout, not its parent.
 * It does not change the size of the parent, but makes the child
 * smaller. Borders can be enabled on all 4 sides.
 */
struct BorderLayout
{
	// The direction on which to enable the border.
	uint32_t direction;

	// The size of the border in pixels for each direction.
	uint32_t size;
};

/**
 * The floating layout defines a fixed size and position.
 *
 * This layout is a property of the current layout. It does not change
 * the size of the parent, but makes the child a fixed size and fixed
 * position.
 *
 * Invalid size and position combinations will be ignored. So if the
 * parent is too small to completely fit this layout, it will not be
 * considered for rendering.
 */
struct FloatingLayout
{
	// The direction for the edges.
	uint32_t direction;

	// The distance from the front edge.
	uint32_t front;

	// The distance from the back edge.
	uint32_t back;

	// The distance from the left edge.
	uint32_t left;

	// The distance from the right edge.
	uint32_t right;
};

/**
 * The grid layout arrange child layouts in a grid.
 *
 * This layout changes the size and position of the children layouts.
 * Children are ordered inside a grid. The number of rows depends on
 * the number of children and how many columns this layout has.
 *
 * The rows are ordered from front to back, and the columns from left
 * to right. If columns equals 1, then this is essentially just an
 * linear layout. The number of columns should never be zero.
 *
 * The size of each row and column is determined by the maximum weight
 * along the other direction. Each weight point is then one share of the
 * total size. The grid cells may not be aligned along each direction.
 */
struct GridLayout
{
	// The primary direction for the rows.
	uint32_t direction;

	// The number of items along each row.
	uint32_t columns;
};

/**
 * The grid item layout defines the size and position inside the grid layout.
 *
 * Layouts which share the same position might be drawn above each other. So
 * it is best to avoid them. The grid layout determines how much each grid
 * cell owns. If that size is smaller than one of the minimum size, then the
 * layout will not be considered for rendering.
 */
struct GridItemLayout
{
	// The minimum size along the primary direction.
	uint32_t min_primary_size;

	// The minimum size along the secondary direction.
	uint32_t min_secondary_size;

	// The weight that determines the size of this widget.
	uint32_t weight;

	// The unique position of this widget, pos = y * ncols + x.
	uint32_t position;
};

/**
 * The render layout defines the absolute size and position for rendering.
 *
 * This component is computed after all the layout calculations are done.
 * The size is absolute in regards to other layouts but relative to the
 * render medium (e.g. window). The top-left corner is (0, 0).
 *
 * We keep track of the direction since it is used for rendering. However,
 * the size and position are absolute.
 */
struct RenderLayout
{
	// The absolute direction of the layout.
	uint32_t direction;

	// The absolute position along the x-axis.
	uint32_t xpos;

	// The absolute position along the y-axis.
	uint32_t ypos;

	// The absolute size along the x-axis.
	uint32_t width;

	// The absolute size along the y-axis.
	uint32_t height;
};

/**
 * Defines that the entity contains some layout.
 *
 * Each entity with layout should have this flag. Otherwise it is
 * not considered by the layout system.
 */
struct IsLayoutFlag {};

/**
 * The layout system is responsible for computing layouts.
 *
 * The system iterates over all layouts and finds those which not
 * define a render layout. Using the surrounding data it computes
 * that render layout and potentially updates the surrounding tree.
 *
 * The class can be used with the builder pattern:
 *
 * LayoutSystem system = LayoutSystem::create({
 *     .owner = {},
 *     ...
 * });
 *
 * The default values can be changed afterwards, but this might revolve
 * recomputing the whole layout tree. Multiple layout systems should not
 * be used at the same time.
 */
class LayoutSystem
{
public:
	/**
	 * The parameters that are used to build the layout system.
	 *
	 * The owner destroys this system automatically once the owner is
	 * destroyed. There are currently no further options.
	 */
	struct Builder
	{
		// The owner of this system.
		Entity owner;
	};

	/**
	 * Creates the layout system with special syntax.
	 *
	 * @param builder The struct containing all parameters.
	 * @return The newly created layout system.
	 */
	static LayoutSystem create(Builder builder);

	/**
	 * Returns the entity containing the implementation.
	 *
	 * @return The entity containing the implementation.
	 */
	operator Entity();

	/**
	 * Updates all changed layouts.
	 *
	 * This will dynamically find all changed layouts and calculate their
	 * render layout. This will only consider entities with the IsLayoutFlag
	 * enabled.
	 */
	void tick();
};

}
