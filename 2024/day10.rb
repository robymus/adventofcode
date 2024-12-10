require 'set'
# Input
m = $<.map{|line|[-1] + line.strip.chars.map(&:to_i) + [-1]}
w = m[0].size
m = [[-1]*w] + m + [[-1]*w]
h = m.size
# Part 1 - each node gets assigned a set of 9s it can reach
mm = h.times.map{|y|w.times.map{|x|[hh=m[y][x], hh==9 ? [[x,y]].to_set : nil]}}
8.downto(0).each{|t|
     h.times{|y|w.times{|x| mm[y][x][1] = Set.new([mm[y-1][x],mm[y+1][x],mm[y][x-1],mm[y][x+1]].select{|e|e[0]==t+1}.flat_map{|e|e[1].to_a}) if mm[y][x][0]==t}}
}
p mm.sum{|line|line.select{|e|e[0]==0}.sum{|e|e[1].size}}
# Part 2 - each node gets assigned a number how many different paths are leading there starting from 0
mm = h.times.map{|y|w.times.map{|x|[hh=m[y][x], hh==0 ? 1 : 0]}}
1.upto(9).each{|t|
    h.times{|y|w.times{|x| mm[y][x][1] = [mm[y-1][x],mm[y+1][x],mm[y][x-1],mm[y][x+1]].select{|e|e[0]==t-1}.sum{|e|e[1]} if mm[y][x][0]==t}}
}
p mm.sum{|line|line.select{|e|e[0]==9}.sum{|e|e[1]}}
