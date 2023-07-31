# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
#
# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information
#
# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output
#
# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

project = 'Kodanuki'
copyright = '2023, Lukas Niegsch'
author = 'Lukas Niegsch'

extensions = [
	'sphinx_rtd_theme',
	'sphinx_copybutton',
	'breathe',
	'exhale'
]

html_theme = 'sphinx_rtd_theme'
pygments_style = 'manni'
exclude_patterns = ['Thumbs.db', '.DS_Store']

html_theme_options = {
	"collapse_navigation" : False
}

breathe_projects = {
	'engine': '_build/doxygen/xml'
}
breathe_default_project = 'engine'

exhale_args = {
	"containmentFolder":     "_build/api",
	"rootFileName":          "api.rst",
	"rootFileTitle":         "Engine API reference",
	"doxygenStripFromPath":  "..",
	"createTreeView":        True,
}

primary_domain = 'cpp'
highlight_language = 'cpp'
