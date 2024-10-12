-- Lua script.
p=tetview:new()
p:load_mesh("D:/Code/cpp/PoissonReconCGAL/data/layerblock.ele")
rnd=glvCreate(0, 0, 500, 500, "TetView")
p:plot(rnd)
glvWait()
