# Liquid simulation

Attempt to reproduce [video by Sebastian Lague](https://youtu.be/rSKMYc1CQHE?si=3oE1KMeIPU83HOOW) with Defold compute shaders.

Only basic implementation, no bitonic sort, number of particles can't exceed the maximum number of workers inside a worker group.

Each particle is calculated by a worker group and inside each group workers check influence of all other particles (one particle by worker).

See other projects for more advanced stuff.