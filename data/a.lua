-- Lua script.
p=tetview:new()
p:load_medit("cube_2_edges.mesh")
rnd=glvCreate(0, 0, 500, 500, "TetView")
p:plot(rnd)
glvWait()
