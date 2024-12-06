require 'set'
# Input
m=$<.map(&:strip)
m=(['*'*m[0].size]+m+['*'*m[0].size]).map{|s|"*#{s}*"}
w=m[0].size
h=m.size
gy_s=m.find_index{|l|l['^']}
gx_s=m[gy_s].index('^')
m[gy_s][gx_s]='.'
# Part 1
gx,gy=gx_s,gy_s
dir=[ [0,-1], [1,0], [0,1], [-1,0] ]
d=0
visited=Set.new
while m[gy][gx]!='*' do
    visited << [gx,gy]
    d=(d+1)%dir.size while m[ny=gy+dir[d][1]][nx=gx+dir[d][0]] == '#'
    gx,gy=nx,ny
end
p visited.size
# Part 2 (brute force time: 16s)
def is_loop(m, gx, gy, ox, oy)
    return false if gx==ox && gy==oy
    dir=[ [0,-1], [1,0], [0,1], [-1,0] ]
    d=0
    visited=Set.new
    loop do
        return true if visited.include? [gx,gy,d]
        return false if m[gy][gx]=='*'
        visited << [gx,gy,d]
        d=(d+1)%dir.size while m[ny=gy+dir[d][1]][nx=gx+dir[d][0]] == '#' || (nx == ox && ny == oy)
        gx,gy=nx,ny
    end
end
p visited.count{|ox,oy|is_loop(m, gx_s, gy_s, ox, oy)}
