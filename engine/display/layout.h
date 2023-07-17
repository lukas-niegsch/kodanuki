#pragma once
#include "engine/central/entity.h"

namespace kodanuki
{

/**
 * Defines that the entity contains layouts.
 * 
 * Layouts are defined axis-wise (horizontal or verticle) for
 * the family tree. For example, if an entity defines a linear
 * layout, then its children with this flag are rendered.
 * 
 * The layout system computes the render layout.
 */
struct IsLayoutFlag {};

/**
 * The rotation layout allows changing the axis.
 * 
 * Rotations are defined relative to their parent rotations.
 * If some layout works along the x-axis, this layout flips the
 * rotation for the children to the y-axis or keeps it.
 * 
 * The root rotation is the y-axis with positive direction.
 */
struct RotationLayout
{
	// Flips the axis for the children layouts.
	bool flip_axis;

	// Mirrors the children layouts around their axis.
	bool mirror;
};

/**
 * The floating layout ignores sibling layouts.
 * 
 * It can be placed anywhere within the parent. It is not affected
 * by the size and position of the sibling and vice versa. If it does
 * not fit then this layout will not be considered for rendering.
 */
struct FloatingLayout
{
	// The distance along the axis from normal direction.
	uint32_t offset;

	// The size of this layout along the axis.
	uint32_t size;
};

/**
 * The border layout defines small pixel edges along the axis.
 * 
 * Both sides can be changed independently. This layout is only
 * affects the size of the current layout in regards to their
 * parent layout. The parent size stays constant.
 */
struct BorderLayout
{
	// The number of pixels in the normal direction.
	uint32_t normal_size;

	// The number of pixels in the mirrored direction.
	uint32_t mirror_size;
};

/**
 * The scaler layout defines size constraints.
 * 
 * This is used inside the parent layout to determine if it fits
 * or how much space to give this layout. If it does not fit then
 * this layout will not be considered for rendering. This will
 * always happen if max_size < min_size.
 */
struct ScalerLayout
{
	// The minimum size along the axis.
	uint32_t min_size;

	// The maximum size along the axis.
	uint32_t max_size;
};

/**
 * The weight layout defines properties along siblings.
 * 
 * This is used inside  the parent layout to determine the size
 * and order of this layout. For example inside the linear layout
 * children layouts are ordered along their gravity.
 * 
 * If not defined then weight=0 and gravity=0 is assumed.
 */
struct WeightLayout
{
	// The weight that determines the size of this widget.
	uint32_t weight;

	// The higher the gravity the lower the layout ranks.
	uint32_t gravity;
};

/**
 * The linear layout orders children linearly.
 * 
 * Children are ordered along the current axis from the normal
 * direction to the mirrored direction. Their size depend on
 * the children layouts and the available size.
 *
 * If not defined then separator_size=0 is assumed.
 */
struct LinearLayout
{
	// The number of pixels between each children.
	uint32_t separator_size;
};

/**
 * The render layout defines the bounding rectangle for rendering.
 *
 * This component is computed after all the layout calculations are done.
 * The size is absolute in regards to other layouts but relative to the
 * render medium (e.g. window). The top-left corner is (0, 0).
 */
struct RenderLayout
{
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

private:
	// The entity storing the implementation.
	Entity itself;
};

}
