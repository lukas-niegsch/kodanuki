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
	'sphinx_copybutton'
]

html_theme = 'sphinx_rtd_theme'
pygments_style = 'manni'
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme_options = {
	"collapse_navigation" : False
}
