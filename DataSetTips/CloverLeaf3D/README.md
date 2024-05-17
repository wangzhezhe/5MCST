Cloverleaf3d is an proxy simulation adopted by ascent in situ visualization code
https://github.com/Alpine-DAV/ascent/tree/develop/src/examples/proxies/cloverleaf3d-ref


This is an fortran code.
Remember to enable the fortran for both conduit build and the ascent build.
Otherwise, the cloverleaf binary file will not be built.

On the frontier system, **remember to load fortran module and gcc module**.

We list several cloverleaf configuration here

In order to make it run properly, we also need to put the ascent configurations here.

Such as this one:

https://github.com/Alpine-DAV/ascent/blob/develop/src/examples/proxies/cloverleaf3d-ref/ascent_actions.json

We can also put some objects into the simulation domain (set geometries) to simulate more complicated behaviours.

https://github.com/wangzhezhe/VisPerfExp/blob/main/commonScripts/clover.in_default


