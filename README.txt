// $Id$

The Five Star voting module adds a clean, attractive voting widget to nodes in Drupal 5. It features:

 * jQuery rollover effects and AJAX no-reload voting
 * Graceful degradation to an HTML rating form when JavaScript is turned off
 * Per-nodetype configurability
 * Support for anonymous voters
 * Spam protection to keep users from filling your DB with bogus votes
 * Easy-to-use integration with Views module for lists sorted by rating, or filtered by min/max ratings
 * A Fivestar CCK field for use in custom node types
 * An easy-to-use Form API element type for use in other modules

Fivestar was designed by Nate Haug and Jeff Eaton.

This Module Made by Robots: http://www.lullabot.com


Dependencies
------------
 * votingapi

Fivestar also provides additional features for both the CCK and Views modules.

Install
-------
Installing the Five Star voting module is simple:

1) Copy the fivestar folder to the modules folder in your installation.

2) Enable the module using Administer -> Modules (/admin/build/modules)

Note: Aggressive caching will complain that fivestar doesn't work, but it
actually causes no problems. To improve performance, the module implements
hook_init() -- and the caching advisor screen uses that as the only metric to
determine whether a module will work with the caching system. Activate it
without fear, friends -- Fivestar will continue to hum happily along.

Configuration
-------------
There is no global settings page for Fivestar. It's configuration is spread
between the content type settings page and access permissions. To configure:

1) Activate voting on each content type. For example, if you want Fivestar to
   appear on story nodes, use Administer -> Content Management ->
   Content Types -> Story, and check the "Enable Five Star rating" box under
   the "Five Star ratings" heading. Repeat for each content type desired.

2) Enable anonymous voting.
   If you want to allow anonymous voting, you'll need to set permissions for
   that. Use Administer -> User Management -> Access Control, and check the
   "rate content" and "view ratings" checkboxes for the roles you'd like.
   You'll find these permission items under the "fivestar module" heading.

Support
-------
If you experience a problem with cck_imagefield or have a problem, file a
request or issue on the fivestar queue at
http://drupal.org/project/issues/fivestar. DO NOT POST IN THE FORUMS. Posting in
the issue queues is a direct line of communication with the module authors.