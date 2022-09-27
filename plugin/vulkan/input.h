#pragma once
class GLFWwindow;

// Signal that the entity should receive key events.
struct KeyReceiver {};

/**
 * Iterates over all key receivers and updates/removes
 * pressed or unpressed keys.
 *
 * The key receiver will have an key component iff the
 * key is pressed when this function was last called.
 *
 * @param window The window that provides the key events.
 */
template <typename Key>
void system_update_key_receiver(class GLFWwindow* window);

