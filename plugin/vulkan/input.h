#pragma once

// Signal that the entity should receive key events.
struct KeyReceiver {};

/**
 * Iterates over all entities with input receiver
 * components and updates their input events.
 */
void system_input_event_receiver(class GLFWwindow* window);

