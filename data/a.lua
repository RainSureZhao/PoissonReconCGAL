-- Lua script.
p=tetview:new()
p:load_medit("patch_model_test.mesh")
rnd=glvCreate(0, 0, 500, 500, "TetView")
p:plot(rnd)
glvWait()
