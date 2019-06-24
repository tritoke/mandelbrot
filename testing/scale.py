# test platform for the logic of scaling the coordinates down to the area being studied

def scale(x, y, x_len, y_len, bl_x, bl_y, tr_x, tr_y):
    x /= (x_len / (tr_x - bl_x))
    y /= (y_len / (tr_y - bl_y))
    z = (bl_x + x) + ((tr_y - y) * 1j)
    return z

# tests: x and y len will be 1000 for all tests
xlen = ylen = 1000
shape = (xlen, ylen)
area = (-2,-2,2,2)

coord = (0,0)
target = -2+2j
val = scale(*coord,*shape,*area)
print(val == target)

coord = (0,ylen)
target = -2-2j
val = scale(*coord,*shape,*area)
print(val == target)

coord = (xlen,0)
target = 2+2j
val = scale(*coord,*shape,*area)
print(val == target)

coord = (xlen,ylen)
target = 2-2j
val = scale(*coord,*shape,*area)
print(val == target)


xlen = ylen = 1000
shape = (xlen, ylen)
area = (-2,-2,0,0)

coord = (0,0)
target = -2
val = scale(*coord,*shape,*area)
print(val == target)


coord = (xlen,0)
target = 0
val = scale(*coord,*shape,*area)
print(val == target)

coord = (0,ylen)
target = -2-2j
val = scale(*coord,*shape,*area)
print(val == target)


coord = (xlen,ylen)
target = -2j
val = scale(*coord,*shape,*area)
print(val == target)
