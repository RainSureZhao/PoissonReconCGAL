-- Lua script.
p=tetview:new()
p:load_medit("patch_model_facet_approximation_error_test.mesh")
rnd=glvCreate(0, 0, 500, 500, "TetView")
p:plot(rnd)
glvWait()
