Vector from B to A (non-commutative - order matters):
VecC = VecA - VecB = < Bx - Ax, By - Ay >
||VecC|| = distance between A and B

Scaling a vector:
2VecA = < 2Ax, 2Ay >

Adding vectors (following one, then the other. Commutative):
VecC = VecA + VecB = < Ax + Bx, Ay + By >

Vector magnitude (length):
||VecA|| = sqrt(Ax² + Ay²)
If we want to know whether C is closer to A or B, use the fact vector lengths should always be positive to say:
||VecA|| < ||VecB|| equiv. ||VecA²|| < ||VecB²||
and compare lengths without taking sqrt (expensive)
In this books math.h, use LengthSquared() for this.

Unit Vectors
Length 1, direction preserved
Aunit = < Ax / ||VecA|| , Ay / ||VecA|| > (always check for divide by 0)

In this books Math.h, a.Normalize(), and Vector2::Normalize(a)

This is used commonly for the vector showing an objects forwards direction

In this books Math.h, Actor::GetForward()

(Note SDL considers +y to be down, whereas a unit circle shows +y as up

In 2D, if you want the angle from a direction vector, use angle = Math::Atan2(Vector.x, Vector.y)

Dot product: VecA • VecB = Ax * Bx + Ay * By
Angle between 2 vectors: θ = arccos(Aunit • Bunit) (0 = perpendicular, 1 = parallel, -1 = antiparallel)

Cross product - calculates the normal (perpendicular) to a plane. Left hand rule: VecA follows index finger, VecB follows middle finger, thumb follows normal. Cross products only work on 3D vectors, a 2D vector can be converted to a 3D vector by adding 0 as the third component.

Anticommutative: VecA x VecB = - VecB x VecA

VecC = VecA x VecB = < AyBz - AzBy, AzBx - AxBz, AxBy - AyBx >

Eg for triangle ABC, VecU = B - A, VecV = C - A, normal = VecU x VecV
