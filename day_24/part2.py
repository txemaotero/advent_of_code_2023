import sympy as sp

x1, x2, x3 = sp.symbols('x1 x2 x3', integer=True)
v1, v2, v3 = sp.symbols('v1 v2 v3', integer=True)
t0, t1, t2 = sp.symbols('t0 t1 t2', integer=True)

A0 = [165202579340993, 295858127172353, 306521398134969]
A1 = [237754683140019, 314961377106211, 352227681591321]
A2 = [310378612723573, 304955313774575, 184608502104129]

V0 = [104, -69, 104]
V1 = [51, -89, 5   ]
V2 = [6, -60, 68   ]

eq1 = sp.Eq(x1 + (v1 - V0[0]) * t0, A0[0])
eq2 = sp.Eq(x2 + (v2 - V0[1]) * t0, A0[1])
eq3 = sp.Eq(x3 + (v3 - V0[2]) * t0, A0[2])

eq4 = sp.Eq(x1 + (v1 - V1[0]) * t1, A1[0])
eq5 = sp.Eq(x2 + (v2 - V1[1]) * t1, A1[1])
eq6 = sp.Eq(x3 + (v3 - V1[2]) * t1, A1[2])

eq7 = sp.Eq(x1 + (v1 - V2[0]) * t2, A2[0])
eq8 = sp.Eq(x2 + (v2 - V2[1]) * t2, A2[1])
eq9 = sp.Eq(x3 + (v3 - V2[2]) * t2, A2[2])

solution = sp.solve([eq1, eq2, eq3, eq4, eq5, eq6, eq7, eq8, eq9], (x1, x2, x3, v1, v2, v3, t0, t1, t2), dict=True)

# Mostrar la solución
if solution:
    for sol in solution:
        print("Part 2: ", sol[x1] + sol[x2] + sol[x3])
else:
    print("No solutions")

