# Formulae for Raytracing
Timothy van der Valk
*To finally put them on paper for the third time, let it be a lesson to always properly write down your findings!*

---

## 1. Ray-Surface Intersection
We wish to compute the intersection of our ray with a surface specified by a single normal. This surface always has $n-1$ dimensions from the space it exists in. In the case of $n=2$ we have a line segment. In $n=3$ we have a plane.

Our ray and surface are given by the following two equations. Here $\vec{u}$ is a point on the target surface.
$$
\begin{aligned}
(1) \quad& \vec{x} = \vec{o} + t\vec{d} \\
(2) \quad& \langle\vec{n}, \vec{x} \rangle = \langle\vec{n}, \vec{u}\rangle
\end{aligned}
$$

Then combining these two and simplify the expression of inproducts results in the formula for $t$ that intersects the surface, given that the ray is not parallel to the surface.
$$
\begin{aligned}
\langle\vec{n}, \vec{o}+t\vec{d}\rangle=\langle\vec{n}, \vec{o}\rangle + t\langle\vec{n}, \vec{d}\rangle&=\langle\vec{n}, \vec{u}\rangle \\
t = \frac{\langle\vec{n}, \vec{u}-\vec{o}\rangle}{\langle\vec{n}, \vec{d}\rangle}
\end{aligned}
$$

From this formula it is clear that the $\vec{n}$ and $\vec{d}$ may not be perpendicular to each other, since then the direction $\vec{d}$ is parallel to the surface, which yields no intersection.

This formula extends to higher dimensions since it uses the simple and elegant language of inner products.

## 2. Ray-Circle Intersection

Now again we are given an origin and a direction, but we now collide against a circle with center $\vec{c}$ and radius $r$. The two defining equations for a ray and a circle are

$$
\begin{aligned}
(1) \quad& \vec{x} = \vec{o} + t\vec{d} \\
(2) \quad& \langle\vec{x}-\vec{c}, \vec{x}-\vec{c} \rangle = r^2
\end{aligned}
$$

Note that this is in the same inner product language, so we can naturally solve this by inserting $(1)$ into $(2)$. We will end up with three cases based on a discriminant that correspond to a ray miss, near-hit and full hit.

We set $\vec{r} = \vec{o}-\vec{c}$ as the relative origin to the circle. Then we solve for $t$ simply as

$$
\begin{aligned}
\langle\vec{r}+t\vec{d}, \vec{r}+t\vec{d}\rangle &= r^2 \\
t^2 \langle\vec{d}, \vec{d}\rangle +2t\langle\vec{r}, \vec{d}\rangle +  \langle\vec{r}, \vec{r}\rangle - r^2 &= 0
\end{aligned}
$$

Now we observe a quadratic formula and simply let
$$
\begin{aligned}
a &=\langle\vec{d}, \vec{d}\rangle \\ 
b &= 2\langle\vec{r}, \vec{d}\rangle \\
c &= \langle\vec{r}, \vec{r}\rangle - r^2 \\
D &= b^2-4ac
\end{aligned}
$$

Then the solution is given as

$$
t_\pm = \frac{-b \pm \sqrt{D}}{2a}.
$$

The discriminant $D$ determines if the ray hits or not. We distinguish these cases. When there are two solutions for $t$, we must still consider the cases where one or both of the collisions are behind the ray, having a negative value for $t$.

- If $D < 0$ then the ray does not intersect the circle.
- If $D = 0$ then the ray touches the circle at a single point.

Given that $D > 0$ there are more cases to distinguish on based on the position of the ray's origin.
- If $t_\pm > 0$, then the ray enters the circle at $t_-$ and exits the circle at $t_+$. Use either collision based on whether you want to collide with the circle convex (externally) or concave (internally).
- If $t_- < 0$, then ray is inside the circle and only collides at the internal point $t_+$. So $t_-$ is behind the ray.
- If $t_\pm < 0$ then both points lie behind the ray and there are no hits.

A special measure should be taken for $t=0$, since in this case the ray is *on* the circle. To have reflections working properly, we require that $t>\varepsilon$ with $\varepsilon$ near zero for a hit to occur. This enforces at least a small step forward to avoid being trapped on the mirror planes.

## 3. Vector Reflections
Given a vector $\vec{v}$ and a vector $\vec{n}$ we wish to compute the mirror reflection $\vec{r}$ of $\vec{v}$ in the space orthogonal to $\vec{n}$, which is a plane in $\mathbb{R}^3$ or a line in $\mathbb{R}^2$.

To compute the mirroring, we observe that the vector $\vec{v}-\vec{r}$ is exactly twice the projection of $\vec{v}$ on $\vec{n}$.

Therefore
$$
\vec{r} = \vec{v} - 2\vec{n}\langle \vec{n}, \vec{v}\rangle
$$

If we wish to compute the reflection *off* the surface, then we simply mirror the incoming vector pointing towards the normal. So this formula is a *mirror* reflection when $\vec{v}$ points in the same direction as the normal, and a *surface* reflection if $\vec{v}$ points in the opposite direction.

## 4. Raytracer3D overview

The rays are cast by the fragment shader, which computes the ray direction using the projection and view matrices. NDC Coordinates are inverted to local space by multiplying with the inverse of the model-view-projection matrix.

```mermaid
graph LR;
3dm["3D Model"]-->vs["Vertex Shader"]--NDC coords [-1, 1]-->fs["Fragment Shader"]
```

The fragment shader is where the raytracing happens. In general for raytracing, the essence is to

1. Compute the local space ray origin $\vec{o}$ and direction $\vec{d}$ from NDC coordinates.
2. For each reflection (numBounces) do the following:
2.1 Determine which mirror has been collided with. This can be optimized but we simply iterate every mirror.
2.2. Given the collided mirror and collision point, determine if there is an edge intersection. If so, stop and give color. This edge intersection test requires a distance check for each edge (straight or curved) which adds up to the cost of the algorithm. Edge visualising is not mandatory and can be substituted for corner visualisation or even single prop visualiation.
2.3. Reflect the ray in the normal and set new origin.
3. Set the fragment color.

```mermaid
graph TD
Ray-->rayloop["For each reflection"]-->mloop["For each mirror"]-->det["Determine collision step"]
det--Set new closest-->mloop
mloop--Closest hit found-->edgeloop["For each edge"]-->detdi["Determine hit distance to edge"]
detdi--Edge collision-->break["Break loops and set color"]
edgeloop--No edge collision-->compref["Set new ray origin and direction"]
compref--Next reflection-->rayloop
rayloop--End of loop-->black["Set background color (Black)"]
```

## 5. Determine If Point In Polygon

For the 3D curved mirrors we use a region of a circle as the surface. We must be able to determine of a collision occured inside that surface, which is determine by a polygon on a sphere. So given a point and a polygon, we have to determine if the point is inside. 

There are two ways to do this

### Convex Polygons

Here we determine the winding number as a sum of internal angles of the vertices to the point. The sum of internal angles must be 360.

### Any Polygon

Here we must trace a ray from the point to any constant direction. The number of intersections with the polygon must be an odd number. This is more costly.