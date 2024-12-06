# Part 1
a,b=$<.map{|s|s.split.map(&:to_i)}.transpose.map(&:sort)
p a.zip(b).sum{|x,y|(x-y).abs}
# Part 2
t=b.tally
p a.sum{|x|x*(t[x]||0)}