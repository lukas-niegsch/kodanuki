Build System
============

.. code-block:: toml

    [engine]
    description = "The kodanuki game engine."
    path        = "engine/"
    depends     = ["vulkan", "glfw"]
    library     = true
