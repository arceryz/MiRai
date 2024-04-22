# Formulae for Raytracing
Timothy van der Valk
*To finally put them on paper for the third time, let it be a lesson to always properly write down your findings!*

---

## 1. Ray-Line Intersection
In this case we have an origin point $\vec{o}$ and a direction $\vec{d}$. We compute the intersection point given by a line segment between $\vec{u}$ and $\vec{v}$.

Our ray and line are given by the following two equations.
$$
\begin{aligned}
(1) \quad& \vec{x} = \vec{o} + t\vec{d} \\
(2) \quad& \langle\vec{n}, \vec{x} \rangle = \langle\vec{n}, \vec{u}\rangle
\end{aligned}
$$

Then combining these two and simplify the expression of inproducts results in the formula for $\hat{t}$ that intersects the line, given that they are not parallel.
$$
\begin{aligned}
\langle\vec{n}, \vec{o}+\hat{t}\vec{d}\rangle=\langle\vec{n}, \vec{o}\rangle + \hat{t}\langle\vec{n}, \vec{d}\rangle&=\langle\vec{n}, \vec{u}\rangle \\
\hat{t} = \frac{\langle\vec{n}, \vec{u}-\vec{o}\rangle}{\langle\vec{n}, \vec{d}\rangle}
\end{aligned}
$$

From this formula it is clear that the $\vec{n}$ and $\vec{d}$ may not be perpendicular to each other, since then the direction $\vec{d}$ is parallel to the line segment, which yields no intersection.

This formula extends to higher dimensions since it uses the simple and elegant language of inner products.

## 2. Ray-Circle Intersection

Now again we are given an origin and a direction, but we now collide against a circle with center $\vec{c}$ and radius $r$. The two defining equations for a ray and a circle are

$$
\begin{aligned}
(1) \quad& \vec{x} = \vec{o} + t\vec{d} \\
(2) \quad& \langle\vec{x}, \vec{x} \rangle = r^2
\end{aligned}
$$

Note that this is in the same inner product language, so we can naturally solve this by inserting $(1)$ into $(2)$. We will end up with three cases based on a discriminant that correspond to a ray miss, near-hit and full hit.

$$
\begin{aligned}
\langle\vec{o}+t\vec{d}, \vec{o}+t\vec{d}\rangle &= r^2 \\
t^2 \langle\vec{d}, \vec{d}\rangle +2t\langle\vec{o}, \vec{d}\rangle +  \langle\vec{o}, \vec{o}\rangle - r^2 &= 0
\end{aligned}
$$

Now we observe a quadratic formula and simply let
$$
\begin{aligned}
a &=\langle\vec{d}, \vec{d}\rangle \\ 
b &= 2\langle\vec{o}, \vec{d}\rangle \\
c &= \langle\vec{o}, \vec{o}\rangle - r^2 \\
D &= b^2-4ac
\end{aligned}
$$

Then the solution is given as

$$
\hat{t}_\pm = \frac{-b \pm \sqrt{D}}{2a}.
$$

The discriminant $D$ determines if the ray hits or not. We distinguish these cases. When there are two solutions for $\hat{t}$, we must still consider the cases where one or both of the collisions are behind the ray, having a negative value for $\hat{t}$.

- If $D < 0$ then the ray does not intersect the circle.
- If $D = 0$ then the ray touches the circle at a single point.

Given that $D > 0$ there are more cases to distinguish on based on the position of the ray's origin.
- If $\hat{t}_\pm > 0$, then the ray enters the circle at $\hat{t}_-$ and exits the circle at $\hat{t}_+$. Use either collision based on whether you want to collide with the circle convex (externally) or concave (internally).
- If $\hat{t}_- < 0$, then ray is inside the circle and only collides at the internal point $\hat{t}_+$. So $\hat{t}_-$ is behind the ray.
- If $\hat{t}_\pm < 0$ then both points lie behind the ray and there are no hits.

A special measure should be taken for $\hat{t}=0$, since in this case the ray is *on* the circle. To have reflections working properly, we require that $\hat{t}>\varepsilon$ with $\varepsilon$ near zero for a hit to occur. This enforces at least a small step forward to avoid being trapped on the mirror planes.

## 3. Vector Reflections
Given a vector $\vec{v}$ and a vector $\vec{n}$ we wish to compute the mirror reflection $\vec{r}$ of $\vec{v}$ in the space orthogonal to $\vec{n}$, which is a plane in $\mathbb{R}^3$ or a line in $\mathbb{R}^2$.

To compute the mirroring, we observe that the vector $\vec{v}-\vec{r}$ is exactly twice the projection of $\vec{v}$ on $\vec{n}$.

Therefore
$$
\vec{r} = \vec{v} - 2\vec{n}\langle \vec{n}, \vec{v}\rangle
$$

If we wish to compute the reflection *off* the surface, then we simply mirror the incoming vector pointing towards the normal. So this formula is a *mirror* reflection when $\vec{v}$ points in the same direction as the normal, and a *surface* reflection if $\vec{v}$ points in the opposite direction.